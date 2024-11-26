#include <omnetpp.h>

using namespace omnetpp;

namespace testlib {

class VerboseModule : public cSimpleModule
{
  public:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual ~VerboseModule() override;
};

Define_Module(VerboseModule);

void VerboseModule::initialize()
{
   EV << "initialize() at " << simTime() << endl;
   cMessage *msg = new cMessage();
   scheduleAt(15.0, msg);
}

void VerboseModule::handleMessage(cMessage *msg)
{
   EV << "handleMessage() at " << simTime() << endl;
   delete msg;
}

void VerboseModule::finish()
{
   EV << "finish() at " << simTime() << endl;
}

VerboseModule::~VerboseModule()
{
   EV << "destructor at " << simTime() << endl;
}

}



