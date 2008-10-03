//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_RESOURCEPOOL_H__
#define __QUEUEING_RESOURCEPOOL_H__

#include <list>
#include "QueueingDefs.h"
#include "IResourcePool.h"

namespace queueing {

/**
 * A resource representation. Can represent any amount from a single type of resource.
 * Cooperates with ResourceBasedQueues which allocate from this resource. This module do not
 * receive any messages. Resource is allocated/released by direct Method calls
 */
class ResourcePool : public cSimpleModule, public IResourcePool
{
    public:
        struct AllocationRequest {
            IResourceAllocator *allocator;
            long amountToAllocate;
            int priority;
        };
    private:
        long amount;          // the amount of resource currently available
        typedef std::list<AllocationRequest> AllocationRequestList;
        AllocationRequestList allocatorList;

        void add(AllocationRequest& request);
        void updateDisplayString();

    protected:
        virtual void initialize();
        virtual void finish();

    public:
        ResourcePool();
        virtual ~ResourcePool();
        virtual bool tryToAllocate(IResourceAllocator *allocator, long amountToAllocate, int priority);
        virtual void release(long amountToRelease);
};

}; // namespace

#endif


