//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "Host.h"

namespace aloha {

Define_Module(Host);


Host::Host()
{
    endTxEvent = NULL;
}


Host::~Host()
{
    cancelAndDelete(endTxEvent);
}


void Host::initialize()
{
    server = simulation.getModuleByPath("server");
    if (!server) error("server not found");

    txRate = par("txRate");
    radioDelay = par("radioDelay");
    iaTime = &par("iaTime");
    pkLenBits = &par("pkLenBits");

    slotTime = par("slotTime");
    isSlotted = slotTime>0;
    WATCH(slotTime);
    WATCH(isSlotted);

    endTxEvent = new cMessage("send/endTx");
    state = IDLE;
    pkCounter = 0;
    WATCH((int&)state);
    WATCH(pkCounter);

    if (ev.isGUI())
        getDisplayString().setTagArg("t",2,"#808000");

    scheduleAt(iaTime->doubleValue(), endTxEvent);
}


void Host::handleMessage(cMessage *msg)
{
    ASSERT(msg==endTxEvent);

    if (state==IDLE)
    {
        // generate packet and schedule timer when it ends
        char pkname[40];
        sprintf(pkname,"pk-%d-#%d", getId(), pkCounter++);
        ev << "generating packet " << pkname << endl;

        state = TRANSMIT;

        // update network graphics
        if (ev.isGUI())
        {
            getDisplayString().setTagArg("i",1,"yellow");
            getDisplayString().setTagArg("t",0,"TRANSMIT");
        }

        cMessage *pk = new cMessage(pkname);
        pk->setLength(pkLenBits->longValue());
        simtime_t txtime = pk->length() / txRate;
        sendDirect(pk, radioDelay, server->gate("in"));

        scheduleAt(simTime()+txtime, endTxEvent);
    }
    else if (state==TRANSMIT)
    {
        // endTxEvent signals end of transmission
        state = IDLE;

        // schedule next sending
        if (!isSlotted)
        {
            scheduleAt(simTime()+iaTime->doubleValue(), endTxEvent);
        }
        else
        {
            // align time of next transmission to a slot boundary
            simtime_t t = simTime()+iaTime->doubleValue();
            t = slotTime * ceil(t/slotTime);
            scheduleAt(t, endTxEvent);
        }

        // update network graphics
        if (ev.isGUI())
        {
            getDisplayString().setTagArg("i",1,"");
            getDisplayString().setTagArg("t",0,"");
        }
    }
    else
    {
        error("invalid state");
    }
}

}; //namespace
