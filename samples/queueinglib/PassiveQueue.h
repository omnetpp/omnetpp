//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
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
class IPassiveQueue
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

