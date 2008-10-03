//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "ResourcePool.h"

namespace queueing {

Define_Module(ResourcePool);

std::ostream& operator << (std::ostream& out, const ResourcePool::AllocationRequest& req)
{
    out << req.allocator->getFullPath() << " amount: " << req.amountToAllocate << " priority: " << req.priority;
    return out;
}

ResourcePool::ResourcePool()
{
}

ResourcePool::~ResourcePool()
{
}

void ResourcePool::initialize()
{
    WATCH_LIST(allocatorList);
    WATCH(amount);
    amount = par("amount");
    if (ev.isGUI())
        updateDisplayString();
}

void ResourcePool::finish()
{
}

bool ResourcePool::tryToAllocate(IResourceAllocator *allocator, long amountToAllocate, int priority)
{
    Enter_Method("allocate(%ld): %s", amountToAllocate, amount >= amountToAllocate ? "success" : "fail");
    if (amount >= amountToAllocate) {
        amount -= amountToAllocate;
        if (ev.isGUI())
            updateDisplayString();
        return true;
    }
    else {
        // register the allocator for later notification
        AllocationRequest req;
        req.priority = priority;
        req.amountToAllocate = amountToAllocate;
        req.allocator = allocator;
        add(req);
        if (ev.isGUI())
            updateDisplayString();
        return false;
    }
}

void ResourcePool::release(long amountToRelease)
{
    Enter_Method("release(%ld)", amountToRelease);

    amount += amountToRelease;
    AllocationRequestList::iterator firstIt = allocatorList.begin();
    if (!allocatorList.empty() && firstIt->amountToAllocate <= amount) {
        amount -= firstIt->amountToAllocate;
        firstIt->allocator->resourceGranted(this);
        allocatorList.pop_front();
    }
    if (ev.isGUI())
        updateDisplayString();
}


void ResourcePool::add(AllocationRequest& request)
{
    // insert request into list, observing priority.
    // start from the back and look for the first request where priority is <= than ours,
    // and insert the new request after that one.
    for (AllocationRequestList::reverse_iterator i = allocatorList.rbegin(); i != allocatorList.rend(); i++) {
        if (i->priority <= request.priority) {
            allocatorList.insert(i.base(), request);
            return;
        }
    }

    // if not found, insert at the beginning
    allocatorList.push_front(request);
}

void ResourcePool::updateDisplayString()
{
    char buf[80];
    sprintf(buf, "amount: %ld\nrequests: %d", amount, (int)allocatorList.size());
    getDisplayString().setTagArg("t",0, buf);
}

}; // namespace
