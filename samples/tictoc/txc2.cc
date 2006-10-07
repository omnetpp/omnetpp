//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003 Ahmet Sekercioglu
// Copyright (C) 2003-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <string.h>
#include <omnetpp.h>


/**
 * In this class we add some debug messages to Txc1. When you run the
 * simulation in the OMNeT++ GUI Tkenv, the output will appear in
 * the main text window, and you can also open separate output windows
 * for `tic' and `toc'.
 */
class Txc2 : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Txc2);

void Txc2::initialize()
{
    if (strcmp("tic", name()) == 0)
    {
        // The `ev' object works like `cout' in C++.
        ev << "Sending initial message\n";
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
    }
}

void Txc2::handleMessage(cMessage *msg)
{
    // msg->name() is name of the msg object, here it will be "tictocMsg".
    ev << "Received message `" << msg->name() << "', sending it out again\n";
    send(msg, "out");
}

