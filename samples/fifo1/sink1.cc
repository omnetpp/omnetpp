//-------------------------------------------------------------
// file: sink1.cc
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "sink1.h"


Define_Module( FF1Sink );

void FF1Sink::activity()
{
    qstats.setName("queuing time stats");
    cOutVector qtime("queueing time vector");
    for(;;)
    {
        cMessage *msg = receive();
        double d = simTime()-msg->timestamp();
        ev << "Received " << msg->name() << ", queueing time: " << d << "sec" << endl;
        qtime.record( d );
        qstats.collect( d );
        delete msg;
    }
}

void FF1Sink::finish()
{
    ev << "*** Module: " << fullPath() << "***" << endl;
    ev << "Total jobs processed: " << qstats.samples() << endl;
    ev << "Avg queueing time:    " << qstats.mean() << endl;
    ev << "Max queueing time:    " << qstats.max() << endl;
    ev << "Standard deviation:   " << qstats.stddev() << endl;

    ev << endl;
    ev << "Stack allocated:      " << stackSize() << " bytes";
    ev << " (includes " << ev.extraStackForEnvir() << " bytes for environment)" << endl;
    ev << "Stack actually used: " << stackUsage() << " bytes" << endl;
}
