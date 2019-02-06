//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "stresssink.h"
#include "stress_m.h"

Define_Module(StressSink);

void StressSink::handleMessage(cMessage *msg)
{
    EV << "Deleting message: "  << msg << "\n";
    delete msg;
}

