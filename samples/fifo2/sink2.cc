//-------------------------------------------------------------
// file: sink2.cc
//        (part of Fifo2 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include <omnetpp.h>


class FF2Sink : public cSimpleModule
{
  public:
    Module_Class_Members(FF2Sink,cSimpleModule,0)

    cStdDev qstats;
    cOutVector qtime;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module( FF2Sink );

void FF2Sink::initialize()
{
    qstats.setName("queuing time stats");
    qtime.setName("queueing time vector");
}

void FF2Sink::handleMessage(cMessage *msg)
{
    double d = simTime()-msg->timestamp();
    ev << "Received " << msg->name() << ", queueing time: " << d << "sec" << endl;
    qstats.collect( d );
    qtime.record( d );
    delete msg;
}

void FF2Sink::finish()
{
    ev << "Total jobs processed: " << qstats.samples() << endl;
    ev << "Avg queueing time:    " << qstats.mean() << endl;
    ev << "Max queueing time:    " << qstats.max() << endl;
    ev << "Standard deviation:   " << qstats.stddev() << endl;
}
