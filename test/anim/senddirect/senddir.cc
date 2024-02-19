#include <omnetpp.h>

using namespace omnetpp;

class Sink : public cSimpleModule
{
    virtual void handleMessage(cMessage *msg) override;
};

void Sink::handleMessage(cMessage *msg)
{
    delete msg;
}

Define_Module(Sink);

class Gen : public cSimpleModule
{
  private:
    int ctr;

  public:
    Gen() : cSimpleModule(16384) {}
    virtual void activity() override;
    void sendTo(const char *modname, const char *gatename);
};

Define_Module(Gen);

void Gen::activity()
{
    ctr = 0;

    // the following cases (hopefully) cover all equivalence classes for animation

    sendTo("boxedGen", "out1");
    sendTo("boxedGen", "out2");
    sendTo("dummy", "out");

    sendTo("boxedGen.sink", "directIn");
    sendTo("boxedGen.boxedSink", "directIn");
    sendTo("boxedGen.boxedSink.sink", "directIn");

    sendTo("sink", "directIn");
    sendTo("boxedSink", "directIn");
    sendTo("boxedSink.sink", "directIn");
}

void Gen::sendTo(const char *modname, const char *gatename)
{
    char msgname[32];
    snprintf(msgname, sizeof(msgname), "job-%d", ctr++);
    cMessage *msg = new cMessage(msgname);

    cModule *target = getSimulation()->getModuleByPath(modname);
    EV << "Sending " << msgname <<  " to " << modname << "." << gatename << endl;
    wait(0);
    sendDirect(msg, target->gate(gatename));
    wait(1);
}

