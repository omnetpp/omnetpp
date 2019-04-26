//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#if defined(WITH_OSG) && defined(WITH_OSGEARTH)
#include "OsgEarthScene.h"

#include <osgDB/ReadFile>
#include <osgEarth/Version>
#include <osgEarth/Viewpoint>
#include <osgEarth/MapNode>
#include <osgEarth/Capabilities>
#include <osgEarthAnnotation/RectangleNode>

using namespace omnetpp;
using namespace osgEarth;
using namespace osgEarth::Annotation;

Define_Module(OsgEarthScene);

OsgEarthScene *OsgEarthScene::instance = nullptr;

OsgEarthScene::OsgEarthScene()
{
    if (instance)
        throw cRuntimeError("There can be only one OsgEarthScene instance in the network");
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

    playgroundLat = getSystemModule()->par("playgroundLatitude");
    playgroundLon = getSystemModule()->par("playgroundLongitude");
    playgroundHeight = getSystemModule()->par("playgroundHeight");
    playgroundWidth = getSystemModule()->par("playgroundWidth");
    double centerLongitude = toLongitude(playgroundWidth/2);
    double centerLatitude = toLatitude(playgroundHeight/2);

    cOsgCanvas *builtinOsgCanvas = getParentModule()->getOsgCanvas();

    auto mapNode = MapNode::findMapNode(scene);
    ASSERT(mapNode != nullptr);

    // set up viewer
    const SpatialReference *geoSRS = mapNode->getMapSRS()->getGeographicSRS();
    builtinOsgCanvas->setViewerStyle(cOsgCanvas::STYLE_EARTH);
    // and move the initial view right above it
    builtinOsgCanvas->setEarthViewpoint(cOsgCanvas::EarthViewpoint(centerLongitude, centerLatitude, 50, 0, -90, playgroundHeight*2));
    // fine tune the ZLimits (clipping) to better fit this scenario
    builtinOsgCanvas->setZLimits(1, 100000);
    builtinOsgCanvas->setScene(scene);

    // set up an annotation to show the playground area
    Style rectStyle;
    rectStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Black, 0.3);
    rectStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
    rectStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
        RectangleNode *rect = new RectangleNode(
#if OSGEARTH_VERSION_LESS_THAN(2, 10, 0)
        mapNode,
#endif
        GeoPoint(geoSRS, centerLongitude, centerLatitude),
        Linear(playgroundWidth, Units::METERS),
        Linear(playgroundHeight, Units::METERS),
        rectStyle);
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2, 10, 0)
    mapNode->addChild(rect);
#else
    mapNode->getModelLayerGroup()->addChild(rect);
#endif
}

OsgEarthScene *OsgEarthScene::getInstance()
{
    if (!instance)
        throw cRuntimeError("OsgEarthScene::getInstance(): there is no OsgEarthScene module in the network");
    return instance;
}

void OsgEarthScene::handleMessage(cMessage *msg)
{
    throw cRuntimeError("This module does not handle messages from the outside");
}

#endif // WITH_OSG
