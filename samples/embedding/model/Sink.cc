//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

using namespace omnetpp;

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
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

Define_Module(Sink);

void Sink::initialize()
{
    qstats.setName("queueing time stats");
    qtime.setName("queueing time vector");
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t d = simTime()-msg->getCreationTime();
    EV << "Received " << msg->getName() << ", queueing time: " << d << "sec" << endl;
    qstats.collect(d);
    qtime.record(d);
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

