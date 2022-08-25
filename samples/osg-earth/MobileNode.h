//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __MOBILENODE_H__
#define __MOBILENODE_H__

#include <omnetpp.h>

#ifdef WITH_OSGEARTH

#include <osg/PositionAttitudeTransform>
#include <osgEarth/MapNode>
#include <osgEarth/GeoTransform>
#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/FeatureNode>

#include "OsgEarthScene.h"

using namespace omnetpp;

/**
 * Interface to be implemented by mobile nodes to be able to
 * register in ChannelController.
 */
class IMobileNode
{
  public:
    virtual double getX() const  = 0;
    virtual double getY() const  = 0;
    virtual double getLatitude() const  = 0;
    virtual double getLongitude() const = 0;
    virtual double getTxRange() const  = 0;
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
    osg::ref_ptr<osgEarth::GeoTransform> geoTransform = nullptr;
    // osg node for orientation
    osg::ref_ptr<osg::PositionAttitudeTransform> localTransform = nullptr;
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

    double getX() const override { return x; }
    double getY() const override { return y; }
    double getLatitude() const override { return OsgEarthScene::getInstance()->toLatitude(y); }
    double getLongitude() const override { return OsgEarthScene::getInstance()->toLongitude(x); }
    double getTxRange() const override { return txRange; }

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 2; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    virtual void move() = 0;
};

#endif // WITH_OSGEARTH

#endif // __MOBILENODE_H__
