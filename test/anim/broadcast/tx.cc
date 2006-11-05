#include <omnetpp.h>

class Tx : public cSimpleModule
{
  public:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Tx);

void Tx::initialize()
{
    scheduleAt(exponential(1.0), new cMessage("send"));
}

void Tx::handleMessage(cMessage *msg)
{
    int n = parentModule()->size();
    for (int i=0; i<n; i++)
        if (i!=parentModule()->index())
            sendDirect(new cMessage("msg"), 2.0, parentModule()->submodule("node", i)->gate("in"));

    scheduleAt(exponential(1.0), msg);
}



