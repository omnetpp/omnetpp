//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __RAMBLENODE_H__
#define __RAMBLENODE_H__

#include <omnetpp.h>
#include "MobileNode.h"

USING_NAMESPACE

/**
 * A mobile node (with a 3D model) moving around with a constant speed, changing
 * the heading randomly.
 */
class RambleNode : public MobileNode
{
  protected:
    // configuration
    double playgroundHeight, playgroundWidth;  // in meters
    double speed;

  public:
    RambleNode();
    virtual ~RambleNode();

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 2; }
    virtual void move() override;
};

#endif
