//-------------------------------------------------------------
// file: sink.cc
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#ifndef __SINK_H
#define __SINK_H

#include "omnetpp.h"

class Sink : public cSimpleModule
{
    Module_Class_Members(Sink,cSimpleModule,16384)

    cStdDev qstats; // needs to be accessed from finish() too

    virtual void activity();
    virtual void finish();
};

#endif
