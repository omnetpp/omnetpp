//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __WAYPOINTTRACKERNODE_H__
#define __WAYPOINTTRACKERNODE_H__

#include <vector>
#include <string>
#include <omnetpp.h>
#include "MobileNode.h"

using namespace omnetpp;

// a structure to store time coded waypoints
struct Waypoint
{
    Waypoint(double x, double y, double timestamp) {
        this->x = x;
        this->y = y;
        this->timestamp = timestamp;
    }
    double x;
    double y;
    double timestamp;
};

typedef std::vector<Waypoint> WaypointVector;

/**
 * A mobile node that follows a predefined track.
 */
class WaypointTrackerNode : public MobileNode
{
  protected:
    // configuration
    WaypointVector waypoints;

    double speed;
    double  waypointProximity;
    double angularSpeed;
    int targetPointIndex;

  public:
    WaypointTrackerNode();
    virtual ~WaypointTrackerNode();

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return 2; }
    virtual void move() override;
    void readWaypointsFromFile(const char *fileName);
};

#endif
