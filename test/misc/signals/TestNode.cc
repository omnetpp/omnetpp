#include <omnetpp.h>

using namespace omnetpp;

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
    while (true) {
        EV << "beep...\n";
        emit(beepSignal, 1L);
        wait(exponential(1.0));

        EV << "crunch...\n";
        emit(crunchSignal, "grr");
        wait(exponential(1.0));
    }
}

