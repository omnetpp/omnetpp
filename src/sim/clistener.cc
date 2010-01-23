//=========================================================================
//  CLISTENER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "clistener.h"
#include "ccomponent.h"
#include "cmodule.h"
#include "cchannel.h"

USING_NAMESPACE


struct Subscription {cComponent *component; simsignal_t signalID;};
typedef std::vector<Subscription> SubscriptionList;

static void findListenerOccurences(cComponent *component, cIListener *listener, SubscriptionList& result)
{
    std::vector<simsignal_t> signals = component->getLocalListenedSignals();
    for (unsigned int i = 0; i < signals.size(); i++)
    {
        simsignal_t signalID = signals[i];
        if (component->isSubscribed(signalID, listener))
        {
            result.push_back(Subscription());
            result.back().component = component;
            result.back().signalID = signalID;
        }
    }

    if (component->isModule())
    {
        cModule *module = (cModule *)component;
        for (cModule::SubmoduleIterator submod(module); !submod.end(); submod++)
            findListenerOccurences(submod(), listener, result);
        for (cModule::ChannelIterator chan(module); !chan.end(); chan++)
            findListenerOccurences(chan(), listener, result);
    }
}

//---

cListener::cListener()
{
    subscribecount = 0;
}

cListener::~cListener()
{
    ASSERT(subscribecount >= 0);
    if (subscribecount)
    {
        // note: throwing an exception would is risky here: it would typically
        // cause other exceptions, and eventually crash
        ev.printfmsg("cListener destructor: listener at address %p is still on "
                "%d listener list(s). This will likely result in a crash: "
                "Listeners must be fully unsubscribed before deletion. "
                "Trying to determine components where this listener is subscribed...",
                this, subscribecount);

        // print components and signals where this listener is subscribed
        std::stringstream out;
        SubscriptionList list;
        findListenerOccurences(simulation.getSystemModule(), this, list);
        for (int i=0; i<(int)list.size(); i++) {
            out << "- signal \"" << cComponent::getSignalName(list[i].signalID) << "\" (id=" << list[i].signalID << ") ";
            out << "at (" << list[i].component->getClassName() << ")" << list[i].component->getFullPath() << "\n";
        }
        ev.printfmsg("Subscriptions for listener at address %p:\n%s", this, out.str().c_str());
    }
}


void cListener::unsupportedType(simsignal_t signalID, const char *dataType)
{
    const char *signalName = cComponent::getSignalName(signalID);
    throw cRuntimeError("%s: Unsupported signal data type %s for signal %s (id=%d)",
                        opp_typename(typeid(*this)), dataType, signalName, (int)signalID);
}

void cListener::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    unsupportedType(signalID, "long");
}

void cListener::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    unsupportedType(signalID, "double");
}

void cListener::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)
{
    unsupportedType(signalID, "simtime_t");
}

void cListener::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    unsupportedType(signalID, "const char *");
}

void cListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    unsupportedType(signalID, "cObject *");
}

void cListener::listenerAdded(cComponent *component, simsignal_t signalID)
{
    ASSERT(subscribecount >= 0);
    subscribecount++;
}

void cListener::listenerRemoved(cComponent *component, simsignal_t signalID)
{
    ASSERT(subscribecount > 0);
    subscribecount--;
}

