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
    int n = getParentModule()->size();
    for (int i=0; i<n; i++)
        if (i!=getParentModule()->getIndex())
            sendDirect(new cMessage("msg"), 0.5, simulation.getSystemModule()->getSubmodule("node", i)->gate("in"));

    scheduleAt(simTime()+exponential(1.0), msg);
}



