//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Satellite.h"

#include <osg/Node>
#include <osg/Texture>
#include <osg/LineWidth>
#include <osg/PolygonMode>
#include <osg/Texture2D>
#include <osg/Image>
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


        phase = startingPhase = par("startingPhase");

        normal = osg::Vec3d(std::cos(getIndex()), std::sin(getIndex()), 4);
        normal.normalize();

        auto c1 = normal ^ osg::Vec3d(1, 0, 0);
        auto c2 = normal ^ osg::Vec3d(0, 1, 0);
        osg::Vec3d &cross = c1.length2() < 0.1 ? c2 : c1;

        cross.normalize();
        orbitX = cross;
        orbitY = cross ^ normal;

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
        mapNode->addChild(locatorNode);

        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        osg::ref_ptr<osg::DrawArrays> drawArrayLines = new  osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP);

        osg::ref_ptr<osg::Vec3Array> vertexData = new osg::Vec3Array;


        geom->addPrimitiveSet(drawArrayLines);
        geom->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
        geom->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
        geom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        geom->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(1.5), osg::StateAttribute::ON);

        for (int i = 0; i <= 100; ++i)
            vertexData->push_back(getPositionAtPhase(i / 100.0 * M_PI*2));


        geom->setVertexArray(vertexData);
        drawArrayLines->setFirst(0);
        drawArrayLines->setCount(vertexData->size());


       osg::ref_ptr<osg::Vec4Array> colorData = new osg::Vec4Array;
       colorData->push_back(osg::Vec4(0.5, 0.75, 0.75, 0.75));
       geom->setColorArray(colorData, osg::Array::BIND_OVERALL);

       // Add the Geometry (Drawable) to a Geode and return the Geode.
       osg::ref_ptr<osg::Geode> geode = new osg::Geode;
       geode->addDrawable( geom.get() );

       mapNode->addChild(geode);

       double R = earthRadius + altitude;
       double alpha = std::asin(earthRadius / R);
       double th = std::sqrt(R * R - earthRadius * earthRadius);
       double h = std::sin(alpha)*th*1000;
       double r = std::cos(alpha)*th*1000;
       osg::Cone *cone = new osg::Cone(osg::Vec3(0, 0, -r*0.75), h, r);

       osg::ref_ptr<osg::Geode> geode2 = new osg::Geode;
       auto shdr = new osg::ShapeDrawable(cone);
       shdr->setColor(osg::Vec4(1, 1, 1, 0.1));

        geode2->addDrawable(shdr);
        geode2->getOrCreateStateSet()->setAttribute(new osg::PolygonMode(
          osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ));

       locatorNode->addChild(geode2);

        // position the nodes, so we will see them at correct position right after initialization
        refreshVisuals();

        // schedule first move
        cMessage *timer = new cMessage("move");
        scheduleAt(0, timer);
        break;
    }
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

void Satellite::handleMessage(cMessage *msg)
{
    // update the node position
    move();

    // synchronize the OSG node positions to the module position
    refreshVisuals();

    // schedule next movement
    scheduleAt(simTime() + timeStep, msg);
}
