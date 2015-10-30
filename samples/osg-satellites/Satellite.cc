//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifdef WITH_OSG
#include "Satellite.h"

#include <sstream>
#include <iomanip>

#include <osg/Node>
#include <osg/Texture>
#include <osg/LineWidth>
#include <osg/PolygonMode>
#include <osg/Texture2D>
#include <osg/Image>
#include <osg/Depth>
#include <osg/PositionAttitudeTransform>
#include <osgEarth/Capabilities>
#include <osgEarthAnnotation/LabelNode>
#include <osgEarthSymbology/Geometry>
#include <osgEarthFeatures/Feature>

#include "OsgEarthScene.h"
#include "ChannelController.h"

USING_NAMESPACE

Define_Module(Satellite)

using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace osgEarth::Features;


void Satellite::initialize(int stage)
{
    switch (stage) {
    case 0: {
        timeStep = par("timeStep");
        modelURL = par("modelURL").stringValue();
        modelScale = par("modelScale");
        labelColor = par("labelColor").stringValue();
        altitude = par("altitude");

        phase = startingPhase = par("startingPhase").doubleValue() * M_PI / 180.0;

        std::string normalString = par("orbitNormal");
        if (normalString.empty()) {
            // it is not a correct spherical distribution, nor deterministic, but will do here
             normal.set(dblrand() * 2 - 1, dblrand() * 2 - 1, dblrand() * 2 - 1);
        } else {
            std::stringstream ss(normalString);

            double x, y, z;
            ss >> x;
            ss.ignore();
            ss >> y;
            ss.ignore();
            ss >> z;

            if (!ss)
                throw cRuntimeError("Couldn't parse orbit normal \"%s\", the correct format is for example \"2.5,3,0\", or leave it empty for random.", normalString.c_str());

            normal.set(x, y, z);
        }

        normal.normalize();

        auto c1 = normal ^ osg::Vec3d(0, 1, 0);
        auto c2 = normal ^ osg::Vec3d(1, 0, 0);
        osg::Vec3d &cross = c1.length2() < 0.1 ? c2 : c1;

        cross.normalize();
        orbitX = cross;
        orbitY = normal ^ cross;

        break;
    }
    case 1:
        ChannelController::getInstance()->addSatellite(this);

        auto scene = OsgEarthScene::getInstance()->getScene(); // scene is initialized in stage 0 so we have to do our init in stage 1
        mapNode = osgEarth::MapNode::findMapNode(scene);

        // build up the node representing this module
        // an ObjectLocatorNode allows positioning a model using world coordinates
        locatorNode = new osgEarth::Util::ObjectLocatorNode(mapNode->getMap());

        auto modelNode = osgDB::readNodeFile(modelURL);

        modelNode->getOrCreateStateSet()->setAttributeAndModes(
          new osg::Program(), osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
        modelNode->getOrCreateStateSet()->setMode(
                  GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

        // scale and rotate the model if necessary
        auto pat = new osg::PositionAttitudeTransform();
        pat->setScale(osg::Vec3d(modelScale, modelScale, modelScale));

        auto objectNode = cOsgCanvas::createOmnetppObjectNode(this);
        pat->addChild(objectNode);
        objectNode->addChild(modelNode);
        locatorNode->addChild(pat);

        // set the name label if the color is specified
        if (!labelColor.empty()) {
            Style labelStyle;
            labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_TOP;
            labelStyle.getOrCreate<TextSymbol>()->declutter() = true;
            labelStyle.getOrCreate<TextSymbol>()->pixelOffset() = osg::Vec2s(0,50);
            labelStyle.getOrCreate<TextSymbol>()->fill()->color() = osgEarth::Color(labelColor);
            locatorNode->addChild(new LabelNode(getFullName(), labelStyle));
        }

        // add the locator node to the scene
        scene->asGroup()->addChild(locatorNode);

        // making the orbit circle
        std::string orbitColor = par("orbitColor");
        if (!orbitColor.empty()) {
            osg::ref_ptr<osg::Geometry> orbitGeom = new osg::Geometry;
            osg::ref_ptr<osg::DrawArrays> drawArrayLines = new  osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP);
            osg::ref_ptr<osg::Vec3Array> vertexData = new osg::Vec3Array;

            orbitGeom->addPrimitiveSet(drawArrayLines);
            auto stateSet = orbitGeom->getOrCreateStateSet();
            stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
            stateSet->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
            stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
            stateSet->setAttributeAndModes(new osg::LineWidth(1.5), osg::StateAttribute::ON);
            stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            auto depth = new osg::Depth;
            depth->setWriteMask(false);
            stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

            for (int i = 0; i <= 100; ++i)
                vertexData->push_back(getPositionAtPhase(i / 100.0 * M_PI*2));

            orbitGeom->setVertexArray(vertexData);
            drawArrayLines->setFirst(0);
            drawArrayLines->setCount(vertexData->size());

            osg::ref_ptr<osg::Vec4Array> colorData = new osg::Vec4Array;
            colorData->push_back(osgEarth::Color(orbitColor));
            orbitGeom->setColorArray(colorData, osg::Array::BIND_OVERALL);

            osg::ref_ptr<osg::Geode> orbitGeode = new osg::Geode;
            orbitGeode->addDrawable(orbitGeom.get());
            scene->asGroup()->addChild(orbitGeode);
        }

        std::string coneColor = par("coneColor");
        if (!coneColor.empty()) {
            double orbitRadius = earthRadius + altitude; // in kilometers
            // the angle between the center of the earth and the horizon as seen from the satellite, in radians
            double alpha = std::asin(earthRadius / orbitRadius);
            // the distance of the horizon from the satellite, in meters
            double horizonDistance = std::sqrt(orbitRadius * orbitRadius - earthRadius * earthRadius) * 1000;
            double coneHeight = std::sin(alpha)*horizonDistance;
            double coneRadius = std::cos(alpha)*horizonDistance;
            // the offset is to position the tip to the satellite
            osg::Cone *cone = new osg::Cone(osg::Vec3(0, 0, -coneRadius*0.75), coneHeight, coneRadius);

            osg::ref_ptr<osg::Geode> coneGeode = new osg::Geode;
            auto coneDrawable = new osg::ShapeDrawable(cone);
            coneDrawable->setColor(osgEarth::Color(coneColor));

            coneGeode->addDrawable(coneDrawable);
            coneGeode->getOrCreateStateSet()->setAttribute(new osg::PolygonMode(
                osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
            coneGeode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            auto depth = new osg::Depth;
            depth->setWriteMask(false);
            coneGeode->getOrCreateStateSet()->setAttributeAndModes(depth, osg::StateAttribute::ON);
            locatorNode->addChild(coneGeode);
        }

        // position the nodes, so we will see them at correct position right after initialization
        refreshVisuals();

        // schedule first move
        cMessage *timer = new cMessage("move");
        scheduleAt(0, timer);
        break;
    }
}

void Satellite::handleMessage(cMessage *msg)
{
    // update the node position
    move();

    // synchronize the OSG node positions to the module position
    refreshVisuals();

    // schedule next movement
    scheduleAt(simTime() + timeStep, msg);
}

void Satellite::refreshVisuals()
{
    osg::Vec3d pos = getPosition();

	osg::Vec3d v;
    mapNode->getMap()->getSRS()->transformFromWorld(pos, v);
    locatorNode->getLocator()->setPosition(v);

    ChannelController::getInstance()->updateConnectionGraph();

    // update the position on the 2D canvas, too*/
    getDisplayString().setTagArg("p", 0, 300 + pos.x() / 100000);
    getDisplayString().setTagArg("p", 1, 300 - pos.y() / 100000);
}

void Satellite::move()
{
    phase = startingPhase + (getSimulation()->getSimTime().inUnit(SIMTIME_MS) / 1000.0) * getOmega();
}

#endif // WITH_OSG