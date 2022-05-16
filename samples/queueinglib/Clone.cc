//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <cstdio>
#include "Clone.h"
#include "Job.h"

namespace queueing {

Define_Module(Clone);

void Clone::initialize()
{
    changeMsgNames = par("changeMsgNames");
}

void Clone::handleMessage(cMessage *msg)
{
    Job *job = check_and_cast<Job *>(msg);

    // increment the generation counter
    job->setGeneration(job->getGeneration()+1);

    int n = gateSize("out");
    if (n == 0) {
        // no output gate: drop
        delete job;
    }
    else {
        // send a copy on each gate but the last, and the original on the last gate
        for (int i = 0; i < n; i++) {
            Job *job2 = (i == n-1) ? job : job->dup();
            if (changeMsgNames)
                updateJobName(job2, i);
            send(job2, "out", i);
        }
    }
}

void Clone::updateJobName(Job *job, int i)
{
    char buf[80];
    snprintf(buf, sizeof(buf), "%.70s.%d", job->getName(), i);
    job->setName(buf);
}

}; //namespace

