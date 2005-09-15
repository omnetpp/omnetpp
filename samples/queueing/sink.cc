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
 * Discards incoming messages, and collects statistics.
 */
class QSink : public cSimpleModule
{
  private:
    cStdDev qstats;
    cOutVector qtime;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module( QSink );

void QSink::initialize()
{
    qstats.setName("lifetime stats");
    qtime.setName("lifetime vector");
}

void QSink::handleMessage(cMessage *msg)
{
    double d = simTime()-msg->creationTime();
    ev << "Received " << msg->name() << ", lifetime: " << d << "sec" << endl;
    qstats.collect( d );
    qtime.record( d );
    delete msg;
}

void QSink::finish()
{
    ev << "Total jobs processed: " << qstats.samples() << endl;
    ev << "Avg lifetime:         " << qstats.mean() << endl;
    ev << "Max lifetime:         " << qstats.max() << endl;
    ev << "Standard deviation:   " << qstats.stddev() << endl;
}

