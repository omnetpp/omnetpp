#include <omnetpp.h>

#include "problematic.h"

using namespace omnetpp;

class InspectorSafety : public cSimpleModule
{
  public:
    InspectorSafety() {}
    virtual void initialize() override { new Problematic(); };
    virtual void finish() override { };
    virtual void handleMessage(cMessage *msg) override {};
};

Define_Module(InspectorSafety);

