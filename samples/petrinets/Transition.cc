//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <algorithm>
#include "Transition.h"
#include "IPlace.h"

Define_Module(Transition);

simsignal_t Transition::firingSignal = cComponent::registerSignal("firing");

Transition::~Transition()
{
    transitionScheduler = dynamic_cast<TransitionScheduler*>(findModuleByPath(par("transitionSchedulerModule").stringValue()));
    if (transitionScheduler) // still exists
        transitionScheduler->deregisterTransition(this);
    cancelAndDelete(fireEvent);
    cancelAndDelete(endFireEvent);
    cancelAndDelete(endFire2Event);
}

void Transition::initialize()
{
    animating = par("animation");
    transitionTimePar = &par("transitionTime");

    fireEvent = new cMessage("fire");
    endFireEvent = new cMessage("endFire");
    endFire2Event = new cMessage("endFire2");

    discoverNeighbours();

    transitionScheduler = check_and_cast<TransitionScheduler*>(getModuleByPath(par("transitionSchedulerModule").stringValue()));
    transitionScheduler->registerTransition(this);

    if (animating)
        canvas = getParentModule()->getCanvas();
}

void Transition::discoverNeighbours()
{
    inputPlaces.clear();
    outputPlaces.clear();

    int numInputs = gateSize("in");
    inputPlaces.reserve(numInputs);
    for (int i = 0; i < numInputs; i++) {
        cGate *g = gate("in", i);
        if (g->getPreviousGate() != nullptr) {
            Neighbour neighbour;
            neighbour.place = check_and_cast<IPlace *>(g->getPathStartGate()->getOwnerModule());
            neighbour.arcSourceGate = g->getPreviousGate();
            cChannel *arc = g->getPreviousGate()->getChannel();
            neighbour.multiplicity = arc->par("multiplicity");
            if (std::find_if(inputPlaces.begin(), inputPlaces.end(), [&] (const Neighbour& j) { return j.place == neighbour.place; }) != inputPlaces.end())
                throw cRuntimeError("Multiple input arcs from place '%s'", g->getPathStartGate()->getOwnerModule()->getFullPath().c_str());
            inputPlaces.push_back(neighbour);
        }
    }

    int numOutputs = gateSize("out");
    outputPlaces.reserve(numOutputs);
    for (int i = 0; i < numOutputs; i++) {
        cGate *g = gate("out", i);
        if (g->getNextGate() != nullptr) {
            Neighbour neighbour;
            neighbour.place = check_and_cast<IPlace *>(g->getPathEndGate()->getOwnerModule());
            neighbour.arcSourceGate = g;
            cChannel *arc = g->getChannel();
            neighbour.multiplicity = arc->par("multiplicity");
            outputPlaces.push_back(neighbour);
        }
    }

    EV << inputPlaces.size() << " input places, " << outputPlaces.size() << " output places\n";
}

void Transition::handleMessage(cMessage *msg)
{
    if (msg == fireEvent)
        startFire();
    else if (msg == endFireEvent)
        endFire();
    else if (msg == endFire2Event)
        endFire2();
    else
        error("Unexpected message received");
}

void Transition::refreshDisplay() const
{
    updateDisplayString();
    if (animating)
        drawAnimationFrame();
}

bool Transition::canFire()
{
    Enter_Method_Silent("canFire");

    // check if all inputs have enough tokens
    for (auto& inputPlace : inputPlaces)
        if (inputPlace.multiplicity > 0 ?
            inputPlace.place->getNumTokens() < inputPlace.multiplicity : // normal arc
            inputPlace.place->getNumTokens() > -inputPlace.multiplicity) // inhibitor arc
            return false;
    return evaluateGuardCondition();
}

void Transition::triggerFiring()
{
    Enter_Method_Silent("scheduleFiring");
    scheduleAt(simTime(), fireEvent);
}

void Transition::startFire()
{
    ASSERT(canFire());

    EV << "startFire: removing tokens from input places\n";

    for (auto& inputPlace : inputPlaces)
        if (inputPlace.multiplicity > 0)
            inputPlace.place->removeTokens(inputPlace.multiplicity);

    emit(firingSignal, 1);

    if (animating) {
        EV << "endFire: initiating inbound token animation\n";
        inboundAnimationStartAnimTime = getEnvir()->getAnimationTime();
        inboundAnimationEndAnimTime = inboundAnimationStartAnimTime + 0.5;
        canvas->holdSimulationFor(0.5);
    }

    // do or schedule endFire()
    simtime_t transitionTime = transitionTimePar->doubleValue();
    if (transitionTime == 0)
        endFire();
    else {
        transitionScheduler->scheduleNextFiring();
        scheduleAt(simTime()+transitionTime, endFireEvent);
    }
}

