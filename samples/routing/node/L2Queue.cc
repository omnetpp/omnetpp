//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
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
    cGate *gateToWatch;

    cQueue queue;
    cMessage *endTransmissionEvent;

    long numFramesDropped;

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
    numFramesDropped = 0;
    WATCH(numFramesDropped);

    queue.setName("queue");
    endTransmissionEvent = new cMessage("endTxEvent");

    frameCapacity = par("frameCapacity");

    // we're connected if other end of connection path is an input gate
    cGate *lineOut = gate("line$o");
    bool connected = lineOut->getDestinationGate()->getType()==cGate::INPUT;

    // if we're connected, get the gate with transmission rate
    gateToWatch = lineOut;
    double datarate = 0;
    if (connected)
    {
        while (gateToWatch)
        {
            // does this gate have data rate?
            cDatarateChannel *chan = dynamic_cast<cDatarateChannel*>(gateToWatch->getChannel());
            if (chan && (datarate=chan->par("datarate").doubleValue())>0)
                break;
            // otherwise just check next connection in path
            gateToWatch = gateToWatch->getToGate();
        }
        if (!gateToWatch)
            error("gate line must be connected (directly or indirectly) to a link with data rate");
    }
}

void L2Queue::startTransmitting(cMessage *msg)
{
    if (ev.isGUI()) displayStatus(true);

    ev << "Starting transmission of " << msg << endl;
    send(msg, "line$o");

    // The schedule an event for the time when last bit will leave the gate.
    simtime_t endTransmission = gateToWatch->getTransmissionFinishTime();
    scheduleAt(endTransmission, endTransmissionEvent);
}

void L2Queue::handleMessage(cMessage *msg)
{
    if (msg==endTransmissionEvent)
    {
        // Transmission finished, we can start next one.
        ev << "Transmission finished.\n";
        if (ev.isGUI()) displayStatus(false);
        if (!queue.empty())
        {
            msg = (cMessage *) queue.pop();
            startTransmitting(msg);
        }
    }
    else if (msg->arrivedOn("line$i"))
    {
        // pass up
        send(msg,"out");
    }
    else // arrived on gate "in"
    {
        if (endTransmissionEvent->isScheduled())
        {
            // We are currently busy, so just queue up the packet.
            if (frameCapacity && queue.length()>=frameCapacity)
            {
                ev << "Received " << msg << " but transmitter busy and queue full: discarding\n";
                numFramesDropped++;
                delete msg;
            }
            else
            {
                ev << "Received " << msg << " but transmitter busy: queueing up\n";
                queue.insert(msg);
            }
        }
        else
        {
            // We are idle, so we can start transmitting right away.
            ev << "Received " << msg << endl;
            startTransmitting(msg);
        }
    }
}

void L2Queue::displayStatus(bool isBusy)
{
    getDisplayString().setTagArg("t",0, isBusy ? "transmitting" : "idle");
    getDisplayString().setTagArg("i",1, isBusy ? (queue.length()>=3 ? "red" : "yellow") : "");
}

