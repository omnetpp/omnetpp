//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

#ifndef WITH_OSG

// fallback code used in case OpenSceneGraph and osgEarth is not present on the system
using namespace omnetpp;

class OsgScene : public cSimpleModule
{
  protected:
    virtual void initialize() { throw cRuntimeError("This example requires OpenSceneGraph and osgEarth installed"); }
    virtual void handleMessage(cMessage *msg) {};
};

Define_Module(OsgScene);

#endif // WITH_OSG
