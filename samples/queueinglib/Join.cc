//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Join.h"

namespace queueing {

Define_Module(Join);

Join::~Join()
{
    while (!jobsHeld.empty()) {
        delete jobsHeld.front();
        jobsHeld.pop_front();
    }
}

void Join::initialize()
{
    WATCH_PTRLIST(jobsHeld);
}

void Join::handleMessage(cMessage *msg)
{
    // store job
    Job *job = check_and_cast<Job *>(msg);
    jobsHeld.push_back(job);

    Job *parent = job->getParent();
    ASSERT(parent != nullptr);  // still exists

    // count all sub-jobs with this parent
    int subJobsHeld = 0;
    for (std::list<Job *>::iterator it = jobsHeld.begin(); it != jobsHeld.end(); it++)
        if ((*it)->getParent() == parent)
            subJobsHeld++;


    // if we have all, destroy them, then obtain the parent and send it out
    if (subJobsHeld == parent->getNumChildren()) {
        take(parent);
        for (std::list<Job *>::iterator it = jobsHeld.begin(); it != jobsHeld.end();  /*nop*/) {
            if ((*it)->getParent() != parent)
                ++it;
            else {
                std::list<Job *>::iterator tmp = it++;
                delete (*tmp);
                jobsHeld.erase(tmp);
            }
        }
        send(parent, "out");
    }
}

void Join::refreshDisplay() const
{
    getDisplayString().setTagArg("i2", 0, jobsHeld.size()>0 ? "status/hourglass" : "");
}


}; // namespace

