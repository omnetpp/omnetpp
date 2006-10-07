//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "dynapacket_m.h"

#define STACKSIZE 16384

/**
 * Simulates a switch between clients and server; see NED file for more info
 */
class Switch : public cSimpleModule
{
  public:
    Switch() : cSimpleModule(STACKSIZE) {}
    virtual void activity();
};

Define_Module( Switch );

void Switch::activity()
{
    double pkDelay = 1 / (double)par("pkRate");
    int queueMaxLen = (int) par("queueMaxLen");
    cQueue queue("queue");
    for(;;)
    {
        // receive msg
        cMessage *msg;
        if (!queue.empty())
            msg = (cMessage *) queue.pop();
        else
            msg = receive();

        // model processing delay; packets that arrive meanwhile are queued
        waitAndEnqueue(pkDelay, &queue);

        // send msg to destination
        DynaPacket *pk = check_and_cast<DynaPacket *>(msg);
        int dest = pk->getDestAddress();
        ev << "Relaying msg to addr=" << dest << '\n';
        send( msg, "out", dest);

        // display status: normal=queue empty, yellow=queued packets; red=queue overflow
        int qLen = queue.length();
        if (ev.isGUI()) displayString().setTagArg("i",1, qLen==0 ? "" : qLen<queueMaxLen ? "gold" : "red");

        // model finite queue size
        while (queue.length() > queueMaxLen)
        {
            ev << "Buffer overflow, discarding " << queue.tail()->name() << endl;
            delete queue.pop();
        }
    }
}


