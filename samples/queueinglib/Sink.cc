//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Sink.h"
#include "Job.h"

namespace queueing {

Define_Module(Sink);

void Sink::initialize()
{
    lifeTimeVector.setName("total lifetime");
    queueingTimeVector.setName("total queueing time");
    queueVector.setName("number of queue nodes visited");
    serviceTimeVector.setName("total service time");
    delayTimeVector.setName("total delay");
    delayVector.setName("number of delay nodes visited");
    generationVector.setName("generation");
    keepJobs = par("keepJobs");
}

void Sink::handleMessage(cMessage *msg) 
{
    Job *job = check_and_cast<Job *>(msg);

    // gather statistics
    lifeTimeVector.record(simTime()- job->getCreationTime());
    queueingTimeVector.record(job->getTotalQueueingTime());
    queueVector.record(job->getQueueCount());
    serviceTimeVector.record(job->getTotalServiceTime());
    delayTimeVector.record(job->getTotalDelayTime());
    delayVector.record(job->getDelayCount());
    generationVector.record(job->getGeneration());

    if (!keepJobs)
        delete msg;
}

void Sink::finish()
{
    // TODO missing scalar statistics
}

}; //namespace

