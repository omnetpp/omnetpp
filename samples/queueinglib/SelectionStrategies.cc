//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "SelectionStrategies.h"
#include "PassiveQueue.h"
#include "Server.h"

namespace queueing {

SelectionStrategy::SelectionStrategy(cSimpleModule *module, bool selectOnInGate)
{
    hostModule = module;
    isInputGate = selectOnInGate;
    gateSize = isInputGate ? hostModule->gateSize("in") : hostModule->gateSize("out");
}

SelectionStrategy::~SelectionStrategy()
{
}

SelectionStrategy *SelectionStrategy::create(const char *algName, cSimpleModule *module, bool selectOnInGate)
{
    SelectionStrategy *strategy = NULL;

    if (strcmp(algName, "priority") == 0) {
        strategy = new PrioritySelectionStrategy(module, selectOnInGate);
    } else if (strcmp(algName, "random") == 0) {
        strategy = new RandomSelectionStrategy(module, selectOnInGate);
    } else if (strcmp(algName, "roundRobin") == 0) {
        strategy = new RoundRobinSelectionStrategy(module, selectOnInGate);
    } else if (strcmp(algName, "shortestQueue") == 0) {
        strategy = new ShortestQueueSelectionStrategy(module, selectOnInGate);
    } else if (strcmp(algName, "longestQueue") == 0) {
        strategy = new LongestQueueSelectionStrategy(module, selectOnInGate);
    }

    return strategy;
}

cGate *SelectionStrategy::selectableGate(int i)
{
    if (isInputGate)
        return hostModule->gate("in", i)->getPreviousGate();
    else
        return hostModule->gate("out", i)->getNextGate();
}

bool SelectionStrategy::isSelectable(cModule *module)
{
    IPassiveQueue *pqueue = dynamic_cast<IPassiveQueue *>(module);
    if (pqueue != NULL)
        return pqueue->length() > 0;

    IServer *server = dynamic_cast<IServer *>(module);
    if (server != NULL)
        return server->isIdle();

    opp_error("Only IPassiveQueue and IServer is supported by this Strategy");
    return true;
}

// --------------------------------------------------------------------------------------------

PrioritySelectionStrategy::PrioritySelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    SelectionStrategy(module, selectOnInGate)
{
}

int PrioritySelectionStrategy::select()
{
    // return the smallest selectable index
    for (int i=0; i<gateSize; i++)
        if (isSelectable(selectableGate(i)->getOwnerModule()))
            return i;

    // if none of them is selectable return an invalid no.
    return -1;
}

// --------------------------------------------------------------------------------------------

RandomSelectionStrategy::RandomSelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    SelectionStrategy(module, selectOnInGate)
{
}

int RandomSelectionStrategy::select()
{
    // return the smallest selectable index
    int noOfSelectables = 0;
    for (int i=0; i<gateSize; i++)
        if (isSelectable(selectableGate(i)->getOwnerModule()))
            noOfSelectables++;

    int rnd = intuniform(1, noOfSelectables);

    for (int i=0; i<gateSize; i++)
        if (isSelectable(selectableGate(i)->getOwnerModule()) && (--rnd == 0))
            return i;
    return -1;
}

// --------------------------------------------------------------------------------------------

RoundRobinSelectionStrategy::RoundRobinSelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    SelectionStrategy(module, selectOnInGate)
{
    lastIndex = -1;
}

int RoundRobinSelectionStrategy::select()
{
    // return the smallest selectable index
    for (int i = 0; i<gateSize; ++i)
    {
        lastIndex = (lastIndex+1) % gateSize;
        if (isSelectable(selectableGate(lastIndex)->getOwnerModule()))
            return lastIndex;
    }

    // if none of them is selectable return an invalid no.
    return -1;
}

// --------------------------------------------------------------------------------------------

ShortestQueueSelectionStrategy::ShortestQueueSelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    SelectionStrategy(module, selectOnInGate)
{
}

int ShortestQueueSelectionStrategy::select()
{
    // return the smallest selectable index
    int result = -1;            // by default none of them is selectable
    int sizeMin = INT_MAX;
    for (int i = 0; i<gateSize; ++i)
    {
        cModule *module = selectableGate(i)->getOwnerModule();
        int length = (check_and_cast<IPassiveQueue *>(module))->length();;
        if (isSelectable(module) && (length<sizeMin))
        {
            sizeMin = length;
            result = i;
        }
    }
    return result;
}

// --------------------------------------------------------------------------------------------

LongestQueueSelectionStrategy::LongestQueueSelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    SelectionStrategy(module, selectOnInGate)
{
}

int LongestQueueSelectionStrategy::select()
{
    // return the longest selectable queue
    int result = -1;            // by default none of them is selectable
    int sizeMax = -1;
    for (int i = 0; i<gateSize; ++i)
    {
        cModule *module = selectableGate(i)->getOwnerModule();
        int length = (check_and_cast<IPassiveQueue *>(module))->length();;
        if (isSelectable(module) && length>sizeMax)
        {
            sizeMax = length;
            result = i;
        }
    }
    return result;
}

}; //namespace

