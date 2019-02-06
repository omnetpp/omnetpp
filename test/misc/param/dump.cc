#include <omnetpp.h>

using namespace omnetpp;

class Dump : public cSimpleModule
{
  protected:
    bool printClassNames;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void dump(cModule *mod);
};

Define_Module(Dump);

void Dump::initialize()
{
    scheduleAt(1, new cMessage());
}

void Dump::handleMessage(cMessage *msg)
{
    delete msg;

    printf("==============================\n");
    dump(getSimulation()->getSystemModule());
    printf("==============================\n");
}

void Dump::dump(cModule *mod)
{
    if (mod == this)
        return;

    for (int i = 0; i < mod->getNumParams(); i++)
        printf("%s = %s\n", mod->par(i).getFullPath().c_str(), mod->par(i).str().c_str());

    for (cModule::SubmoduleIterator submod(mod); !submod.end(); submod++)
        dump(submod());
}

