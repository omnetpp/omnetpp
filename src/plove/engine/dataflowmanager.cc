//=========================================================================
//  DATAFLOWMANAGER.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

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
    int i;
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
        if (!node) break;
        DBG(("execute: invoking %s\n", node->nodeType()->name()));
        node->process();
    }
    DBG(("execute: processing finished\n"));

    // check all nodes have finished now
    for (int i=0; i<nodes.size(); i++)
        if (!nodes[i]->finished())
            throw new Exception("execute: deadlock: no ready nodes but node %s not finished",
                                nodes[i]->nodeType()->name());

    // check all channel buffers are empty
    for (int j=0; j<channels.size(); j++)
        if (!channels[j]->eof())
            throw new Exception("execute: all nodes finished but channel %d not at eof", j);
}

Node *DataflowManager::selectNode()
{
    // FIXME todo:
    // if node says it's finished():
    // - call consumerClose() on its input channels (they'll ignore futher writes then);
    // - call close() on its output channels
    // - set state flag in node to FINISHED (so that we won't need to keep asking it)
    // FIXME todo: remove all close(), closeAtEof(), flush() etc calls from nodes
    //

    // if a channel has buffered too much, try to schedule its consumer node
    int nc = channels.size();
    for (int j=0; j!=nc; j++)
    {
        Channel *ch = channels[j];
        if (ch->length()>threshold && ch->consumerNode()->isReady() && !ch->consumerNode()->finished())
        {
            return channels[j]->consumerNode();
        }
    }

    // round robin scheduling
    int n = nodes.size();
    int i = lastnode;
    do
    {
        i = (i+1)%n;
        Node *node = nodes[i];
        if (node->isReady() && !node->finished())
        {
            lastnode = i;
            return node;
        }
    }
    while (i!=lastnode);
    return NULL;
}

