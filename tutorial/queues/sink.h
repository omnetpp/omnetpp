//-------------------------------------------------------------
// file: sink.h
//        (part of Queues - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#ifndef __SINK_H
#define __SINK_H

#include <omnetpp.h>

class Sink : public cSimpleModule
{
    Module_Class_Members(Sink,cSimpleModule,8192)

    cStdDev waitStats;

    virtual void initialize();
    virtual void activity();
    virtual void finish();
};

#endif













