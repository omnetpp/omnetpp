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

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "channel.h"
#include "nodetype.h"
#include "dataflowmanager.h"


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
        throw new Exception("connect: source port already connected");
    if (dest->channel())
        throw new Exception("connect: destination port already connected");
    if (!dest->node())
        throw new Exception("connect: port's owner node not filled in");

    Channel *ch = new Channel();
    addChannel(ch);

    src->setChannel(ch);
    dest->setChannel(ch);
    ch->setProducerNode(src->node());
    ch->setConsumerNode(dest->node());
}

// FIXME: validate node attributes

void DataflowManager::execute()
{
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
    while (true)
    {
        Node *node = selectNode();
        if (!node)
        {
            // try once more -- see selectNode()'s code for explanation
            node = selectNode();
            if (!node)
                break;
        }
        DBG(("execute: invoking %s\n", node->nodeType()->name()));
        node->process();
    }
    DBG(("execute: processing finished\n"));

    // propagate finished state to all nodes (transitive closure)
    unsigned int i=0;
    while (i<nodes.size())
    {
        if (nodes[i]->alreadyFinished())
            i++;
        else if (!nodeFinished(nodes[i]))
            i++;  // if not finished, skip it
        else
            i=0;  // if one node finished, start over (to let it cascade)
    }

    // check all nodes have finished now
    for (i=0; i<nodes.size(); i++)
        if (!nodes[i]->alreadyFinished())
            throw new Exception("execute: deadlock: no ready nodes but node %s not finished",
                                nodes[i]->nodeType()->name());

    // check all channel buffers are empty
    for (i=0; i<channels.size(); i++)
        if (!channels[i]->eof())
            throw new Exception("execute: all nodes finished but channel %d not at eof", i);
}

bool DataflowManager::nodeFinished(Node *node)
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
            ASSERT(!node->alreadyFinished());
            if (!nodeFinished(node))
                return node;
        }
    }

    // round robin scheduling
    int n = nodes.size();
    int i = lastnode;
    do
    {
        i = (i+1)%n;
        Node *node = nodes[i];
        if (!node->alreadyFinished() && !nodeFinished(node) && node->isReady())
        {
            if (i==lastnode)
                DBG(("DBG: %s invoked again -- perhaps its process() doesn't do as much at once as it could?\n", node->nodeType()->name()));
            lastnode = i;
            return node;
        }
    }
    while (i!=lastnode);

    // IMPORTANT: caller must try it again once, when it receives NULL!
    // In the while loop above, nodeFinished() may have a side effect
    // of enabling a node (via setting eof() on its input) which we've
    // already tried.
    return NULL;
}

