//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __RESOURCE_BASED_QUEUE_H
#define __RESOURCE_BASED_QUEUE_H

#include <omnetpp.h>
#include "IResourcePool.h"

namespace queueing {

class Job;

/**
 * Queue that reserves a resource for processing each job. Can request
 * resources from a ResourcePool block or any module that implements the IResourcePool
 * interface.
 */
class ResourceBasedQueue : public cSimpleModule, public IResourceAllocator
{
    private:
        // statistics
        cOutVector droppedVector;
        cOutVector lengthVector;
        cOutVector queueingTimeVector;
        cWeightedStdDev scalarWeightedLengthStats;
        cStdDev scalarLengthStats;
        cWeightedStdDev scalarUtilizationStats;
        int droppedJobs;

        // state
        Job *jobServiced;
        cMessage *endServiceMsg;
        cQueue queue;
        bool resourceAllocated;

        // parameters
        int capacity;
        bool fifo;
        IResourcePool *resourcePool;
        int resourceAmount;
        int resourcePriority;

        Job *getFromQueue();
        Job *peek();

        simtime_t prevQueueEventTimeStamp;   // the time stamp of the last queue length change
        simtime_t prevServiceEventTimeStamp; // the time stamp of the last service state change
        void queueLengthWillChange();
        void processorStateWillChange();

        // resource allocation helpers
        bool allocateResource(Job *job);
        void releaseResource();
        bool isResourceAllocated();
    public:
        ResourceBasedQueue();
        virtual ~ResourceBasedQueue();
        int length();

        // call-back method called from any associated resource
        virtual void resourceGranted(IResourcePool *provider);
        virtual std::string getFullPath() const;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

        // hook functions to (re)define behavior
        virtual void arrival(Job *job);
        virtual simtime_t startService(Job *job);
        virtual void endService(Job *job);
};

}; //namespace

#endif
