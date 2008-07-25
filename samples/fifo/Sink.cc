//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

namespace fifo {

/**
 * Packet sink; see NED file for more info.
 */
class Sink : public cSimpleModule
{
  private:
    cStdDev qstats;
    cOutVector qtime;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module( Sink );

void Sink::initialize()
{
    qstats.setName("queueing time stats");
    qtime.setName("queueing time vector");
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t d = simTime()-msg->getCreationTime();
    EV << "Received " << msg->getName() << ", queueing time: " << d << "sec" << endl;
    qstats.collect( d );
    qtime.record( d );
    delete msg;
}

void Sink::finish()
{
    EV << "Total jobs processed: " << qstats.getCount() << endl;
    EV << "Avg queueing time:    " << qstats.getMean() << endl;
    EV << "Max queueing time:    " << qstats.getMax() << endl;
    EV << "Standard deviation:   " << qstats.getStddev() << endl;
}

}; //namespace

