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

#ifndef __PASSIVE_QUEUE_H
#define __PASSIVE_QUEUE_H

#include <omnetpp.h>
#include "SelectionStrategies.h"

namespace queueing {

class SelectionStrategy;

/**
 * The following interface must be implemented by a queue which can't process
 * jobs on its own. A server process uses these methods to query for new jobs
 * once it becomes idle.
 */
class IPassiveQueue : public ISelectable
{
	public:
		virtual ~IPassiveQueue() { };
		// the current length of the queue
		virtual int length() = 0;
		// requests the queue to send out the next job on its "gateIndex" gate.
		virtual void request(int gateIndex) = 0;
};

/**
 * A passive queue, designed to co-operate with IServer using method calls.
 */
class PassiveQueue : public cSimpleModule, public IPassiveQueue
{
    private:
        cOutVector droppedVector;
        cOutVector lengthVector;
        cOutVector queueingTimeVector;
        cWeightedStdDev scalarWeightedLengthStats;
        cStdDev scalarLengthStats;
        cWeightedStdDev scalarUtilizationStats;

        bool fifo;
        int capacity;
        int droppedJobs;
        cQueue queue;
        SelectionStrategy *selectionStrategy;
        simtime_t prevEventTimeStamp;               // the timestamp of the last queuelength change

        void queueLengthChanged();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

    public:
        PassiveQueue();
        virtual ~PassiveQueue();
        // The following methods are called from IServer:
        virtual int length();
        virtual void request(int gateIndex);
};

}; //namespace

#endif

