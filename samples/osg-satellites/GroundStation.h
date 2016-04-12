//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __GROUNDSTATION_H__
#define __GROUNDSTATION_H__

#include <osgEarth/MapNode>
#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthUtil/ObjectLocator>

#include <omnetpp.h>
#include "OsgEarthScene.h"

using namespace omnetpp;

/**
 * A mobile node (with a 3D model) moving around . A range indicator, and the
 * model's track can be shown along with its label.
 */
class GroundStation : public cSimpleModule
{
  protected:
    // configuration
    double timeStep;
    unsigned int trailLength;
    std::string labelColor;
    std::string rangeColor;
    std::string trailColor;
    std::string modelURL;
    double modelScale;

    // the node containing the osgEarth data
    osg::observer_ptr<osgEarth::MapNode> mapNode = nullptr;
    // osgEarth node for 3D visualization
    osgEarth::Util::ObjectLocatorNode *locatorNode = nullptr;

    double longitude = -70, latitude = 40, altitude = 400;

  public:

    osg::Vec3d getPosition() { return osg::Vec3d(longitude, latitude, altitude); }
    osg::Node *getLocatorNode() { return locatorNode; };

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 2; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshVisuals();
};

#endif
