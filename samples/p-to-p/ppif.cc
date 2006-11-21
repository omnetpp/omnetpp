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
class PointToPointIF : public cSimpleModule
{
  private:
    long frameCapacity;
    cGate *gateToWatch;

    cQueue queue;
    cMessage *endTransmissionEvent;

    long numFramesDropped;

  public:
    PointToPointIF();
    virtual ~PointToPointIF();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    virtual void startTransmitting(cMessage *msg);

    virtual void displayStatus(bool isBusy);
};

Define_Module(PointToPointIF);

PointToPointIF::PointToPointIF()
{
    endTransmissionEvent = NULL;
}

PointToPointIF::~PointToPointIF()
{
    cancelAndDelete(endTransmissionEvent);
}

void PointToPointIF::initialize()
{
    numFramesDropped = 0;
    WATCH(numFramesDropped);

    queue.setName("queue");
    endTransmissionEvent = new cMessage("endTxEvent");

    frameCapacity = par("frameCapacity");

    gateToWatch = gate("lineOut");

    // check that we're really connected to a gate with data rate
    cBasicChannel *chan = check_and_cast<cBasicChannel*>(gate("lineOut")->channel());
    if (!chan->datarate())
        error("must be connected to a link with data rate");
}

void PointToPointIF::startTransmitting(cMessage *msg)
{
    if (ev.isGUI()) displayStatus(true);

    ev << "Starting transmission of " << msg << endl;
    send(msg, "lineOut");

    // The schedule an event for the time when last bit will leave the gate.
    simtime_t endTransmission = gateToWatch->transmissionFinishes();
    scheduleAt(endTransmission, endTransmissionEvent);
}

void PointToPointIF::handleMessage(cMessage *msg)
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
    else if (msg->arrivedOn("lineIn"))
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

void PointToPointIF::displayStatus(bool isBusy)
{
    displayString().setTagArg("t",0, isBusy ? "transmitting" : "idle");
    displayString().setTagArg("i",1, isBusy ? (queue.length()>=3 ? "red" : "yellow") : "");
}

