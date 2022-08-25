//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "ChannelController.h"

#ifdef WITH_OSGEARTH

#include <osg/PositionAttitudeTransform>
#include <osgEarth/Version>


using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace osgEarth::Features;

Define_Module(ChannelController);

ChannelController *ChannelController::instance = nullptr;

ChannelController::ChannelController()
{
    if (instance)
        throw cRuntimeError("There can be only one ChannelController instance in the network");
    instance = this;
}

ChannelController::~ChannelController()
{
    instance = nullptr;
}

ChannelController *ChannelController::getInstance()
{
    if (!instance)
        throw cRuntimeError("ChannelController::getInstance(): there is no ChannelController module in the network");
    return instance;
}

int ChannelController::findMobileNode(IMobileNode *p)
{
    for (int i = 0; i < (int)nodeList.size(); i++)
        if (nodeList[i] == p)
            return i;

    return -1;
}

void ChannelController::addMobileNode(IMobileNode *p)
{
    if (findMobileNode(p) == -1)
        nodeList.push_back(p);
}

void ChannelController::removeMobileNode(IMobileNode *p)
{
    int k = findMobileNode(p);
    if (k != -1)
        nodeList.erase(nodeList.begin()+k);
}

void ChannelController::initialize(int stage)
{
    switch (stage) {
    case 0:
        playgroundLat = getSystemModule()->par("playgroundLatitude");
        playgroundLon = getSystemModule()->par("playgroundLongitude");
        connectionColor = par("connectionColor").stringValue();
        showConnections = par("showConnections").boolValue();
        break;
    case 1:
        auto scene = OsgEarthScene::getInstance()->getScene(); // scene is initialized in stage 0 so we have to do our init in stage 1
        mapNode = osgEarth::MapNode::findMapNode(scene);
        connectionStyle.getOrCreate<LineSymbol>()->stroke()->color() = osgEarth::Color(connectionColor);
        connectionStyle.getOrCreate<LineSymbol>()->stroke()->width() = 20.0f;
        connectionStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_ABSOLUTE;
        connectionStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;

        if (showConnections) {
            auto geoSRS = mapNode->getMapSRS()->getGeographicSRS();
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2, 10, 0)
            connectionGraphNode = new FeatureNode(new Feature(new LineString(), geoSRS));
#else
            connectionGraphNode = new FeatureNode(mapNode.get(), new Feature(new LineString(), geoSRS));
#endif
            connectionGraphNode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2, 10, 0)
            mapNode->addChild(connectionGraphNode);
#else
            mapNode->getModelLayerGroup()->addChild(connectionGraphNode);
#endif
        }
        break;
    }
}

void ChannelController::refreshDisplay() const
{
    if (!showConnections)
        return;

    auto geoSRS = mapNode->getMapSRS()->getGeographicSRS();
    auto connectionGeometry = new osgEarth::Symbology::MultiGeometry();

    for (int i = 0; i < (int)nodeList.size(); ++i) {
        for (int j = i+1; j < (int)nodeList.size(); ++j) {
            IMobileNode *pi = nodeList[i];
            IMobileNode *pj = nodeList[j];
            double ix = pi->getX(), iy = pi->getY(), jx = pj->getX(), jy = pj->getY();
            if (pi->getTxRange()*pi->getTxRange() > (ix-jx)*(ix-jx)+(iy-jy)*(iy-jy)) {
                auto ls = new LineString(2);  // a single line geometry
                ls->push_back(osg::Vec3d(pi->getLongitude(), pi->getLatitude(), 1.5));
                ls->push_back(osg::Vec3d(pj->getLongitude(), pj->getLatitude(), 1.5));
                connectionGeometry->add(ls);
            }
        }
    }

    auto cgraphFeature = new Feature(connectionGeometry, geoSRS, connectionStyle);
    cgraphFeature->geoInterp() = GEOINTERP_GREAT_CIRCLE;
    connectionGraphNode->setFeature(cgraphFeature);
}

void ChannelController::handleMessage(cMessage *msg)
{
    throw cRuntimeError("This module does not process messages");
}

#endif // WITH_OSGEARTH
