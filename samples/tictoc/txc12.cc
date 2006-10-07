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
#include "tictoc12_m.h"


/**
 * This model is exciting enough so that we can collect some statistics.
 * We'll record in output vectors the hop count of every message upon arrival.
 * Output vectors are written into the omnetpp.vec file and can be visualized
 * with the Plove program.
 *
 * We also collect basic statistics (min, max, mean, std.dev.) and histogram
 * about the hop count which we'll print out at the end of the simulation.
 */
class Txc12 : public cSimpleModule
{
  private:
    long numSent;
    long numReceived;
    cLongHistogram hopCountStats;
    cOutVector hopCountVector;

  protected:
    virtual TicTocMsg12 *generateMessage();
    virtual void forwardMessage(TicTocMsg12 *msg);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    // The finish() function is called by OMNeT++ at the end of the simulation:
    virtual void finish();
};

Define_Module(Txc12);

void Txc12::initialize()
{
    // Initialize variables
    numSent = 0;
    numReceived = 0;
    WATCH(numSent);
    WATCH(numReceived);

    hopCountStats.setName("hopCountStats");
    hopCountStats.setRangeAutoUpper(0, 10, 1.5);
    hopCountVector.setName("HopCount");

    // Module 0 sends the first message
    if (index()==0)
    {
        // Boot the process scheduling the initial message as a self-message.
        TicTocMsg12 *msg = generateMessage();
        scheduleAt(0.0, msg);
    }
}

void Txc12::handleMessage(cMessage *msg)
{
    TicTocMsg12 *ttmsg = check_and_cast<TicTocMsg12 *>(msg);

    if (ttmsg->getDestination()==index())
    {
        // Message arrived
        int hopcount = ttmsg->getHopCount();
        ev << "Message " << ttmsg << " arrived after " << hopcount << " hops.\n";
        bubble("ARRIVED, starting new one!");

        // update statistics.
        numReceived++;
        hopCountVector.record(hopcount);
        hopCountStats.collect(hopcount);

        delete ttmsg;

        // Generate another one.
        ev << "Generating another message: ";
        TicTocMsg12 *newmsg = generateMessage();
        ev << newmsg << endl;
        forwardMessage(newmsg);
        numSent++;
    }
    else
    {
        // We need to forward the message.
        forwardMessage(ttmsg);
    }
}

TicTocMsg12 *Txc12::generateMessage()
{
    // Produce source and destination addresses.
    int src = index();
    int n = size();
    int dest = intuniform(0,n-2);
    if (dest>=src) dest++;

    char msgname[20];
    sprintf(msgname, "tic-%d-to-%d", src, dest);

    // Create message object and set source and destination field.
    TicTocMsg12 *msg = new TicTocMsg12(msgname);
    msg->setSource(src);
    msg->setDestination(dest);
    return msg;
}

void Txc12::forwardMessage(TicTocMsg12 *msg)
{
    // Increment hop count.
    msg->setHopCount(msg->getHopCount()+1);

    // Same routing as before: random gate.
    int n = gate("out")->size();
    int k = intuniform(0,n-1);

    ev << "Forwarding message " << msg << " on port out[" << k << "]\n";
    send(msg, "out", k);
}

void Txc12::finish()
{
    // This function is called by OMNeT++ at the end of the simulation.
    ev << "Sent:     " << numSent << endl;
    ev << "Received: " << numReceived << endl;
    ev << "Hop count, min:    " << hopCountStats.min() << endl;
    ev << "Hop count, max:    " << hopCountStats.max() << endl;
    ev << "Hop count, mean:   " << hopCountStats.mean() << endl;
    ev << "Hop count, stddev: " << hopCountStats.stddev() << endl;

    recordScalar("#sent", numSent);
    recordScalar("#received", numReceived);
    hopCountStats.recordScalar("hop count");
}


