//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Classifier.h"

namespace queueing {

Define_Module(Classifier);

void Classifier::initialize()
{
    dispatchField = par("dispatchField");
}

void Classifier::handleMessage(cMessage *msg)
{
    int outGateIndex = -1;
    if (strcmp(dispatchField, "type") == 0)
        outGateIndex = msg->getKind();
    else if (strcmp(dispatchField, "priority") == 0)
        outGateIndex = msg->getPriority();
    else
        error("invalid dispatchField parameter, must be \"type\" or \"priority\"");
    // TODO we could look for the value in the dynamically added parameters too

    if (outGateIndex < 0 || outGateIndex >= gateSize("out"))
        send(msg, "rest");
    else
        send(msg, "out", outGateIndex);
}

}; //namespace

