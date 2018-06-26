#include <omnetpp.h>

using namespace omnetpp;

class Controller : public cSimpleModule
{
  public:
    Controller() : cSimpleModule(16384) {}
    virtual void activity() override;
};

Define_Module(Controller);

void Controller::activity()
{
    // create two modules, and connect them
    cModuleType *type = cModuleType::find("Node");
    cModule *node1 = type->createScheduleInit("node1", getParentModule());
    wait(1);

    cModule *node2 = type->createScheduleInit("node2", getParentModule());
    wait(1);

    node1->addGate("o1", cGate::OUTPUT);
    node2->addGate("i1", cGate::INPUT);
    node1->gate("o1")->connectTo(node2->gate("i1"));
    wait(1);

    // remove connection
    node1->gate("o1")->disconnect();
    wait(1);

    // connect to parent module then remove connection
    node1->addGate("o2", cGate::OUTPUT);
    getParentModule()->addGate("o2", cGate::OUTPUT);
    node1->gate("o2")->connectTo(getParentModule()->gate("o2"));
    wait(1);

    node1->gate("o2")->disconnect();
    wait(1);

    // create a module inside a compound module, then create a connection
    // that spans hierarchy levels (illegal, but Qtenv should behave nicely)
    cModuleType *compoundType = cModuleType::find("Compound");
    cModule *compound = compoundType->createScheduleInit("compound1", getParentModule());
    getDisplayString().setTagArg("t", 0, "OPEN THE NEW COMPOUND MODULE BEFORE PROCEEDING!");
    bubble("OPEN THE NEW COMPOUND MODULE BEFORE PROCEEDING!");
    wait(1);

    cModule *node3 = type->createScheduleInit("node3", compound);
    node3->addGate("o3", cGate::OUTPUT);
    getParentModule()->addGate("o3", cGate::OUTPUT);
    node3->gate("o3")->connectTo(getParentModule()->gate("o3"));
    wait(1);
}

