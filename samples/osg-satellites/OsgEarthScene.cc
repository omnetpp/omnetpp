//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <osgDB/ReadFile>
#include <osgEarth/Viewpoint>
#include <osgEarth/MapNode>
#include <osgEarth/Capabilities>
#include <osgEarthAnnotation/RectangleNode>
#include <osgEarthUtil/SkyNode>
#include "OsgEarthScene.h"

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
    scene = osgDB::readNodeFile(par("scene"));
    if (!scene)
        throw cRuntimeError("Could not read scene file \"%s\"", par("scene").stringValue());

    cOsgCanvas *builtinOsgCanvas = getParentModule()->getOsgCanvas();

    auto mapNode = MapNode::findMapNode(scene);
    ASSERT(mapNode != nullptr);

    mapNode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    mapNode->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

    // set up viewer
    builtinOsgCanvas->setViewerStyle(cOsgCanvas::STYLE_EARTH);
    builtinOsgCanvas->setClearColor(cOsgCanvas::Color("black"));
    builtinOsgCanvas->setScene(scene);
    osgEarth::Util::SkyNode *sky = new osgEarth::Util::SkyNode(mapNode->getMap());
    scene->asGroup()->addChild(sky);
}

OsgEarthScene *OsgEarthScene::getInstance()
{
    if (!instance)
        throw cRuntimeError("OsgRenderer::getInstance(): there is no OsgRenderer module in the network");
    return instance;
}

void OsgEarthScene::handleMessage(cMessage *msg)
{
    throw cRuntimeError("This module does not handle messages from the outside");
}
