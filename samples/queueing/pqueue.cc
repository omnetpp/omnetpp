//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "pqueue.h"
#include "server.h"


Define_Module(QPassiveQueue);

void QPassiveQueue::initialize()
{
    jobRequestedOnGate = -1;
    queue.setName("queue");
}

void QPassiveQueue::handleMessage(cMessage *msg)
{
    if (jobRequestedOnGate<0)
    {
        queue.insert(msg);
    }
    else
    {
        cModule *servermod = gate("out",jobRequestedOnGate)->toGate()->ownerModule();
        QServer *server = check_and_cast<QServer *>(servermod);

        if (server->okToSend())
            send(msg, "out", jobRequestedOnGate);
        else
            queue.insert(msg);
        jobRequestedOnGate = -1;
    }

    if (ev.isGUI()) displayString().setTagArg("i",1, queue.empty() ? "" : "cyan3");
}

bool QPassiveQueue::isEmpty()
{
    Enter_Method("isEmpty()?");
    return queue.empty();
}

void QPassiveQueue::request(int gateIndex)
{
    Enter_Method("request()!");

    ASSERT(!queue.empty());
    send((cMessage *)queue.pop(), "out", gateIndex);

    if (ev.isGUI()) displayString().setTagArg("i",1, queue.empty() ? "" : "cyan");
}

void QPassiveQueue::storeRequest(int gateIndex)
{
    Enter_Method("storeRequest()!");

    ASSERT(queue.empty());
    if (jobRequestedOnGate!=-1 && jobRequestedOnGate!=gateIndex)
        error("storeRequest() currently cannot handle more than one request, sorry...");

    jobRequestedOnGate = gateIndex;
}


