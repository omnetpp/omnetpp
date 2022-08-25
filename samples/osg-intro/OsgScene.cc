//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

#ifdef WITH_OSG

#include <osgDB/ReadFile>

using namespace omnetpp;

class OsgScene : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(OsgScene);

void OsgScene::initialize()
{
    cOsgCanvas *osgCanvas = getParentModule()->getOsgCanvas();  // get the canvas of the root module

    osg::Node *scene = osgDB::readNodeFile(par("sceneFile"));  // read the scene data
    if (!scene)
        throw cRuntimeError("Could not read scene file \"%s\"", par("sceneFile").stringValue());

    if (par("isEarthStyle"))
#ifdef WITH_OSGEARTH
        osgCanvas->setViewerStyle(cOsgCanvas::STYLE_EARTH);  // we need a different camera manipulator to display terrains
#else
        throw cRuntimeError("This example requires osgEarth installed!");
#endif

    osgCanvas->setScene(scene);
}

void OsgScene::handleMessage(cMessage *msg)
{
}

#endif // WITH_OSG
