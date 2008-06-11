//
// sink1.cc
//
// This file is part of an OMNeT++/OMNEST demo simulation.
//

#include "sink1.h"


Define_Module( FF1Sink );

void FF1Sink::activity()
{
    qstats.setName("queuing time stats");
    cOutVector qtime("queueing time vector");
    for(;;)
    {
        cMessage *msg = receive();
        simtime_t d = simTime() - msg->getTimestamp();
        ev << "Received " << msg->getName() << ", queueing time: " << d << "sec" << endl;
        qtime.record( d );
        qstats.collect( d );
        delete msg;
    }
}

void FF1Sink::finish()
{
    ev << "*** Module: " << getFullPath() << "***" << endl;
    ev << "Total jobs processed: " << qstats.getCount() << endl;
    ev << "Avg queueing time:    " << qstats.getMean() << endl;
    ev << "Max queueing time:    " << qstats.getMax() << endl;
    ev << "Standard deviation:   " << qstats.getStddev() << endl;
}
