//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "QueueBase.h"

using namespace omnetpp;

QueueBase::~QueueBase()
{
    delete msgServiced;
    cancelAndDelete(endServiceMsg);
}

void QueueBase::initialize()
{
    endServiceMsg = new cMessage("end-service");
    queue.setName("queue");
}

void QueueBase::handleMessage(cMessage *msg)
{
    if (msg == endServiceMsg) {
        endService(msgServiced);
        if (queue.isEmpty()) {
            msgServiced = nullptr;
        }
        else {
            msgServiced = (cMessage *)queue.pop();
            simtime_t serviceTime = startService(msgServiced);
            scheduleAt(simTime()+serviceTime, endServiceMsg);
        }
    }
    else if (!msgServiced) {
        arrival(msg);
        msgServiced = msg;
        simtime_t serviceTime = startService(msgServiced);
        scheduleAt(simTime()+serviceTime, endServiceMsg);
    }
    else {
        arrival(msg);
        queue.insert(msg);
    }
}

