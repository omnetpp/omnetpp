#include <omnetpp.h>

using namespace omnetpp;

namespace testlib {

class StatNode : public cSimpleModule
{
    virtual void initialize() override {
        simsignal_t foo = registerSignal("foo");
        for (int i = 0; i < 100; i++)
            emit(foo, binomial(40, 0.5));
    }
};

Define_Module(StatNode);

} // namespace testlib