//-------------------------------------------------------------
// File: token.h
//
// Implementation of simple module types
//
// Authors: Gabor Lencse, Andras Varga (TU Budapest)
// Based on the code by:
//          Maurits Andre, George van Montfort,
//          Gerard van de Weerd (TU Delft)
//-------------------------------------------------------------


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

    cQueue sendQueue;
    int sendQueueBytes;
    cOutVector queueLenPackets;
    cOutVector queueLenBytes;
    cOutVector queueingTime;

    cMessage *transmEnd;
    cMessage *recvEnd;

    bool debug;

    virtual void activity();
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
    Module_Class_Members(Sink,cSimpleModule,16384)
    virtual void activity();
};

#endif


