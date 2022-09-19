//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __OSGSCENE_H__
#define __OSGSCENE_H__

#include <omnetpp.h>

#ifdef WITH_OSG

#include <osg/Node>

using namespace omnetpp;

/**
 * Initialize global 3d canvas and load the configured model file.
 */
class OsgScene : public cSimpleModule
{
  protected:
    static OsgScene *instance;
    osg::ref_ptr<osg::Node> scene;

  public:
    OsgScene();
    virtual ~OsgScene();

    static OsgScene *getInstance();
    virtual osg::Node *getScene() {return scene; }

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
#endif
