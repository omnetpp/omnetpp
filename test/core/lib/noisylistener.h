#ifndef NOISYLISTENER_H
#define NOISYLISTENER_H

#include <omnetpp.h>

using namespace omnetpp;

namespace testlib {

class NoisyListener : public cListener
{
  protected:
    void printFrom(cComponent *source, simsignal_t signalID);

  public:
    NoisyListener();
    virtual ~NoisyListener();
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, intval_t l, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, uintval_t l, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;
    virtual void finish(cComponent *component, simsignal_t signalID) override;
    virtual void subscribedTo(cComponent *component, simsignal_t signalID) override;
    virtual void unsubscribedFrom(cComponent *component, simsignal_t signalID) override;
};

}

#endif
