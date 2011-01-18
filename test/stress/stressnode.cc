//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "stressnode.h"
#include "stress_m.h"

Define_Module(StressNode);

void StressNode::handleMessage(cMessage *msg)
{
    cGate *outGate = gate("out", par("outGateIndex"));
    if (outGate->getTransmissionChannel()->isBusy()) {
        ev << "Output channel is busy, dropping message: " << msg << "\n";
        delete msg;
    }
    else {
        ev << "Sending out message: "  << msg << "\n";;
        msg->setName("Forwarded");
        send(msg, outGate);
    }
}
