//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_ALLOCATE_H
#define __QUEUEING_ALLOCATE_H

#include "QueueingDefs.h"
#include "ResourcePool.h"

namespace queueing {

class Job;

/**
 * Module that reserves resources from a ResourcePool block or any module
 * that implements the IResourcePool interface. See NED file for more info.
 */
class QUEUEING_API Allocate : public cSimpleModule, public IResourceAllocator
{
    private:
        // state
        cQueue queue;

        // parameters
        int capacity;
        bool fifo;
        IResourcePool *resourcePool;
        int resourceAmount;
        int resourcePriority;

        // statistics
		simsignal_t droppedSignal;
		simsignal_t queueLengthSignal;
		simsignal_t queueingTimeSignal;

    public:
        int length();

        // call-back method called from any associated resource
        virtual void resourceGranted(IResourcePool *provider) override;
        virtual std::string getFullPath() const override;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;

        virtual Job *dequeue();
        virtual Job *peek();
        virtual void enqueueOrDrop(Job *job);
        virtual bool allocateResource(Job *job);
};


}; //namespace

#endif
