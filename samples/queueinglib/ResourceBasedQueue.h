//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_RESOURCE_BASED_QUEUE_H
#define __QUEUEING_RESOURCE_BASED_QUEUE_H

#include "QueueingDefs.h"
#include "IResourcePool.h"

namespace queueing {

class Job;

/**
 * Queue that reserves a resource for processing each job. Can request
 * resources from a ResourcePool block or any module that implements the IResourcePool
 * interface.
 */
class QUEUEING_API ResourceBasedQueue : public cSimpleModule, public IResourceAllocator
{
    private:
        // statistics
		simsignal_t droppedSignal;
		simsignal_t queueLengthSignal;
		simsignal_t queueingTimeSignal;
		simsignal_t busySignal;

        // state
        Job *jobServiced = nullptr;
        cMessage *endServiceMsg = nullptr;
        cQueue queue;
        bool resourceAllocated = false;

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
        virtual ~ResourceBasedQueue();
        int length();

        // call-back method called from any associated resource
        virtual void resourceGranted(IResourcePool *provider) override;
        virtual std::string getFullPath() const override;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;

        // hook functions to (re)define behavior
        virtual void arrival(Job *job);
        virtual simtime_t startService(Job *job);
        virtual void endService(Job *job);
};

}; //namespace

#endif
