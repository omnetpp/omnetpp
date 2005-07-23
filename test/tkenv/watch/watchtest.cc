#include <omnetpp.h>

class WatchTest : public cSimpleModule
{
    Module_Class_Members(WatchTest,cSimpleModule,16384)
    virtual void activity();
};

Define_Module(WatchTest);

void WatchTest::activity()
{

    for(;;) {new cMessage("heyho"); wait(1);}
}

