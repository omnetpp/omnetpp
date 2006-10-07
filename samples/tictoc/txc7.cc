//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>


/**
 * Let us take a step back, and remove random delaying from the code.
 * We'll leave in, however, losing the packet with a small probability.
 * And, we'll we do something very common in telecommunication networks:
 * if the packet doesn't arrive within a certain period, we'll assume it
 * was lost and create another one. The timeout will be handled using
 * (what else?) a self-message.
 */
class Tic7 : public cSimpleModule
{
  private:
    double timeout;  // timeout
    cMessage *timeoutEvent;  // holds pointer to the timeout self-message

  public:
    Tic7();
    virtual ~Tic7();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Tic7);

Tic7::Tic7()
{
    timeoutEvent = NULL;
}

Tic7::~Tic7()
{
    cancelAndDelete(timeoutEvent);
}

void Tic7::initialize()
{
    // Initialize variables.
    timeout = 1.0;
    timeoutEvent = new cMessage("timeoutEvent");

    // Generate and send initial message.
    ev << "Sending initial message\n";
    cMessage *msg = new cMessage("tictocMsg");
    send(msg, "out");
    scheduleAt(simTime()+timeout, timeoutEvent);
}

void Tic7::handleMessage(cMessage *msg)
{
    if (msg==timeoutEvent)
    {
        // If we receive the timeout event, that means the packet hasn't
        // arrived in time and we have to re-send it.
        ev << "Timeout expired, resending message and restarting timer\n";
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
        scheduleAt(simTime()+timeout, timeoutEvent);
    }
    else // message arrived
    {
        // Acknowledgement received -- delete the stored message and cancel
        // the timeout event.
        ev << "Timer cancelled.\n";
        cancelEvent(timeoutEvent);

        // Ready to send another one.
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
        scheduleAt(simTime()+timeout, timeoutEvent);
    }
}


/**
 * Sends back an acknowledgement -- or not.
 */
class Toc7 : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Toc7);

void Toc7::handleMessage(cMessage *msg)
{
    if (uniform(0,1) < 0.1)
    {
        ev << "\"Losing\" message.\n";
        bubble("message lost");  // making animation more informative...
        delete msg;
    }
    else
    {
        ev << "Sending back same message as acknowledgement.\n";
        send(msg, "out");
    }
}


