#include <omnetpp.h>

using namespace omnetpp;

class Mod : public cSimpleModule
{
  public:
    Mod() : cSimpleModule(16384) {}
    virtual void activity() override;
};

Define_Module(Mod);

void Mod::activity()
{
    EV << "starting up\n";

    wait(1);

    EV << "before changeParentTo()\n";

    cModule *box2 = getParentModule()->getParentModule()->getSubmodule("box2");
    changeParentTo(box2);

    EV << "after changeParentTo()\n";

    wait(1);

    EV << "the end\n";
}

