//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2004 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>
#include "fifo.h"


void FFAbstractFifo::initialize()
{
    msgServiced = NULL;
    endServiceMsg = new cMessage("end-service");
    queue.setName("queue");
}

void FFAbstractFifo::handleMessage(cMessage *msg)
{
    if (msg==endServiceMsg)
    {
        endService( msgServiced );
        if (queue.empty())
        {
            msgServiced = NULL;
        }
        else
        {
            msgServiced = (cMessage *) queue.getTail();
            simtime_t serviceTime = startService( msgServiced );
            scheduleAt( simTime()+serviceTime, endServiceMsg );
        }
    }
    else if (!msgServiced)
    {
        arrival( msg );
        msgServiced = msg;
        simtime_t serviceTime = startService( msgServiced );
        scheduleAt( simTime()+serviceTime, endServiceMsg );

    }
    else
    {
        arrival( msg );
        queue.insert( msg );
    }
}

//------------------------------------------------

Define_Module( FFPacketFifo );

simtime_t FFPacketFifo::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return par("serviceTime");
}

void FFPacketFifo::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;
    send( msg, "out" );
}

//------------------------------------------------

Define_Module( FFBitFifo );

simtime_t FFBitFifo::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return msg->length() / (double)par("bitsPerSec");
}

void FFBitFifo::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;
    send( msg, "out" );
}

