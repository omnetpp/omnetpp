//
// This file is part of an OMNeT++ simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __SINK_H
#define __SINK_H

#include <omnetpp.h>

class Sink : public cSimpleModule
{
    Module_Class_Members(Sink,cSimpleModule,0)

    cStdDev waitStats;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif













