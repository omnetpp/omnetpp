#include <omnetpp.h>

using namespace omnetpp;

namespace testlib {

class TestChannel : public cIdealChannel
{
  private:
    int numPackets;

  protected:
    virtual void initialize() override;
    virtual Result processMessage(cMessage *msg, const SendOptions& options, simtime_t at) override;
    virtual void finish() override;
};

Define_Channel(TestChannel);

void TestChannel::initialize()
{
    EV << "Initializing TestChannel " << getFullPath() << "\n";
    numPackets = 0;
}

cChannel::Result TestChannel::processMessage(cMessage *msg, const SendOptions& options, simtime_t at)
{
    numPackets++;
    EV << "TestChannel delivering msg: " << msg->getName() << "\n";
    return cIdealChannel::processMessage(msg, options, at);
}

void TestChannel::finish()
{
    EV << "TestChannel finishing, msgs: " << numPackets << "\n";
}

}
