//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * In this class we add a counter, and delete the message after ten exchanges.
 */
class Txc3 : public cSimpleModule
{
  private:
    int counter;  // Note the counter here

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Txc3);

void Txc3::initialize()
{
    // Initialize counter to ten. We'll decrement it every time and delete
    // the message when it reaches zero.
    counter = 10;

    // The WATCH() statement below will let you examine the variable under
    // Tkenv. After doing a few steps in the simulation, double-click either
    // `tic' or `toc', select the Contents tab in the dialog that pops up,
    // and you'll find "counter" in the list.
    WATCH(counter);

    if (strcmp("tic", getName()) == 0) {
        EV << "Sending initial message\n";
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
    }
}

void Txc3::handleMessage(cMessage *msg)
{
    // Increment counter and check value.
    counter--;
    if (counter == 0) {
        // If counter is zero, delete message. If you run the model, you'll
        // find that the simulation will stop at this point with the message
        // "no more events".
        EV << getName() << "'s counter reached zero, deleting message\n";
        delete msg;
    }
    else {
        EV << getName() << "'s counter is " << counter << ", sending back message\n";
        send(msg, "out");
    }
}

