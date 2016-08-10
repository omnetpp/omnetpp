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

Host::Host()
{
    endTxEvent = nullptr;
}

Host::~Host()
{
    delete lastPacket;
    cancelAndDelete(endTxEvent);
}

void Host::initialize()
{
    stateSignal = registerSignal("state");
    server = getModuleByPath("server");
    if (!server)
        throw cRuntimeError("server not found");

    txRate = par("txRate");
    iaTime = &par("iaTime");
    pkLenBits = &par("pkLenBits");

    slotTime = par("slotTime");
    isSlotted = slotTime > 0;
    WATCH(slotTime);
    WATCH(isSlotted);

    endTxEvent = new cMessage("send/endTx");
    state = IDLE;
    emit(stateSignal, state);
    pkCounter = 0;
    WATCH((int&)state);
    WATCH(pkCounter);

    x = intrand(1000);
    y = intrand(1000);

    double dist = std::sqrt((x - 500) * (x-500) + (y-500) * (y-500));
    radioDelay = lightToReachOneMeter * dist;

    lastTransmission = new cRingFigure();
    lastTransmission->setFillColor(cFigure::GOOD_LIGHT_COLORS[getId() % cFigure::NUM_GOOD_LIGHT_COLORS]);
    lastTransmission->setFillOpacity(0.5);
    lastTransmission->setFilled(true);
    lastTransmission->setVisible(false);
    getParentModule()->getCanvas()->addFigure(lastTransmission);

    scheduleAt(getNextTransmissionTime(), endTxEvent);
}

void Host::handleMessage(cMessage *msg)
{
    ASSERT(msg == endTxEvent);

    if (par("controlAnimationSpeed").boolValue())
        getParentModule()->getCanvas()->setAnimationSpeed(1e-6, this);

    if (state == IDLE) {
        // generate packet and schedule timer when it ends
        char pkname[40];
        sprintf(pkname, "pk-%d-#%d", getId(), pkCounter++);
        EV << "generating packet " << pkname << endl;

        state = TRANSMIT;
        emit(stateSignal, state);

        cPacket *pk = new cPacket(pkname);
        pk->setBitLength(pkLenBits->longValue());
        simtime_t duration = pk->getBitLength() / txRate;
        sendDirect(pk, radioDelay, duration, server->gate("in"));

        delete lastPacket;
        lastPacket = pk->dup();
        lastTransmission->setAssociatedObject(lastPacket);
        lastPacketStarted = simTime();

        scheduleAt(simTime()+duration, endTxEvent);
    }
    else if (state == TRANSMIT) {
        // endTxEvent indicates end of transmission
        state = IDLE;
        emit(stateSignal, state);

        // schedule next sending
        scheduleAt(getNextTransmissionTime(), endTxEvent);
    }
    else {
        throw cRuntimeError("invalid state");
    }
}

simtime_t Host::getNextTransmissionTime()
{
    simtime_t t = simTime() + iaTime->doubleValue();

    if (!isSlotted)
        return t;
    else
        // align time of next transmission to a slot boundary
        return slotTime * ceil(t/slotTime);
}

void Host::refreshDisplay() const
{
    double animSpeed = 0.1;
    if (lastPacket) {
        auto st = simTime();

        auto frontDelta = st - lastPacketStarted;
        auto backDelta = st - (lastPacketStarted + lastPacket->getDuration());

        double frontRadius = std::min(10000.0, frontDelta / lightToReachOneMeter);
        double backRadius = backDelta / lightToReachOneMeter;

        if (backRadius > 10000) {
            lastTransmission->setVisible(false);
            lastTransmission->setAssociatedObject(nullptr);
        }
        else {
            lastTransmission->setVisible(true);
            lastTransmission->setBounds(cFigure::Rectangle(x - frontRadius, y - frontRadius, 2*frontRadius, 2*frontRadius));
            lastTransmission->setInnerRadius(std::max(0.0, backRadius));
        }

        if ((frontRadius >= 0 && frontRadius < 1000) || (backRadius >= 0 && backRadius < 1000))
            animSpeed = 1e-6;
    }
    else
        lastTransmission->setVisible(false);

    if (par("controlAnimationSpeed").boolValue())
        getParentModule()->getCanvas()->setAnimationSpeed(animSpeed, this);

    getDisplayString().setTagArg("p", 0, x);
    getDisplayString().setTagArg("p", 1, y);

    getDisplayString().setTagArg("t", 2, "#808000");

    if (state == IDLE) {
        getDisplayString().setTagArg("i", 1, "");
        getDisplayString().setTagArg("t", 0, "");
    }
    else if (state == TRANSMIT) {
        getDisplayString().setTagArg("i", 1, "yellow");
        getDisplayString().setTagArg("t", 0, "TRANSMIT");
    }
}


}; //namespace
