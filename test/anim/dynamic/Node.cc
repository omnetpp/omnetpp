#include <omnetpp.h>

using namespace omnetpp;

class Node : public cSimpleModule
{
  public:
    Node() : cSimpleModule(16384) {}
    virtual void activity() override;
};

Define_Module(Node);

void Node::activity()
{
    cQueue q;
    while (true) {
        EV << "sending messages on all gates...\n";
        for (GateIterator it(this); !it.end(); ++it) {
            cGate *g = *it;
            if (g->getType() == cGate::OUTPUT && g->isConnected() && g->getPathEndGate()->getOwnerModule()->isSimple())
                send(new cMessage("msg"), g);
        }
        waitAndEnqueue(0.3, &q);
    }
}

