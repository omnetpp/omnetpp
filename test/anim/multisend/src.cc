#include <omnetpp.h>

using namespace omnetpp;

class Src : public cSimpleModule
{
  public:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual ~Src() { }
};

Define_Module(Src);

void Src::initialize()
{
    scheduleAt(exponential(1.0), new cMessage("send"));
}

void Src::handleMessage(cMessage *msg)
{
    send(new cMessage("msg1"), "out1");
    send(new cMessage("msg2"), "out2");
    send(new cMessage("msg3"), "out1");

    scheduleAt(simTime()+exponential(1.0) + 0.01, msg);
}

