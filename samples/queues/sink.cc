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
    waitStats.collect( d );
    delete msg;
}

void Sink::finish()
{
    ev << "*** Module: " << fullPath() << "***" << endl;
    ev << "Total jobs processed: " << waitStats.samples() << endl;
    ev << "Avg waiting time:    " << waitStats.mean() << endl;
    ev << "Max queueing time:    " << waitStats.max() << endl;
    ev << "Standard deviation:   " << waitStats.stddev() << endl;
}



