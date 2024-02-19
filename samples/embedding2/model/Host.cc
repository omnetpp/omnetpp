//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Host.h"

namespace aloha {
Define_Module(Host);

Host::~Host()
{
    cancelAndDelete(endTxEvent);
}

void Host::initialize()
{
    server = getModuleByPath("server");

    txRate = par("txRate");
    radioDelay = par("radioDelay");
    iaTime = &par("iaTime");
    pkLenBits = &par("pkLenBits");

    slotTime = par("slotTime");
    isSlotted = slotTime > 0;
    WATCH(slotTime);
    WATCH(isSlotted);

    endTxEvent = new cMessage("send/endTx");
    state = IDLE;
    pkCounter = 0;
    WATCH((int&)state);
    WATCH(pkCounter);

    scheduleAt(getNextTransmissionTime(), endTxEvent);
}

void Host::handleMessage(cMessage *msg)
{
    ASSERT(msg == endTxEvent);

    if (state == IDLE) {
        // generate packet and schedule timer when it ends
        char pkname[40];
        snprintf(pkname, sizeof(pkname), "pk-%d-#%d", getId(), pkCounter++);
        EV << "generating packet " << pkname << endl;

        state = TRANSMIT;

        cPacket *pk = new cPacket(pkname);
        pk->setBitLength(pkLenBits->intValue());
        simtime_t duration = pk->getBitLength() / txRate;
        sendDirect(pk, radioDelay, duration, server->gate("in"));

        scheduleAt(simTime()+duration, endTxEvent);
    }
    else if (state == TRANSMIT) {
        // endTxEvent indicates end of transmission
        state = IDLE;

        // schedule next sending
        scheduleAt(getNextTransmissionTime(), endTxEvent);

    }
    else {
        throw cRuntimeError("invalid state");
    }
}

simtime_t Host::getNextTransmissionTime()
{
    simtime_t t = simTime()+iaTime->doubleValue();

    if (!isSlotted)
        return t;
    else
        // align time of next transmission to a slot boundary
        return slotTime * ceil(t/slotTime);
}

}; //namespace
