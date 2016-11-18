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

    x = par("x").doubleValue();
    y = par("y").doubleValue();

    double serverX = server->par("x").doubleValue();
    double serverY = server->par("y").doubleValue();

    idleAnimationSpeed = par("idleAnimationSpeed");
    transmissionEdgeAnimationSpeed = par("transmissionEdgeAnimationSpeed");
    midTransmissionAnimationSpeed = par("midTransmissionAnimationSpeed");

    double dist = std::sqrt((x-serverX) * (x-serverX) + (y-serverY) * (y-serverY));
    radioDelay = dist / propagationSpeed;

    getDisplayString().setTagArg("p", 0, x);
    getDisplayString().setTagArg("p", 1, y);

    scheduleAt(getNextTransmissionTime(), endTxEvent);
}

void Host::handleMessage(cMessage *msg)
{
    ASSERT(msg == endTxEvent);

    getParentModule()->getCanvas()->setAnimationSpeed(transmissionEdgeAnimationSpeed, this);

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

        if (hasGUI() && transmissionRing != nullptr) {
            delete lastPacket;
            lastPacket = pk->dup();
            transmissionRing->setAssociatedObject(lastPacket);
            for (auto c : transmissionCircles)
                c->setAssociatedObject(lastPacket);
        }

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
    const int numRipples = 20;

    if (!transmissionRing) {
        auto color = cFigure::GOOD_DARK_COLORS[getId() % cFigure::NUM_GOOD_DARK_COLORS];

        transmissionRing = new cRingFigure();
        transmissionRing->setFillColor(color);
        transmissionRing->setLineColor(color);
        transmissionRing->setFillOpacity(0.25);
        transmissionRing->setFilled(true);
        transmissionRing->setVisible(false);
        getParentModule()->getCanvas()->addFigure(transmissionRing);

        for (int i = 0; i < numRipples; ++i) {
            auto circle = new cOvalFigure();
            circle->setFilled(false);
            circle->setLineColor(color);
            circle->setLineOpacity(0.75);
            circle->setLineWidth(10);
            circle->setZoomLineWidth(true);
            circle->setVisible(false);
            transmissionCircles.push_back(circle);
            getParentModule()->getCanvas()->addFigure(circle);
        }
    }

    double animSpeed = idleAnimationSpeed;

    if (lastPacket) {
        auto now = simTime();

        auto frontTravelTime = now - lastPacket->getSendingTime();
        auto backTravelTime = now - (lastPacket->getSendingTime() + lastPacket->getDuration());

        // conversion from time to distance in m using speed
        double frontRadius = std::min(maxRadius, frontTravelTime.dbl() * propagationSpeed);
        double backRadius = backTravelTime.dbl() * propagationSpeed;
        double interCircleRadius = rippleRadius / numRipples;

        double opacity = 1.0;
        if (backRadius > maxRadius) {
            transmissionRing->setVisible(false);
            transmissionRing->setAssociatedObject(nullptr);
        }
        else {
            transmissionRing->setVisible(true);
            transmissionRing->setBounds(cFigure::Rectangle(x - frontRadius, y - frontRadius, 2*frontRadius, 2*frontRadius));
            transmissionRing->setInnerRadius(std::max(0.0, std::min(maxRadius, backRadius)));
            if (backRadius > 0)
                opacity = std::max(0.0, 1.0 - backRadius / rippleRadius);
        }

        transmissionRing->setLineOpacity(opacity);
        transmissionRing->setFillOpacity(opacity/5);

        double radius0 = std::fmod(frontTravelTime.dbl() * propagationSpeed, interCircleRadius);
        for (int i = 0; i < (int)transmissionCircles.size(); ++i) {
            double circleRadius = std::min(maxRadius, radius0 + i * interCircleRadius);
            if (circleRadius < frontRadius - interCircleRadius/2 && circleRadius > backRadius + interCircleRadius/2) {
                transmissionCircles[i]->setVisible(true);
                transmissionCircles[i]->setBounds(cFigure::Rectangle(x - circleRadius, y - circleRadius, 2*circleRadius, 2*circleRadius));
                transmissionCircles[i]->setLineOpacity(std::max(0.0, 0.2 - 0.2 * (circleRadius / rippleRadius)));
            }
            else
                transmissionCircles[i]->setVisible(false);
        }

        if ((frontRadius >= 0 && frontRadius < rippleRadius) || (backRadius >= 0 && backRadius < rippleRadius))
            animSpeed = transmissionEdgeAnimationSpeed;
        if (frontRadius > rippleRadius && backRadius < 0)
            animSpeed = midTransmissionAnimationSpeed;
    }
    else {
        transmissionRing->setVisible(false);
        transmissionRing->setAssociatedObject(nullptr);

        for (auto c : transmissionCircles) {
            c->setVisible(false);
            c->setAssociatedObject(nullptr);
        }
    }

    getParentModule()->getCanvas()->setAnimationSpeed(animSpeed, this);

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
