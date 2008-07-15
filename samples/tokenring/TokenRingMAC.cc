//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Authors: Gabor Lencse, Andras Varga
// Based on the code by:
//          Maurits Andre, George van Montfort,
//          Gerard van de Weerd (TU Delft)
//

#include <omnetpp.h>
#include "token_m.h"


// Token length is 3 octets (24 bits)
#define TR_TOKEN_BITS  24

// Frame header and trailer are 21 octets (168 bits), with the following
// frame layout (lengths in parens):
//   SD(1), AC(1), FC(1), DEST(6), SOURCE(6), DATA(n), CS(4), ED(1), FS(1)
#define TR_HEADER_BITS 168

#define STACKSIZE 16384


/**
 * Token Ring MAC layer; see NED file for more info.
 */
class TokenRingMAC : public cSimpleModule
{
  private:
    // configuration
    int myAddress;
    long dataRate;
    simtime_t tokenHoldingTime;
    int queueMaxLen;
    bool debug;

    // state variables
    cQueue sendQueue;    // send buffer
    int sendQueueBytes;  // queue length in bytes

    cMessage *transmEnd; // self-message to signal end of transmission
    cMessage *recvEnd;   // self-message to signal end of reception

    TRToken *token;      // non-NULL while we're holding the token


    // statistics
    cOutVector queueLenPackets;
    cOutVector queueLenBytes;
    cOutVector queueingTime;
    int numPacketsToSend;
    int numPacketsToSendDropped;
    cOutVector queueDrops;

  public:
    TokenRingMAC();
    virtual ~TokenRingMAC();

  protected:
    virtual void activity();
    virtual void finish();
    virtual void handleMessagesWhileTransmitting();
    virtual void storeDataPacket(TRApplicationData *data);
    virtual void beginReceiveFrame(TRFrame *frame);
    virtual void endReceiveFrame(cMessage *data);
};


Define_Module( TokenRingMAC );


TokenRingMAC::TokenRingMAC() : cSimpleModule(STACKSIZE)
{
    transmEnd = recvEnd = token = NULL;
}

TokenRingMAC::~TokenRingMAC()
{
    cancelAndDelete(transmEnd);
    cancelAndDelete(recvEnd);
    delete token;
}

void TokenRingMAC::activity()
{
    gate("phyIn")->getFromGate()->setDeliverOnReceptionStart(true); //FIXME getFromGate shuold NOT be needed!!!!!!!!

    dataRate = par("dataRate");     // 4 or 16 Mbit/s
    tokenHoldingTime = par("THT");   // typically 10ms
    myAddress = par("address");
    queueMaxLen = par("queueMaxLen");

    debug = true;
    WATCH(debug);

    sendQueue.setName("sendQueue");
    sendQueueBytes = 0;
    WATCH (sendQueueBytes);
    queueLenPackets.setName("Queue length (packets)");
    queueLenBytes.setName("Queue length (bytes)");
    queueingTime.setName("Queueing time (sec)");

    numPacketsToSend = 0;
    WATCH (numPacketsToSend);
    numPacketsToSendDropped = 0;
    WATCH (numPacketsToSendDropped);
    queueDrops.setName("App data packets dropped");

    char msgname[30];

    transmEnd = new cMessage("transmEnd");
    recvEnd = new cMessage("recvEnd");

    // station 0 issues the token
    if (myAddress == 0)
    {
        TRToken *token = new TRToken("token", TR_TOKEN);
        token->setBitLength(TR_TOKEN_BITS);
        send(token, "phyOut");
    }

    // enter token ring protocol
    for(;;)
    {
        cMessage *msg = receive();

        // data from higher layer?
        if (dynamic_cast<TRApplicationData *>(msg)!=NULL)
        {
            // store packet in queue and update statistics
            storeDataPacket((TRApplicationData *)msg);
        }
        // token arrived?
        else if (dynamic_cast<TRToken *>(msg)!=NULL)
        {
            token = (TRToken *)msg;
            if (debug)
            {
                ev << "Token arrived (we can keep it for THT=" << tokenHoldingTime << ")" << endl;
                if (ev.isGUI())
                {
                    getParentModule()->getDisplayString().setTagArg("i",1,"gold");
                    getParentModule()->getDisplayString().setTagArg("t",0,"ACTIVE");
                    getParentModule()->getDisplayString().setTagArg("t",2,"#707000");
                    getParentModule()->bubble(sendQueue.empty() ? "Nothing to send!" : "Captured token.");
                }
            }


            // if we have messages to send, send them now, within the tokenHoldingTime
            simtime_t tokenHoldingTime_expires = simTime()+tokenHoldingTime;
            while (!sendQueue.empty())
            {
                // peek at next data packet and see if it can be sent within the tokenHoldingTime
                TRApplicationData *data = (TRApplicationData *) sendQueue.front();
                simtime_t transmission_time = (TR_HEADER_BITS+data->getBitLength())/(double)dataRate;
                if (simTime()+transmission_time > tokenHoldingTime_expires)
                {
                    // no time left for this packet
                    if (debug)
                    {
                        ev << "Data packet \"" << data->getName() << "\" cannot be sent within THT, skipping" << endl;
                    }
                    break;
                }

                // remove data packet from the send buffer
                sendQueue.pop();
                sendQueueBytes -= data->getByteLength();
                queueLenPackets.record(sendQueue.length());
                queueLenBytes.record(sendQueueBytes);

                // write queueing time statistics
                queueingTime.record(simTime() - data->getTimestamp());

                // create Token Ring frame, and send data in it
                sprintf(msgname, "%d-->%d", myAddress, data->getDestination());
                TRFrame *frame = new TRFrame(msgname, TR_FRAME);
                frame->setBitLength(TR_HEADER_BITS);
                frame->setSource(myAddress);
                frame->setDestination(data->getDestination());
                frame->encapsulate(data);

                if (debug)
                {
                    ev << "Began transmitting \"" << data->getName() << "\""
                          " in frame \"" << frame->getName() << "\"" << endl;
                }
                send(frame, "phyOut");

                simtime_t transmStartTime = simTime();

                // we're busy until we finished transmission
                scheduleAt(simTime()+transmission_time, transmEnd);
                handleMessagesWhileTransmitting();

                if (debug)
                {
                    ev << "End transmission " << transmStartTime << endl;
                }
            }

            // release token
            if (debug)
            {
                ev << "Releasing token." << endl;
                if (ev.isGUI())
                {
                    getParentModule()->getDisplayString().setTagArg("i",1,"");
                    getParentModule()->getDisplayString().setTagArg("t",0,"");
                }
            }
            send(token, "phyOut");
            token = NULL;
        }
        // frame arrived from network?
        else if (dynamic_cast<TRFrame *>(msg)!=NULL)
        {
            TRFrame *frame = (TRFrame *)msg;
            beginReceiveFrame(frame);
        }
        // end receiving a packet?
        else if (msg==recvEnd)
        {
            cMessage *data = (cMessage *) recvEnd->getContextPointer();
            endReceiveFrame(data);
        }
        else
        {
            throw cRuntimeError("unexpected message arrived: (%s)%s", msg->getClassName(), msg->getName());
        }
    }
}

void TokenRingMAC::handleMessagesWhileTransmitting()
{
    cMessage *msg;
    while ((msg = receive())!=transmEnd)
    {
        // while we are transmitting, a token cannot arrive (because
        // we hold it), but any of the other events may occur:

        // data from higher layer
        if (dynamic_cast<TRApplicationData *>(msg)!=NULL)
        {
            // store packet in queue and update statistics
            storeDataPacket((TRApplicationData *)msg);
        }
        // frame from network (must be one of our own frames)
        else if (dynamic_cast<TRFrame *>(msg)!=NULL)
        {
            TRFrame *frame = (TRFrame *)msg;
            beginReceiveFrame(frame);
        }
        // end receiving a packet (if we sent a frame to ourselves,
        // its reception may end while we're still transmitting)
        else if (msg==recvEnd)
        {
            cMessage *data = (cMessage *) recvEnd->getContextPointer();
            endReceiveFrame(data);
        }
        else
        {
            throw cRuntimeError("unexpected message arrived: (%s)%s", msg->getClassName(), msg->getName());
        }
    }
}

void TokenRingMAC::storeDataPacket(TRApplicationData *msg)
{
    if (debug)
    {
        ev << "App data received from higher layer: \"" << msg->getName() << "\", "
              "length=" << msg->getBitLength()/8 << "bytes" << endl;
    }

    // inc counter
    numPacketsToSend++;

    // if queue is full, we have to drop it
    if (sendQueue.length() >= queueMaxLen)
    {
        if (debug)
        {
            ev << "Queue full, DROPPED!" << endl;
        }
        delete msg;
        numPacketsToSendDropped++;
        queueDrops.record(numPacketsToSendDropped);
        return;
    }

    // mark enqeueing time, we'll need it for calculating time spent in queue
    msg->setTimestamp();

    // insert message into send buffer
    sendQueue.insert( msg );
    sendQueueBytes += msg->getBitLength()/8;
    queueLenPackets.record( sendQueue.length() );
    queueLenBytes.record(sendQueueBytes);

    if (debug)
    {
        ev << "Enqueued, queue length=" << sendQueue.length() << endl;
    }

}

void TokenRingMAC::beginReceiveFrame(TRFrame *frame)
{
    if (debug)
    {
        ev << "Received beginning of frame \"" << frame->getName() << "\"" << endl;
    }

    ASSERT(frame->isReceptionStart());

    // extract source and destination
    int dest = frame->getDestination();
    int source = frame->getSource();

    // is this station the addressee?
    if (dest == myAddress)
    {
        if (debug)
        {
            ev << "We are the destination: as soon as we fully received the frame," << endl;
            ev << "we'll pass up a copy of the payload \"" <<
                  frame->getEncapsulatedMsg()->getName() << "\" to the higher layer." << endl;
            ev << "Frame will be extracted from the ring by the sender." << endl;
        }

        // some sanity check: because of rounding errors in double arithmetic,
        // it is possible that the "beginning of a frame" event arrives earlier
        // than the "end of receiving the previous frame" (recvEnd) event.
        // In this case, we do as if we've'd just received the recvEnd event
        // (as we should have, ideally).
        if (recvEnd->isScheduled())
        {
            // make sure our assumption is right
            // note: checking for equality only works with 64-bit fixed-point simtime_t!
            ASSERT(recvEnd->getArrivalTime() == simTime());

            if (debug)
            {
                ev << "'recvEnd' event should have occurred already, do it now" << endl;
            }

            // then remedy the situation
            cancelEvent(recvEnd);
            cMessage *data = (cMessage *) recvEnd->getContextPointer();
            endReceiveFrame(data);
        }

        // make a copy of the payload in the frame -- we'll send this one
        // to the higher layer
        cMessage *data2 = (cMessage *) frame->getEncapsulatedMsg()->dup();

        // The arrival of a message in the model represents the arrival
        // of the first bit of the packet. It has to be completely received
        // before we can pass it up to the higher layer. So here we compute
        // when receiving will finish, and schedule an event for that time.
        //
        recvEnd->setContextPointer(data2);
        scheduleAt(simTime()+frame->getDuration(), recvEnd);
    }

    // In the Token Ring protocol, a frame is stripped out from the ring
    // by the source of the frame when the frame arrives back to its
    // originating station. The addresse just repeats the frame, like
    // any other station in the ring.
    if (source == myAddress)
    {
        if (debug)
        {
            ev << "Arrived back to sender, stripping it out of the ring" << endl;
        }
        delete frame;
    }
    else
    {
        if (debug)
        {
            ev << "Forwarding to next station" << endl;
        }
        send(frame, "phyOut");
    }
}

void TokenRingMAC::endReceiveFrame(cMessage *data)
{
    if (debug)
    {
        ev << "End receiving frame containing \"" << data->getName() << "\", passing up to higher layer." << endl;
    }
    send(data, "toHL");
}

void TokenRingMAC::finish()
{
    ev << "Module: " << getFullPath() << endl;
    ev << "Total packets received from higher layer: " << numPacketsToSend << endl;
    ev << "Higher layer still in queue: " << sendQueue.length() << endl;
    ev << "Higher layer packets dropped: " << numPacketsToSendDropped << endl;
    if (numPacketsToSend>sendQueue.length())
    {
        ev << "Percentage dropped: " << (100*numPacketsToSendDropped/(numPacketsToSend-sendQueue.length())) << "%" << endl;
    }
    ev << endl;
}


