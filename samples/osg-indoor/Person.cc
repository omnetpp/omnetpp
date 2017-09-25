//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifdef WITH_OSG
#include "Person.h"
#include "omnetpp/osgutil.h"
#include <osgDB/ReadFile>
#include "OsgScene.h"

using namespace omnetpp;

Define_Module(Person);

void Person::initialize(int stage)
{
    switch (stage) {
    case 0:
        WATCH(x);
        WATCH(y);
        WATCH(heading);
        // fill the track
        readWaypointsFromFile(par("trackFile"));
        // initial location
        targetPointIndex = 0;
        x = waypoints[targetPointIndex].x;
        y = waypoints[targetPointIndex].y;
        speed = par("speed");
        wayponitProximity = par("waypointProximity");
        heading = 0;
        angularSpeed = 0;
        timeStep = par("timeStep");
        modelURL = par("modelURL").stringValue();
        break;

    case 1:
        auto scene = OsgScene::getInstance()->getScene(); // scene is initialized in stage 0 so we have to do our init in stage 1

        // build up the node representing this module
        auto modelNode = osgDB::readNodeFile(modelURL);
        if (!modelNode)
            throw cRuntimeError("Model file \"%s\" not found or format is not recognized", modelURL.c_str());

        auto objectNode = new cObjectOsgNode(this);  // make the node selectable in Qtenv
        objectNode->addChild(modelNode);

        patNode = new osg::PositionAttitudeTransform();
        patNode->addChild(objectNode);

        ((osg::Group *)scene)->addChild(patNode);

        // schedule first move
        cMessage *timer = new cMessage("move");
        scheduleAt(par("startTime"), timer);
        break;
    }
}

void Person::refreshDisplay() const
{
    double modelheading = fmod((360 + 90 + heading), 360) - 180;

    // update the 3D position of the model node
    patNode->setPosition(osg::Vec3d(x, y, 1.5));
    patNode->setAttitude(osg::Quat(modelheading / 180.0 * M_PI, osg::Vec3d(0, 0, 1)));

    // update the position on the 2D canvas, too
    getDisplayString().setTagArg("p", 0, x);
    getDisplayString().setTagArg("p", 1, y);
}

void Person::handleMessage(cMessage *msg)
{
    // update the node position
    move();

    // schedule next movement
    scheduleAt(simTime() + timeStep, msg);
}

void Person::readWaypointsFromFile(const char *fileName)
{
    std::ifstream inputFile(fileName);
    while (true) {
       double wpx, wpy;
       inputFile >> wpx >> wpy;
       if (!inputFile.fail())
           waypoints.push_back(Waypoint(wpx, wpy));
       else
           break;
    }
}

void Person::move()
{
    Waypoint target = waypoints[targetPointIndex];
    double dx = target.x - x;
    double dy = target.y - y;
    if (dx*dx + dy*dy < wayponitProximity*wayponitProximity)  // reached so change to next (within the predefined proximity of the waypoint)
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

#endif  // WITH_OSG