void Transition::endFire()
{
    if (animating) {
        EV << "endFire: initiating outbound token animation\n";
        double animTime = getEnvir()->getAnimationTime();
        outboundAnimationStartAnimTime = std::max(inboundAnimationEndAnimTime, animTime);
        outboundAnimationEndAnimTime = outboundAnimationStartAnimTime + 0.5;
        canvas->holdSimulationFor(outboundAnimationEndAnimTime - animTime);
    }

    if (animating)
        scheduleAt(simTime(), endFire2Event);
    else
        endFire2();
}

void Transition::endFire2()
{
    EV << "endFire2: adding tokens to output places\n";

    // add tokens to output places
    for (auto& outputPlace : outputPlaces) {
        ASSERT(outputPlace.multiplicity > 0); // outgoing inhibitor arcs don't exist
        outputPlace.place->addTokens(outputPlace.multiplicity);
    }

    emit(firingSignal, 0);

    transitionScheduler->scheduleNextFiring();
}

void Transition::updateDisplayString() const
{
    bool timed = transitionTimePar->isExpression() || transitionTimePar->doubleValue() != 0;
    if (timed)
        getDisplayString().setTagArg("t", 0, transitionTimePar->str().c_str());

    if (endFireEvent->isScheduled() || endFire2Event->isScheduled()) // firing
        getDisplayString().setTagArg("i", 1, "yellow");  // firing
    else {
        if (fireEvent->isScheduled())
            getDisplayString().setTagArg("i", 1, "green"); // armed
        else
            getDisplayString().setTagArg("i", 1, "dimgrey");  // disabled
    }
}

void Transition::drawAnimationFrame() const
{
    clearTokenAnimations(tokenAnimations);
    if (endFireEvent->isScheduled() || endFire2Event->isScheduled()) { // firing
        double animTime = getEnvir()->getAnimationTime();
        if (animTime <= inboundAnimationEndAnimTime) {
            addInboundTokenAnimations(tokenAnimations);
            double alpha = std::min(1.0, (animTime-inboundAnimationStartAnimTime) / (inboundAnimationEndAnimTime-inboundAnimationStartAnimTime));
            setAnimationPosition(tokenAnimations, alpha);
        }
        else if (animTime >= outboundAnimationStartAnimTime && endFire2Event->isScheduled()) {
            addOutboundTokenAnimations(tokenAnimations);
            double alpha = std::min(1.0, (animTime-outboundAnimationStartAnimTime) / (outboundAnimationEndAnimTime-outboundAnimationStartAnimTime));
            setAnimationPosition(tokenAnimations, alpha);
        }
    }
}

void Transition::addInboundTokenAnimations(std::vector<TokenAnimation>& tokenAnimations) const
{
    ASSERT(tokenAnimations.empty());

    for (const auto& inputPlace : inputPlaces) {
        if (inputPlace.multiplicity > 0) {
            auto line = getEnvir()->getConnectionLine(inputPlace.arcSourceGate);
            if (!line.empty()) {
                auto figure = createTokenFigure(inputPlace.multiplicity);
                tokenAnimations.push_back(TokenAnimation { line.front(), line.back(), figure});
                canvas->addFigure(figure);
            }
        }
    }
}

void Transition::addOutboundTokenAnimations(std::vector<TokenAnimation>& tokenAnimations) const
{
    ASSERT(tokenAnimations.empty());

    for (const auto& outputPlace : outputPlaces) {
        ASSERT(outputPlace.multiplicity > 0);
        auto line = getEnvir()->getConnectionLine(outputPlace.arcSourceGate);
        if (!line.empty()) {
            auto figure = createTokenFigure(outputPlace.multiplicity);
            tokenAnimations.push_back(TokenAnimation { line.front(), line.back(), figure});
            canvas->addFigure(figure);
        }
    }

}

void Transition::clearTokenAnimations(std::vector<TokenAnimation>& tokenAnimations) const
{
    if (tokenAnimations.empty())
        return;
    for (auto anim : tokenAnimations) {
        anim.figure->removeFromParent();
        delete anim.figure;
    }
    tokenAnimations.clear();
}

cOvalFigure *Transition::createTokenFigure(int numTokens) const
{
    cOvalFigure *oval = new cOvalFigure();
    oval->setBounds(cFigure::Rectangle(0,0,10,10));
    oval->setFillColor(cFigure::GREY);
    oval->setFilled(true);
    oval->setLineWidth(std::min(8, numTokens)); // for now
    return oval;
}

void Transition::setAnimationPosition(std::vector<TokenAnimation>& tokenAnimations, double alpha) const
{
    for (auto anim : tokenAnimations) {
        Point pos = anim.start*(1-alpha) + anim.end*alpha;
        anim.figure->setPosition(pos, cFigure::ANCHOR_CENTER);
    }
}
