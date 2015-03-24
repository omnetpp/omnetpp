#include <omnetpp.h>

USING_NAMESPACE

namespace testlib {

class Sink : public cSimpleModule
{
  protected:
    int numPackets;
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
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

