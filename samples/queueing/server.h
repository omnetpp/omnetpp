//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __SERVER_H
#define __SERVER_H

#include <omnetpp.h>

class QPassiveQueue;

class QServer : public cSimpleModule
{
  private:
    int numQueues;
    QPassiveQueue **queues;

    cMessage *msgServiced;
    cMessage *endServiceMsg;
    bool hasAcceptedOffer;

  public:
    QServer();
    virtual ~QServer();

  protected:
    virtual void initialize(int stage);
    virtual int numInitStages() const {return 2;}
    virtual void handleMessage(cMessage *msg);

  public:
    // The following method is called from QPassiveQueue:
    virtual bool okToSend();
};

#endif


