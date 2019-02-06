//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "stressduplicate.h"
#include "stress_m.h"

Define_Module(StressDuplicate);

void StressDuplicate::handleMessage(cMessage *msg)
{
    for (int i = par("numberOfDuplicates"); i >= 0; i--) {
        cGate *outGate = gate("out", intrand(gateSize("out")));
        if (outGate->getTransmissionChannel()->isBusy())
            EV << "Output channel is busy, dropping message: " << msg << "\n";
        else {
            EV << "Sending out duplicated message: "  << msg << "\n";
            cMessage *dupMsg = msg->dup();
            dupMsg->setName("Duplicate");
            send(dupMsg, outGate);
        }
    }

    cGate *outGate = gate("out", intrand(gateSize("out")));
    if (outGate->getTransmissionChannel()->isBusy()) {
        EV << "Output channel is busy, dropping message: " << msg << "\n";
        delete msg;
    }
    else {
        EV << "Sending out original message: "  << msg << "\n";
        msg->setName("Original");
        send(msg, outGate);
    }
}

