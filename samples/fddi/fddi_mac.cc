//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//
//  Author: Gabor.Lencse@hit.bme.hu
//

/*--------------------------------------------------------------*
  Copyright (C) 1996,97 Gabor Lencse,
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <omnetpp.h>

#include "fddi_def.h"
#include "fddi_mac.h"

#define TRACE_MSG               // turn on output messages
// #define STATE_CHK // turn on E-FSM state checking

static bool accelerated = false;


Define_Module(FDDI_MAC);
Define_Module(FDDI_MAC4Ring);
Define_Module(FDDI_MAC4Sniffer);

char *mysimtimeToStr(simtime_t t, char *dest = 0)
{
    // place result either into dest  or into a static buffer
    static char buf[64];
    if (dest == NULL)
        dest = buf;
    double ts = (double) t + 0.000000005;       // ts: sim.time(secs)+0.005us
    long h;
    int m, s, ms;
    double us;
    h = (long) (ts / 3600);
    ts -= h * 3600;
    m = (int) (ts / 60);
    ts -= m * 60;
    s = (int) (ts);
    ts -= s;
    ms = (int) (ts * 1000);
    ts -= ms / 1000.0;
    us = (ts * 1000000L);
    us = ((long) (100L * us)) / 100.0;
    sprintf(dest, "%ds %dms %.2fu", s, ms, us);
    return dest;
}

void printMsg(cMessage & msg)
{
    char time1[64], time2[64];
    char msgname[32];
    const char *s = msg.fullName();
    const int LEN = 21;

    strncpy(msgname, s, LEN);
    int i;
    for (i = strlen(s); i < LEN; i++)
        msgname[i] = ' ';
    msgname[LEN] = 0;
    ev.printf("%s #%2i %15.15s #%2i %15.15s %4d %3d\n",
              msgname, msg.senderModuleId(),
              mysimtimeToStr(msg.sendingTime(), time1),
              msg.arrivalModuleId(),
              mysimtimeToStr(msg.arrivalTime(), time2), (int) msg.length(), (int) msg.kind());
}

void FDDI_MAC::StateCheck(MACState required_state, MACState also_good_state)
{
    if ((State & required_state) | !required_state)
    {
        // system is in the required state, OK
    }
    else
    {
        ev.printf("FDDI MAC error: During processing event of type %i\n", CurrentEvent);
        ev.printf("FDDI MAC is NOT in the required State 0x%x, ", required_state);
        ev.printf("Current State is 0x%x\n", State);
        endSimulation();
    }
    if (State & ~required_state & ~also_good_state)
    {
        ev.printf("FDDI MAC error: During processing event #%i\n", CurrentEvent);
        ev.printf("Current sate is 0x%x, but 0x%x is/are not allowed\n",
                  State, (State & ~also_good_state) & ~required_state);
        endSimulation();
    }
}

void FDDI_MAC::TokenArrived(cMessage * msg)
{
    cMessage *m;

    CurrentEvent = TOKEN_ARRIVED;
#ifdef STATE_CHK
    //Required State:  -
    //Also Good State: REPEAT
    StateCheck(IDLE, REPEAT);
#endif
    RestrictedToken = msg->par("restricted");
    // ?? the address of the 2 stations, who have the restr. token ...
    // capture the token if it is usable, repeat it otherwise:
    if (!sync_buf.empty() || (!async_buf.empty() && !LateCounter && ((double) TRT > token_time)))
    {                           // according to the STANDARD, it should be: TRT>token_time+station_latency
        State |= TOKEN_CAPTURE;
        m = new cMessage("TOKEN_CAPTURE_END", TOKEN_CAPTURE_END, 0);
        scheduleAt(simTime() + token_time, m);
    }
    else
    {
        State |= TOKEN_RECEIVE | BEFORE_TOKEN_REPEAT;
        m = new cMessage("TOKEN_RECEIVE_END", TOKEN_RECEIVE_END, 0);
        scheduleAt(simTime() + token_time, m);
        m = new cMessage("TOKEN_REPEAT_BEGIN", TOKEN_REPEAT_BEGIN, 0);
        scheduleAt(simTime() + station_latency, m);
    }
    delete msg;
}

void FDDI_MAC::TokenRepeatBegin(cMessage * msg)
{
    CurrentEvent = TOKEN_REPEAT_BEGIN;
#ifdef STATE_CHK
    //Required State:  BEFORE_TOKEN_REPEAT
    //Also Good State: TOKEN_RECEIVE
    StateCheck(BEFORE_TOKEN_REPEAT, TOKEN_RECEIVE);
#endif
    State &= ~BEFORE_TOKEN_REPEAT;
    State |= TOKEN_REPEAT;
    cMessage *tok = new cMessage("FDDI_TOKEN", FDDI_TOKEN);
    tok->setLength(TokenLength);
    tok->addPar("restricted") = (bool) RestrictedToken;
    send(tok, "out");
    cMessage *m = new cMessage("TOKEN_REPEAT_END", TOKEN_REPEAT_END, 0);
    scheduleAt(simTime() + token_time, m);
    delete msg;
}

void FDDI_MAC::TokenRepeatEnd(cMessage * msg)
{
    CurrentEvent = TOKEN_REPEAT_END;
#ifdef STATE_CHK
    //Required State:  TOKEN_REPEAT
    //Also Good State: -
    StateCheck(TOKEN_REPEAT, IDLE);
#endif
    State &= ~TOKEN_REPEAT;
    delete msg;
}

void FDDI_MAC::TokenReceiveEnd(cMessage * msg)
{
    CurrentEvent = TOKEN_RECEIVE_END;
#ifdef STATE_CHK
    //Required State:  TOKEN_RECEIVE
    //Also Good State: TOKEN_REPEAT
    StateCheck(TOKEN_RECEIVE, TOKEN_REPEAT);
#endif
    State &= ~TOKEN_RECEIVE;
    if ((EarlyToken = !LateCounter) != 0)
    {
        TRT = T_Opr;            // it is auto enabled
        cancelEvent(TRTExpire);
        scheduleAt(simTime() + T_Opr, TRTExpire);
    }
    else
        LateCounter = 0;
    delete msg;
}

void FDDI_MAC::TokenCaptureEnd(cMessage * msg)
{
    CurrentEvent = TOKEN_CAPTURE_END;
#ifdef STATE_CHK
    //Required State:  TOKEN_CAPTURE
    //Also Good State: -
    StateCheck(TOKEN_CAPTURE, IDLE);
#endif
    State &= ~TOKEN_CAPTURE;
    State |= BEFORE_TRANSMIT_OWN;
    scheduleAt(simTime() + station_latency,
               new cMessage("TRANSMIT_OWN_BEGIN", TRANSMIT_OWN_BEGIN, 0));
    if ((EarlyToken = !LateCounter) != 0)
    {
        THT = (double) TRT;     // it is auto enabled
        THT.disable();          // only BeforeTransmitEnd should disable it! see STANDARD
        TRT = T_Opr;            // it is auto enabled
        cancelEvent(TRTExpire);
        scheduleAt(simTime() + T_Opr, TRTExpire);
    }
    else
        LateCounter = 0;
    SyncFinished = false;
    UsedSyncBandwidth = 0;
    delete msg;
}

void FDDI_MAC::TransmitOwnBegin(cMessage * msg)
{
    CurrentEvent = TRANSMIT_OWN_BEGIN;
#ifdef STATE_CHK
    //Required State:  BEFORE_TRANSMIT_OWN
    //Also Good State: -
    StateCheck(BEFORE_TRANSMIT_OWN, IDLE);
#endif
    State &= ~BEFORE_TRANSMIT_OWN;      // this must be unset here, PlayTTRP unsets only TRANSMIT_OWN
    // do the actions on the basis of the protocol, set the new state too:
    PlayTTRP();
    delete msg;
}

void FDDI_MAC::TransmissionEnd(cMessage * msg)
{
    CurrentEvent = TRANSMISSION_END;
#ifdef STATE_CHK
    //Required State:  TRANSMIT_OWN
    //Also Good State: FRAME_STRIP
    StateCheck(TRANSMIT_OWN, FRAME_STRIP);
#endif
    // do the actions on the basis of the protocol, set the new state too:
    PlayTTRP();
    delete msg;
}

void FDDI_MAC::TokenSendEnd(cMessage * msg)
{
    CurrentEvent = TOKEN_SEND_END;
#ifdef STATE_CHK
    //Required State:  TOKEN_SEND
    //Also Good State: FRAME_STRIP
    StateCheck(TOKEN_SEND, FRAME_STRIP);
#endif
    State &= ~TOKEN_SEND;
    delete msg;
}

void FDDI_MAC::OwnFrameArrived(cMessage * msg)
{
    CurrentEvent = OWN_FRAME_ARRIVED;
#ifdef STATE_CHK
    //Required State:  -
    //Also Good State: TRANSMIT_OWN,TOKEN_SEND
    StateCheck(IDLE, TRANSMIT_OWN | TOKEN_SEND);
#endif
    State |= FRAME_STRIP;
    cMessage *m = new cMessage("FRAME_STRIP_END", FRAME_STRIP_END, 0);
    scheduleAt(simTime() + msg->length() * byte_tr_time - inorder_epsilon, m);
    delete msg;
}

void FDDI_MAC::FrameStripEnd(cMessage * msg)
{
    CurrentEvent = FRAME_STRIP_END;
#ifdef STATE_CHK
    //Required State:  FRAME_STRIP
    //Also Good State: TRANSMIT_OWN,TOKEN_SEND
    StateCheck(FRAME_STRIP, TRANSMIT_OWN | TOKEN_SEND);
#endif
    State &= ~FRAME_STRIP;
    delete msg;
}

void FDDI_MAC::Frame2MeArrived(cMessage * msg)
{
    CurrentEvent = FR2ME_ARRIVED;
#ifdef STATE_CHK
    //Required State:  -
    //Also Good State: REPEAT
    StateCheck(IDLE, REPEAT);
#endif
    State |= FR2ME_RECEIVE;
    cMessage *m = new cMessage("FR2ME_RECEIVE_END", FR2ME_RECEIVE_END, 0);
    scheduleAt(simTime() + msg->length() * byte_tr_time - inorder_epsilon, m);
    msg_being_recd = msg;
    if (!accelerated)
    {
        State |= BEFORE_REPEAT;
        //msg_to_repeat = new cMessage(*msg); <-- this may cause segmentation fault
        msg_to_repeat = (cMessage *) msg->dup();
        scheduleAt(simTime() + station_latency, new cMessage("REPEAT_BEGIN", REPEAT_BEGIN, 0));
    }
}

void FDDI_MAC::FrameToMeReceiveEnd(cMessage * msg)
{
    CurrentEvent = FR2ME_RECEIVE_END;
#ifdef STATE_CHK
    //Required State:  FR2ME_RECEIVE
    //Also Good State: REPEAT
    StateCheck(FR2ME_RECEIVE, REPEAT);
#endif
    State &= ~FR2ME_RECEIVE;
    send(msg_being_recd, "to_LLC");
    delete msg;
}

void FDDI_MAC::Frame2RepArrived(cMessage * msg)
{
    CurrentEvent = FR2REP_ARRIVED;
#ifdef STATE_CHK
    //Required State:  -
    //Also Good State: REPEAT
    StateCheck(IDLE, REPEAT);
#endif
    State |= BEFORE_REPEAT;
    msg_to_repeat = msg;
    scheduleAt(simTime() + station_latency, new cMessage("REPEAT_BEGIN", REPEAT_BEGIN, 0));
}

void FDDI_MAC::RepeatEnd(cMessage * msg)
{
    CurrentEvent = REPEAT_END;
#ifdef STATE_CHK
    //Required State:  REPEAT
    //Also Good State: BEFORE_REPEAT,FR2ME_RECEIVE,TOKEN_RECEIVE,BEFORE_TOKEN_REPEAT,TOKEN_CAPTURE  (but NOT at the same time ...)
    StateCheck(REPEAT,
               BEFORE_REPEAT | FR2ME_RECEIVE | TOKEN_RECEIVE | BEFORE_TOKEN_REPEAT | TOKEN_CAPTURE);
#endif
    State &= ~REPEAT;
    delete msg;
}

void FDDI_MAC::RepeatBegin(cMessage * msg)
{
    CurrentEvent = REPEAT_BEGIN;
#ifdef STATE_CHK
    //Required State:  BEFORE_REPEAT
    //Also Good State: FR2ME_RECEIVE
    StateCheck(BEFORE_REPEAT, FR2ME_RECEIVE);
#endif
    State &= ~BEFORE_REPEAT;
    State |= REPEAT;
    RepeatEndsAt = simTime() + msg_to_repeat->length() * byte_tr_time;
    scheduleAt(RepeatEndsAt - inorder_epsilon, new cMessage("REPEAT_END", REPEAT_END, 0));
    send(msg_to_repeat, "out");
    delete msg;
    IdleTimes = 0;              // a packet is repeated => not idle
}

void FDDI_MAC::TRTExpired(cMessage * msg)
{
    CurrentEvent = TRT_EXPIRED;
    if (LateCounter == 0)
    {
        LateCounter++;
        TRT = T_Opr;            // it is auto enabled
        scheduleAt(simTime() + T_Opr, msg);     // Recycling: *msg is reused
    }
    else
    {
        State = TOKEN_LOST;
        delete msg;
        error("FDDI_MAC Error: Token was lost (TRT expired twice in %s)\n", fullPath().c_str());
    }
}

void FDDI_MAC::PlayTTRP()       // Play the Timed Token Ring Protocol
{
    if (!SyncFinished && !sync_buf.empty())
    {                           // try to ransmit the first sync. packet
        cMessage *m = (cMessage *) sync_buf.pop();
        int length = m->length();
        if (AllocatedSyncBandwidth >= UsedSyncBandwidth + length)
        {                       // this packet can be transmitted
            UsedSyncBandwidth += length;
            m->addPar("sendtime") = simTime();
            send(m, "out");
            State |= TRANSMIT_OWN;
            double sending_time = length * byte_tr_time;
            scheduleAt(simTime() + sending_time,
                       new cMessage("TRANSMISSION_END", TRANSMISSION_END, 0));
            IdleTimes = 0;      // a packet is transmitted => not idle
            return;             // !! HEY there is a RETURN here!
            // ^ the next sync may be transmitted by the next call of this function
        }
    }
    // the function didn't return => no more sync could be transmitted
    if (!SyncFinished)
    {
        SyncFinished = true;
        if (EarlyToken)
        {
            THT.enable();
            priority_i = 7;
        }
    }
    // here SyncFinished is true
    if (EarlyToken && !async_buf.empty() &&
        (!RestrictedToken || IAmRestrictedOwner) && ((double) THT) > 0)
    {                           // the 1st packet can be transmitted
        cMessage *m = (cMessage *) async_buf.pop();
        m->addPar("sendtime") = simTime();
        send(m, "out");
        State |= TRANSMIT_OWN;
        int length = m->length();
        double tr_time = length * byte_tr_time;
        scheduleAt(simTime() + tr_time, new cMessage("TRANSMISSION_END", TRANSMISSION_END, 0));
        IdleTimes = 0;          // a packet is transmitted => not idle
        return;                 // note "return" here
        // ^ the next async may be transmitted by the next call of this function
    }
    // the function didn't return => no more async could be transmitted
    // now the token must be passed
    cMessage *tok = new cMessage("FDDI_TOKEN", FDDI_TOKEN);
    tok->setLength(TokenLength);
    tok->addPar("restricted") = RestrictedToken;
    send(tok, "out");
    State &= ~TRANSMIT_OWN;
    State |= TOKEN_SEND;
    scheduleAt(simTime() + TokenLength * byte_tr_time,
               new cMessage("TOKEN_SEND_END", TOKEN_SEND_END, 0));
}

FDDI_MAC::FDDI_MAC() : cSimpleModule(FDDI_MAC_STACKSIZE),
    sync_buf("sync_buf"),
    async_buf("async_buf")
{
    State = IDLE;
    CurrentEvent = 0;

    msg_being_recd = 0;         // message is currently being received
    msg_to_repeat = 0;          // this msg will be repeated when REPEAT_BEGIN event arrives
    RepeatEndsAt = -1;          // the current repeating will end at this time

    EarlyToken = 0;             // Token is early
    RestrictedToken = 0;        // Token is restricted
    LateCounter = 0;            // TRT expired LateCounter times since last TOKEN_ARRIVE
    TRTExpire = 0;              // holds a pointer to the TRT expire event
    AllocatedSyncBandwidth = 0; // Allocated Synchronous Bandwidth (in bytes)
    UsedSyncBandwidth = 0;      // Used Synchronous Bandwidth (in bytes)
    RingID = -1;                // To cause error, if not set later
    IdleTimes = 0;              // # of tokens arrived since packet was transmitted for the last time
    IAmRestrictedOwner = false; // This station is a/the ResrictedToken owner
    SyncFinished = false;       // transm. of sync. packets is already finished
}


void FDDI_MAC::activity()
{
    RingID = (short) (long) parentModule()->parentModule()->par("RingID");
    T_Opr = parentModule()->parentModule()->par("TTRT");
    if (findPar("StationID") < 0)
        my_station_id = parentModule()->index();        // RO: use only in a ring made by indexing ...
    else
        my_station_id = par("StationID");
    IAmRestrictedOwner = false;
    TRT = T_Opr;                // it is auto enabled
    TRTExpire = new cMessage("TRT_EXPIRED", TRT_EXPIRED, 0);
    scheduleAt(simTime() + T_Opr, TRTExpire);
    if (my_station_id == 0)
    {
        // Issue a token:
        cMessage *tok = new cMessage("FDDI_TOKEN", FDDI_TOKEN);
        tok->setLength(TokenLength);
        tok->addPar("restricted") = false;
        send(tok, "out");
    }
    for (;;)
    {
        cMessage *msg = receive();

#ifdef TRACE_MSG
        ev.printf("0x%04x ", State);
        printMsg(*msg);
#endif

        if (msg->arrivedOn("from_LLC")) // new message from the generator
            async_buf.insert(msg);      // insert message into the asynchronous send buffer
        else
        {
            int msgkind = msg->kind();
            if (msgkind >= FDDI_MAC_MSG2SELF)   // if it is just a self message
                switch (msgkind)
                {
                case TOKEN_RECEIVE_END:
                    TokenReceiveEnd(msg);
                    break;
                case TRANSMISSION_END:
                    TransmissionEnd(msg);
                    break;
                case TOKEN_SEND_END:
                    TokenSendEnd(msg);
                    break;
                case FRAME_STRIP_END:
                    FrameStripEnd(msg);
                    break;
                case FR2ME_RECEIVE_END:
                    FrameToMeReceiveEnd(msg);
                    break;
                case REPEAT_END:
                    RepeatEnd(msg);
                    break;
                case TRT_EXPIRED:
                    TRTExpired(msg);
                    break;
                case REPEAT_BEGIN:
                    RepeatBegin(msg);
                    break;
                case TOKEN_REPEAT_BEGIN:
                    TokenRepeatBegin(msg);
                    break;
                case TOKEN_REPEAT_END:
                    TokenRepeatEnd(msg);
                    break;
                case TOKEN_CAPTURE_END:
                    TokenCaptureEnd(msg);
                    break;
                case TRANSMIT_OWN_BEGIN:
                    TransmitOwnBegin(msg);
                    break;
                default:
                    error("FDDI_MAC Error: Bad msgkind: %i in %s", msgkind, fullPath().c_str());

                }
            else if (msgkind == FDDI_TOKEN)
                TokenArrived(msg);
            else if (msgkind == FDDI_FRAME)
            {
                int source = (long) msg->par("source");
                if (source == my_station_id)
                    OwnFrameArrived(msg);
                else
                {
                    int dest = (long) msg->par("dest");
                    if (dest == my_station_id)
                        Frame2MeArrived(msg);
                    else
                        Frame2RepArrived(msg);
                }
            }
            else
            {
                ev.printf("Msg pointer=%p\n", msg);
                error("(2)FDDI_MAC Error: Bad msgkind: %i in %s", msgkind, fullPath().c_str());
            }
        }
    }                           // endfor
}

void FDDI_MAC4Sniffer::OwnFrameArrived(cMessage * msg)
{                               // The code is taken from: virtual void FDDI_MAC::OwnFrameArrived(cMessage *)
    CurrentEvent = OWN_FRAME_ARRIVED;
#ifdef STATE_CHK
    //Required State:  -
    //Also Good State: TRANSMIT_OWN,TOKEN_SEND
    StateCheck(IDLE, TRANSMIT_OWN | TOKEN_SEND);
#endif
    State |= FRAME_STRIP;
    cMessage *m = new cMessage("FRAME_STRIP_END", FRAME_STRIP_END, 0);
    scheduleAt(simTime() + msg->length() * byte_tr_time - inorder_epsilon, m);
    // delete msg;
    // CHANGE: the message is not deleted, but it is forwarded to the Monitor
    sendDelayed(msg, msg->length() * byte_tr_time, "to_LLC");
}

void FDDI_MAC4Sniffer::Frame2RepArrived(cMessage * msg)
{                               // The code is taken from: virtual void FDDI_MAC::Frame2RepArrived(cMessage *msg)
    CurrentEvent = FR2REP_ARRIVED;
#ifdef STATE_CHK
    //Required State:  -
    //Also Good State: REPEAT
    StateCheck(IDLE, REPEAT);
#endif
    State |= BEFORE_REPEAT;
    // CHANGE: msg is copied and sent to LLC:
    cMessage *msgcpy = (cMessage *) msg->dup();
    sendDelayed(msgcpy, msg->length() * byte_tr_time, "to_LLC");
    // END OF CHANGE
    msg_to_repeat = msg;
    scheduleAt(simTime() + station_latency, new cMessage("REPEAT_BEGIN", REPEAT_BEGIN, 0));
}
