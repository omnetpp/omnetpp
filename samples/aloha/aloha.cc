//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>

class Server : public cSimpleModule
{
  protected:
    bool channelBusy;
    cMessage *endRxEvent;
    double txRate;

    long totalFrames;
    long collidedFrames;

  public:
    Module_Class_Members(Server,cSimpleModule,0);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    // called from Hosts:
    virtual bool isChannelBusy();
};

class Host : public cSimpleModule
{
  protected:
    Server *server;
    cMessage *endTxEvent;
    enum {IDLE=0, DEFER=1, TRANSMIT=2} state;
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

//--------------------

//FIXME add channel statistics and verify it!!!!

Define_Module(Server);

void Server::initialize()
{
    txRate = par("txRate");
    endRxEvent = new cMessage("end-reception");
    channelBusy = false;

    totalFrames = 0;
    collidedFrames = 0;
    WATCH(totalFrames);
    WATCH(collidedFrames);
}

void Server::handleMessage(cMessage *msg)
{
    if (msg==endRxEvent)
    {
        channelBusy = false;
        if (ev.isGUI()) displayString().setTagArg("i",1,"");
    }
    else
    {
        totalFrames++;
        double endReception = simTime() + msg->length() / txRate;
        if (!channelBusy)
        {
            channelBusy = true;
            scheduleAt(endReception, endRxEvent);
            if (ev.isGUI()) displayString().setTagArg("i",1,"green");
        }
        else
        {
            collidedFrames++;
            if (endReception > endRxEvent->arrivalTime())
            {
                cancelEvent(endRxEvent);
                scheduleAt(endReception, endRxEvent);
            }
            if (ev.isGUI()) displayString().setTagArg("i",1,"red");
            if (ev.isGUI()) bubble("Collision");
        }
        channelBusy = true;
        delete msg;
    }
}

bool Server::isChannelBusy()
{
    Enter_Method("isChannelBusy()?");
    return channelBusy;
}

//--------------------

Define_Module(Host);

void Host::initialize()
{
    server = check_and_cast<Server *>(simulation.moduleByPath("server"));
    endTxEvent = new cMessage("endTxEvent");
    state = IDLE;
    pkCounter = 0;
    txRate = par("txRate");
    radioDelay = par("radioDelay");
    iaTime = &par("iaTime");
    pkLenBits = &par("pkLenBits");

    if (ev.isGUI()) displayString().setTagArg("i",2,"100");

    scheduleAt(iaTime->doubleValue(), endTxEvent);
}

void Host::handleMessage(cMessage *msg)
{
    ASSERT(msg==endTxEvent);

    if (state==IDLE || state==DEFER)
    {
        if (server->isChannelBusy())
        {
            // defer sending
            state = DEFER;
            scheduleAt(simTime()+iaTime->doubleValue(), endTxEvent);

            if (ev.isGUI()) displayString().setTagArg("i",1,"orange");
        }
        else
        {
            // generate packet and schedule timer when it ends
            char pkname[40];
            sprintf(pkname,"pk-%d-#%d", id(), pkCounter++);
            ev << "generating packet " << pkname << endl;

            displayString().setTagArg("i",1,"green");

            cMessage *pk = new cMessage(pkname);
            pk->setLength(pkLenBits->longValue());
            double txtime = pk->length() / txRate;
            sendDirect(pk, radioDelay, server->gate("in"));

            state = TRANSMIT;
            scheduleAt(simTime()+txtime, endTxEvent);
        }
    }
    else if (state==TRANSMIT)
    {
        // endTxEvent signals end of transmission
        state = IDLE;

        // schedule next sending
        scheduleAt(simTime()+iaTime->doubleValue(), endTxEvent);

        if (ev.isGUI()) displayString().setTagArg("i",1,"");
    }
    else
    {
        error("invalid state");
    }
}

