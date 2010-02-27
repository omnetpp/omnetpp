//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "PassiveQueue.h"
#include "Job.h"

namespace queueing {

Define_Module(PassiveQueue);

PassiveQueue::PassiveQueue()
{
    selectionStrategy = NULL;
}

PassiveQueue::~PassiveQueue()
{
    delete selectionStrategy;
}

void PassiveQueue::initialize()
{
    droppedSignal = registerSignal("dropped");
    queueingTimeSignal = registerSignal("queueingTime");
    queueLengthSignal = registerSignal("queueLength");
    emit(queueLengthSignal, 0);

    capacity = par("capacity");
    queue.setName("queue");

    fifo = par("fifo");

    selectionStrategy = SelectionStrategy::create(par("sendingAlgorithm"), this, false);
    if (!selectionStrategy)
        error("invalid selection strategy");
}

void PassiveQueue::handleMessage(cMessage *msg)
{
    Job *job = check_and_cast<Job *>(msg);
    job->setTimestamp();

    // check for container capacity
    if (capacity >=0 && queue.length() >= capacity)
    {
        EV << "Queue full! Job dropped.\n";
        if (ev.isGUI()) bubble("Dropped!");
        emit(droppedSignal, 1);
        delete msg;
        return;
    }

    int k = selectionStrategy->select();
    if (k < 0)
    {
        // enqueue if no idle server found
        queue.insert(job);
        emit(queueLengthSignal, length());
        job->setQueueCount(job->getQueueCount() + 1);
    }
    else if (length() == 0)
    {
        // send through without queueing
        send(job, "out", k);
    }
    else
        error("This should not happen. Queue is NOT empty and there is an IDLE server attached to us.");

    // change the icon color
    if (ev.isGUI())
        getDisplayString().setTagArg("i",1, queue.empty() ? "" : "cyan3");
}

int PassiveQueue::length()
{
    return queue.length();
}

void PassiveQueue::request(int gateIndex)
{
    Enter_Method("request()!");

    ASSERT(!queue.empty());

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
    emit(queueLengthSignal, length());

    job->setQueueCount(job->getQueueCount()+1);
    simtime_t d = simTime() - job->getTimestamp();
    job->setTotalQueueingTime(job->getTotalQueueingTime() + d);
    emit(queueingTimeSignal, d);

    send(job, "out", gateIndex);

    if (ev.isGUI())
        getDisplayString().setTagArg("i",1, queue.empty() ? "" : "cyan");
}

}; //namespace

