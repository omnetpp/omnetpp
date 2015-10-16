#include <omnetpp.h>

using namespace omnetpp;

namespace testlib {

class Sink : public cSimpleModule
{
  protected:
    int numPackets;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

Define_Module(Sink);

void Sink::initialize()
{
    EV << "Initializing Sink " << getFullPath() << "\n";
    numPackets = 0;
}

void Sink::handleMessage(cMessage *msg)
{
    EV << "Sink received msg " << msg->getName() << "\n";
    numPackets++;
    delete msg;
}

void Sink::finish()
{
    EV << "Sink finishing, received msgs: " << numPackets << "\n";
}

}

