//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __OSGRENDERER_H__
#define __OSGRENDERER_H__

#include <omnetpp.h>
#include <osg/Node>
#include <osgEarth/MapNode>

USING_NAMESPACE

/**
 * Initialize global 3d canvas and load the configured earth model file.
 */
class OsgEarthScene : public cSimpleModule
{
  protected:
    double timeStep = 60; // for earth rotation
    static OsgEarthScene *instance;
    osg::ref_ptr<osg::Node> scene;
    osg::PositionAttitudeTransform *earthRotator = nullptr;

  public:
    OsgEarthScene();
    virtual ~OsgEarthScene();

    static OsgEarthScene *getInstance();
    virtual osg::Node *getScene() { return scene; }
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
