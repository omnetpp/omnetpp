//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "stresscapsulate.h"
#include "stress_m.h"

Define_Module(StressCapsulate);

void StressCapsulate::handleMessage(cMessage *msg)
{
    cPacket *packet = check_and_cast<cPacket *>(msg);

    if (packet->getEncapsulatedMsg() && uniform(0, 1) < par("decapsulateProbability").doubleValue()) {
        ev << "Decapsulating message: "  << packet << "\n";;
        cPacket *decapsulatedPacket = packet->decapsulate();
        decapsulatedPacket->setName("Decapsulated");
        delete packet;
        packet = decapsulatedPacket;
    }

    if (uniform(0, 1) < par("encapsulateProbability").doubleValue()) {
        ev << "Encapsulating message: "  << packet << "\n";;
        StressPacket *encapsulatedPacket = new StressPacket();
        encapsulatedPacket->setName("Encapsulated");
        encapsulatedPacket->setBitLength((long)exponential(par("messageLength").doubleValue()));
        encapsulatedPacket->encapsulate(packet);
        packet = encapsulatedPacket;
    }

    cGate *outGate = gate("out", intrand(gateSize("out")));
    if (outGate->getTransmissionChannel()->isBusy()) {
        ev << "Output channel is busy, dropping message: " << packet << "\n";
        delete packet;
    }
    else {
        ev << "Sending out message: "  << packet << "\n";;
        send(packet, outGate);
    }
}
