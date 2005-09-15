//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __SINK_H
#define __SINK_H

#include <omnetpp.h>

/**
 * Consumes packets; see NED file for more info.
 */
class Sink : public cSimpleModule
{
  private:
    cStdDev waitStats;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif













