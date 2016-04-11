//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

using namespace omnetpp;

class Clock : public cSimpleModule
{
  protected:
    simtime_t timeStep;
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Clock);

void Clock::initialize()
{
    timeStep = par("timeStep");
    scheduleAt(timeStep, new cMessage("timer"));
}

void Clock::handleMessage(cMessage *msg)
{
    scheduleAt(simTime() + timeStep, msg);
}
