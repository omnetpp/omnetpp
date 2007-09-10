//
// Copyright (C) 2006 Rudolf Hornig
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#include <omnetpp.h>
#include "Queue.h"
#include "Job_m.h"

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
    droppedStats.setName("dropped jobs");
    lengthStats.setName("length");
    queueingTimeStats.setName("queueing time");
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

        // processor was idle
        if (!jobServiced)
        {
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
                ev << "Capacity full! Job dropped.\n";
                if (ev.isGUI()) bubble("Dropped!");
                droppedStats.record(++droppedJobs);
                delete job;
                return;
            }
            arrival( job );
            queueLengthWillChange();
            queue.insert( job );
        }
    }

    lengthStats.record(length());

    if (ev.isGUI()) displayString().setTagArg("i",1, !jobServiced ? "" : "cyan3");
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
    scalarUtilizationStats.collect2((jobServiced ? 1 : 0), simTime()-prevServiceEventTimeStamp);
    prevServiceEventTimeStamp = simTime();
}

void Queue::arrival(Job *job)
{
    job->setTimestamp();
}

simtime_t Queue::startService(Job *job)
{
    // gather queueing time statistics
    simtime_t d = simTime() - job->timestamp();
    job->setTotalQueueingTime(job->getTotalQueueingTime() + d);
    queueingTimeStats.record(d);
    ev << "Starting service of " << job->name() << endl;
    job->setTimestamp();
    return par("serviceTime").doubleValue();
}

void Queue::endService(Job *job)
{
    ev << "Finishing service of " << job->name() << endl;
    simtime_t d = simTime() - job->timestamp();
    job->setTotalServiceTime(job->getTotalServiceTime() + d);
    send(job, "out");
}

void Queue::finish()
{
    recordScalar("min length",scalarLengthStats.min());
    recordScalar("max length",scalarLengthStats.max());
    recordScalar("avg length",scalarWeightedLengthStats.mean());
    recordScalar("utilization",scalarUtilizationStats.mean());
    recordScalar("dropped jobs", droppedJobs);
}

