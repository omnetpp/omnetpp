//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Classifier.h"
#include "Job.h"

namespace queueing {

Define_Module(Classifier);

void Classifier::initialize()
{
    dispatchField = par("dispatchField");
}

void Classifier::handleMessage(cMessage *msg)
{
    Job *job = check_and_cast<Job *>(msg);
    int outGateIndex = -1;
    if (strcmp(dispatchField, "type") == 0)
        outGateIndex = job->getKind();
    else if (strcmp(dispatchField, "priority") == 0)
        outGateIndex = job->getPriority();
    else
        throw cRuntimeError("invalid dispatchField parameter, must be \"type\" or \"priority\"");  // TODO we could look for the value in the dynamically added parameters too

    if (outGateIndex < 0 || outGateIndex >= gateSize("out"))
        send(job, "rest");
    else
        send(job, "out", outGateIndex);
}

}; //namespace

