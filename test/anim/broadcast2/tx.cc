#include <omnetpp.h>

using namespace omnetpp;

class Tx : public cSimpleModule
{
  public:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Tx);

void Tx::initialize()
{
    scheduleAt(exponential(1.0), new cMessage("send"));
}

void Tx::handleMessage(cMessage *msg)
{
    int n = gateSize("out");
    for (int i = 0; i < n; i++)
        send(new cMessage("msg"), "out", i);

    scheduleAt(simTime()+exponential(1.0), msg);
}

