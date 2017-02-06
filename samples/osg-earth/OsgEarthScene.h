//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __OSGEARTHSCENE_H__
#define __OSGEARTHSCENE_H__

#include <omnetpp.h>

#include <osg/Node>
#include <osgEarth/MapNode>

using namespace omnetpp;

/**
 * Initialize global 3d canvas and load the configured earth model file.
 */
class OsgEarthScene : public cSimpleModule
{
  protected:
    double playgroundLat;
    double playgroundLon;
    double playgroundHeight;
    double playgroundWidth;
    static OsgEarthScene *instance;
    osg::ref_ptr<osg::Node> scene;

  public:
    OsgEarthScene();
    virtual ~OsgEarthScene();

    static OsgEarthScene *getInstance();
    virtual osg::Node *getScene() {return scene; }
    // latitude from local y coordinate
    virtual double toLatitude(double y) { return playgroundLat - y / 111111; }
    // longitude from local x coordinate
    virtual double toLongitude(double x) { return playgroundLon +  x / 111111 / cos(fabs(playgroundLat/180*M_PI)); }
    // local Y coordinate from latitude
    virtual double toY(double latitude) {  return (playgroundLat - latitude) * 111111; }
    // local X coordinate from longitude
    virtual double toX(double longitude) { return (longitude - playgroundLon) * cos(fabs(playgroundLat/180*M_PI)) * 111111; }

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
