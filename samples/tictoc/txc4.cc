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
 * In this step you'll learn how to add input parameters to the simulation:
 * we'll turn the "magic number" 10 into a parameter.
 */
class Txc4 : public cSimpleModule
{
  private:
    int counter;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Txc4);

void Txc4::initialize()
{
    // Initialize the counter with the "limit" module parameter, declared
    // in the NED file (tictoc4.ned).
    counter = par("limit");

    if (strcmp("tic", name()) == 0)
    {
        ev << "Sending initial message\n";
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
    }
}

void Txc4::handleMessage(cMessage *msg)
{
    counter--;
    if (counter==0)
    {
        ev << name() << "'s counter reached zero, deleting message\n";
        delete msg;
    }
    else
    {
        ev << name() << "'s counter is " << counter << ", sending back message\n";
        send(msg, "out");
    }
}

