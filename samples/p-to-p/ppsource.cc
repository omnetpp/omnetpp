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


/**
 * A bursty packet generator; see NED file for more info.
 */
class PPSource : public cSimpleModule
{
  private:
    // parameters
    cPar *sleepTime;
    cPar *burstTime;
    cPar *sendIATime;
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

  public:
    PPSource();
    virtual ~PPSource();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module( PPSource );

PPSource::PPSource()
{
    startStopBurst = sendMessage = NULL;
}

PPSource::~PPSource()
{
    cancelAndDelete(startStopBurst);
    cancelAndDelete(sendMessage);
}

void PPSource::initialize()
{
    fsm.setName("fsm");

    sleepTime = &par("sleepTime");
    burstTime = &par("burstTime");
    sendIATime = &par("sendIaTime");
    msgLength = &par("msgLength");

    i = 0;
    WATCH(i); // always put watches in initialize(), NEVER in handleMessage()
    startStopBurst = new cMessage("startStopBurst");
    sendMessage = new cMessage("sendMessage");

    scheduleAt(0.0,startStopBurst);
}

void PPSource::handleMessage(cMessage *msg)
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
        displayString().setTagArg("i",1,"");
        break;

      case FSM_Exit(SLEEP):
        // schedule end of this burst
        d = burstTime->doubleValue();
        scheduleAt(simTime() + d, startStopBurst);

        // display message, turn icon yellow
        ev << "starting burst of duration " << d << "s\n";
        bubble("burst started");
        displayString().setTagArg("i",1,"yellow");

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
        // generate and send out frame
        char msgname[32];
        sprintf( msgname, "frame-%d", ++i);
        ev << "Generating " << msgname << endl;
        cMessage *frame = new cMessage(msgname);
        frame->setLength( (long) *msgLength );
        frame->setTimestamp();
        send(frame, "out" );

        // update status string above icon
        char txt[32];
        sprintf(txt, "sent: %d", i);
        displayString().setTagArg("t",0, txt);

        // return to ACTIVE
        FSM_Goto(fsm,ACTIVE);
        break;
      }
    }
}
