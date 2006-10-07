//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#ifndef __QUEUEBASE_H
#define __QUEUEBASE_H

#include <omnetpp.h>

/**
 * Abstract base class for single-server queues.
 */
class QueueBase : public cSimpleModule
{
  private:
    cMessage *msgServiced;
    cMessage *endServiceMsg;
    cQueue queue;

  public:
    QueueBase();
    virtual ~QueueBase();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    // hook functions to (re)define behaviour
    virtual void arrival(cMessage *msg) {}
    virtual simtime_t startService(cMessage *msg) = 0;
    virtual void endService(cMessage *msg) = 0;
};

#endif

