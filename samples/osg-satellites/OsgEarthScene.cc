//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#if defined(WITH_OSG) && defined(WITH_OSGEARTH)

#include "OsgEarthScene.h"
#include "ChannelController.h"

#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osgEarth/Viewpoint>
#include <osgEarth/MapNode>
#include <osgEarth/Capabilities>
#include <osgEarthAnnotation/RectangleNode>
#include <osgEarth/Version>
#include <osgEarthUtil/Sky>

using namespace osgEarth;
using namespace osgEarth::Annotation;

Define_Module(OsgEarthScene);

OsgEarthScene *OsgEarthScene::instance = nullptr;

OsgEarthScene::OsgEarthScene()
{
    if (instance)
        throw cRuntimeError("There can be only one OsgEarthScene instance in the network");
    instance = this;
}

OsgEarthScene::~OsgEarthScene()
{
    instance = nullptr;
}

void OsgEarthScene::initialize()
{
    auto mapNode = dynamic_cast<osgEarth::MapNode*>(osgDB::readNodeFile(par("scene")));
    if (!mapNode)
        throw cRuntimeError("Could not read scene file \"%s\"", par("scene").stringValue());

    cOsgCanvas *builtinOsgCanvas = getParentModule()->getOsgCanvas();

    builtinOsgCanvas->setCameraManipulatorType(cOsgCanvas::CAM_TRACKBALL);

    // set up viewer
    builtinOsgCanvas->setViewerStyle(cOsgCanvas::STYLE_EARTH);
    builtinOsgCanvas->setClearColor(cOsgCanvas::Color("black"));
    // unit is meters
    builtinOsgCanvas->setZNear(100000);
    builtinOsgCanvas->setZFar(1000000000);

    earthRotator = new osg::PositionAttitudeTransform();
    earthRotator->addChild(mapNode);

    scene = new osg::Group();
    scene->asGroup()->addChild(earthRotator);
    scene->asGroup()->addChild(osgEarth::Util::SkyNode::create(mapNode));
    auto stateSet = scene->getOrCreateStateSet();
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    mapNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);

    builtinOsgCanvas->setScene(scene);

    // one hour in the simulation will be one second in reality (with 1.0 playback speed)
    getParentModule()->getCanvas()->setAnimationSpeed(360, this);
}

OsgEarthScene *OsgEarthScene::getInstance()
{
    if (!instance)
        throw cRuntimeError("OsgEarthScene::getInstance(): there is no OsgEarthScene module in the network");
    return instance;
}

void OsgEarthScene::refreshDisplay() const
{
    double angle = simTime().inUnit(SIMTIME_S) / 60.0 / 60 / 24 * 2 * M_PI;
    earthRotator->setAttitude(osg::Quat(angle, osg::Vec3d(0, 0, 1)));
}
#endif // WITH_OSG
