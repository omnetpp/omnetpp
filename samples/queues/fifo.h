//
// This file is part of an OMNeT++ simulation example.
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
  public:
    Module_Class_Members(Fifo,cSimpleModule,0);

    cMessage *msgServiced;
    cMessage *endServiceMsg;
    cQueue queue;

    // Note that jobDist is a pointer, and jobsInSys is not. I included both ways to show you
    // two ways of doing the same kind of thing.
    cDoubleHistogram *jobDist;
    cOutVector jobsInSys;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    void arrival(cMessage *msg) {}
    simtime_t serviceRequirement(cMessage *msg);
    void endService(cMessage *msg);
};

#endif

