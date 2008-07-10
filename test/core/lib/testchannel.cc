#include <omnetpp.h>

class TestChannel : public cIdealChannel
{
  private:
    int numPackets;
  protected:
    virtual void initialize();
    virtual bool deliver(cMessage *msg, simtime_t at);
    virtual void finish();
};

Define_Channel(TestChannel);


void TestChannel::initialize()
{
    ev << "Initializing TestChannel " << getFullPath() << "\n";
    numPackets = 0;
}

bool TestChannel::deliver(cMessage *msg, simtime_t at)
{
    numPackets++;
    ev << "TestChannel delivering msg: " << msg->getName() << "\n";
    return cIdealChannel::deliver(msg, at);
}

void TestChannel::finish()
{
    ev << "TestChannel finishing, msgs: " << numPackets << "\n";
}

