#include <omnetpp.h>

using namespace omnetpp;

namespace testlib {

class ThrowError : public cSimpleModule
{
  protected:
    virtual void initialize() override {
        if (par("throwError"))
            throw cRuntimeError("This is an intentionally bogus run");
    }
};

Define_Module(ThrowError);

}

