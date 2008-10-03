//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Source.h"
#include "Job.h"

namespace queueing {

Define_Module(Source);

Source::~Source()
{
    cancelAndDelete(selfMsg);
}

void Source::initialize()
{
    jobCounter = 0;
    startTime =  par("startTime");
    stopTime =  par("stopTime");
    numJobs =  par("numJobs");

    // if empty, use the module name as the default for job names
    jobName = par("jobName");
    if (strcmp(jobName, "") == 0)
        jobName = getName();

    // schedule the first message timer for start time
    selfMsg = new cMessage("newJobTimer");
    scheduleAt(startTime, selfMsg);
}

void Source::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()
        && (numJobs < 0 || numJobs > jobCounter)
        && (stopTime < 0 || stopTime > simTime()))
    {
        // reschedule the timer for the next message
        scheduleAt(simTime() + par("interArrivalTime").doubleValue(), msg);

        // create a new job to be sent
        Job *job = new Job();
        char buff[80];
        sprintf(buff, "%.60s %d", jobName, ++jobCounter);
        job->setName(buff);
        job->setKind(par("jobType"));
        job->setPriority(par("jobPriority"));
        send(job, "out");
    }
}

void Source::finish()
{
}

}; //namespace

