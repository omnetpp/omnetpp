//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __FIFO_H
#define __FIFO_H

#include <omnetpp.h>

/**
 * Abstract base class for single-server queues
 */
class Fifo : public cSimpleModule
{
  protected:
    cMessage *msgServiced;
    cMessage *endServiceMsg;
    cQueue queue;

    cDoubleHistogram *jobDist;
    cOutVector jobsInSys;

  public:
    Fifo() {}   //NEWCTOR

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    void arrival(cMessage *msg) {}
    simtime_t serviceRequirement(cMessage *msg);
    void endService(cMessage *msg);
};

#endif

