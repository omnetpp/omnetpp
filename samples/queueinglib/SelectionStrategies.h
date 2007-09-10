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

#ifndef __SELECTIONSTRATEGIES_H
#define __SELECTIONSTRATEGIES_H

#include <omnetpp.h>

/**
 * Selection strategies used in queue, server and router classes to decide
 * which module to choose for further interaction.
 */
class SelectionStrategy : public cObject
{
    protected:
        bool isInputGate;
        int gateSize;        // the size of the gate vector
        cModule *hostModule; // the module using the strategy
    public:
        // on which module's gates should be used for selection
        // if selectOnInGate is true, then we will use "in" gate otherwise "out" is used
        SelectionStrategy(cSimpleModule *module, bool selectOnInGate);
        virtual ~SelectionStrategy();

        static SelectionStrategy * create(const char *algName, cSimpleModule *module, bool selectOnInGate);

        // which gate index the selection strategy selected
        virtual int select() = 0;
        // is this module selectable accordig to the policy? (queue is selectable if not empty, server is selectable if idle)
        virtual bool isSelectable(cModule *module);
        // returns the i-th module's gate which connects to our host module
        cGate *selectableGate(int i);
};

/**
 * Priority based selection. The first selectable index will be returned.
 */
class PrioritySelectionStrategy : public SelectionStrategy
{
    public:
        PrioritySelectionStrategy(cSimpleModule *module, bool selectOnInGate);
        virtual int select();
};

/**
 * Random selection from the selectable modules, with uniform distribution.
 */
class RandomSelectionStrategy : public SelectionStrategy
{
    public:
        RandomSelectionStrategy(cSimpleModule *module, bool selectOnInGate);
        virtual int select();
};

/**
 * Uses Round Robin selection, but skips any module that is not available currently.
 */
class RoundRobinSelectionStrategy : public SelectionStrategy
{
    protected:
        int lastIndex; // the index of the module last time used
    public:
        RoundRobinSelectionStrategy(cSimpleModule *module, bool selectOnInGate);
        virtual int select();
};

/**
 * Chooses the shortest queue. If there are more than one
 * with the same length, it chooses by priority among them.
 * This strategy is for output only (i.e. for router module).
 */
class ShortestQueueSelectionStrategy : public SelectionStrategy
{
    public:
        ShortestQueueSelectionStrategy(cSimpleModule *module, bool selectOnInGate);
        virtual int select();
};

/**
 * Chooses the longest queue (where length>0 of course).
 * Input strategy (for servers).
 */
class LongestQueueSelectionStrategy : public SelectionStrategy
{
    public:
        LongestQueueSelectionStrategy(cSimpleModule *module, bool selectOnInGate);
        virtual int select();
};

#endif

