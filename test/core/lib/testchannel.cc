#include <omnetpp.h>

USING_NAMESPACE

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
    EV << "Initializing TestChannel " << getFullPath() << "\n";
    numPackets = 0;
}

void TestChannel::processMessage(cMessage *msg, simtime_t at, result_t& result)
{
    numPackets++;
    EV << "TestChannel delivering msg: " << msg->getName() << "\n";
    cIdealChannel::processMessage(msg, at, result);
}

void TestChannel::finish()
{
    EV << "TestChannel finishing, msgs: " << numPackets << "\n";
}

}
