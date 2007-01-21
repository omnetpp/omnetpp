#include <omnetpp.h>

class Dump : public cSimpleModule
{
  public:
    virtual void initialize();
};

Define_Module(Dump);

void Dump::initialize()
{
    printf("module %s\n", fullPath().c_str());
}

