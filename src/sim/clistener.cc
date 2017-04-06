//=========================================================================
//  CLISTENER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/clistener.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/csimulation.h"

namespace omnetpp {

struct Subscription
{
    cComponent *component;
    simsignal_t signalID;
};
typedef std::vector<Subscription> SubscriptionList;

static void findListenerOccurences(cComponent *component, cIListener *listener, SubscriptionList& result)
{
    std::vector<simsignal_t> signals = component->getLocalListenedSignals();
    for (int signalID : signals) {
        if (component->isSubscribed(signalID, listener)) {
            result.push_back(Subscription());
            result.back().component = component;
            result.back().signalID = signalID;
        }
    }

    if (component->isModule()) {
        cModule *module = (cModule *)component;
        for (cModule::SubmoduleIterator it(module); !it.end(); ++it)
            findListenerOccurences(*it, listener, result);
        for (cModule::ChannelIterator it(module); !it.end(); ++it)
            findListenerOccurences(*it, listener, result);
    }
}

//---

cIListener::cIListener()
{
    subscribeCount = 0;
}

cIListener::~cIListener()
{
    if (subscribeCount) {
        // note: throwing an exception would is risky here: it would typically
        // cause other exceptions, and eventually crash
        if (subscribeCount < 0) {
            getEnvir()->printfmsg(
                    "cListener destructor: internal error: negative subscription "
                    "count (%d) in listener at address %p", subscribeCount, this);
            return;
        }

        // subscribecount > 0:
        getEnvir()->printfmsg(
                "cListener destructor: listener at address %p is still added to "
                "%d listener list(s). This will likely result in a crash: "
                "Listeners must be fully unsubscribed before deletion. "
                "Trying to determine components where this listener is subscribed...",
                this, subscribeCount);

        // print components and signals where this listener is subscribed
        std::stringstream out;
        SubscriptionList subscriptionList;
        findListenerOccurences(getSimulation()->getSystemModule(), this, subscriptionList);
        for (auto & subscription : subscriptionList) {
            out << "- signal \"" << cComponent::getSignalName(subscription.signalID) << "\" (id=" << subscription.signalID << ") ";
            out << "at (" << subscription.component->getClassName() << ")" << subscription.component->getFullPath() << "\n";
        }
        getEnvir()->printfmsg("Subscriptions for listener at address %p:\n%s", this, out.str().c_str());
    }
}

//---

void cListener::unsupportedType(simsignal_t signalID, const char *dataType)
{
    const char *signalName = cComponent::getSignalName(signalID);
    throw cRuntimeError("%s: Unsupported signal data type %s for signal %s (id=%d)",
            opp_typename(typeid(*this)), dataType, signalName, (int)signalID);
}

#ifndef WITH_OMNETPP4x_LISTENER_SUPPORT

void cListener::receiveSignal(cComponent *, simsignal_t signalID, bool, cObject *)
{
    unsupportedType(signalID, "bool");
}

void cListener::receiveSignal(cComponent *, simsignal_t signalID, long, cObject *)
{
    unsupportedType(signalID, "long");
}

void cListener::receiveSignal(cComponent *, simsignal_t signalID, unsigned long, cObject *)
{
    unsupportedType(signalID, "unsigned long");
}

void cListener::receiveSignal(cComponent *, simsignal_t signalID, double, cObject *)
{
    unsupportedType(signalID, "double");
}

void cListener::receiveSignal(cComponent *, simsignal_t signalID, const SimTime&, cObject *)
{
    unsupportedType(signalID, "simtime_t");
}

void cListener::receiveSignal(cComponent *, simsignal_t signalID, const char *, cObject *)
{
    unsupportedType(signalID, "const char *");
}

void cListener::receiveSignal(cComponent *, simsignal_t signalID, cObject *, cObject *)
{
    unsupportedType(signalID, "cObject *");
}

#else // WITH_OMNETPP4x_LISTENER_SUPPORT

// Support for OMNeT++ 4.x listeners:
#define LISTENER(TYPE) \
    void cListener::receiveSignal(cComponent *, simsignal_t signalID, TYPE) { unsupportedType(signalID, #TYPE); } \
    void cListener::receiveSignal(cComponent *source, simsignal_t signalID, TYPE d, cObject *) { receiveSignal(source, signalID, d); }
LISTENER(bool);
LISTENER(long);
LISTENER(unsigned long);
LISTENER(double);
LISTENER(const SimTime&);
LISTENER(const char *);
LISTENER(cObject *);
#undef LISTENER

#endif  // WITH_OMNETPP4x_LISTENER_SUPPORT

}  // namespace omnetpp

