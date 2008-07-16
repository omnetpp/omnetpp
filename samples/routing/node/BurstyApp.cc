//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#define FSM_DEBUG
#include <omnetpp.h>
#include "Packet_m.h"


/**
 * A bursty packet generator; see NED file for more info.
 */
class BurstyApp : public cSimpleModule
{
  private:
    // configuration
    int myAddress;
    std::vector<int> destAddresses;
    cPar *sleepTime;
    cPar *burstTime;
    cPar *sendIATime;
    cPar *packetLengthBytes;

    // state
    cFSM fsm;
    enum {
       INIT = 0,
       SLEEP = FSM_Steady(1),
       ACTIVE = FSM_Steady(2),
       SEND = FSM_Transient(1),
    };

    int pkCounter;
    cMessage *startStopBurst;
    cMessage *sendMessage;

    // statistics
    long numSent;
    long numReceived;
    cOutVector eedVector;

  public:
    BurstyApp();
    virtual ~BurstyApp();

  protected:
    // redefined cSimpleModule methods
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    // new methods
    virtual void processTimer(cMessage *msg);
    virtual void processPacket(cMessage *msg);
    virtual void generatePacket();
    virtual void updateDisplayString();
};

Define_Module(BurstyApp);

BurstyApp::BurstyApp()
{
    startStopBurst = sendMessage = NULL;
}

BurstyApp::~BurstyApp()
{
    cancelAndDelete(startStopBurst);
    cancelAndDelete(sendMessage);
}

void BurstyApp::initialize()
{
    numSent = numReceived = 0;
    WATCH(numSent);
    WATCH(numReceived);

    fsm.setName("fsm");
    eedVector.setName("end-to-end delay");

    destAddresses = cStringTokenizer(par("destAddresses").stdstringValue().c_str()).asIntVector();
    myAddress = par("address").longValue();
    sleepTime = &par("sleepTime");
    burstTime = &par("burstTime");
    sendIATime = &par("sendIaTime");
    packetLengthBytes = &par("packetLength");

    pkCounter = 0;
    WATCH(pkCounter); // always put watches in initialize(), NEVER in handleMessage()
    startStopBurst = new cMessage("startStopBurst");
    sendMessage = new cMessage("sendMessage");

    scheduleAt(0, startStopBurst);
}

void BurstyApp::handleMessage(cMessage *msg)
{
    // process the self-message or incoming packet
    if (msg->isSelfMessage())
        processTimer(msg);
    else
        processPacket(msg);

    // update visual appearance of the module
    if (ev.isGUI())
        updateDisplayString();
}

void BurstyApp::processTimer(cMessage *msg)
{
    simtime_t d;
    FSM_Switch(fsm)
    {
        case FSM_Exit(INIT):
            // transition to SLEEP state
            FSM_Goto(fsm,SLEEP);
            break;

        case FSM_Enter(SLEEP):
            // schedule end of sleep period (start of next burst)
            d = sleepTime->doubleValue();
            scheduleAt(simTime() + d, startStopBurst);

            // display message, restore normal icon color
            ev << "sleeping for " << d << "s\n";
            bubble("burst ended, sleeping");
            getDisplayString().setTagArg("i",1,"");
            break;

        case FSM_Exit(SLEEP):
            // schedule end of this burst
            d = burstTime->doubleValue();
            scheduleAt(simTime() + d, startStopBurst);

            // display message, turn icon yellow
            ev << "starting burst of duration " << d << "s\n";
            bubble("burst started");
            getDisplayString().setTagArg("i",1,"yellow");

            // transition to ACTIVE state:
            if (msg!=startStopBurst)
                error("invalid event in state ACTIVE");
            FSM_Goto(fsm,ACTIVE);
            break;

        case FSM_Enter(ACTIVE):
            // schedule next sending
            d = sendIATime->doubleValue();
            ev << "next sending in " << d << "s\n";
            scheduleAt(simTime() + d, sendMessage);
            break;

        case FSM_Exit(ACTIVE):
            // transition to either SEND or SLEEP
            if (msg==sendMessage) {
                FSM_Goto(fsm,SEND);
            } else if (msg==startStopBurst) {
                cancelEvent(sendMessage);
                FSM_Goto(fsm,SLEEP);
            } else
                error("invalid event in state ACTIVE");
            break;

        case FSM_Exit(SEND):
        {
            // send out a packet
            generatePacket();

            // return to ACTIVE
            FSM_Goto(fsm,ACTIVE);
            break;
        }
    }
}

void BurstyApp::generatePacket()
{
    // generate and send out a packet
    int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];

    char pkname[40];
    sprintf(pkname,"pk-%d-to-%d-#%d", myAddress, destAddress, pkCounter++);
    ev << "generating packet " << pkname << endl;

    Packet *pk = new Packet(pkname);
    pk->setByteLength(packetLengthBytes->longValue());
    pk->setSrcAddr(myAddress);
    pk->setDestAddr(destAddress);
    send(pk,"out");
}

void BurstyApp::processPacket(cMessage *msg)
{
    // update statistics and delete message
    simtime_t eed = simTime()-msg->getCreationTime();
    ev << "Received " << msg->getName() << ", end-to-end delay: " << eed << "sec" << endl;
    delete msg;

    numReceived++;
    eedVector.record(eed);
}

void BurstyApp::updateDisplayString()
{
    // update status string above icon
    char txt[64];
    sprintf(txt, "sent:%ld received:%ld", numSent, numReceived);
    getDisplayString().setTagArg("t", 0, txt);
}

