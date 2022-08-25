//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __CHANNELCONTROLLER_H_
#define __CHANNELCONTROLLER_H_

#include <omnetpp.h>

#ifdef WITH_OSGEARTH

#include "OsgEarthScene.h"
#include "GroundStation.h"
#include "Satellite.h"

#include <osg/Node>
#include <osgEarth/MapNode>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/LocalGeometryNode>
#include <osgEarthUtil/LineOfSight>
#include <osgEarthUtil/LinearLineOfSight>
#include <osgEarthSymbology/Style>
#include <osgEarthSymbology/Geometry>
#include <osgEarthFeatures/Feature>

using namespace omnetpp;

/**
 * This module is responsible for tracking the distance of mobile nodes
 * and visualizing the connectivity graph using OSG nodes.
 */
class ChannelController : public cSimpleModule
{
  protected:
    static osg::ref_ptr<osg::Drawable> createLineBetweenPoints(osg::Vec3 start, osg::Vec3 end, float width, osg::Vec4 color);

    static ChannelController *instance;
    std::vector<Satellite *> satellites;
    std::vector<GroundStation *> stations;

    std::vector<osgEarth::Util::LinearLineOfSightNode *> losNodes;
    std::map<Satellite *, osg::Geometry *> orbitsMap;

    osg::ref_ptr<osg::Geode> connections = nullptr;
    // visual parameters of the connections
    std::string satToSatColor;
    double satToSatWidth = 0;
    std::string satToGroundColor;
    double satToGroundWidth = 0;

    // the node containing the osgEarth data
    osg::Group *scene = nullptr;

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 3; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    void addLineOfSight(osg::Node *a, osg::Node *b, int type);

  public:
    ChannelController();
    virtual ~ChannelController();
    static ChannelController *getInstance();
    void addSatellite(Satellite *p); // to be called exactly in initialize stage 1
    void addGroundStation(GroundStation *p); // to be called exactly in initialize stage 1
};

#endif // WITH_OSGEARTH

#endif // __CHANNELCONTROLLER_H_
