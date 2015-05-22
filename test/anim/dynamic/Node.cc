#include <omnetpp.h>

USING_NAMESPACE

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
    while (true)
    {
        EV << "sending messages on all gates...\n";
        for (GateIterator i(this); !i.end(); i++)
        {
            cGate *g = i();
            if (g->getType() == cGate::OUTPUT && g->isConnected() && g->getPathEndGate()->getOwnerModule()->isSimple())
                send(new cMessage("msg"), g);
        }
        waitAndEnqueue(0.3, &q);
    }
}

