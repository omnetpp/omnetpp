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

#ifdef WITH_OSGEARTH

#include <osg/Node>
#include <osgEarth/MapNode>

using namespace omnetpp;

/**
 * Initialize global 3D canvas and load the configured Earth model file.
 */
class OsgEarthScene : public cSimpleModule
{
  protected:
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
    virtual void refreshDisplay() const override;
};

#endif
#endif
