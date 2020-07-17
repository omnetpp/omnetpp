#include <omnetpp.h>

using namespace omnetpp;

class Target : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg) override;

  public:
    virtual void doWhatever(int x);
};

void Target::handleMessage(cMessage *msg)
{
    delete msg;
}

void Target::doWhatever(int x)
{
    Enter_Method("doWhatever(%d)", x);
    EV << "x = " << x << endl;
}

Define_Module(Target);

class Mod : public cSimpleModule
{
  protected:
    int ctr;

  public:
    Mod() : cSimpleModule(16384) {}
    virtual void activity() override;
    void callPrintX(const char *modname);
};

Define_Module(Mod);

void Mod::activity()
{
    ctr = 0;

    // the following cases (hopefully) cover all equivalence classes for animation
    callPrintX("boxedMod.target");
    callPrintX("boxedMod.boxedTarget.target");

    callPrintX("target");
    callPrintX("boxedTarget.target");

    // for(;;) {new cMessage("heyho"); wait(1);}
}

void Mod::callPrintX(const char *modname)
{
    EV << "Calling doWhatever() of module " << modname << endl;
    Target *target = dynamic_cast<Target *>(getSimulation()->getModuleByPath(modname));

    wait(0);

    EV << "Calling now:\n";
    target->doWhatever(ctr++);
    EV << "Back again in caller\n";

    wait(1);
}

