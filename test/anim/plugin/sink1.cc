//
// sink1.cc
//
// This file is part of an OMNeT++/OMNEST demo simulation.
//

#include "sink1.h"

Define_Module(FF1Sink);

void FF1Sink::activity()
{
    qstats.setName("queuing time stats");
    cOutVector qtime("queueing time vector");
    for ( ; ; ) {
        cMessage *msg = receive();
        simtime_t d = simTime() - msg->getTimestamp();
        EV << "Received " << msg->getName() << ", queueing time: " << d << "sec" << endl;
        qtime.record(d);
        qstats.collect(d);
        delete msg;
    }
}

void FF1Sink::finish()
{
    EV << "*** Module: " << getFullPath() << "***" << endl;
    EV << "Total jobs processed: " << qstats.getCount() << endl;
    EV << "Avg queueing time:    " << qstats.getMean() << endl;
    EV << "Max queueing time:    " << qstats.getMax() << endl;
    EV << "Standard deviation:   " << qstats.getStddev() << endl;
}

