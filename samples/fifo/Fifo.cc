//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "Fifo.h"

namespace fifo {

Define_Module(PacketFifo);

simtime_t PacketFifo::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return par("serviceTime");
}

void PacketFifo::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;
    send( msg, "out" );
}

//----

Define_Module(BitFifo);

simtime_t BitFifo::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return msg->length() / (double)par("bitsPerSec");
}

void BitFifo::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;
    send( msg, "out" );
}

}; //namespace

