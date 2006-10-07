//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>


/**
 * Packet sink; see NED file for more info.
 */
class FFSink : public cSimpleModule
{
  private:
    cStdDev qstats;
    cOutVector qtime;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module( FFSink );

void FFSink::initialize()
{
    qstats.setName("queueing time stats");
    qtime.setName("queueing time vector");
}

void FFSink::handleMessage(cMessage *msg)
{
    double d = simTime()-msg->creationTime();
    ev << "Received " << msg->name() << ", queueing time: " << d << "sec" << endl;
    qstats.collect( d );
    qtime.record( d );
    delete msg;
}

void FFSink::finish()
{
    ev << "Total jobs processed: " << qstats.samples() << endl;
    ev << "Avg queueing time:    " << qstats.mean() << endl;
    ev << "Max queueing time:    " << qstats.max() << endl;
    ev << "Standard deviation:   " << qstats.stddev() << endl;
}

