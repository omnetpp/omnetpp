//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2004 Andras Varga
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

class TokenRingMAC : public cSimpleModule
{
    Module_Class_Members(TokenRingMAC,cSimpleModule,16384)

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

    virtual void activity();
    virtual void finish();
    virtual void storeDataPacket(TRApplicationData *data);
    virtual void beginReceiveFrame(TRFrame *frame);
    virtual void endReceiveFrame(cMessage *data);
};

class Generator : public cSimpleModule
{
    Module_Class_Members(Generator,cSimpleModule,16384)
    virtual void activity();
};

class Sink : public cSimpleModule
{
  protected:
    // output vector to record statistics
    cOutVector endToEndDelay;

    // histograms
    cKSplit endToEndDelayKS;
    cPSquare endToEndDelayPS;

    bool debug;

  public:
    Module_Class_Members(Sink,cSimpleModule,0)
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif


