//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Job.h"
#include "JobList.h"

namespace queueing {

Define_Module(JobList);

JobList *JobList::defaultInstance = nullptr;

JobList::JobList()
{
    if (defaultInstance == nullptr)
        defaultInstance = this;
}

JobList::~JobList()
{
    if (defaultInstance == this)
        defaultInstance = nullptr;
    for (std::set<Job *>::iterator it = jobs.begin(); it != jobs.end(); ++it)
        (*it)->jobList = nullptr;
}

void JobList::initialize()
{
    WATCH_PTRSET(jobs);
}

void JobList::handleMessage(cMessage *msg)
{
    throw cRuntimeError("this module does not process messages");
}

void JobList::registerJob(Job *job)
{
    jobs.insert(job);
}

void JobList::deregisterJob(Job *job)
{
    std::set<Job *>::iterator it = jobs.find(job);
    ASSERT(it != jobs.end());
    jobs.erase(it);
}

JobList *JobList::getDefaultInstance()
{
    return defaultInstance;
}

const std::set<Job *> JobList::getJobs()
{
    return jobs;
}

}; // namespace
