//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <osgEarthUtil/ObjectLocator>
#include <osgEarthUtil/LinearLineOfSight>
#include <osg/ValueObject>
#include "ChannelController.h"

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

// modified, source: http://forum.openscenegraph.org/viewtopic.php?t=7805&view=previous
osg::ref_ptr<osg::ShapeDrawable> ChannelController::createCylinderBetweenPoints(osg::Vec3 start, osg::Vec3 end, float radius, osg::Vec4 color) {
    osg::Vec3 center = (start + end) / 2;
    float height = (start - end).length();

    // This is the default direction for the cylinders to face in OpenGL
    osg::Vec3 z(0, 0, 1);

    // Get diff between two points you want cylinder along
    osg::Vec3 p = (start - end);

    // Get CROSS product (the axis of rotation)
    osg::Vec3 t = z ^ p;

    // Get angle. length is magnitude of the vector
    double angle = std::acos((z * p) / p.length());

    // Create a cylinder between the two points with the given radius
    osg::ref_ptr<osg::Cylinder> cylinder = new osg::Cylinder(center, radius, height);
    cylinder->setRotation(osg::Quat(angle, t));

    osg::ref_ptr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(cylinder);
    drawable->setColor(color);
    return drawable;
}

int ChannelController::findSatellite(Satellite *p)
{
    for (int i = 0; i < (int)satellites.size(); i++)
        if (satellites[i] == p)
            return i;

    return -1;
}

int ChannelController::findGroundStation(GroundStation *p)
{
    for (int i = 0; i < (int)stations.size(); i++)
        if (stations[i] == p)
            return i;

    return -1;
}

void ChannelController::addLineOfSight(osg::Node *a, osg::Node *b, int type) {
    auto mapNode = osgEarth::MapNode::findMapNode(scene);
    osgEarth::Util::LinearLineOfSightNode *los = new osgEarth::Util::LinearLineOfSightNode(mapNode);
    losNodes.push_back(los);

    // not drawing the line of sight nodes' lines
    los->setGoodColor(osg::Vec4f(0, 0, 0, 0));
    los->setBadColor(osg::Vec4f(0, 0, 0, 0));
    los->setUserValue("type", type);

    los->setUpdateCallback(new osgEarth::Util::LineOfSightTether(a, b));
    los->setTerrainOnly(true); // so the dish model itself won't occlude

    scene->addChild(los);
}

void ChannelController::addSatellite(Satellite *p)
{
    if (findSatellite(p) == -1)
        satellites.push_back(p);
    updateConnectionGraph();
}

void ChannelController::removeSatellite(Satellite *p)
{
    int k = findSatellite(p);
    if (k != -1)
        satellites.erase(satellites.begin()+k);
}


void ChannelController::addGroundStation(GroundStation *p)
{
    if (findGroundStation(p) == -1)
        stations.push_back(p);
    updateConnectionGraph();
}

void ChannelController::removeGroundStation(GroundStation *p)
{
    int k = findGroundStation(p);
    if (k != -1)
        stations.erase(stations.begin()+k);
}

void ChannelController::initialize(int stage)
{
    switch (stage) {
    case 0:
        connectionColor = par("connectionColor").stringValue();
        showConnections = par("showConnections").boolValue();
        break;
    case 1:
        connections = new osg::Geode();
        scene = OsgEarthScene::getInstance()->getScene()->asGroup();
        scene->addChild(connections);
        break;
    case 2:
        for (int i = 0; i < (int)satellites.size(); ++i) {
            for (int j = 0; j < (int)stations.size(); ++j)
                addLineOfSight(satellites[i]->getLocatorNode(), stations[j]->getLocatorNode(), 0);

            for (int j = i+1; j < (int)satellites.size(); ++j)
                addLineOfSight(satellites[i]->getLocatorNode(), satellites[j]->getLocatorNode(), 1);
        }
    	break;
    }
}

void ChannelController::updateConnectionGraph()
{
	connections->removeDrawables(0, connections->getNumDrawables());
	auto s2s = osg::Vec4(0, 1, 0, 0.75);
	auto s2g = osg::Vec4(1, 0.5, 0, 0.75);

	int ns2s = 0;
	int ns2g = 0;
	//static int id = 0;
    if (showConnections)
		for (auto n : losNodes)
			if (n->getHasLOS()) {
			    auto start = n->getStartWorld();
			    auto end = n->getEndWorld();
			    /*std::stringstream name;
			    name << "conn-" << id++;
			    auto line = new cLineFigure(name.str().c_str());
			    line->setVisible(true);
			    line->setLineWidth(2);
			    line->setLineColor(cFigure::Color("green"));
			    line->setStart(cFigure::Point(300 + start.x() / 100000, 300 - start.y() / 100000));
			    line->setEnd(cFigure::Point(300 + end.x() / 100000, 300 - end.y() / 100000));
			    EV << "owner:" << getParentModule()->getFullName() << "\n";
			    getParentModule()->getCanvas()->addFigure(line);*/
			    int type;
			    n->getUserValue("type", type);
			    switch (type) {
			    case 0: // sat to ground
			        connections->addDrawable(createCylinderBetweenPoints(start, end, 20000, s2g));
			        ++ns2g;
			        break;
			    case 1: // sat to sat
			        connections->addDrawable(createCylinderBetweenPoints(start, end, 20000, s2s));
			        ++ns2s;
			        break;
			    }

			}

    EV << "Active connections: " << ns2s << " sat-to-sat and " << ns2g << " sat-to-ground \n";
}

void ChannelController::handleMessage(cMessage *msg)
{
    throw cRuntimeError("This module does not process messages");
}
