#ifndef NOISYLISTENER_H
#define NOISYLISTENER_H

#include <omnetpp.h>

namespace testlib {

class NoisyListener : public cListener
{
  protected:
    void printFrom(cComponent *source, simsignal_t signalID);
  public:
    NoisyListener();
    virtual ~NoisyListener();
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
    virtual void finish(cComponent *component, simsignal_t signalID);
    virtual void subscribedTo(cComponent *component, simsignal_t signalID);
    virtual void unsubscribedFrom(cComponent *component, simsignal_t signalID);
};

}

#endif
