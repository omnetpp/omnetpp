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
#include "ctimestampedvalue.h"


void ResultListener::subscribedTo(ResultFilter *prev)
{
    ASSERT(getSubscribeCount() == 1);  // except for multi-signal ExpressionRecorders
}

void ResultListener::unsubscribedFrom(ResultFilter *prev)
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
        receiveSignal(NULL, simulation.getSimTime(), l);
    }
    catch (std::exception& e)
    {
        THROW(source, signalID, "long", e);
    }
}

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l)
{
    try
    {
        receiveSignal(NULL, simulation.getSimTime(), l);
    }
    catch (std::exception& e)
    {
        THROW(source, signalID, "unsigned long", e);
    }
}

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    try
    {
        receiveSignal(NULL, simulation.getSimTime(), d);
    }
    catch (std::exception& e)
    {
        THROW(source, signalID, "double", e);
    }
}

void ResultListener::receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& v)
{
    try
    {
        receiveSignal(NULL, simulation.getSimTime(), v);
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
        receiveSignal(NULL, simulation.getSimTime(), s);
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
        cITimestampedValue *v = dynamic_cast<cITimestampedValue *>(obj);
        if (!v)
            receiveSignal(NULL, simulation.getSimTime(), obj);
        else
        {
            // dispatch cITimestampedValue by data type
            switch (v->getValueType(signalID))
            {
                case cITimestampedValue::LONG: receiveSignal(NULL, v->getTimestamp(signalID), v->longValue(signalID)); break;
                case cITimestampedValue::ULONG: receiveSignal(NULL, v->getTimestamp(signalID), v->unsignedLongValue(signalID)); break;
                case cITimestampedValue::DOUBLE: receiveSignal(NULL, v->getTimestamp(signalID), v->doubleValue(signalID)); break;
                case cITimestampedValue::SIMTIME: receiveSignal(NULL, v->getTimestamp(signalID), v->simtimeValue(signalID)); break;
                case cITimestampedValue::STRING: receiveSignal(NULL, v->getTimestamp(signalID), v->stringValue(signalID)); break;
                case cITimestampedValue::OBJECT: receiveSignal(NULL, v->getTimestamp(signalID), v->objectValue(signalID)); break;
                default: throw opp_runtime_error("got cITimestampedValue with blank or invalid data type");
            }
        }
    }
    catch (std::exception& e)
    {
        THROW(source, signalID, (obj ? obj->getClassName() : "cObject* (NULL)"), e);
    }
}

#undef THROW

void ResultListener::subscribedTo(cComponent *component, simsignal_t signalID)
{
    subscribedTo(NULL);
}

void ResultListener::unsubscribedFrom(cComponent *component, simsignal_t signalID)
{
    unsubscribedFrom(NULL);
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

