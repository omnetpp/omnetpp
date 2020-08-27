#include "noisylistener.h"

namespace testlib {

NoisyListener::NoisyListener()
{
    EV << "NOISYLISTENER CONSTRUCTOR\n";
}

NoisyListener::~NoisyListener()
{
    EV << "NOISYLISTENER DESTRUCTOR\n";
    unsubscribeAll();
}

void NoisyListener::printFrom(cComponent *source, simsignal_t signalID)
{
    if (dynamic_cast<cModule *>(source))
        EV << "module";
    else if (dynamic_cast<cChannel *>(source))
        EV << "channel";
    else
        EV << "destructing component";
    EV << " \"" << source->getFullPath() << "\", ";
    EV << "signal \"" << cComponent::getSignalName(signalID) << "\""; // don't print numeric signalID, as it's prone to change
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", bool " << b << "\n";
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, intval_t l, cObject *details)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", long " << l << "\n";
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, uintval_t l, cObject *details)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", long " << l << "\n";
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, double d, cObject *details)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", double " << d << "\n";
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t, cObject *details)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", simtime " << t << "\n";
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, const char *s, cObject *details)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", string " << s << "\n";
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", class " << obj->getClassName() << ", info: " << obj->str() << "\n";
}

void NoisyListener::finish(cComponent *component, simsignal_t signalID)
{
    EV << "FINISH on "; printFrom(component, signalID); EV << "\n";

    ASSERT(component->isSubscribed(signalID, this));
}

void NoisyListener::subscribedTo(cComponent *component, simsignal_t signalID)
{
    cListener::subscribedTo(component, signalID); // needed for refcounting
    EV << "SUBSCRIBED at "; printFrom(component, signalID); EV << "\n";

    ASSERT(component->isSubscribed(signalID, this));
}

void NoisyListener::unsubscribedFrom(cComponent *component, simsignal_t signalID)
{
    cListener::unsubscribedFrom(component, signalID); // needed for refcounting
    EV << "UNSUBSCRIBED from "; printFrom(component, signalID); EV << "\n";

    ASSERT(!component->isSubscribed(signalID,this));
}

}

