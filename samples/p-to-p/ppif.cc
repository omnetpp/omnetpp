//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>


/**
 * Point-to-point interface module.
 *
 * While one frame is transmitted, additional frames get queued up.
 */
class PointToPointIF : public cSimpleModule
{
  protected:
    long frameCapacity;
    long bitCapacity;
    cGate *gateToWatch;

    cQueue queue;
    cMessage *endTransmissionEvent;

  public:
    Module_Class_Members(PointToPointIF, cSimpleModule, 0);

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    virtual void startTransmitting(cMessage *msg);
};

Define_Module(PointToPointIF);

void PointToPointIF::initialize()
{
    queue.setName("queue");
    endTransmissionEvent = new cMessage("endTxEvent");

    frameCapacity = par("frameCapacity"); // FIXME implement, via cMessageQueue!!
    bitCapacity = par("bitCapacity");

    gateToWatch = gate("lineOut"); // FIXME the one with 1st transmissionfinishes

    // check that we're really connected to a gate with data rate
    cSimpleChannel *chan = check_and_cast<cSimpleChannel*>(gate("lineOut")->channel());
    if (!chan->datarate())
        error("must be connected to a link with data rate");
}


void PointToPointIF::startTransmitting(cMessage *msg)
{
    if (ev.isGUI()) displayString().setTagArg("i",1, queue.length()>=3 ? "red" : "yellow");

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
        if (ev.isGUI()) displayString().setTagArg("i",1,"");
        if (!queue.empty())
        {
            msg = (cMessage *) queue.getTail();
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
            ev << "Received " << msg << " but transmitter busy, queueing.\n";
            queue.insert(msg);
        }
        else
        {
            // We are idle, so we can start transmitting right away.
            ev << "Received " << msg << endl;
            startTransmitting(msg);
        }
    }
}

