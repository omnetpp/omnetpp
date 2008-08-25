#include <omnetpp.h>

class Controller : public cSimpleModule
{
  private:
    std::vector<cModule *> modules;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Controller);


void Controller::initialize()
{
    scheduleAt(1, new cMessage());
}

void Controller::handleMessage(cMessage *msg)
{
    // do something
    double r01 = dblrand();
    if (r01 < 0.5) {
        // create a node
        cModuleType *type = cModuleType::find("Node");
        cModule *mod = type->createScheduleInit("node", getParentModule());
        modules.push_back(mod);
    }
    else if (r01 < 0.9) {
        // delete a node
        int k = intrand(modules.size());
        modules[k]->deleteModule();
        modules.erase(modules.begin()+k);
    }

    // schedule next action
    scheduleAt(simTime()+1, msg);
}

