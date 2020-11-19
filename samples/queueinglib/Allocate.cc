//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Allocate.h"
#include "Job.h"

namespace queueing {

Define_Module(Allocate);

void Allocate::initialize()
{
    droppedSignal = registerSignal("dropped");
    queueingTimeSignal = registerSignal("queueingTime");
    queueLengthSignal = registerSignal("queueLength");
    emit(queueLengthSignal, 0l);

    fifo = par("fifo");
    capacity = par("capacity");
    queue.setName("queue");

    resourceAmount = par("resourceAmount");
    resourcePriority = par("resourcePriority");

    const char *resourceName = par("resourceModuleName");
    cModule *mod = getParentModule()->getModuleByPath(resourceName);
    resourcePool = check_and_cast<IResourcePool *>(mod);
}

void Allocate::handleMessage(cMessage *msg)
{
    Job *job = check_and_cast<Job *>(msg);
    if (queue.isEmpty() && allocateResource(job))
        send(job, "out");
    else
        enqueueOrDrop(job);
}

bool Allocate::allocateResource(Job *job)
{
    return resourcePool->tryToAllocate(this, resourceAmount, resourcePriority + job->getPriority());
}

std::string Allocate::getFullPath() const
{
    return cSimpleModule::getFullPath();
}

void Allocate::resourceGranted(IResourcePool *provider)
{
    Enter_Method("resourceGranted");

    // send out job for which resource was granted
    ASSERT2(!queue.isEmpty(), "Resource granted while no jobs are waiting");
    Job *job = dequeue();
    send(job, "out");

    // try to handle other waiting jobs as well
    while (!queue.isEmpty() && allocateResource(peek())) {
        Job *job = dequeue();
        send(job, "out");
    }
}

Job *Allocate::peek()
{
    return fifo ? (Job *)queue.front() : (Job *)queue.back();
}

Job *Allocate::dequeue()
{
    Job *job;
    if (fifo) {
        job = (Job *)queue.pop();
    }
    else {
        job = (Job *)queue.back();
        queue.remove(job);
    }
    emit(queueLengthSignal, queue.getLength());

    simtime_t dt = simTime() - job->getTimestamp();
    job->setTotalQueueingTime(job->getTotalQueueingTime() + dt);
    emit(queueingTimeSignal, dt);

    return job;
}

void Allocate::enqueueOrDrop(Job *job)
{
    // check for container capacity
    if (capacity >= 0 && queue.getLength() >= capacity) {
        EV << "Capacity full! Job dropped.\n";
        if (hasGUI())
            bubble("Dropped!");
        emit(droppedSignal, 1);
        delete job;
        return;
    }
    else {
        EV << "Job enqueued.\n";
        job->setTimestamp();
        queue.insert(job);
        emit(queueLengthSignal, queue.getLength());
    }
}

}; //namespace


