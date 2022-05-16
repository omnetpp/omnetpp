//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003 Ahmet Sekercioglu
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <cstring>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * In this step we add some debug messages to Txc1. When you run the
 * simulation in the OMNeT++ Qtenv GUI, the log will appear in the bottom
 * panel of the Qtenv window. To see only the log from `tic' or `toc' alone,
 * go into them by double-clicking their icons, and the bottom panel will
 * be filtered accordingly. (You can go back with the up arrow button on
 * the toolbar.)
 */
class Txc2 : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Txc2);

void Txc2::initialize()
{
    if (strcmp("tic", getName()) == 0) {
        // The `ev' object works like `cout' in C++.
        EV << "Sending initial message\n";
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
    }
}

void Txc2::handleMessage(cMessage *msg)
{
    // msg->getName() is name of the msg object, here it will be "tictocMsg".
    EV << "Received message `" << msg->getName() << "', sending it out again\n";
    send(msg, "out");
}

