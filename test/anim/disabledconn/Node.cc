#include <omnetpp.h>

class Node : public cSimpleModule
{
  public:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Node);

void Node::initialize()
{
    scheduleAt(0, new cMessage("timer"));
}

void Node::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        ev << "Sending...\n";
        scheduleAt(simTime()+0.1, msg);
        send(new cPacket("pkt"), "g$o");
    }
    else {
        ev << "Received " << msg->getName() << "\n";
        delete msg;
    }
}

