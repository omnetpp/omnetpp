#include <omnetpp.h>

using namespace omnetpp;

namespace testlib {

class DeletionTesterBase : public cSimpleModule
{
  public:
    DeletionTesterBase(size_t stack=0) : cSimpleModule(stack) {}
    virtual ~DeletionTesterBase();
    virtual void deleteTargetModule();
};

Define_Module(DeletionTesterBase);

DeletionTesterBase::~DeletionTesterBase()
{
    EV << " *** " << getFullPath() << " deleted\n";
}

void DeletionTesterBase::deleteTargetModule()
{
    const char *targetPath = par("moduleToDelete").stringValue();
    if (strlen(targetPath) > 0) {
        cModule *target = getModuleByPath(targetPath);
        EV << "t=" << simTime() << "s: " << getFullPath() << ": deleting " << target->getFullPath() << endl;
        int targetModuleId = target->getId();
	cSimulation *simulation = getSimulation(); // save it in case we delete ourselves
        target->deleteModule();
        ASSERT(simulation->getModule(targetModuleId) == nullptr); // actually deleted
    }
}

//---

class DeletionTesterHandleMessage : public DeletionTesterBase
{
    cMessage *deleteTimer;
  public:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(DeletionTesterHandleMessage);

void DeletionTesterHandleMessage::initialize()
{
    // this will trigger a deleteTargetModule()
    scheduleAt(1, deleteTimer=new cMessage());

    // additional events, to test what happens with orphaned self messages
    scheduleAt(5, new cMessage());
    scheduleAt(6, new cMessage());
}

void DeletionTesterHandleMessage::handleMessage(cMessage *msg)
{
    if (msg == deleteTimer) {
        delete msg;
        deleteTargetModule();
    }
    else {
        delete msg;
    }
}

//---

struct Noisy {
    std::string name;
    Noisy(std::string name) : name(name) {}
    ~Noisy() {EV << " *** Noisy object " << name << " deleted\n"; }
};

class DeletionTesterActivity : public DeletionTesterBase
{
  public:
    DeletionTesterActivity() : DeletionTesterBase(32768) {}
    virtual void activity() override;
};

Define_Module(DeletionTesterActivity);

void DeletionTesterActivity::activity()
{
    Noisy noisy(getFullPath()+".noisy"); // to verify that stack cleanup takes place
    wait(1);
    scheduleAt(5, new cMessage()); // to test what happens with orphaned self messages
    deleteTargetModule();
    cQueue queue;
    waitAndEnqueue(1000, &queue);
}

}
