//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Delay.h"
#include "Job.h"

namespace queueing {

Define_Module(Delay);

void Delay::initialize()
{
    currentlyStored = 0;
    delayedJobsSignal = registerSignal("delayedJobs");
    emit(delayedJobsSignal, 0);
    WATCH(currentlyStored);
}

void Delay::handleMessage(cMessage *msg)
{
    Job *job = check_and_cast<Job *>(msg);

    if (!job->isSelfMessage()) {
        // if it is not a self-message, send it to ourselves with a delay
        currentlyStored++;
        double delay = par("delay");
        scheduleAt(simTime() + delay, job);
    }
    else {
        job->setDelayCount(job->getDelayCount()+1);
        simtime_t d = simTime() - job->getSendingTime();
        job->setTotalDelayTime(job->getTotalDelayTime() + d);

        // if it was a self message (ie. we have already delayed) so we send it out
        currentlyStored--;
        send(job, "out");
    }
    emit(delayedJobsSignal, currentlyStored);

}

void Delay::refreshDisplay() const
{
    getDisplayString().setTagArg("i", 1, currentlyStored == 0 ? "" : "cyan");
}


}; //namespace

