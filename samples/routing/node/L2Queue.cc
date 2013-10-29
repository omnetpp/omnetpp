//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>


/**
 * Point-to-point interface module. While one frame is transmitted,
 * additional frames get queued up; see NED file for more info.
 */
class L2Queue : public cSimpleModule
{
  private:
    long frameCapacity;

    cQueue queue;
    cMessage *endTransmissionEvent;

    simsignal_t qlenSignal;
    simsignal_t busySignal;
    simsignal_t queueingTimeSignal;
    simsignal_t dropSignal;
    simsignal_t txBytesSignal;
    simsignal_t rxBytesSignal;

  public:
    L2Queue();
    virtual ~L2Queue();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    virtual void startTransmitting(cMessage *msg);

    virtual void displayStatus(bool isBusy);
};

Define_Module(L2Queue);

L2Queue::L2Queue()
{
    endTransmissionEvent = NULL;
}

L2Queue::~L2Queue()
{
    cancelAndDelete(endTransmissionEvent);
}

void L2Queue::initialize()
{
    queue.setName("queue");
    endTransmissionEvent = new cMessage("endTxEvent");

    frameCapacity = par("frameCapacity");

    qlenSignal = registerSignal("qlen");
    busySignal = registerSignal("busy");
    queueingTimeSignal = registerSignal("queueingTime");
    dropSignal = registerSignal("drop");
    txBytesSignal = registerSignal("txBytes");
    rxBytesSignal = registerSignal("rxBytes");

    emit(qlenSignal, queue.length());
    emit(busySignal, false);
}

void L2Queue::startTransmitting(cMessage *msg)
{
    if (ev.isGUI()) displayStatus(true);

    EV << "Starting transmission of " << msg << endl;
    int64 numBytes = check_and_cast<cPacket *>(msg)->getByteLength();
    send(msg, "line$o");

    emit(txBytesSignal, (long)numBytes);

    // Schedule an event for the time when last bit will leave the gate.
    simtime_t endTransmission = gate("line$o")->getTransmissionChannel()->getTransmissionFinishTime();
    scheduleAt(endTransmission, endTransmissionEvent);
}

void L2Queue::handleMessage(cMessage *msg)
{
    if (msg==endTransmissionEvent)
    {
        // Transmission finished, we can start next one.
        EV << "Transmission finished.\n";
        if (ev.isGUI()) displayStatus(false);
        if (queue.empty())
        {
            emit(busySignal, false);
        }
        else
        {
            msg = (cMessage *) queue.pop();
            emit(queueingTimeSignal, simTime() - msg->getTimestamp());
            emit(qlenSignal, queue.length());
            startTransmitting(msg);
        }
    }
    else if (msg->arrivedOn("line$i"))
    {
        // pass up
        emit(rxBytesSignal, (long)check_and_cast<cPacket *>(msg)->getByteLength());
        send(msg,"out");
    }
    else // arrived on gate "in"
    {
        if (endTransmissionEvent->isScheduled())
        {
            // We are currently busy, so just queue up the packet.
            if (frameCapacity && queue.length()>=frameCapacity)
            {
                EV << "Received " << msg << " but transmitter busy and queue full: discarding\n";
                emit(dropSignal, (long)check_and_cast<cPacket *>(msg)->getByteLength());
                delete msg;
            }
            else
            {
                EV << "Received " << msg << " but transmitter busy: queueing up\n";
                msg->setTimestamp();
                queue.insert(msg);
                emit(qlenSignal, queue.length());
            }
        }
        else
        {
            // We are idle, so we can start transmitting right away.
            EV << "Received " << msg << endl;
            emit(queueingTimeSignal, SIMTIME_ZERO);
            startTransmitting(msg);
            emit(busySignal, true);
        }
    }
}

void L2Queue::displayStatus(bool isBusy)
{
    getDisplayString().setTagArg("t",0, isBusy ? "transmitting" : "idle");
    getDisplayString().setTagArg("i",1, isBusy ? (queue.length()>=3 ? "red" : "yellow") : "");
}

