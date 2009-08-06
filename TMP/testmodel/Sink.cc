//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

void emitD(int signal, double d);
void emitX(int signal, double d);

//#define emit(x,y)    // disable signals
//#define emit(x,y)    emitD(x,y) // empty version
//#define emit(x,y)    emitX(x,y) // lite version

namespace fifo {

/**
 * Packet sink; see NED file for more info.
 */
class Sink : public cSimpleModule
{
  private:
    //cStdDev qstats;
    //cOutVector qtime;
    simsignal_t endToEndDelaySignal;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module( Sink );

void Sink::initialize()
{
    //qstats.setName("queueing time stats");
    //qtime.setName("queueing time vector");

    endToEndDelaySignal = registerSignal("endToEndDelaySignal");
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t d = simTime()-msg->getCreationTime();
    EV << "Received " << msg->getName() << ", queueing time: " << d << "sec" << endl;
    //qstats.collect( d );
    //qtime.record( d );
    emit(endToEndDelaySignal, d.dbl());
    delete msg;
}

void Sink::finish()
{
    //EV << "Total jobs processed: " << qstats.getCount() << endl;
    //EV << "Avg queueing time:    " << qstats.getMean() << endl;
    //EV << "Max queueing time:    " << qstats.getMax() << endl;
    //EV << "Standard deviation:   " << qstats.getStddev() << endl;
}

}; //namespace

