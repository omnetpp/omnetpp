//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#if defined(WITH_OSG) && defined(WITH_OSGEARTH)
#include "RambleNode.h"
#include "OsgEarthScene.h"
#include "ChannelController.h"
#include <osg/Depth>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>

using namespace omnetpp;

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
        transmissionDuration = par("transmissionDuration");
        initialAlpha = par("transmissionAlpha");
        // this will make the animation smoother
        getParentModule()->getCanvas()->setAnimationSpeed(1, this);
        break;
    case 1:
        // creating the transmission bubble
        transmissionNode = new osg::PositionAttitudeTransform; // we will scale using this node
        geoTransform->addChild(transmissionNode);

        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        transmissionNode->addChild(geode);

        osg::Vec3 center(0.0f, 0.0f, 0.0f); // in the local coordinate system of the node
        double radius = 1.0; // the scaling transformation will modulate this
        auto drawable = new osg::ShapeDrawable(new osg::Sphere(center, radius));
        geode->addDrawable(drawable);

        // setting up rendering to deal with transparency
        auto stateSet = geode->getOrCreateStateSet();
        stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
        osg::Depth *depth = new osg::Depth;
        depth->setWriteMask(false);
        stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);
        stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

        // setting up color and alpha in the material
        cOsgCanvas::Color col(par("transmissionColor"));
        auto color = osg::Vec4(col.red / 255.0, col.green / 255.0, col.blue / 255.0, initialAlpha);
        material = new osg::Material;
        material->setEmission(osg::Material::FRONT_AND_BACK, color);
        material->setAmbient(osg::Material::FRONT_AND_BACK, color);
        geode->getStateSet()->setAttributeAndModes(material, osg::StateAttribute::OVERRIDE);

        // We only have to do this because in older OSG versions the material didn't work properly...
        // In 3.4.0 it's no longer needed. This way at least it will be somewhat transparent.
        drawable->setColor(color);

        // scheduling the first signal to ourselves
        transmitMessage = new cMessage;
        scheduleAt(simTime() + par("transmitInterval"), transmitMessage);
        break;
    }
}

void RambleNode::handleMessage(cMessage *msg)
{
    if (msg == transmitMessage) {
        // if the message is to make a transmission, do it
        transmit();
        scheduleAt(simTime() + par("transmitInterval"), transmitMessage);
    } else {
        // otherwise let the base class handle it (it is most likely a "move" message)
        MobileNode::handleMessage(msg);
    }
}

void RambleNode::refreshDisplay() const
{
    MobileNode::refreshDisplay();

    // this will run from 0 to 1 during the propagation of the last transmission
    double t = 0;
    if (lastTransmissionStarted > SIMTIME_ZERO) {
        t = (simTime() - lastTransmissionStarted).dbl() / transmissionDuration;
        if (t > 1)
            t = 0; // if it is over, resetting
    }

    // fading and scaling according to the parameters
    material->setAlpha(osg::Material::FRONT, initialAlpha * (1.0 - t));
    t *= txRange;
    transmissionNode->setScale(osg::Vec3d(t, t, t));
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

void RambleNode::transmit()
{
    // simply remembering the current time as the time of the latest transmission
    lastTransmissionStarted = simTime();
}

#endif // WITH_OSG
