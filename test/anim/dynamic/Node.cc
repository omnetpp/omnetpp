#include <omnetpp.h>

class Node : public cSimpleModule
{
  public:
    Node() : cSimpleModule(16384) {}
    virtual void activity();
};

Define_Module(Node);


void Node::activity()
{
    cQueue q;
    while (true)
    {
        ev << "sending messages on all gates...\n";
        for (GateIterator i(this); !i.end(); i++)
        {
            cGate *g = i();
            if (g->getType() == cGate::OUTPUT && g->isConnected() && g->getPathEndGate()->getOwnerModule()->isSimple())
                send(new cMessage("msg"), g);
        }
        waitAndEnqueue(0.3, &q);
    }
}

