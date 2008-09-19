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

#include "PassiveQueue.h"
#include "Server.h"
#include "Job.h"
#include "SelectionStrategies.h"

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
    droppedStats.setName("dropped jobs");
    lengthStats.setName("length");
    queueingTimeStats.setName("queueing time");
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
        droppedStats.record(++droppedJobs);
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
    lengthStats.record(length());

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
    queueingTimeStats.record(d);

    send(job, "out", gateIndex);

    // statistics
    lengthStats.record(length());

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

