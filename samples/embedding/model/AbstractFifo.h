//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __ABSTRACTFIFO_H
#define __ABSTRACTFIFO_H

#include <omnetpp.h>

using namespace omnetpp;

namespace fifo {

/**
 * Abstract base class for single-server queues. Subclasses should
 * define startService() and endService(), and may override other
 * virtual functions.
 */
class AbstractFifo : public cSimpleModule
{
  protected:
    cMessage *msgServiced = nullptr;
    cMessage *endServiceMsg = nullptr;
    cQueue queue;

  public:
    virtual ~AbstractFifo();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    // hook functions to (re)define behaviour
    virtual void arrival(cMessage *msg) {}
    virtual simtime_t startService(cMessage *msg) = 0;
    virtual void endService(cMessage *msg) = 0;
};

}; //namespace

#endif
