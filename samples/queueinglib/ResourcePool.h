//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __RESOURCE_POOL_H__
#define __RESOURCE_POOL_H__

#include <omnetpp.h>
#include <list>

namespace queueing {

class IResourcePool;

/**
 * Interface implemented by modules that allocate resources
 */
class IResourceAllocator
{
    public:
        // returns true if allocator accepted the resource
        virtual void resourceGranted(IResourcePool *pool) = 0;
        virtual std::string getFullPath() const = 0;
        virtual ~IResourceAllocator() {}
};

/**
 * The interface that should be implemented by resource providers
 */
class IResourcePool
{
    public:
        virtual bool tryToAllocate(IResourceAllocator *allocator, long amountToAllocate, int priority) = 0;
        virtual void release(long amountToRelease) = 0;
        virtual ~IResourcePool() {}
};


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
