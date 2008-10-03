//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
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
        long numDropped;
        cOutVector droppedVector;
        cOutVector lengthVector;
        cOutVector queueingTimeVector;

    public:
        Allocate();
        virtual ~Allocate();
        int length();

        // call-back method called from any associated resource
        virtual void resourceGranted(IResourcePool *provider);
        virtual std::string getFullPath() const;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

        virtual Job *dequeue();
        virtual Job *peek();
        virtual void enqueueOrDrop(Job *job);
        virtual bool allocateResource(Job *job);
};


}; //namespace

#endif
