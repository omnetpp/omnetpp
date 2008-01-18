//
// Copyright (C) 2006 Rudolf Hornig
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include <omnetpp.h>
#include "SelectionStrategies.h"
#include "PQueue.h"
#include "Server.h"

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
        return hostModule->gate("in", i)->fromGate();
    else
        return hostModule->gate("out", i)->toGate();
}

bool SelectionStrategy::isSelectable(cModule *module)
{
    PQueue *pqueue = dynamic_cast<PQueue *>(module);
    if (pqueue != NULL)
        return pqueue->length() > 0;

    Server *server = dynamic_cast<Server *>(module);
    if (server != NULL)
        return server->isIdle();

    opp_error("Only PQueue and Server is supported by this Strategy");
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
        if (isSelectable(selectableGate(i)->ownerModule()))
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
        if (isSelectable(selectableGate(i)->ownerModule()))
            noOfSelectables++;

    int rnd = intuniform(1, noOfSelectables);

    for (int i=0; i<gateSize; i++)
        if (isSelectable(selectableGate(i)->ownerModule()) && (--rnd == 0))
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
        if (isSelectable(selectableGate(lastIndex)->ownerModule()))
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
        PQueue *queue = check_and_cast<PQueue *>(selectableGate(i)->ownerModule());
        if (isSelectable(queue) && (queue->length()<sizeMin))
        {
            sizeMin = queue->length();
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
        PQueue *queue = check_and_cast<PQueue *>(selectableGate(i)->ownerModule());
        if (isSelectable(queue) && queue->length()>sizeMax)
        {
            sizeMax = queue->length();
            result = i;
        }
    }
    return result;
}


