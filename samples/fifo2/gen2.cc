//-------------------------------------------------------------
// file: gen2.cc
//        (part of Fifo2 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#define FSM_DEBUG
#include <omnetpp.h>


class FF2BurstyGenerator : public cSimpleModule
{
  public:
    Module_Class_Members(FF2BurstyGenerator,cSimpleModule,0)

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

Define_Module( FF2BurstyGenerator );

void FF2BurstyGenerator::initialize()
{
    fsm.setName("fsm");

    sleepTimeMean = par("sleep_time_mean");
    burstTimeMean = par("burst_time_mean");
    sendIATime = par("send_ia_time");
    msgLength = &par("msg_length");

    i = 0;
    WATCH(i); // always put watches in initialize(), NEVER in handleMessage()
    startStopBurst = new cMessage("startStopBurst");
    sendMessage = new cMessage("sendMessage");

    scheduleAt(0.0,startStopBurst);
}

void FF2BurstyGenerator::handleMessage(cMessage *msg)
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
        // generate and send out job
        char msgname[32];
        sprintf( msgname, "job-%d", ++i);
        ev << "Generating " << msgname << endl;
        cMessage *job = new cMessage(msgname);
        job->setLength( (long) *msgLength );
        job->setTimestamp();
        send( job, "out" );

        // return to ACTIVE
        FSM_Goto(fsm,ACTIVE);
        break;
      }
    }
}
