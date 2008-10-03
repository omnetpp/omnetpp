//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

#include "Queue.h"
#include "Job.h"

namespace queueing {

Define_Module(Queue);

Queue::Queue()
{
    jobServiced = NULL;
    endServiceMsg = NULL;
}

Queue::~Queue()
{
    delete jobServiced;
    cancelAndDelete(endServiceMsg);
}

void Queue::initialize()
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
}

void Queue::handleMessage(cMessage *msg)
{
    if (msg==endServiceMsg)
    {
        endService( jobServiced );
        if (queue.empty())
        {
            jobServiced = NULL;
            processorStateWillChange();
        }
        else
        {
            queueLengthWillChange();
            jobServiced = getFromQueue();
            simtime_t serviceTime = startService( jobServiced );
            scheduleAt( simTime()+serviceTime, endServiceMsg );
        }
    }
    else
    {
        Job *job = check_and_cast<Job *>(msg);

        job->setTimestamp();

        if (!jobServiced)
        {
        // processor was idle
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

Job *Queue::getFromQueue()
{
    Job *job;
    if (fifo)
    {
        job = (Job *)queue.pop();
    }
    else
    {
        job = (Job *)queue.back();
        // FIXME this may have bad performance as remove uses linear serch
        queue.remove(job);
    }
    return job;
}

int Queue::length()
{
    return queue.length();
}

void Queue::queueLengthWillChange()
{
    scalarWeightedLengthStats.collect2(length(), simTime()-prevQueueEventTimeStamp);
    scalarLengthStats.collect(length());
    prevQueueEventTimeStamp = simTime();
}

void Queue::processorStateWillChange()
{
    scalarUtilizationStats.collect2((jobServiced ? 0 : 1), simTime()-prevServiceEventTimeStamp);
    prevServiceEventTimeStamp = simTime();
}

void Queue::arrival(Job *job)
{
    job->setTimestamp();
}

simtime_t Queue::startService(Job *job)
{
    // gather queueing time statistics
    simtime_t d = simTime() - job->getTimestamp();
    job->setTotalQueueingTime(job->getTotalQueueingTime() + d);
    queueingTimeVector.record(d);
    EV << "Starting service of " << job->getName() << endl;
    job->setTimestamp();
    return par("serviceTime").doubleValue();
}

void Queue::endService(Job *job)
{
    EV << "Finishing service of " << job->getName() << endl;
    simtime_t d = simTime() - job->getTimestamp();
    job->setTotalServiceTime(job->getTotalServiceTime() + d);
    send(job, "out");
}

void Queue::finish()
{
    recordScalar("min length",scalarLengthStats.getMin());
    recordScalar("max length",scalarLengthStats.getMax());
    recordScalar("avg length",scalarWeightedLengthStats.getMean());
    recordScalar("utilization",scalarUtilizationStats.getMean());
    recordScalar("dropped jobs", droppedJobs);
}

}; //namespace

