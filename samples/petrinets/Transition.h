//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __TRANSITION_H__
#define __TRANSITION_H__

#include <omnetpp.h>
#include "ITransition.h"
#include "TransitionScheduler.h"

using namespace omnetpp;

/**
 * A transition in a Petri net
 */
class Transition : public cSimpleModule, public ITransition
{
  protected:
    cPar *transitionTimePar = nullptr;
    cMessage *fireEvent = nullptr;
    cMessage *endFireEvent = nullptr;
    cMessage *endFire2Event = nullptr;
    static simsignal_t firingSignal;

    TransitionScheduler *transitionScheduler = nullptr;
    struct Neighbour { IPlace *place; cGate *arcSourceGate; int multiplicity; }; // multiplicity is negative for inhibitor arcs
    std::vector<Neighbour> inputPlaces;
    std::vector<Neighbour> outputPlaces;

    // animation
    typedef cFigure::Point Point;
    typedef cFigure::Rectangle Rectangle;
    bool animating = true;
    cCanvas *canvas = nullptr;
    double inboundAnimationStartAnimTime = -1, inboundAnimationEndAnimTime = -1;
    double outboundAnimationStartAnimTime = -1, outboundAnimationEndAnimTime = -1;
    struct TokenAnimation {Point start, end; cOvalFigure *figure; };
    mutable std::vector<TokenAnimation> tokenAnimations;

  public:
    virtual ~Transition();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    virtual void updateDisplayString() const;
    virtual void drawAnimationFrame() const;

    // utility methods
    virtual bool evaluateGuardCondition() {return true;} // override if needed!
    virtual void startFire();
    virtual void endFire();
    virtual void endFire2();
    virtual void discoverNeighbours();
    virtual void addInboundTokenAnimations(std::vector<TokenAnimation>& tokenAnimations) const;
    virtual void addOutboundTokenAnimations(std::vector<TokenAnimation>& tokenAnimations) const;
    virtual void clearTokenAnimations(std::vector<TokenAnimation>& tokenAnimations) const;
    virtual void setAnimationPosition(std::vector<TokenAnimation>& tokenAnimations, double alpha) const;
    virtual cOvalFigure *createTokenFigure(int numTokens) const;

  public:
    virtual bool canFire() override;
    virtual void triggerFiring() override;
};

#endif


