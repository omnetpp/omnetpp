#include <omnetpp.h>

using namespace omnetpp;

class Controller : public cSimpleModule
{
  private:
    int k;

  public:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Controller);

void Controller::initialize()
{
    k = 0;
    scheduleAt(0, new cMessage());
}

void Controller::handleMessage(cMessage *msg)
{
    // delete some modules
    double p = par("deleteProbability");
    std::vector<cModule *> toDelete; // to avoid modification during iteration
    for (cModule::SubmoduleIterator it(getSystemModule()); !it.end();  /**/) {
        cModule *mod = *it++;
        if (strncmp(mod->getName(), "node-", 5) == 0 && dblrand() < p)
            toDelete.push_back(mod);
    }
    for (auto mod : toDelete)
        mod->deleteModule();

    // and create new ones
    int n = par("numToCreate");
    cModuleType *type = cModuleType::find("Node");
    for (int i = 0; i < n; i++) {
        char name[20];
        snprintf(name, sizeof(name), "node-%d", ++k);
        type->createScheduleInit(name, getSystemModule());
    }
    bubble("created a bunch of modules");

    scheduleAt(simTime()+par("waitTime").doubleValue(), msg);
}

