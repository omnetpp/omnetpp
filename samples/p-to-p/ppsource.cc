//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2004 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#define FSM_DEBUG
#include <omnetpp.h>


class PPSource : public cSimpleModule
{
  public:
    Module_Class_Members(PPSource,cSimpleModule,0)

    // parameters
    double sleepTimeMean;
    double burstTimeMean;
    double sendIATime;
    cPar *msgLength;

    // FSM and its states
    cFSM fsm;
    enum {
       INIT = 0,
       SLEEP = FSM_Steady(1),
       ACTIVE = FSM_Steady(2),
       SEND = FSM_Transient(1),
    };

    // variables used
    int i;
    cMessage *startStopBurst;
    cMessage *sendMessage;

    // the virtual functions
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module( PPSource );

void PPSource::initialize()
{
    fsm.setName("fsm");

    sleepTimeMean = par("sleepTimeMean");
    burstTimeMean = par("burstTimeMean");
    sendIATime = par("sendIaTime");
    msgLength = &par("msgLength");

    i = 0;
    WATCH(i); // always put watches in initialize(), NEVER in handleMessage()
    startStopBurst = new cMessage("startStopBurst");
    sendMessage = new cMessage("sendMessage");

    scheduleAt(0.0,startStopBurst);
}

void PPSource::handleMessage(cMessage *msg)
{
    FSM_Switch(fsm)
    {
      case FSM_Exit(INIT):
        // transition to SLEEP state
        FSM_Goto(fsm,SLEEP);
        break;

      case FSM_Enter(SLEEP):
        // schedule end of sleep period (start of next burst)
        scheduleAt(simTime()+exponential(sleepTimeMean), startStopBurst);
        break;

      case FSM_Exit(SLEEP):
        // schedule end of this burst
        scheduleAt(simTime()+exponential(burstTimeMean), startStopBurst);
        // transition to ACTIVE state:
        if (msg!=startStopBurst)
            error("invalid event in state ACTIVE");
        FSM_Goto(fsm,ACTIVE);
        break;

      case FSM_Enter(ACTIVE):
        // schedule next sending
        scheduleAt(simTime()+exponential(sendIATime), sendMessage);
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
        // generate and send out frame
        char msgname[32];
        sprintf( msgname, "frame-%d", ++i);
        ev << "Generating " << msgname << endl;
        cMessage *frame = new cMessage(msgname);
        frame->setLength( (long) *msgLength );
        frame->setTimestamp();
        send(frame, "out" );

        // return to ACTIVE
        FSM_Goto(fsm,ACTIVE);
        break;
      }
    }
}
