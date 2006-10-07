//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __PQUEUE_H
#define __PQUEUE_H

#include <omnetpp.h>

/**
 * A passive queue, designed to co-operate with QServer using method calls.
 */
class QPassiveQueue : public cSimpleModule
{
  private:
    cQueue queue;
    int jobRequestedOnGate;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

  public:
    // The following methods are called from QServer:
    virtual bool isEmpty();
    virtual void request(int gateIndex);
    virtual void storeRequest(int gateIndex);
};

#endif

