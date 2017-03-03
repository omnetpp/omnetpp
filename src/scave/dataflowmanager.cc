//=========================================================================
//  DATAFLOWMANAGER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include "channel.h"
#include "nodetype.h"
#include "commonnodes.h"
#include "dataflowmanager.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

DataflowManager::DataflowManager()
{
    lastNode = 0;
    threshold = 1000;
}

DataflowManager::~DataflowManager()
{
    unsigned int i;
    for (i = 0; i < nodes.size(); i++)
        delete nodes[i];
    for (i = 0; i < channels.size(); i++)
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
    if (src->getChannel())
        throw opp_runtime_error("connect: Source port already connected");
    if (dest->getChannel())
        throw opp_runtime_error("connect: Destination port already connected");
    if (!dest->getNode())
        throw opp_runtime_error("connect: Port's owner node not filled in");

    Channel *ch = new Channel();
    addChannel(ch);

    src->setChannel(ch);
    dest->setChannel(ch);
    ch->setProducerNode(src->getNode());
    ch->setConsumerNode(dest->getNode());
}

// FIXME: validate node attributes

void DataflowManager::execute(IProgressMonitor *monitor)
{
    if (nodes.empty())
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
    int64_t onePercentFileSize = 0;
    int64_t bytesRead = 0;
    int readPercentage = 0;
    if (monitor) {
        onePercentFileSize = getTotalBytesToBeRead() / 100;
        monitor->beginTask("Executing dataflow network", 100);
    }

    while (true) {
        ReaderNode *readerNode = nullptr;
        int64_t readBefore = 0;
        Node *node = selectNode();
        if (!node)
            break;

        if (monitor) {
            if (monitor->isCanceled()) {
                monitor->done();
                return;
            }
            if (isReaderNode(node)) {
                readerNode = dynamic_cast<ReaderNode *>(node);
                readBefore = readerNode->getNumReadBytes();
            }
        }

        DBG(("execute: invoking %s\n", node->getNodeType()->getName()));
        node->process();

        if (monitor) {
            if (onePercentFileSize > 0 && readerNode) {
                bytesRead += (readerNode->getNumReadBytes() - readBefore);
                int currentPercentage = bytesRead / onePercentFileSize;
                if (currentPercentage > readPercentage) {
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
    unsigned int i = 0;
    while (i < nodes.size()) {
        if (nodes[i]->getAlreadyFinished())
            i++;
        else if (!updateNodeFinished(nodes[i]))
            i++;  // if not finished, skip it
        else
            i = 0;  // if one node finished, start over (to let it cascade)
    }

    // check all nodes have finished now
    for (i = 0; i < nodes.size(); i++)
        if (!nodes[i]->getAlreadyFinished())
            throw opp_runtime_error("execute: Deadlock: No ready nodes but node %s not finished",
                    nodes[i]->getNodeType()->getName());


    // check all channel buffers are empty
    for (i = 0; i < channels.size(); i++)
        if (!channels[i]->eof())
            throw opp_runtime_error("execute: All nodes finished but channel %d not at eof", i);

}

bool DataflowManager::updateNodeFinished(Node *node)
{
    //
    // if node says it's isFinished():
    // - call consumerClose() on its input channels (they'll ignore futher writes then);
    // - call close() on its output channels
    // - set getAlreadyFinished() state flag in node, so that we won't keep asking it
    //
    if (!node->isFinished())
        return false;

    DBG(("DBG: %s finished\n", node->getNodeType()->getName()));
    node->setAlreadyFinished();
    int nc = channels.size();
    for (int i = 0; i != nc; i++) {
        Channel *ch = channels[i];
        if (ch->getConsumerNode() == node) {
            DBG(("DBG:   one input closed\n"));
            ch->consumerClose();
        }
        if (ch->getProducerNode() == node) {
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
    for (int j = 0; j != nc; j++) {
        Channel *ch = channels[j];
        if (ch->length() > threshold && ch->getConsumerNode()->isReady()) {
            Node *node = ch->getConsumerNode();
            Assert(!node->getAlreadyFinished());
            if (!updateNodeFinished(node))
                return node;
        }
    }

    // round robin scheduling
    int n = nodes.size();
    int i = lastNode;
    assert(n != 0);
    do {
      CONTINUE:
        i = (i+1)%n;
        Node *node = nodes[i];
        if (!node->getAlreadyFinished()) {
            if (updateNodeFinished(node)) {
                // When a node finished, some node might get ready that was
                // not ready before (e.g. has some buffered data), so start over the loop.
                i = lastNode;
                goto CONTINUE;
            }
            else if (node->isReady()) {
                if (i == lastNode)
                    DBG(("DBG: %s invoked again -- perhaps its process() doesn't do as much at once as it could?\n", node->getNodeType()->getName()));
                lastNode = i;
                return node;
            }
        }
    } while (i != lastNode);

    return nullptr;
}

bool DataflowManager::isReaderNode(Node *node)
{
    return strcmp(node->getNodeType()->getCategory(), "reader-node") == 0;
}

int64_t DataflowManager::getTotalBytesToBeRead()
{
    int64_t totalFileSize = 0;
    for (int i = 0; i < (int)nodes.size(); ++i) {
        if (isReaderNode(nodes[i])) {
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
    for (int i = 0; i < n; i++) {
        Node *node = nodes[i];
        NodeType *nodeType = node->getNodeType();
        printf(" node[%d]: %p %s\n", i, node, nodeType->getName());
    }

    int nc = channels.size();
    printf("Channels (%d):\n", nc);
    for (int j = 0; j < nc; j++) {
        Channel *ch = channels[j];
        Node *prodNode = ch->getProducerNode();
        Node *consNode = ch->getConsumerNode();
        printf(" channel[%d]: node %p %s --> node %p %s\n", j,
                prodNode, prodNode->getNodeType()->getName(),
                consNode, consNode->getNodeType()->getName());
    }
    fflush(stdout);
}

}  // namespace scave
}  // namespace omnetpp

