//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "stressqueue.h"
#include "stress_m.h"

Define_Module(StressQueue);

StressQueue::StressQueue()
{
    timer = new cMessage("Queue timer");
}

StressQueue::~StressQueue()
{
    EV << "Cancelling and deleting self message: "  << timer << "\n";
    cancelAndDelete(timer);
}

void StressQueue::handleMessage(cMessage *msg)
{
    cMessage *sendOutMsg = nullptr;

    if (msg == timer) {
        if (!queue.isEmpty()) {
            sendOutMsg = (cMessage *)queue.pop();

            EV << "Sending out queued message: "  << sendOutMsg << "\n";
        }
    }
    else {
        if (!timer->isScheduled()) {
            sendOutMsg = msg;
            EV << "Immediately sending out message: "  << sendOutMsg << "\n";
        }
        else {
            EV << "Queuing message: "  << msg << "\n";
            queue.insert(msg);
        }
    }

    if (sendOutMsg) {
        cGate *outGate = gate("out", intrand(gateSize("out")));
        sendOutMsg->setName("Dequeued");
        send(sendOutMsg, outGate);
        scheduleAt(sendOutMsg->getArrivalTime(), timer);
    }

    // colorize icon
    if (!timer->isScheduled())
        getDisplayString().setTagArg("i", 1, "");
    else if (queue.isEmpty())
        getDisplayString().setTagArg("i", 1, "green");
    else
        getDisplayString().setTagArg("i", 1, "yellow");
}

