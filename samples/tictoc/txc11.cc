//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 2003 Ahmet Sekercioglu
// Copyright (C) 2003-2004 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include "tictoc11_m.h"


/**
 * In this step we keep track of how many messages we send and received,
 * and display it above the icon.
 */
class Txc11 : public cSimpleModule
{
  protected:
    long numSent;
    long numReceived;

  public:
    Module_Class_Members(Txc11, cSimpleModule, 0);

    virtual TicTocMsg11 *generateMessage();
    virtual void forwardMessage(TicTocMsg11 *msg);
    virtual void updateDisplay();

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Txc11);

TicTocMsg11 *Txc11::generateMessage()
{
    // Produce source and destination addresses.
    int src = index();   // our module index
    int n = size();      // module vector size
    int dest = intuniform(0,n-2);
    if (dest>=src) dest++;

    char msgname[20];
    sprintf(msgname, "tic-%d-to-%d", src, dest);

    // Create message object and set source and destination field.
    TicTocMsg11 *msg = new TicTocMsg11(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    return msg;
}

void Txc11::forwardMessage(TicTocMsg11 *msg)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);

    // Same routing as before: random gate.
    int n = gate("out")->size();
    int k = intuniform(0,n-1);

    ev << "Forwarding message " << msg << " on port out[" << k << "]\n";
    send(msg, "out", k);
}

void Txc11::initialize()
{
    numSent = numReceived = 0;

    // Module 0 sends the first message
    if (index()==0)
    {
        // Boot the process scheduling the initial message as a self-message.
        TicTocMsg11 *msg = generateMessage();
        scheduleAt(0.0, msg);
    }
}

void Txc11::handleMessage(cMessage *msg)
{
    TicTocMsg11 *ttmsg = check_and_cast<TicTocMsg11 *>(msg);

    if (ttmsg->getDestination()==index())
    {
        // Message arrived
        int hopcount = ttmsg->getHopCount();
        ev << "Message " << ttmsg << " arrived after " << hopcount << " hops.\n";
        numReceived++;
        delete ttmsg;
        bubble("ARRIVED, starting new one!");

        // Generate another one.
        ev << "Generating another message: ";
        TicTocMsg11 *newmsg = generateMessage();
        ev << newmsg << endl;
        forwardMessage(newmsg);
        numSent++;

        if (ev.isGUI())
            updateDisplay();
    }
    else
    {
        // We need to forward the message.
        forwardMessage(ttmsg);
    }
}

void Txc11::updateDisplay()
{
    char buf[40];
    sprintf(buf, "rcvd: %d sent: %d", numReceived, numSent);
    displayString().setTagArg("t",0,buf);
}


