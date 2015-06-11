#ifndef NOISYLISTENER_H
#define NOISYLISTENER_H

#include <omnetpp.h>

USING_NAMESPACE

namespace testlib {

class NoisyListener : public cListener
{
  protected:
    void printFrom(cComponent *source, simsignal_t signalID);

  public:
    NoisyListener();
    virtual ~NoisyListener();
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, unsigned long t) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) override;
    virtual void finish(cComponent *component, simsignal_t signalID) override;
    virtual void subscribedTo(cComponent *component, simsignal_t signalID) override;
    virtual void unsubscribedFrom(cComponent *component, simsignal_t signalID) override;
};

}

#endif
