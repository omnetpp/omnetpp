#include <omnetpp.h>

using namespace omnetpp;

class Node : public cSimpleModule
{
  public:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Node);

void Node::initialize()
{
    scheduleAt(0, new cMessage("timer"));
}

void Node::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        EV << "Sending...\n";
        scheduleAt(simTime()+0.1, msg);
        send(new cPacket("pkt"), "g$o");
    }
    else {
        EV << "Received " << msg->getName() << "\n";
        delete msg;
    }
}

