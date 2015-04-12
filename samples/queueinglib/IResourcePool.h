//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_IRESOURCEPOOL_H
#define __QUEUEING_IRESOURCEPOOL_H

#include "QueueingDefs.h"
#include <list>

namespace queueing {

class IResourceAllocator;

/**
 * The interface that should be implemented by resource providers
 */
class QUEUEING_API IResourcePool
{
    public:
        virtual bool tryToAllocate(IResourceAllocator *allocator, long amountToAllocate, int priority) = 0;
        virtual void release(long amountToRelease) = 0;
        virtual ~IResourcePool() {}
};

/**
 * Interface implemented by modules that allocate resources
 */
class QUEUEING_API IResourceAllocator
{
    public:
        // returns true if allocator accepted the resource
        virtual void resourceGranted(IResourcePool *pool) = 0;
        virtual std::string getFullPath() const = 0;
        virtual ~IResourceAllocator() {}
};


}; // namespace

#endif
