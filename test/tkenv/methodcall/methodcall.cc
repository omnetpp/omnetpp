#include <omnetpp.h>

class Target : public cSimpleModule
{
    Module_Class_Members(Target,cSimpleModule,0)
    virtual void handleMessage(cMessage *msg);
  public:  
    virtual void doWhatever(int x);
};

void Target::handleMessage(cMessage *msg)
{
    delete msg;
}

void Target::doWhatever(int x)
{
    Enter_Method("doWhatever(%d)",x);
    ev << "x = " << x << endl;
}

Define_Module(Target);

class Mod : public cSimpleModule
{
    int ctr;
    Module_Class_Members(Mod,cSimpleModule,16384)
    virtual void activity();
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

    //for(;;) {new cMessage("heyho"); wait(1);}
}

void Mod::callPrintX(const char *modname)
{
    ev << "Calling doWhatever() of module " << modname << endl;
    Target *target = dynamic_cast<Target *>(simulation.moduleByPath(modname));
    if (!target) error("target module not found");

    wait(0);

    ev << "Calling now:\n";
    target->doWhatever(ctr++);
    ev << "Back again in caller\n";

    wait(1);
}


