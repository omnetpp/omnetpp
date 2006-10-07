//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "server.h"
#include "pqueue.h"


Define_Module(QServer);

QServer::QServer()
{
    queues = NULL;
    msgServiced = endServiceMsg = NULL;
}

QServer::~QServer()
{
    delete msgServiced;
    cancelAndDelete(endServiceMsg);
    delete [] queues;
}

void QServer::initialize(int stage)
{
    // use multi-stage initialization: QPassiveQueues initialize in stage 0,
    // storeRequest() can only be done after that
    if (stage==0)
    {
        numQueues = gate("in")->size();
        queues = new QPassiveQueue *[numQueues];
        for (int i=0; i<numQueues; i++)
        {
            if (!gate("in",i)->isConnected())
                error("some in[] gates are not connected");
            queues[i] = check_and_cast<QPassiveQueue *>(gate("in",i)->fromGate()->ownerModule());
        }

        endServiceMsg = new cMessage("end-service");
        msgServiced = NULL;
        hasAcceptedOffer = false;
    }
    else if (stage==1)
    {
        for (int j=0; j<numQueues; j++)
            queues[j]->storeRequest(0);
    }
}

void QServer::handleMessage(cMessage *msg)
{
    if (msg==endServiceMsg)
    {
        ASSERT(msgServiced!=NULL);

        ev << "job service finished\n";

        send(msgServiced, "out");
        msgServiced = NULL;

        if (ev.isGUI()) displayString().setTagArg("i",1,"");

        // examine all input queues, in order of priority (0 is highest)
        ev << "scanning queues in priority order if they contain jobs...\n";
        int k;
        for (k=0; k<numQueues; k++)
            if (!queues[k]->isEmpty())
                break;

        if (k!=numQueues)
        {
            ev << "requesting job from queue " << k << endl;
            queues[k]->request(0);
        }
        else
        {
            // if all queues are empty, request from all of them (an accept only 1st one)
            ev << "all queues empty, requesting them to report if something arrives\n";
            for (int i=0; i<numQueues; i++)
                queues[i]->storeRequest(0);
            hasAcceptedOffer = false;
        }
    }
    else
    {
        if (msgServiced)
            error("job arrived while already servicing one");

        ev << "job arrived, starting its service\n";

        msgServiced = msg;
        simtime_t serviceTime = par("serviceTime");
        scheduleAt(simTime()+serviceTime, endServiceMsg);

        if (ev.isGUI()) displayString().setTagArg("i",1,"cyan");
    }
}

bool QServer::okToSend()
{
    Enter_Method("okToSend()?");

    if (msgServiced || hasAcceptedOffer)
        return false;

    hasAcceptedOffer = true;
    return true;
}

