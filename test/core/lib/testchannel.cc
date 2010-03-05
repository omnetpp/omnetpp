#include <omnetpp.h>

class TestChannel : public cIdealChannel
{
  private:
    int numPackets;
  protected:
    virtual void initialize();
    virtual void process(cMessage *msg, simtime_t at, result_t& result);
    virtual void finish();
};

Define_Channel(TestChannel);


void TestChannel::initialize()
{
    ev << "Initializing TestChannel " << getFullPath() << "\n";
    numPackets = 0;
}

void TestChannel::process(cMessage *msg, simtime_t at, result_t& result)
{
    numPackets++;
    ev << "TestChannel delivering msg: " << msg->getName() << "\n";
    cIdealChannel::process(msg, at, result);
}

void TestChannel::finish()
{
    ev << "TestChannel finishing, msgs: " << numPackets << "\n";
}

