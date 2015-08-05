//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __MOBILENODE_H__
#define __MOBILENODE_H__

#include <osgEarth/MapNode>
#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthUtil/ObjectLocator>

#include <omnetpp.h>
#include "OsgEarthScene.h"

USING_NAMESPACE

/**
 * Interface to be implemented by mobile nodes to be able to
 * register in ChannelController.
 */
class IMobileNode
{
  public:
    virtual double getX() = 0;
    virtual double getY() = 0;
    virtual double getLatitude() = 0;
    virtual double getLongitude() = 0;
    virtual double getTxRange() = 0;
};

/**
 * A mobile node (with a 3D model) moving around . A range indicator, and the
 * model's track can be shown along with its label.
 */
class MobileNode : public cSimpleModule, public IMobileNode
{
  protected:
    // configuration
    double timeStep;
    unsigned int trailLength;
    std::string labelColor;
    std::string rangeColor;
    std::string trailColor;
    std::string modelURL;
    bool showTxRange;
    double txRange;

    // the node containing the osgEarth data
    osg::observer_ptr<osgEarth::MapNode> mapNode = nullptr;
    // osgEarth node for 3D visualization
    osg::ref_ptr<osgEarth::Util::ObjectLocatorNode> locatorNode = nullptr;
    // range indicator node
    osg::ref_ptr<osgEarth::Annotation::CircleNode> rangeNode = nullptr;
    // trail annotation
    osg::ref_ptr<osgEarth::Annotation::FeatureNode> trailNode = nullptr;
    osgEarth::Style trailStyle;
    osgEarth::Vec3dVector trail;  // recently visited points

    // node position and heading (speed is constant in this model)
    double heading;  // in degrees
    double x, y;  // in meters, relative to playground origin

  public:
    MobileNode();
    virtual ~MobileNode();

    double getX() override { return x; }
    double getY() override { return y; }
    double getLatitude() override { return OsgEarthScene::getInstance()->toLatitude(y); }
    double getLongitude() override { return OsgEarthScene::getInstance()->toLongitude(x); }
    double getTxRange() override { return txRange; }

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 2; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshVisuals();
    virtual void move() = 0;
};

#endif
