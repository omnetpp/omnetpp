//=========================================================================
//  DATAFLOWMANAGER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "channel.h"
#include "nodetype.h"
#include "commonnodes.h"
#include "dataflowmanager.h"

USING_NAMESPACE


DataflowManager::DataflowManager()
{
    lastnode = 0;
    threshold = 1000;
}

DataflowManager::~DataflowManager()
{
    unsigned int i;
    for (i=0; i<nodes.size(); i++)
        delete nodes[i];
    for (i=0; i<channels.size(); i++)
        delete channels[i];
}

void DataflowManager::addNode(Node *node)
{
    nodes.push_back(node);
    node->setDataflowManager(this);
}

void DataflowManager::addChannel(Channel *channel)
{
    channels.push_back(channel);
}

void DataflowManager::connect(Port *src, Port *dest)
{
    if (src->channel())
        throw opp_runtime_error("connect: source port already connected");
    if (dest->channel())
        throw opp_runtime_error("connect: destination port already connected");
    if (!dest->node())
        throw opp_runtime_error("connect: port's owner node not filled in");

    Channel *ch = new Channel();
    addChannel(ch);

    src->setChannel(ch);
    dest->setChannel(ch);
    ch->setProducerNode(src->node());
    ch->setConsumerNode(dest->node());
}

// FIXME: validate node attributes

void DataflowManager::execute(IProgressMonitor *monitor)
{
    if (nodes.size()==0)
        return;

    //
    // repeat until all nodes have finished:
    //   select a node which is:
    //     - ready and not finished
    //     - and its input channel has buffered a lot
    //     - or it's a source node and its output channel is (nearly) empty
    //   then call process() on it
    // deadlock is when a node has not finished yet but none of the others are ready;
    // deadlock should not (i.e. will not) happen with proper scheduling
    //
    int64 onePercentFileSize = 0;
    int64 bytesRead = 0;
    int readPercentage = 0;
    if (monitor)
    {
        onePercentFileSize = totalBytesToBeRead() / 100;
        monitor->beginTask("Executing dataflow network", 100);
    }

    while (true)
    {
        ReaderNode *readerNode = NULL;
        int64 readBefore = 0;
        Node *node = selectNode();
        if (!node)
            break;

        if (monitor)
        {
            if (monitor->isCanceled())
            {
                monitor->done();
                return;
            }
            if (isReaderNode(node))
            {
                readerNode = dynamic_cast<ReaderNode *>(node);
                readBefore = readerNode->getNumReadBytes();
            }
        }

        DBG(("execute: invoking %s\n", node->nodeType()->name()));
        node->process();

        if (monitor)
        {
            if (onePercentFileSize > 0 && readerNode)
            {
                bytesRead += (readerNode->getNumReadBytes() - readBefore);
                int currentPercentage = bytesRead / onePercentFileSize;
                if (currentPercentage > readPercentage)
                {
                    monitor->worked(currentPercentage - readPercentage);
                    readPercentage = currentPercentage;
                }
            }
        }
    }

    if (monitor)
        monitor->done();

    DBG(("execute: processing finished\n"));

    // propagate finished state to all nodes (transitive closure)
    unsigned int i=0;
    while (i<nodes.size())
    {
        if (nodes[i]->alreadyFinished())
            i++;
        else if (!updateNodeFinished(nodes[i]))
            i++;  // if not finished, skip it
        else
            i=0;  // if one node finished, start over (to let it cascade)
    }

    // check all nodes have finished now
    for (i=0; i<nodes.size(); i++)
        if (!nodes[i]->alreadyFinished())
            throw opp_runtime_error("execute: deadlock: no ready nodes but node %s not finished",
                                nodes[i]->nodeType()->name());

    // check all channel buffers are empty
    for (i=0; i<channels.size(); i++)
        if (!channels[i]->eof())
            throw opp_runtime_error("execute: all nodes finished but channel %d not at eof", i);
}

bool DataflowManager::updateNodeFinished(Node *node)
{
    //
    // if node says it's finished():
    // - call consumerClose() on its input channels (they'll ignore futher writes then);
    // - call close() on its output channels
    // - set alreadyFinished() state flag in node, so that we won't keep asking it
    //
    if (!node->finished())
        return false;

    DBG(("DBG: %s finished\n", node->nodeType()->name()));
    node->setAlreadyFinished();
    int nc = channels.size();
    for (int i=0; i!=nc; i++)
    {
        Channel *ch = channels[i];
        if (ch->consumerNode()==node)
        {
            DBG(("DBG:   one input closed\n"));
            ch->consumerClose();
        }
        if (ch->producerNode()==node)
        {
            DBG(("DBG:   one output closed\n"));
            ch->close();
        }
    }
    return true;
}

Node *DataflowManager::selectNode()
{
    // if a channel has buffered too much, try to schedule its consumer node
    int nc = channels.size();
    for (int j=0; j!=nc; j++)
    {
        Channel *ch = channels[j];
        if (ch->length()>threshold && ch->consumerNode()->isReady())
        {
            Node *node = ch->consumerNode();
            Assert(!node->alreadyFinished());
            if (!updateNodeFinished(node))
                return node;
        }
    }

    // round robin scheduling
    int n = nodes.size();
    int i = lastnode;
    assert(n!=0);
    do
    {
        CONTINUE:
            i = (i+1)%n;
            Node *node = nodes[i];
            if (!node->alreadyFinished())
            {
                if (updateNodeFinished(node))
                {
                    // When a node finished, some node might get ready that was
                    // not ready before (e.g. has some buffered data), so start over the loop.
                    i = lastnode;
                    goto CONTINUE;
                }
                else if (node->isReady())
                {
                    if (i==lastnode)
                        DBG(("DBG: %s invoked again -- perhaps its process() doesn't do as much at once as it could?\n", node->nodeType()->name()));
                    lastnode = i;
                    return node;
                }
            }
    }
    while (i!=lastnode);

    return NULL;
}

bool DataflowManager::isReaderNode(Node *node)
{
    return strcmp(node->nodeType()->category(), "reader-node") == 0;
}

int64 DataflowManager::totalBytesToBeRead()
{
    int64 totalFileSize = 0;
    for (int i = 0; i < nodes.size(); ++i)
    {
        if (isReaderNode(nodes[i]))
        {
            ReaderNode *readerNode = dynamic_cast<ReaderNode *>(nodes[i]);
            totalFileSize += readerNode->getFileSize();
        }
    }
    return totalFileSize;
}

void DataflowManager::dump()
{
    printf("DATAFLOW NETWORK:\n");
    int n = nodes.size();
    printf("Nodes (%d):\n", n);
    for (int i=0; i<n; i++)
    {
        Node *node = nodes[i];
        NodeType *nodeType = node->nodeType();
        printf(" node[%d]: %p %s\n", i, node, nodeType->name());
    }

    int nc = channels.size();
    printf("Channels (%d):\n", nc);
    for (int j=0; j<nc; j++)
    {
        Channel *ch = channels[j];
        Node *prodNode = ch->producerNode();
        Node *consNode = ch->consumerNode();
        printf(" channel[%d]: node %p %s --> node %p %s\n", j,
             prodNode, prodNode->nodeType()->name(),
             consNode, consNode->nodeType()->name());
    }
    fflush(stdout);
}

