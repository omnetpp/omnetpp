//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "ResourceBasedQueue.h"
#include "Job.h"

namespace queueing {

Define_Module(ResourceBasedQueue);

ResourceBasedQueue::ResourceBasedQueue()
{
    jobServiced = NULL;
    endServiceMsg = NULL;
    resourceAllocated = false;
}

ResourceBasedQueue::~ResourceBasedQueue()
{
    delete jobServiced;
    cancelAndDelete(endServiceMsg);
}

void ResourceBasedQueue::initialize()
{
    droppedVector.setName("dropped jobs");
    lengthVector.setName("length");
    queueingTimeVector.setName("queueing time");
    scalarUtilizationStats.setName("utilization");
    scalarWeightedLengthStats.setName("time weighted length");
    scalarLengthStats.setName("length");

    endServiceMsg = new cMessage("end-service");
    fifo = par("fifo");
    capacity = par("capacity");
    droppedJobs = 0;
    prevQueueEventTimeStamp = 0.0;
    prevServiceEventTimeStamp = 0.0;
    queue.setName("queue");

    resourceAmount = par("resourceAmount");
    resourcePriority = par("resourcePriority");
    resourcePool = NULL;

    const char *resourceName = par("resourceModuleName");
    cModule *tmpModule = getParentModule()->getModuleByRelativePath(resourceName);
    if (strlen(resourceName) > 0)
        resourcePool = check_and_cast<IResourcePool*>(tmpModule);
}

void ResourceBasedQueue::handleMessage(cMessage *msg)
{
    if (msg==endServiceMsg)
    {
        endService( jobServiced );
        if (!queue.empty() && allocateResource(peek()))
        {
            queueLengthWillChange();
            jobServiced = getFromQueue();
            simtime_t serviceTime = startService( jobServiced );
            scheduleAt( simTime()+serviceTime, endServiceMsg );
        }
        else
        {
            jobServiced = NULL;
            processorStateWillChange();
        }
    }
    else
    {
        Job *job = check_and_cast<Job *>(msg);

        job->setTimestamp();

        if (!jobServiced && queue.isEmpty() && allocateResource(job))
        {
            // processor was idle and the allocation is successful
            arrival( job );
            jobServiced = job;
            processorStateWillChange();
            simtime_t serviceTime = startService( jobServiced );
            scheduleAt( simTime()+serviceTime, endServiceMsg );
        }
        else
        {
            // check for container capacity
            if (capacity >=0 && queue.length() >= capacity)
            {
                EV << "Capacity full! Job dropped.\n";
                if (ev.isGUI()) bubble("Dropped!");
                droppedVector.record(++droppedJobs);
                delete job;
                return;
            }
            arrival( job );
            queueLengthWillChange();
            queue.insert( job );
        }
    }

    lengthVector.record(length());

    if (ev.isGUI()) getDisplayString().setTagArg("i",1, !jobServiced ? "" : "cyan3");
}

Job *ResourceBasedQueue::getFromQueue()
{
    Job *job;
    if (fifo)
    {
        job = (Job *)queue.pop();
    }
    else
    {
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
    return queue.length();
}

void ResourceBasedQueue::queueLengthWillChange()
{
    scalarWeightedLengthStats.collect2(length(), simTime()-prevQueueEventTimeStamp);
    scalarLengthStats.collect(length());
    prevQueueEventTimeStamp = simTime();
}

void ResourceBasedQueue::processorStateWillChange()
{
    scalarUtilizationStats.collect2((jobServiced ? 0 : 1), simTime()-prevServiceEventTimeStamp);
    prevServiceEventTimeStamp = simTime();
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
    queueingTimeVector.record(d);
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

void ResourceBasedQueue::finish()
{
    recordScalar("min length",scalarLengthStats.getMin());
    recordScalar("max length",scalarLengthStats.getMax());
    recordScalar("avg length",scalarWeightedLengthStats.getMean());
    recordScalar("utilization",scalarUtilizationStats.getMean());
    recordScalar("dropped jobs", droppedJobs);
}

bool ResourceBasedQueue::allocateResource(Job *job)
{
    if (resourcePool != NULL)
        return resourceAllocated = resourcePool->tryToAllocate(this, resourceAmount, resourcePriority + job->getPriority());
    else
        return true;
}

void ResourceBasedQueue::releaseResource()
{
    if (resourcePool != NULL) {
        ASSERT2(resourceAllocated, "Attempt to release an unallocated resource");
        resourcePool->release(resourceAmount);
    }
}

bool ResourceBasedQueue::isResourceAllocated() {
    return resourceAllocated || resourcePool == NULL;
}

std::string ResourceBasedQueue::getFullPath() const
{
    return cSimpleModule::getFullPath();
}

void ResourceBasedQueue::resourceGranted(IResourcePool *provider)
{
    Enter_Method("resourceGranted");
    ASSERT2(!jobServiced && !queue.empty(), "Resource granted while the node is busy or the queue is empty");
    resourceAllocated = true;
    // start servicing if the processor is idle and the queue is not empty
    if (!jobServiced && !queue.empty())
    {
        queueLengthWillChange();
        jobServiced = getFromQueue();
        simtime_t serviceTime = startService( jobServiced );
        scheduleAt( simTime()+serviceTime, endServiceMsg );
    }
}

}; //namespace


