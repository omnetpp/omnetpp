//------------------------------------------------------------- file:
// file: sink.cc
//      (part of Queues - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "sink.h"


Define_Module( Sink );

void Sink::initialize()
{
    waitStats.setName("Waiting time statistics");
}

void Sink::activity()
{
    for(;;)
    {
        cMessage *msg = receive();
        double d = simTime()-msg->timestamp();

        waitStats.collect( d );

        delete msg;

    }
}

void Sink::finish()
{
    ev << "*** Module: " << fullPath() << "***" << endl;
    ev << "Total jobs processed: " << waitStats.samples() << endl;
    ev << "Avg waiting time:    " << waitStats.mean() << endl;
    ev << "Max queueing time:    " << waitStats.max() << endl;
    ev << "Standard deviation:   " << waitStats.stddev() << endl;
}



