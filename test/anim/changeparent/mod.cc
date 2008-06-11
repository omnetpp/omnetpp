#include <omnetpp.h>

class Mod : public cSimpleModule
{
  public:
    Mod() : cSimpleModule(16384) {}
    virtual void activity();
};

Define_Module(Mod);

void Mod::activity()
{
    ev << "starting up\n";

    wait(1);

    ev << "before changeParentTo()\n";

    cModule *box2 = getParentModule()->getParentModule()->getSubmodule("box2");
    changeParentTo(box2);

    ev << "after changeParentTo()\n";

    wait(1);

    ev << "the end\n";
}


