//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "ResourceBasedQueue.h"
#include "Job.h"

namespace queueing {

Define_Module(ResourceBasedQueue);

ResourceBasedQueue::~ResourceBasedQueue()
{
    delete jobServiced;
    cancelAndDelete(endServiceMsg);
}

void ResourceBasedQueue::initialize()
{
    droppedSignal = registerSignal("dropped");
    queueingTimeSignal = registerSignal("queueingTime");
    queueLengthSignal = registerSignal("queueLength");
    emit(queueLengthSignal, 0);
    busySignal = registerSignal("busy");
    emit(busySignal, false);

    endServiceMsg = new cMessage("end-service");
    fifo = par("fifo");
    capacity = par("capacity");
    queue.setName("queue");

    resourceAmount = par("resourceAmount");
    resourcePriority = par("resourcePriority");
    resourcePool = nullptr;

    const char *resourceName = par("resourceModuleName");
    if (strlen(resourceName) > 0) {
        cModule *tmpModule = getParentModule()->getModuleByPath(resourceName);
        resourcePool = check_and_cast<IResourcePool *>(tmpModule);
    }
}

void ResourceBasedQueue::handleMessage(cMessage *msg)
{
    if (msg == endServiceMsg) {
        endService(jobServiced);
        if (!queue.isEmpty() && allocateResource(peek())) {
            jobServiced = getFromQueue();
            emit(queueLengthSignal, length());
            simtime_t serviceTime = startService(jobServiced);
            scheduleAt(simTime()+serviceTime, endServiceMsg);
        }
        else {
            jobServiced = nullptr;
            emit(busySignal, false);
        }
    }
    else {
        Job *job = check_and_cast<Job *>(msg);

        arrival(job);

        if (!jobServiced && queue.isEmpty() && allocateResource(job)) {
            // processor was idle and the allocation is successful
            jobServiced = job;
            emit(busySignal, true);
            simtime_t serviceTime = startService(jobServiced);
            scheduleAt(simTime()+serviceTime, endServiceMsg);
        }
        else {
            // check for container capacity
            if (capacity >= 0 && queue.getLength() >= capacity) {
                EV << "Capacity full! Job dropped.\n";
                if (hasGUI())
                    bubble("Dropped!");
                emit(droppedSignal, 1);
                delete job;
                return;
            }
            queue.insert(job);
            emit(queueLengthSignal, length());
            job->setQueueCount(job->getQueueCount() + 1);
        }
    }
}

void ResourceBasedQueue::refreshDisplay() const
{
    getDisplayString().setTagArg("i2", 0, jobServiced ? "status/execute" : "");
    getDisplayString().setTagArg("i", 1, queue.isEmpty() ? "" : "cyan");
}

Job *ResourceBasedQueue::getFromQueue()
{
    Job *job;
    if (fifo) {
        job = (Job *)queue.pop();
    }
    else {
        job = (Job *)queue.back();
        // FIXME this may have bad performance as remove uses linear search
        queue.remove(job);
    }
    return job;
}

// returns the next job without removing it from the queue
Job *ResourceBasedQueue::peek()
{
    return (Job *)(fifo ? queue.front() : queue.back());
}

int ResourceBasedQueue::length()
{
    return queue.getLength();
}

void ResourceBasedQueue::arrival(Job *job)
{
    job->setTimestamp();
}

simtime_t ResourceBasedQueue::startService(Job *job)
{
    // gather queueing time statistics
    simtime_t d = simTime() - job->getTimestamp();
    job->setTotalQueueingTime(job->getTotalQueueingTime() + d);
    emit(queueingTimeSignal, d);
    EV << "Starting service of " << job->getName() << endl;
    job->setTimestamp();
    return par("serviceTime").doubleValue();
}

void ResourceBasedQueue::endService(Job *job)
{
    EV << "Finishing service of " << job->getName() << endl;
    simtime_t d = simTime() - job->getTimestamp();
    job->setTotalServiceTime(job->getTotalServiceTime() + d);
    send(job, "out");
    releaseResource();
}

bool ResourceBasedQueue::allocateResource(Job *job)
{
    if (resourcePool != nullptr)
        return resourceAllocated = resourcePool->tryToAllocate(this, resourceAmount, resourcePriority + job->getPriority());
    else
        return true;
}

void ResourceBasedQueue::releaseResource()
{
    if (resourcePool != nullptr) {
        ASSERT2(resourceAllocated, "Attempt to release an unallocated resource");
        resourcePool->release(resourceAmount);
    }
}

bool ResourceBasedQueue::isResourceAllocated()
{
    return resourceAllocated || resourcePool == nullptr;
}

std::string ResourceBasedQueue::getFullPath() const
{
    return cSimpleModule::getFullPath();
}

void ResourceBasedQueue::resourceGranted(IResourcePool *provider)
{
    Enter_Method("resourceGranted");
    ASSERT2(!jobServiced && !queue.isEmpty(), "Resource granted while the node is busy or the queue is empty");
    resourceAllocated = true;
    // start servicing if the processor is idle and the queue is not empty
    if (!jobServiced && !queue.isEmpty()) {
        jobServiced = getFromQueue();
        emit(queueLengthSignal, length());
        emit(busySignal, true);
        simtime_t serviceTime = startService(jobServiced);
        scheduleAt(simTime()+serviceTime, endServiceMsg);
    }
}

}; //namespace


