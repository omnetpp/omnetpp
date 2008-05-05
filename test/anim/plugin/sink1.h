//
// sink1.h
//
// This file is part of an OMNeT++/OMNEST demo simulation.
//

#ifndef __SINK_H
#define __SINK_H

#include <omnetpp.h>

class FF1Sink : public cSimpleModule
{
  private:
    cStdDev qstats; // needs to be accessed from finish() too
  public:
    FF1Sink() : cSimpleModule(16384) {}
    virtual void activity();
    virtual void finish();
};

#endif
