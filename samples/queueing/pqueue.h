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

class QPassiveQueue : public cSimpleModule
{
  protected:
    cQueue queue;
    int jobRequestedOnGate;

  public:
    QPassiveQueue() {}   //NEWCTOR
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    // Following methods are called from QServer:
    virtual bool isEmpty();
    virtual void request(int gateIndex);
    virtual void storeRequest(int gateIndex);
};

#endif

