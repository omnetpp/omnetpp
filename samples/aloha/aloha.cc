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

    long currentCollisionNumFrames;

    long totalFrames;
    long collidedFrames;

  public:
    Module_Class_Members(Server,cSimpleModule,0);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

//FIXME add channel statistics and verify it!!!!

Define_Module(Server);

void Server::initialize()
{
    txRate = par("txRate");
    endRxEvent = new cMessage("end-reception");
    channelBusy = false;

    currentCollisionNumFrames = 0;
    totalFrames = 0;
    collidedFrames = 0;
    WATCH(currentCollisionNumFrames);
    WATCH(totalFrames);
    WATCH(collidedFrames);

    if (ev.isGUI()) displayString().setTagArg("i2",0,"x_off");
}

void Server::handleMessage(cMessage *msg)
{
    if (msg==endRxEvent)
    {
        ev << "reception finished\n";
        channelBusy = false;
        currentCollisionNumFrames = 0;
        if (ev.isGUI())
        {
            displayString().setTagArg("i2",0,"x_off");
            displayString().setTagArg("t",0,"");
        }
    }
    else
    {
        totalFrames++;
        double endReception = simTime() + msg->length() / txRate;
        if (!channelBusy)
        {
            ev << "started receiving\n";
            channelBusy = true;
            scheduleAt(endReception, endRxEvent);
            if (ev.isGUI())
            {
                displayString().setTagArg("i2",0,"x_yellow");
                displayString().setTagArg("t",0,"RECEIVING");
                displayString().setTagArg("t",2,"#808000");
            }
        }
        else
        {
            ev << "another frame arrived while receiving -- collision!\n";

            collidedFrames++;
            if (currentCollisionNumFrames==0)
                currentCollisionNumFrames = 2;
            else
                currentCollisionNumFrames++;

            if (endReception > endRxEvent->arrivalTime())
            {
                cancelEvent(endRxEvent);
                scheduleAt(endReception, endRxEvent);
            }

            if (ev.isGUI())
            {
                displayString().setTagArg("i2",0,"x_red");
                displayString().setTagArg("t",0,"COLLISION");
                displayString().setTagArg("t",2,"#800000");
                char buf[32];
                sprintf(buf, "Collision! (%d frames)", currentCollisionNumFrames);
                bubble(buf);
            }
        }
        channelBusy = true;
        delete msg;
    }
}

//--------------------

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

