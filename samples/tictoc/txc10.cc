//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003 Ahmet Sekercioglu
// Copyright (C) 2003-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>


// Include a generated file: the header file created from tictoc10.msg.
// It contains the definition of the TictocMsg10 class, derived from
// cMessage.
#include "tictoc10_m.h"


/**
 * In this step the destination address is no longer node 2 -- we draw a
 * random destination, and we'll add the destination address to the message.
 *
 * The best way is to subclass cMessage and add destination as a data member.
 * Hand-coding the message class is usually tiresome because it contains
 * a lot of boilerplate code, so we let OMNeT++ generate the class for us.
 * The message class specification is in tictoc10.msg -- tictoc10_m.h
 * and .cc will be generated from this file automatically.
 *
 * To make the model execute longer, after a message arrives to its destination
 * the destination node will generate another message with a random destination
 * address, and so forth.
 */
class Txc10 : public cSimpleModule
{
  protected:
    virtual TicTocMsg10 *generateMessage();
    virtual void forwardMessage(TicTocMsg10 *msg);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Txc10);

void Txc10::initialize()
{
    // Module 0 sends the first message
    if (index()==0)
    {
        // Boot the process scheduling the initial message as a self-message.
        TicTocMsg10 *msg = generateMessage();
        scheduleAt(0.0, msg);
    }
}

void Txc10::handleMessage(cMessage *msg)
{
    TicTocMsg10 *ttmsg = check_and_cast<TicTocMsg10 *>(msg);

    if (ttmsg->getDestination()==index())
    {
        // Message arrived.
        ev << "Message " << ttmsg << " arrived after " << ttmsg->getHopCount() << " hops.\n";
        bubble("ARRIVED, starting new one!");
        delete ttmsg;

        // Generate another one.
        ev << "Generating another message: ";
        TicTocMsg10 *newmsg = generateMessage();
        ev << newmsg << endl;
        forwardMessage(newmsg);
    }
    else
    {
        // We need to forward the message.
        forwardMessage(ttmsg);
    }
}

TicTocMsg10 *Txc10::generateMessage()
{
    // Produce source and destination addresses.
    int src = index();   // our module index
    int n = size();      // module vector size
    int dest = intuniform(0,n-2);
    if (dest>=src) dest++;

    char msgname[20];
    sprintf(msgname, "tic-%d-to-%d", src, dest);

    // Create message object and set source and destination field.
    TicTocMsg10 *msg = new TicTocMsg10(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    return msg;
}

void Txc10::forwardMessage(TicTocMsg10 *msg)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);

    // Same routing as before: random gate.
    int n = gate("out")->size();
    int k = intuniform(0,n-1);

    ev << "Forwarding message " << msg << " on port out[" << k << "]\n";
    send(msg, "out", k);
}

