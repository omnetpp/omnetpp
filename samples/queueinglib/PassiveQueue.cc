//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

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
    droppedVector.setName("dropped jobs");
    lengthVector.setName("length");
    queueingTimeVector.setName("queueing time");
    scalarUtilizationStats.setName("utilization");
    scalarWeightedLengthStats.setName("time weighted length");
    scalarLengthStats.setName("length");
    capacity = par("capacity");
    queue.setName("queue");

    droppedJobs = 0;
    prevEventTimeStamp = 0.0;

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
        droppedVector.record(++droppedJobs);
        delete msg;
        return;
    }

    int k = selectionStrategy->select();
    if (k < 0)
    {
        // enqueue if no idle server found
        queueLengthChanged();
        queue.insert(job);
    }
    else if (length() == 0)
    {
        // send through without queueing
        send(job, "out", k);
    }
    else
        error("This should not happen. Queue is NOT empty and there is an IDLE server attached to us.");

    // statistics
    lengthVector.record(length());

    // change the icon color
    if (ev.isGUI())
        getDisplayString().setTagArg("i",1, queue.empty() ? "" : "cyan3");
}

void PassiveQueue::queueLengthChanged()
{
    scalarUtilizationStats.collect2((length() > 0 ? 1 : 0), simTime()-prevEventTimeStamp);
    scalarWeightedLengthStats.collect2(length(), simTime()-prevEventTimeStamp);
    scalarLengthStats.collect(length());
    prevEventTimeStamp = simTime();
}

int PassiveQueue::length()
{
    return queue.length();
}

void PassiveQueue::request(int gateIndex)
{
    Enter_Method("request()!");

    ASSERT(!queue.empty());

    // gather queuelength statistics
    queueLengthChanged();

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

    job->setQueueCount(job->getQueueCount()+1);
    simtime_t d = simTime() - job->getTimestamp();
    job->setTotalQueueingTime(job->getTotalQueueingTime() + d);
    queueingTimeVector.record(d);

    send(job, "out", gateIndex);

    // statistics
    lengthVector.record(length());

    if (ev.isGUI())
        getDisplayString().setTagArg("i",1, queue.empty() ? "" : "cyan");
}

void PassiveQueue::finish()
{
    recordScalar("min length",scalarLengthStats.getMin());
    recordScalar("max length",scalarLengthStats.getMax());
    recordScalar("avg length",scalarWeightedLengthStats.getMean());
    recordScalar("utilization",scalarUtilizationStats.getMean());
    recordScalar("dropped jobs", droppedJobs);
}

}; //namespace

