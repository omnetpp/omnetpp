//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>

class Host : public cSimpleModule
{
  protected:
    cModule *server;
    cMessage *endTxEvent;
    enum {IDLE=0, TRANSMIT=2} state;
    int pkCounter;
    double radioDelay;
    double txRate;
    cPar *iaTime;
    cPar *pkLenBits;

  public:
    Module_Class_Members(Host,cSimpleModule,0);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Host);

void Host::initialize()
{
    server = simulation.moduleByPath("server");
    if (!server) error("server not found");

    endTxEvent = new cMessage("endTxEvent");
    state = IDLE;
    pkCounter = 0;
    txRate = par("txRate");
    radioDelay = par("radioDelay");
    iaTime = &par("iaTime");
    pkLenBits = &par("pkLenBits");

    if (ev.isGUI())
    {
        displayString().setTagArg("i",2,"100");
        displayString().setTagArg("t",2,"#808000");
    }
    scheduleAt(iaTime->doubleValue(), endTxEvent);
}

void Host::handleMessage(cMessage *msg)
{
    ASSERT(msg==endTxEvent);

    if (state==IDLE)
    {
        // generate packet and schedule timer when it ends
        char pkname[40];
        sprintf(pkname,"pk-%d-#%d", id(), pkCounter++);
        ev << "generating packet " << pkname << endl;

        state = TRANSMIT;

        // update network graphics
        if (ev.isGUI())
        {
            displayString().setTagArg("i",1,"yellow");
            displayString().setTagArg("t",0,"TRANSMIT");
        }

        cMessage *pk = new cMessage(pkname);
        pk->setLength(pkLenBits->longValue());
        double txtime = pk->length() / txRate;
        sendDirect(pk, radioDelay, server->gate("in"));

        scheduleAt(simTime()+txtime, endTxEvent);
    }
    else if (state==TRANSMIT)
    {
        // endTxEvent signals end of transmission
        state = IDLE;

        // schedule next sending
        scheduleAt(simTime()+iaTime->doubleValue(), endTxEvent);

        // update network graphics
        if (ev.isGUI())
        {
            displayString().setTagArg("i",1,"");
            displayString().setTagArg("t",0,"");
        }
    }
    else
    {
        error("invalid state");
    }
}

