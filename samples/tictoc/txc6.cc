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
 * In this step we'll introduce random numbers. We change the delay from 1s
 * to a random value which can be set from the NED file or from omnetpp.ini.
 * In addition, we'll "lose" (delete) the packet with a small probability.
 */
class Txc6 : public cSimpleModule
{
  private:
    cMessage *event;
    cMessage *tictocMsg;

  public:
    Txc6();
    virtual ~Txc6();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Txc6);

Txc6::Txc6()
{
    event = tictocMsg = NULL;
}

Txc6::~Txc6()
{
    cancelAndDelete(event);
    delete tictocMsg;
}

void Txc6::initialize()
{
    event = new cMessage("event");
    tictocMsg = NULL;

    if (strcmp("tic", name()) == 0)
    {
        ev << "Scheduling first send to t=5.0s\n";
        scheduleAt(5.0, event);
        tictocMsg = new cMessage("tictocMsg");
    }
}

void Txc6::handleMessage(cMessage *msg)
{
    if (msg==event)
    {
        ev << "Wait period is over, sending back message\n";
        send(tictocMsg, "out");
        tictocMsg = NULL;
    }
    else
    {
        // "Lose" the message with 0.1 probability:
        if (uniform(0,1) < 0.1)
        {
            ev << "\"Losing\" message\n";
            delete msg;
        }
        else
        {
            // The "delayTime" module parameter can be set to values like
            // "exponential(5)" (tictoc6.ned, omnetpp.ini), and then here
            // we'll get a different delay every time.
            double delay = par("delayTime");

            ev << "Message arrived, starting to wait " << delay << " secs...\n";
            tictocMsg = msg;
            scheduleAt(simTime()+delay, event);
        }
    }
}

