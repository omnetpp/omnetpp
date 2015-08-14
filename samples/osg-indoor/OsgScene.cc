//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <osgDB/ReadFile>
#include "OsgScene.h"

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
