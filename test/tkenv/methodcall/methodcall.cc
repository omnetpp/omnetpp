#include <omnetpp.h>

class Sink : public cSimpleModule
{
    Module_Class_Members(Sink,cSimpleModule,0)
    virtual void handleMessage(cMessage *msg);
  public:  
    virtual void printX(int x);
};

void Sink::handleMessage(cMessage *msg)
{
    delete msg;
}

void Sink::printX(int x)
{
    Enter_Method("printX(%d)",x);
    ev << "x = " << x << endl;
}

Define_Module(Sink);

class Gen : public cSimpleModule
{
    int ctr;
    Module_Class_Members(Gen,cSimpleModule,16384)
    virtual void activity();
    void callPrintX(const char *modname);
};

Define_Module(Gen);

void Gen::activity()
{
    ctr = 0;

    // the following cases (hopefully) cover all equivalence classes for animation
    callPrintX("boxedGen.sink");
    callPrintX("boxedGen.boxedSink.sink");

    callPrintX("sink");
    callPrintX("boxedSink.sink");

    for(;;) {new cMessage("heyho"); wait(1);}
}

void Gen::callPrintX(const char *modname)
{
    ev << "Calling printX() of module " << modname << endl;
    Sink *target = dynamic_cast<Sink *>(simulation.moduleByPath(modname));
    if (!target) error("target module not found");

    wait(0);

    ev << "Calling now:\n";
    target->printX(ctr++);
    ev << "Back again in caller\n";

    wait(1);
}


