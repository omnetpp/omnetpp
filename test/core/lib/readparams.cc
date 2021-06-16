#include <omnetpp.h>

using namespace omnetpp;

namespace testlib {

class ReadParams : public cSimpleModule
{
  protected:
    virtual void initialize() override {
        for (int i = 0; i < getNumParams(); i++) {
            cPar& p = par(i);
            EV << p.getFullName();
            switch (p.getType()) {
                case cPar::BOOL: EV << p.boolValue(); break;
                case cPar::INT: EV << p.intValue(); break;
                case cPar::DOUBLE: EV << p.doubleValue(); break;
                case cPar::STRING: EV << p.stdstringValue(); break;
                default: EV << "type not supported in this test";
            }
        }
    }
};

Define_Module(ReadParams);

}

