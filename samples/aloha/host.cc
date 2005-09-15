//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>

/**
 * Aloha host; see NED file for more info.
 */
class AHost : public cSimpleModule
{
  private:
    // parameters
    double radioDelay;
    double txRate;
    cPar *iaTime;
    cPar *pkLenBits;
    double slotTime;
    bool isSlotted;

    // state variables, event pointers etc
    cModule *server;
    cMessage *endTxEvent;
    enum {IDLE=0, TRANSMIT=1} state;
    int pkCounter;

  public:
    AHost();
    virtual ~AHost();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(AHost);

AHost::AHost()
{
    endTxEvent = NULL;
}

AHost::~AHost()
{
    cancelAndDelete(endTxEvent);
}

void AHost::initialize()
{
    server = simulation.moduleByPath("server");
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
        displayString().setTagArg("t",2,"#808000");

    scheduleAt(iaTime->doubleValue(), endTxEvent);
}

void AHost::handleMessage(cMessage *msg)
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
            displayString().setTagArg("i",1,"");
            displayString().setTagArg("t",0,"");
        }
    }
    else
    {
        error("invalid state");
    }
}

