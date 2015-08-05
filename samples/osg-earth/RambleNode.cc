//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "RambleNode.h"
#include "OsgEarthScene.h"
#include "ChannelController.h"

USING_NAMESPACE

Define_Module(RambleNode);

RambleNode::RambleNode()
{
}

RambleNode::~RambleNode()
{
}

void RambleNode::initialize(int stage)
{
    MobileNode::initialize(stage);
    switch (stage) {
    case 0:
        x = par("startX");
        y = par("startY");
        heading = 360 * dblrand();
        speed = par("speed");
        playgroundHeight = getSystemModule()->par("playgroundHeight");
        playgroundWidth = getSystemModule()->par("playgroundWidth");
        break;
    }
}

void RambleNode::move()
{
    // calculate the new location and speed data
    // 1st: update the heading with some random change
    heading += 120*(dblrand() - 0.5) * timeStep;
    heading = fmod(heading + 360, 360);

    // 2nd: update position
    double vx = sin(heading * M_PI/180) * speed;
    double vy = -cos(heading * M_PI/180) * speed;
    x += vx * timeStep;
    y += vy * timeStep;

    // 3rd: keep the module inside the playground
    if (x < 0) {x=0; heading = 360 - heading; }
    if (x > playgroundWidth) {x=playgroundWidth; heading = 360 - heading;}
    if (y < 0) {y=0; heading = 180 - heading;}
    if (y > playgroundHeight) {y=playgroundHeight; heading = 180 - heading;}
}
