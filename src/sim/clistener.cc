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

cIListener::~cIListener()
{
    unsubscribeAll();
}

void cIListener::unsubscribeAll()
{
    auto copy = subscriptions;
    for (auto subscription: copy)
        subscription.first->unsubscribe(subscription.second, this); // affects the subscriptions list, hence the copy above!
    subscriptions.clear();
}

//---

void cListener::unsupportedType(simsignal_t signalID, const char *dataType)
{
    const char *signalName = cComponent::getSignalName(signalID);
    throw cRuntimeError("%s: Unsupported signal data type %s for signal %s (id=%d)",
            opp_typename(typeid(*this)), dataType, signalName, (int)signalID);
}

void cListener::receiveSignal(cComponent *, simsignal_t signalID, bool, cObject *)
{
    unsupportedType(signalID, "bool");
}

void cListener::receiveSignal(cComponent *c, simsignal_t signalID, intval_t i, cObject *d)
{
#ifdef _WIN32
    const char *msg = "Integer signal value does not fit into a long while trying to use deprecated receiveSignal(long) overload";
    receiveSignal(c, signalID, checked_int_cast<long>(i,nullptr,msg), d);
#else
    unsupportedType(signalID, "intval_t");
#endif
}

void cListener::receiveSignal(cComponent *c, simsignal_t signalID, uintval_t i, cObject *d)
{
#ifdef _WIN32
    const char *msg = "Integer signal value does not fit into an unsigned long while trying to use deprecated receiveSignal(unsigned long) overload";
    receiveSignal(c, signalID, checked_int_cast<unsigned long>(i,nullptr,msg), d);
#else
    unsupportedType(signalID, "uintval_t");
#endif
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

#ifdef _WIN32
void cListener::receiveSignal(cComponent *, simsignal_t signalID, long, cObject *)
{
    unsupportedType(signalID, "long");
}

void cListener::receiveSignal(cComponent *, simsignal_t signalID, unsigned long, cObject *)
{
    unsupportedType(signalID, "unsigned long");
}
#endif


}  // namespace omnetpp

