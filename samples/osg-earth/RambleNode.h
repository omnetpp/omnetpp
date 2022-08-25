//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __RAMBLENODE_H__
#define __RAMBLENODE_H__

#include <omnetpp.h>

#ifdef WITH_OSGEARTH

#include "MobileNode.h"
#include <osgAnimation/BasicAnimationManager>

using namespace omnetpp;

/**
 * A mobile node (with a 3D model) moving around with a constant speed, changing
 * the heading randomly, and emitting a transmission from time to time.
 * Note that the transmission spheres are purely for illustration purposes,
 * and do not represent communication between nodes nor simulation messages.
 */
class RambleNode : public MobileNode
{
  protected:
    // configuration parameters
    double playgroundHeight, playgroundWidth;
    double speed;
    double transmissionDuration;
    double initialAlpha;
    // transmission bubble
    osg::ref_ptr<osg::PositionAttitudeTransform> transmissionNode;
    osg::Material *material; // keeping a pointer to it, so we can access it easier when making the sphere fade

    // the last time this node transmitted, -1 if not yet
    SimTime lastTransmissionStarted = -1;
    // this is the timer to the node itself to start a transmission, not a transmitted message
    cMessage *transmitMessage = nullptr;

  public:
    RambleNode();
    virtual ~RambleNode();

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 2; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    virtual void move() override;
    virtual void transmit();
};

#endif // WITH_OSGEARTH

#endif // __RAMBLENODE_H__
