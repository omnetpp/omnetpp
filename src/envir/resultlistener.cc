//==========================================================================
//  RESULTLISTENER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "resultlistener.h"
#include "resultfilters.h"
#include "ccomponent.h"


void ResultListener::listenerAdded(ResultFilter *prev)
{
    ASSERT(getSubscribeCount() == 1);  // except for multi-signal ExpressionRecorders
}

void ResultListener::listenerRemoved(ResultFilter *prev)
{
    if (getSubscribeCount() == 0)
        delete this;
}

// original cIListener API that delegates to the simplified API:

#define THROW(source, signalID, datatype, e) \
    throw cRuntimeError("Error while processing statistic signal %s (id=%d) from (%s)%s, data type %s: %s", \
                        cComponent::getSignalName(signalID), (int)signalID, \
                        source->getClassName(), source->getFullPath().c_str(), datatype, e.what())

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    try
    {
        receiveSignal(NULL, l);
    }
    catch (std::exception& e)
    {
        THROW(source, signalID, "long", e);
    }
}

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    try
    {
        receiveSignal(NULL, d);
    }
    catch (std::exception& e)
    {
        THROW(source, signalID, "double", e);
    }
}

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)
{
    try
    {
        receiveSignal(NULL, t);
    }
    catch (std::exception& e)
    {
        THROW(source, signalID, "simtime_t", e);
    }
}

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    try
    {
        receiveSignal(NULL, s);
    }
    catch (std::exception& e)
    {
        THROW(source, signalID, "const char *", e);
    }
}

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    try
    {
        // recognize cISignalValue, and dispatch to (simtime_t, double) handler
        cISignalValue *v = dynamic_cast<cISignalValue *>(obj);
        if (v)
            receiveSignal(NULL, v->getSignalTime(signalID), v->getSignalValue(signalID));
        else
            receiveSignal(NULL, obj);
    }
    catch (std::exception& e)
    {
        THROW(source, signalID, (obj ? obj->getClassName() : "cObject* (NULL)"), e);
    }
}

#undef THROW

void ResultListener::listenerAdded(cComponent *component, simsignal_t signalID)
{
    listenerAdded(NULL);
}

void ResultListener::listenerRemoved(cComponent *component, simsignal_t signalID)
{
    listenerRemoved(NULL);
}

void ResultListener::finish(cComponent *component, simsignal_t signalID)
{
    finish(NULL);
}

//---

void SignalSource::subscribe(ResultListener *listener) const
{
    if (filter)
        filter->addDelegate(listener);
    else if (component && signalID!=SIMSIGNAL_NULL)
        component->subscribe(signalID, listener);
    else
        throw opp_runtime_error("subscribe() called on blank SignalSource");
}

