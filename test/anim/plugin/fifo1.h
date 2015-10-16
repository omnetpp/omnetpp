//
// fifo1.h
//
// This file is part of an OMNeT++/OMNEST demo simulation.
//

#ifndef __FIFO_H
#define __FIFO_H

#include <omnetpp.h>

using namespace omnetpp;

/**
 * FF1AbstractFifo : abstract base class for single-server queues
 */
class FF1AbstractFifo : public cSimpleModule
{
  public:
    FF1AbstractFifo() : cSimpleModule(16384) {}

    cMessage *msgServiced;
    cMessage *endServiceMsg;
    cQueue queue;

    virtual void activity() override;
    virtual void finish() override;

    // hook functions to (re)define behaviour
    virtual void arrival(cMessage *msg) {}
    virtual simtime_t startService(cMessage *msg) = 0;
    virtual void endService(cMessage *msg) = 0;
};

/**
 * FF1PacketFifo : single-server queue with given service time
 */
class FF1PacketFifo : public FF1AbstractFifo
{
  public:
    FF1PacketFifo() {}
    virtual simtime_t startService(cMessage *msg) override;
    virtual void endService(cMessage *msg) override;
};

/**
 * FF1BitFifo : single-server queue with service time based on message length
 */
class FF1BitFifo : public FF1AbstractFifo
{
  public:
    FF1BitFifo() {}
    virtual simtime_t startService(cMessage *msg) override;
    virtual void endService(cMessage *msg) override;
};

#endif

