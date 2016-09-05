//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#if defined(WITH_OSG) && defined(WITH_OSGEARTH)
#include "WaypointTrackerNode.h"
#include "OsgEarthScene.h"
#include "ChannelController.h"
#include <fstream>
#include <iostream>

using namespace omnetpp;

Define_Module(WaypointTrackerNode);

WaypointTrackerNode::WaypointTrackerNode()
{
}

WaypointTrackerNode::~WaypointTrackerNode()
{
}

void WaypointTrackerNode::initialize(int stage)
{
    MobileNode::initialize(stage);
    switch (stage) {
    case 0:
        // fill the track
        readWaypointsFromFile(par("trackFile"));
        // initial location
        targetPointIndex = 0;
        x = waypoints[targetPointIndex].x;
        y = waypoints[targetPointIndex].y;
        speed = par("speed");
        waypointProximity = par("waypointProximity");
        heading = 0;
        angularSpeed = 0;
        break;
    }
}

void WaypointTrackerNode::readWaypointsFromFile(const char *fileName)
{
    std::ifstream inputFile(fileName);
    while (true) {
       double longitude, latitude;
       inputFile >> longitude >> latitude;
       if (!inputFile.fail())
           waypoints.push_back(Waypoint(OsgEarthScene::getInstance()->toX(latitude), OsgEarthScene::getInstance()->toY(longitude), 0.0));
       else
           break;
    }
}

void WaypointTrackerNode::move()
{
    Waypoint target = waypoints[targetPointIndex];
    double dx = target.x - x;
    double dy = target.y - y;
    if (dx*dx + dy*dy < waypointProximity*waypointProximity)  // reached so change to next (within the predefined proximity of the waypoint)
        targetPointIndex = (targetPointIndex+1) % waypoints.size();

    double targetDirection = atan2(dx, -dy) / M_PI * 180;
    double diff = targetDirection - heading;
    while (diff < -180)
        diff += 360;
    while (diff > 180)
        diff -= 360;

    angularSpeed = diff*5;

    // move
    heading += angularSpeed * timeStep;
    double distance = speed * timeStep;
    x += distance * sin(M_PI * heading / 180);
    y += distance * -cos(M_PI * heading / 180);
}

#endif // WITH_OSG
