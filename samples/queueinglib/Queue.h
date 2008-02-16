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

#ifndef __FIFO_H
#define __FIFO_H

#include <omnetpp.h>

namespace queueing {

class Job;

/**
 * Abstract base class for single-server queues.
 */
class Queue : public cSimpleModule
{
    private:
        cOutVector droppedStats;
        cOutVector lengthStats;
        cOutVector queueingTimeStats;
        cWeightedStdDev scalarWeightedLengthStats;
        cStdDev scalarLengthStats;
        cWeightedStdDev scalarUtilizationStats;

        Job *jobServiced;
        cMessage *endServiceMsg;
        cQueue queue;
        int capacity;
        bool fifo;
        int droppedJobs;

        Job *getFromQueue();
        simtime_t prevQueueEventTimeStamp;   // the timestamp of the last queuelength change
        simtime_t prevServiceEventTimeStamp; // the timestamp of the last service state change
        void queueLengthWillChange();
        void processorStateWillChange();

    public:
        Queue();
        virtual ~Queue();
        int length();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

        // hook functions to (re)define behaviour
        virtual void arrival(Job *job);
        virtual simtime_t startService(Job *job);
        virtual void endService(Job *job);
};

}; //namespace

#endif
