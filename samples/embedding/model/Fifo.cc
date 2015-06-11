//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Fifo.h"

namespace fifo {

Define_Module(Fifo);

simtime_t Fifo::startService(cMessage *msg)
{
    EV << "Starting service of " << msg->getName() << endl;
    return par("serviceTime");
}

void Fifo::endService(cMessage *msg)
{
    EV << "Completed service of " << msg->getName() << endl;
    send(msg, "out");
}

}; //namespace

