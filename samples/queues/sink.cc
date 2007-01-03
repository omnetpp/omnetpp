//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "sink.h"


Define_Module( Sink );

void Sink::initialize()
{
    waitStats.setName("Waiting time statistics");
}

void Sink::handleMessage(cMessage *msg)
{
    double d = simTime() - msg->timestamp();
    waitStats.record( d );
    delete msg;
}

void Sink::finish()
{
    ev << "*** Module: " << fullPath() << "***" << endl;
}



