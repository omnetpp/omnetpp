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
        break;
    case 1:
        // loading the transmission bubble animation
        std::string transmissionAnimUrl = par("transmissionAnimUrl");
        osg::ref_ptr<osg::Node> transmissionNode = osgDB::readNodeFile(transmissionAnimUrl);
        if (!transmissionNode)
            throw cRuntimeError("Could not load transmission animation '%s'!", transmissionAnimUrl.c_str());

        // setting up rendering to deal with transparency
        auto stateSet = transmissionNode->getOrCreateStateSet();
        stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
        osg::Depth* depth = new osg::Depth;
        depth->setWriteMask(false);
        stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);
        stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

        // setting up transparency in the material
        cOsgCanvas::Color color(par("transmissionColor"));
        auto matColor = osg::Vec4(color.red / 255.0, color.green / 255.0, color.blue / 255.0, par("transmissionAlpha").doubleValue());
        auto material = new osg::Material;
        material->setEmission(osg::Material::FRONT, matColor);
        material->setDiffuse(osg::Material::FRONT, matColor);
        material->setAmbient(osg::Material::FRONT, matColor);
        material->setAlpha(osg::Material::FRONT, par("transmissionAlpha").doubleValue());
        transmissionNode->getStateSet()->setAttributeAndModes(material, osg::StateAttribute::OVERRIDE);

        locatorNode->addChild(transmissionNode);

        // finding the animation manager in the loaded node
        struct AnimationManagerFinder : public osg::NodeVisitor {
            osgAnimation::BasicAnimationManager *result = nullptr;
            AnimationManagerFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
            void apply(osg::Node& node) {
                if (result) return; // already found it
                if (osgAnimation::AnimationManagerBase* b = dynamic_cast<osgAnimation::AnimationManagerBase*>(node.getUpdateCallback())) {
                    result = new osgAnimation::BasicAnimationManager(*b); // here it is!
                    return;
                }
                traverse(node);
            }
        } finder;

        transmissionNode->accept(finder);
        animationManager = finder.result;

        if (!animationManager)
            throw cRuntimeError("Could not find the AnimationManagerBase in the loaded transmission animation (%s)!", transmissionAnimUrl.c_str());

        // setting up the animation
        transmissionNode->setUpdateCallback(animationManager);
        if (animationManager->getAnimationList().empty())
            throw cRuntimeError("No animation found in the transmission animation (%s)!", transmissionAnimUrl.c_str());

        transmissionAnim = animationManager->getAnimationList().front();
        transmissionAnim->setPlayMode(osgAnimation::Animation::STAY);
        // duration * 2 because the sphere animation ends at half of its real length
        // (as a workaround, otherwise spheres would randomly stick with intermediate radiuses)
        transmissionAnim->setDuration(par("transmissionDuration").doubleValue() * 2);

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
    animationManager->playAnimation(transmissionAnim);
}

#endif // WITH_OSG
