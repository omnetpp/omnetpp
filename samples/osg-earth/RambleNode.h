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
#include "MobileNode.h"
#include <osgAnimation/BasicAnimationManager>

using namespace omnetpp;

/**
 * A mobile node (with a 3D model) moving around with a constant speed, changing
 * the heading randomly, and emitting a transmission from time to time.
 */
class RambleNode : public MobileNode
{
  protected:
    // configuration
    double playgroundHeight, playgroundWidth;  // in meters
    double speed;
    // transmission bubble
    osg::ref_ptr<osgAnimation::BasicAnimationManager> animationManager = nullptr;
    osg::ref_ptr<osgAnimation::Animation> transmissionAnim = nullptr;
    cMessage *transmitMessage = nullptr;  // keeping the pointer so we can compare with it

  public:
    RambleNode();
    virtual ~RambleNode();

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 2; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void move() override;
    virtual void transmit();
};

#endif
