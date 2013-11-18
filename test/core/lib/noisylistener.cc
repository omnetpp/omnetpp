#include "noisylistener.h"

namespace testlib {

NoisyListener::NoisyListener()
{
    EV << "NOISYLISTENER CONSTRUCTOR\n";
}

NoisyListener::~NoisyListener()
{
    EV << "NOISYLISTENER DESTRUCTOR\n";
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
    EV << "signal \"" << cComponent::getSignalName(signalID) << "\" (id=" << signalID << ")";
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, bool b)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", bool " << b << "\n";
    simulation.getSystemModule()->checkSignalConsistency();
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", long " << l << "\n";
    simulation.getSystemModule()->checkSignalConsistency();
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", double " << d << "\n";
    simulation.getSystemModule()->checkSignalConsistency();
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", simtime " << t << "\n";
    simulation.getSystemModule()->checkSignalConsistency();
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", string " << s << "\n";
    simulation.getSystemModule()->checkSignalConsistency();
}

void NoisyListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    EV << "SIGNAL from "; printFrom(source, signalID);
    EV << ", class " << obj->getClassName() << ", info: " << obj->info() << "\n";
    simulation.getSystemModule()->checkSignalConsistency();
}

void NoisyListener::finish(cComponent *component, simsignal_t signalID)
{
    EV << "FINISH on "; printFrom(component, signalID); EV << "\n";

    simulation.getSystemModule()->checkSignalConsistency();
    ASSERT(component->isSubscribed(signalID,this));
}

void NoisyListener::subscribedTo(cComponent *component, simsignal_t signalID)
{
    cListener::subscribedTo(component, signalID); // needed for refcounting
    EV << "SUBSCRIBED at "; printFrom(component, signalID); EV << "\n";

    simulation.getSystemModule()->checkSignalConsistency();
    ASSERT(component->isSubscribed(signalID,this));
}

void NoisyListener::unsubscribedFrom(cComponent *component, simsignal_t signalID)
{
    cListener::unsubscribedFrom(component, signalID); // needed for refcounting
    EV << "UNSUBSCRIBED from "; printFrom(component, signalID); EV << "\n";

    simulation.getSystemModule()->checkSignalConsistency();
    ASSERT(!component->isSubscribed(signalID,this));
}

}

