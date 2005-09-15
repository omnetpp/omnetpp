//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Authors: Gabor Lencse, Andras Varga (TU Budapest)
// Based on the code by:
//          Maurits Andre, George van Montfort,
//          Gerard van de Weerd (TU Delft)
//


#ifndef __TOKEN_H
#define __TOKEN_H

#include <omnetpp.h>
#include "token_m.h"

#define STACKSIZE 16384


/**
 * Token Ring MAC layer; see NED file for more info.
 */
class TokenRingMAC : public cSimpleModule
{
  private:
    int myAddress;
    long dataRate;
    double tokenHoldingTime;
    int queueMaxLen;

    cQueue sendQueue;
    int sendQueueBytes;
    cOutVector queueLenPackets;
    cOutVector queueLenBytes;
    cOutVector queueingTime;
    int numPacketsToSend;
    int numPacketsToSendDropped;
    cOutVector queueDrops;

    cMessage *transmEnd;
    cMessage *recvEnd;

    bool debug;

  public:
    TokenRingMAC();
    virtual ~TokenRingMAC();

  protected:
    virtual void activity();
    virtual void finish();
    virtual void storeDataPacket(TRApplicationData *data);
    virtual void beginReceiveFrame(TRFrame *frame);
    virtual void endReceiveFrame(cMessage *data);
};


/**
 * Generates traffic; see NED file for more info.
 */
class Generator : public cSimpleModule
{
  public:
    Generator() : cSimpleModule(STACKSIZE) {}
    virtual void activity();
};


/**
 * Consumes packets; see NED file for more info.
 */
class Sink : public cSimpleModule
{
  private:
    // output vector to record statistics
    cOutVector endToEndDelay;

    // histograms
    cKSplit endToEndDelayKS;
    cPSquare endToEndDelayPS;

    bool debug;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif


