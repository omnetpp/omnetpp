//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// Authors: Andras Varga, Gabor Lencse
// Based on the code by: Maurits Andre, George van Montfort,
// Gerard van de Weerd (TU Delft)
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>
#include "TokenRing_m.h"


// Frame header and trailer are 21 octets (168 bits), with the following
// frame layout (lengths in parens):
//   SD(1), AC(1), FC(1), DEST(6), SOURCE(6), DATA(n), CS(4), ED(1), FS(1)
#define TR_FRAME_HEADER_BITS    120
#define TR_FRAME_TRAILER_BITS    48
#define TR_FRAME_OVERHEAD_BITS  168

// Token length is 3 octets (24 bits)
#define TR_TOKEN_BITS  24


/**
 * Token Ring MAC layer; see NED file for more info.
 */
class TokenRingMAC : public cSimpleModule
{
  private:
    // configuration
    int myAddress;
    simtime_t tokenHoldingTime;
    int queueMaxLen;

    // state variables
    cMessage *transmEnd;     // self-message to signal end of transmission
    cMessage *headerRecvEnd; // self-message to signal when header was fully received
    cMessage *recvEnd;       // self-message to signal end of reception
    TRFrame *frameBeingReceived;  // the frame being received, or NULL
    TRToken *token;          // non-NULL while we're holding the token
    simtime_t thtExpiryTime; // if token!=NULL: when tokenHoldingTime expires
    cPacketQueue sendQueue;  // send buffer; holds TRDataFrames

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
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
#if 0  //work in progress
    virtual void enqueueAppData(TRApplicationData *data);
    virtual void tokenCaptured(TRToken *receivedToken);
    virtual void releaseToken();
    virtual bool startTransmittingIfPossible();
    virtual void endTransmission();
    virtual void beginReceiveFrame(TRDataFrame *frame);
    virtual void endReceiveFrame(cMessage *data);
#endif
};


Define_Module(TokenRingMAC);


TokenRingMAC::TokenRingMAC()
{
    transmEnd = recvEnd = headerRecvEnd = NULL;
    frameBeingReceived = NULL;
    token = NULL;
}

TokenRingMAC::~TokenRingMAC()
{
    cancelAndDelete(transmEnd);
    cancelAndDelete(recvEnd);
    cancelAndDelete(headerRecvEnd);
    delete frameBeingReceived;
    delete token;
}

void TokenRingMAC::initialize()
{
    gate("phyIn")->setDeliverOnReceptionStart(true);

    tokenHoldingTime = par("tokenHoldingTime");   // typically 10ms
    myAddress = par("address");
    queueMaxLen = par("queueMaxLen");

    sendQueue.setName("sendQueue");
    queueLenPackets.setName("Queue length (packets)");
    queueLenBytes.setName("Queue length (bytes)");
    queueingTime.setName("Queueing time (sec)");

    numPacketsToSend = 0;
    WATCH (numPacketsToSend);
    numPacketsToSendDropped = 0;
    WATCH (numPacketsToSendDropped);
    queueDrops.setName("App data packets dropped");

    transmEnd = new cMessage("transmEnd");
    recvEnd = new cMessage("recvEnd");
    headerRecvEnd = new cMessage("headerRecvEnd");

    // station 0 issues the token
    if (myAddress == 0)
    {
        TRToken *token = new TRToken("token", TR_TOKEN);
        token->setBitLength(TR_TOKEN_BITS);
        send(token, "phyOut");
    }
}

void TokenRingMAC::handleMessage(cMessage *msg)
{
#if 0  //work in progress
    if (dynamic_cast<TRToken *>(msg)!=NULL) {
        startReceivingToken((TRToken *)msg);
    }
    else if (dynamic_cast<TRDataFrame *>(msg)!=NULL) {
        startReceivingDataFrame((TRDataFrame *)msg);
    }
    else if (msg==recvEnd && token!=NULL) {
        tokenFullyReceived();
    }
    else if (msg==headerRecvEnd) {
        dataFrameHeaderReceived();
    }
    else if (msg==recvEnd && frameBeingReceived!=NULL)
    {
        cMessage *data = (cMessage *) recvEnd->getContextPointer();
        endReceiveFrame(data);
        // end receiving a data frame
        frameBeingReceived = NULL;
        // TODO if we have the token, we can start transmitting another frame if there's time
    }
    if (dynamic_cast<TRApplicationData *>(msg)!=NULL)
    {
        // data from higher layer: store packet in transmit queue
        enqueueAppData((TRApplicationData *)msg);
    }
    else if (msg==transmEnd)
    {
        // end transmission of token or data frame
        EV << "End transmission\n";

        //FIXME
        startTransmittingIfPossible();
        if (!transmEnd->isScheduled())
            releaseToken();
    }
    else
    {
        throw cRuntimeError("unexpected message arrived: (%s)%s", msg->getClassName(), msg->getName());
    }
#endif
}

#if 0  //work in progress
void TokenRingMAC::enqueueAppData(TRApplicationData *data)
{
    EV << "Data received from higher layer: \"" << data->getName() << "\", "
       << "length=" << data->getByteLength() << "bytes" << endl;

    // inc counter
    numPacketsToSend++;

    // if queue is full, we have to drop it
    if (sendQueue.length() >= queueMaxLen)
    {
        EV << "Queue full, DROPPED!" << endl;
        delete data;
        numPacketsToSendDropped++;
        queueDrops.record(numPacketsToSendDropped);
        return;
    }

    // encapsulate into a TokenRingFrame
    char msgname[30];
    sprintf(msgname, "%d-->%d", myAddress, data->getDestination());
    TRDataFrame *frame = new TRDataFrame(msgname, TR_FRAME);
    frame->setBitLength(TR_FRAME_OVERHEAD_BITS);
    frame->setSource(myAddress);
    frame->setDestination(data->getDestination());
    frame->encapsulate(data);

    // mark enqueue time, we'll need it for calculating time spent in queue
    frame->setTimestamp();

    // insert message into send buffer
    sendQueue.insert(frame);
    queueLenPackets.record(sendQueue.length());
    queueLenBytes.record(sendQueue.getByteLength());
    EV << "Enqueued, queue length=" << sendQueue.length() << endl;
}

void TokenRingMAC::startReceivingToken(TRToken *msg)
{
    // beginning of token arrived
    ASSERT(token->isReceptionStart());
    ASSERT(!transmEnd->isScheduled()); // only the station holding the token can be transmitting
    ASSERT(!recvEnd->isScheduled() && !headerRecvEnd->isScheduled()); // we're not already receiving

    token = msg;

    scheduleAt(simTime() + TR_TOKEN_BITS*dataRate, recvEnd);
}

void TokenRingMAC::startReceivingDataFrame(TRDataFrame *msg)
{
    // beginning of data frame arrived
    ASSERT(frameBeingReceived->isReceptionStart());
    ASSERT(!recvEnd->isScheduled() && !headerRecvEnd->isScheduled()); // we're not already receiving

    frameBeingReceived = msg;

    scheduleAt(simTime() + TR_FRAME_HEADER_BITS / dataRate, headerRecvEnd);
    scheduleAt(simTime() + frameBeingReceived->getDuration(), recvEnd);
}

void TokenRingMAC::tokenFullyReceived()
{
    // token fully received
    EV << "Token arrived (we can keep it for THT=" << tokenHoldingTime << ")" << endl;
    thtExpiryTime = simTime() + tokenHoldingTime;

    if (ev.isGUI())
    {
        getParentModule()->getDisplayString().setTagArg("i",1,"gold");
        getParentModule()->getDisplayString().setTagArg("t",0,"ACTIVE");
        getParentModule()->getDisplayString().setTagArg("t",2,"#707000");
        getParentModule()->bubble(sendQueue.empty() ? "Nothing to send!" : "Captured token.");
    }

    // start transmitting data frames, or pass on the token
    startTransmittingIfPossible();
    if (!transmEnd->isScheduled())
        releaseToken();
}

void TokenRingMAC::dataFrameHeaderReceived()
{
    // header of data frame fully arrived
    ASSERT(frameBeingReceived!=NULL);
    TRDataFrame frame = frameBeingReceived; // abbreviation
    EV << "Received header of frame \"" << frame->getName() << "\"" << endl;

    // extract source and destination
    int dest = frame->getDestination();
    int source = frame->getSource();

    // In the Token Ring protocol, a frame is stripped out from the ring
    // by the source of the frame when the frame arrives back to its
    // originating station. The addressee just repeats the frame, like
    // any other station in the ring.
    if (source == myAddress)
    {
        EV << "Arrived back to sender, stripping it out of the ring" << endl;
        delete frame;
    }
    else
    {
        EV << "Forwarding to next station" << endl;
        send(frame, "phyOut");
    }
}



    //TODO extract or repeat; or send up
    //FIXME
    startTransmittingIfPossible();
    if (!transmEnd->isScheduled())
        releaseToken();
}

void TokenRingMAC::releaseToken()
{
    EV << "Releasing token." << endl;
    if (ev.isGUI())
    {
        getParentModule()->getDisplayString().setTagArg("i",1,"");
        getParentModule()->getDisplayString().setTagArg("t",0,"");
    }

    send(token, "phyOut");
    token = NULL;
    scheduleAt(dfdfdf, transmEnd);
}

bool TokenRingMAC::startTransmittingIfPossible()
{
    // if we have messages to send, send them now, within the tokenHoldingTime
    thtExpiryTime = simTime()+tokenHoldingTime;
    if (sendQueue.empty())
    {
        EV << "Transmit queue empty\n";
        return false;
    }

    // check if next frame fits into this THT window
    TRDataFrame *frame = (TRDataFrame *) sendQueue.front();
    simtime_t transmissionDuration = frame->getBitLength() / dataRate;
    if (simTime()+transmissionDuration > thtExpiryTime)
    {
        EV << "Frame \"" << data->getName() << "\" cannot be sent within THT\n";
        return false;
    }

    // remove data packet from the send buffer
    sendQueue.pop();
    queueLenPackets.record(sendQueue.length());
    queueLenBytes.record(sendQueue.getByteLength());
    queueingTime.record(simTime() - frame->getTimestamp());

    EV << "Began transmitting frame \"" << frame->getName() << "\"\n";
    send(frame, "phyOut");

    // we're busy until we finished transmission
    scheduleAt(simTime()+transmissionDuration, transmEnd);

    return true;
}

void TokenRingMAC::endReceiveFrame(cMessage *data)
{
    // is this station the addressee?
    if (dest == myAddress)
    {
        EV << "We are the destination: as soon as we fully received the frame," << endl;
        EV << "we'll pass up a copy of the payload \"" << frame->getEncapsulatedMsg()->getName() << "\" to the higher layer." << endl;
        EV << "Frame will be extracted from the ring by the sender." << endl;

        // make a copy of the payload -- we'll deliver it to the higher layer
        cPacket *data2 = frame->getEncapsulatedMsg()->dup();

        // The arrival of a message in the model represents the arrival
        // of the first bit of the packet. It has to be completely received
        // before we can pass it up to the higher layer. So here we compute
        // when receiving will finish, and schedule an event for that time.
        //
        recvEnd->setContextPointer(data2);
        scheduleAt(simTime()+frame->getDuration(), recvEnd);
    }

    EV << "End receiving frame containing \"" << data->getName() << "\", passing up to higher layer." << endl;
    send(data, "toHL");
}
#endif  //work in progress

void TokenRingMAC::finish()
{
    ev << "Module: " << getFullPath() << endl;
    ev << "Total packets received from higher layer: " << numPacketsToSend << endl;
    ev << "Higher layer still in queue: " << sendQueue.length() << endl;
    ev << "Higher layer packets dropped: " << numPacketsToSendDropped << endl;
    if (numPacketsToSend>sendQueue.length())
        ev << "Percentage dropped: " << (100*numPacketsToSendDropped/(numPacketsToSend-sendQueue.length())) << "%" << endl;
    ev << endl;
}


