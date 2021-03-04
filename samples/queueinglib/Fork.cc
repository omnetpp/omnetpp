//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Fork.h"
#include "Job.h"

namespace queueing {

Define_Module(Fork);

void Fork::handleMessage(cMessage *msg)
{
    Job *job = check_and_cast<Job *>(msg);
    int n = gateSize("out");

    // for each output gate
    for (int i = 0; i < n; i++) {
        // generate a child job
        Job *child = job->dup();
        child->makeChildOf(job);

        // give it a better name
        char buf[256];
        sprintf(buf, "%s-%d", job->getName(), i);
        child->setName(buf);

        // then send it out
        send(child, "out", i);
    }

    // Note: we cannot delete the parent job, as it will be needed
    // in the Join module. It'll stay pending in this module until
    // Join takes it from us.
}

Fork::~Fork()
{
    // delete remaining parent jobs
    while (getNumOwnedObjects() > 0)
        delete getOwnedObject(0);
}

}; // namespace
