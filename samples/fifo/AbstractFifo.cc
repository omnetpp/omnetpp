//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "AbstractFifo.h"

namespace fifo {

AbstractFifo::AbstractFifo()
{
    msgServiced = endServiceMsg = NULL;
}

AbstractFifo::~AbstractFifo()
{
    delete msgServiced;
    cancelAndDelete(endServiceMsg);
}

void AbstractFifo::initialize()
{
    endServiceMsg = new cMessage("end-service");
    queue.setName("queue");

    qlenSignal = registerSignal("qlen");
    busySignal = registerSignal("busy");
    queueingTimeSignal = registerSignal("queueingTime");
    serviceStartSignal = registerSignal("serviceStart");
    emit(qlenSignal, (long)queue.length());
    emit(busySignal, (long)0);
}

void AbstractFifo::handleMessage(cMessage *msg)
{
    if (msg==endServiceMsg)
    {
        endService( msgServiced );
        if (queue.empty())
        {
            msgServiced = NULL;
            emit(busySignal, (long)0);
        }
        else
        {
            msgServiced = (cMessage *) queue.pop();
            emit(qlenSignal, (long)queue.length());
            emit(queueingTimeSignal, simTime() - msgServiced->getTimestamp());
            simtime_t serviceTime = startService( msgServiced );
            scheduleAt( simTime()+serviceTime, endServiceMsg );
            emit(serviceStartSignal, serviceTime);
        }
    }
    else if (!msgServiced)
    {
        arrival( msg );
        msgServiced = msg;
        emit(queueingTimeSignal, 0.0);
        simtime_t serviceTime = startService( msgServiced );
        emit(serviceStartSignal, serviceTime);
        scheduleAt( simTime()+serviceTime, endServiceMsg );
        emit(busySignal, (long)1);
    }
    else
    {
        arrival( msg );
        queue.insert( msg );
        msg->setTimestamp();
        emit(qlenSignal, (long)queue.length());
    }
}

}; //namespace

