#include <omnetpp.h>

namespace testlib {

class TestChannel : public cIdealChannel
{
  private:
    int numPackets;
  protected:
    virtual void initialize();
    virtual void processMessage(cMessage *msg, simtime_t at, result_t& result);
    virtual void finish();
};

Define_Channel(TestChannel);


void TestChannel::initialize()
{
    ev << "Initializing TestChannel " << getFullPath() << "\n";
    numPackets = 0;
}

void TestChannel::processMessage(cMessage *msg, simtime_t at, result_t& result)
{
    numPackets++;
    ev << "TestChannel delivering msg: " << msg->getName() << "\n";
    cIdealChannel::processMessage(msg, at, result);
}

void TestChannel::finish()
{
    ev << "TestChannel finishing, msgs: " << numPackets << "\n";
}

}
