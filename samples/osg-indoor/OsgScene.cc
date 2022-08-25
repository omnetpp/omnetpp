//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "OsgScene.h"

#ifdef WITH_OSG

#include <osgDB/ReadFile>

using namespace omnetpp;

Define_Module(OsgScene);

OsgScene *OsgScene::instance = nullptr;

OsgScene::OsgScene()
{
    if (instance)
        throw cRuntimeError("There can be only one OsgScene instance in the network");
    instance = this;
}

OsgScene::~OsgScene()
{
    instance = nullptr;
}

void OsgScene::initialize()
{
    scene = osgDB::readNodeFile(par("scene"));
    if (!scene)
        throw cRuntimeError("Could not read scene file \"%s\"", par("scene").stringValue());

    cOsgCanvas *builtinOsgCanvas = getParentModule()->getOsgCanvas();
    builtinOsgCanvas->setScene(scene);
    builtinOsgCanvas->setGenericViewpoint(cOsgCanvas::Viewpoint(
            cOsgCanvas::Vec3d(20, -30, 30),
            cOsgCanvas::Vec3d(30, 20, 0),
            cOsgCanvas::Vec3d(0, 0, 1)));

    getParentModule()->getCanvas()->setAnimationSpeed(1.0, this);
}

OsgScene *OsgScene::getInstance()
{
    if (!instance)
        throw cRuntimeError("OsgScene::getInstance(): there is no OsgScene module in the network");
    return instance;
}

void OsgScene::handleMessage(cMessage *msg)
{
    throw cRuntimeError("This module does not handle messages from the outside");
}

#endif  // WITH_OSG
