//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUE_H
#define __QUEUE_H

#include <omnetpp.h>

namespace queueing {

class Job;

/**
 * Abstract base class for single-server queues.
 */
class Queue : public cSimpleModule
{
    private:
        cOutVector droppedVector;
        cOutVector lengthVector;
        cOutVector queueingTimeVector;
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
