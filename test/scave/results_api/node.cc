#include <omnetpp.h>

using namespace omnetpp;

class Node : public cSimpleModule
{
    virtual void initialize() override {
        int bar = par("bar");

        simsignal_t foo1 = registerSignal("foo1");
        simsignal_t foo2 = registerSignal("foo2");

        for (int i = 0; i < 100; i++) {
            cTimestampedValue tmp1((simtime_t)i, (double)binomial(40, 0.5) * bar);
            emit(foo1, &tmp1);

            cTimestampedValue tmp2((simtime_t)i, (double)exponential(10) * bar);
            emit(foo2, &tmp2);
        }

        simsignal_t zero = registerSignal("zero");
        emit(zero, 0.0);

    }
};

Define_Module(Node);
