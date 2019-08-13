//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __CAR_H
#define __CAR_H

#include "OpenStreetMap.h"

using namespace omnetpp;

namespace osm {

// Driver + RoutePlanner
class RoutePlanner;

class Driver : public cSimpleModule
{
  protected:
    OpenStreetMap *osmModule = nullptr;
    RoutePlanner *routePlanner = nullptr;
    typedef cFigure::Point Point;
    cFigure *carFigure;
    Point p1,p2;
    simtime_t t1,t2;
  protected:
    virtual int numInitStages() const override {return 2;}
    virtual void initialize(int stage) override;
    virtual cFigure *createCarFigure();
    virtual Point getNextTarget();
    virtual simtime_t computeTravelTime(const Point& from, const Point& to);
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
};


class RoutePlanner : public cSimpleModule
{
    OpenStreetMap *osmModule = nullptr;
    const Way *way = nullptr;
    int nodeIndex; // driving towards this node
    int downstream; // driving direction on way: true=increasing node indices, false=decreasing node indices
  protected:
    virtual void initialize() override;
  public:
    virtual const Node *getNextTarget();
};



}; //namespace

#endif
