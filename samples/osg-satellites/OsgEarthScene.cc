//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifdef WITH_OSG
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgEarth/Viewpoint>
#include <osgEarth/MapNode>
#include <osgEarth/Capabilities>
#include <osgEarthAnnotation/RectangleNode>
#include <osgEarthUtil/SkyNode>
#include "OsgEarthScene.h"
#include "ChannelController.h"

using namespace osgEarth;
using namespace osgEarth::Annotation;

Define_Module(OsgEarthScene);

OsgEarthScene *OsgEarthScene::instance = nullptr;

OsgEarthScene::OsgEarthScene()
{
    if (instance)
        throw cRuntimeError("There can be only one OsgRenderer instance in the network");
    instance = this;
}

OsgEarthScene::~OsgEarthScene()
{
    instance = nullptr;
}

void OsgEarthScene::initialize()
{
    timeStep = par("timeStep");

    auto mapNode = dynamic_cast<osgEarth::MapNode*>(osgDB::readNodeFile(par("scene")));
    if (!mapNode)
        throw cRuntimeError("Could not read scene file \"%s\"", par("scene").stringValue());

    cOsgCanvas *builtinOsgCanvas = getParentModule()->getOsgCanvas();

    builtinOsgCanvas->setCameraManipulatorType(cOsgCanvas::CAM_TRACKBALL);

    // set up viewer
    builtinOsgCanvas->setViewerStyle(cOsgCanvas::STYLE_EARTH);
    builtinOsgCanvas->setClearColor(cOsgCanvas::Color("black"));

    earthRotator = new osg::PositionAttitudeTransform();
    earthRotator->addChild(mapNode);

    scene = new osg::Group();
    scene->asGroup()->addChild(earthRotator);
    scene->asGroup()->addChild(new osgEarth::Util::SkyNode(mapNode->getMap()));

    auto stateSet = scene->getOrCreateStateSet();
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    mapNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);

    builtinOsgCanvas->setScene(scene);

    cMessage *timer = new cMessage("move");
    scheduleAt(0, timer);
}

OsgEarthScene *OsgEarthScene::getInstance()
{
    if (!instance)
        throw cRuntimeError("OsgRenderer::getInstance(): there is no OsgRenderer module in the network");
    return instance;
}

void OsgEarthScene::handleMessage(cMessage *msg)
{
    double angle = simTime().inUnit(SIMTIME_S) / 60.0 / 60 / 24 * 2 * M_PI;
    earthRotator->setAttitude(osg::Quat(angle, osg::Vec3d(0, 0, 1)));

    ChannelController::getInstance()->updateConnectionGraph();
    scheduleAt(simTime() + SimTime(timeStep, SIMTIME_S), msg);
}

#endif // WITH_OSG