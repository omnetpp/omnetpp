//
// file: gen1.cc
//
// This file is part of an OMNeT++/OMNEST demo simulation.
//

#include "gen1.h"

Define_Module(FF1Generator);

void FF1Generator::activity()
{
    // ia_time and msg_length are taken by reference,
    // because they can contain random values
    int msgKind = par("messageKind");
    cPar& arrivalRate = par("arrivalRate");
    int i = 0;
    while (true) {
        char msgName[32];
        sprintf(msgName, "job-%d", i++);

        EV << "Generating " << msgName << endl;

        cMessage *msg = new cMessage(msgName, msgKind);
        msg->setTimestamp();

        send(msg, "out");

        wait(1 / (double)arrivalRate);
    }
}

void FF1Generator::finish()
{
}

