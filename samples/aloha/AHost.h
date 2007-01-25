//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __AHOST_H_
#define __AHOST_H_

#include <omnetpp.h>


/**
 * Aloha host; see NED file for more info.
 */
class AHost : public cSimpleModule
{
  private:
    // parameters
    double radioDelay;
    double txRate;
    cPar *iaTime;
    cPar *pkLenBits;
    simtime_t slotTime;
    bool isSlotted;

    // state variables, event pointers etc
    cModule *server;
    cMessage *endTxEvent;
    enum {IDLE=0, TRANSMIT=1} state;
    int pkCounter;

  public:
    AHost();
    virtual ~AHost();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif

