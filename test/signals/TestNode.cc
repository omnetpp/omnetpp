#include <omnetpp.h>

class TestNode : public cSimpleModule
{
  public:
    TestNode() : cSimpleModule(16384) {}
    virtual void activity();
};

Define_Module(TestNode);


void TestNode::activity()
{
    int beepSignal = registerSignal("beep");
    int crunchSignal = registerSignal("crunch");
    while (true)
    {
        ev << "beep...\n";
        emit(beepSignal, 1L);
        wait(exponential(1.0));

        ev << "crunch...\n";
        emit(crunchSignal, "grr");
        wait(exponential(1.0));
    }
}

