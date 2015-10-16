//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __MOBILENODE_H__
#define __MOBILENODE_H__

#include <osgEarth/MapNode>
#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthUtil/ObjectLocator>

#include <omnetpp.h>
#include "OsgEarthScene.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

using namespace omnetpp;

/**
 * A mobile node (with a 3D model) moving around . A range indicator, and the
 * model's track can be shown along with its label.
 */
class Satellite : public cSimpleModule
{
  protected:
    // configuration
    double timeStep;
    double startingPhase;
    std::string labelColor;
    std::string modelURL;
    double modelScale;

    // the node containing the osgEarth data
    osg::observer_ptr<osgEarth::MapNode> mapNode = nullptr;
    // osgEarth node for 3D visualization
    osgEarth::Util::ObjectLocatorNode *locatorNode = nullptr;

    const double mu = 398600.4418; // "geocentric gravitational constant" - source: wikipedia, units: km^3 / s^2
    const double earthRadius = 6371; // in km

    double altitude = 10000; // in km, above the surface
    double phase = 0; // on the orbit, in radians, unbounded
    osg::Vec3d normal = osg::Vec3d(0, 0, 1); // doesn't have to be unit length, just can't be 0
    osg::Vec3d orbitX, orbitY; // the base of the orbit plane, orthogonal, and both are unit length, computed from the normal

  public:
    osg::Vec3d getPosition() { return getPositionAtPhase(phase); }
    osg::Node *getLocatorNode() { return locatorNode; };

  protected:
    // angular velocity in rad/sec
    double getOmega() { return std::sqrt(mu / std::pow(altitude + earthRadius, 3)); }

    // in world coordinates, units is meters, phase is the angle on the orbit in radians
    osg::Vec3 getPositionAtPhase(double alpha) {
        return (orbitX * std::cos(alpha) + orbitY * std::sin(alpha)) * (earthRadius + altitude /* km */) * 1000;
    }

    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 2; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshVisuals();
    virtual void move();
};

#endif
