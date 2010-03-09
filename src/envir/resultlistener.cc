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

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    receiveSignal(NULL, l);
}

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    receiveSignal(NULL, d);
}

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)
{
    receiveSignal(NULL, t);
}

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    receiveSignal(NULL, s);
}

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    // recognize cISignalValue, and dispatch to (simtime_t, double) handler
    cISignalValue *v = dynamic_cast<cISignalValue *>(obj);
    if (v)
        receiveSignal(NULL, v->getSignalTime(signalID), v->getSignalValue(signalID));
    receiveSignal(NULL, obj);
}

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

