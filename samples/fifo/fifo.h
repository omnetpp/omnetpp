//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#ifndef __FIFO_H
#define __FIFO_H

#include <omnetpp.h>

/**
 * Abstract base class for single-server queues.
 */
class FFAbstractFifo : public cSimpleModule
{
  protected:
    cMessage *msgServiced;
    cMessage *endServiceMsg;
    cQueue queue;

  public:
    FFAbstractFifo();
    virtual ~FFAbstractFifo();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    // hook functions to (re)define behaviour
    virtual void arrival(cMessage *msg) {}
    virtual simtime_t startService(cMessage *msg) = 0;
    virtual void endService(cMessage *msg) = 0;
};

/**
 * Single-server queue with given service time.
 */
class FFPacketFifo : public FFAbstractFifo
{
  protected:
    virtual simtime_t startService(cMessage *msg);
    virtual void endService(cMessage *msg);
};

/**
 * Single-server queue with service time based on message length.
 */
class FFBitFifo : public FFAbstractFifo
{
  protected:
    virtual simtime_t startService(cMessage *msg);
    virtual void endService(cMessage *msg);
};

#endif
