//==========================================================================
//  CRESULTLISTENER.CC - part of
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

#include "cresultlistener.h"
#include "ccomponent.h"
#include "ctimestampedvalue.h"
#include "exception.h"
#include "stringpool.h"

NAMESPACE_BEGIN


void cResultListener::subscribedTo(cResultFilter *prev)
{
    ASSERT(getSubscribeCount() == 1);  // except for multi-signal ExpressionRecorders
}

void cResultListener::unsubscribedFrom(cResultFilter *prev)
{
    if (getSubscribeCount() == 0)
        delete this;
}

const char *cResultListener::getPooled(const char *s)
{
    static CommonStringPool namesPool;
    return namesPool.get(s);
}

// original cIListener API that delegates to the simplified API:

#define THROW(source, signalID, datatype, e) \
    throw cRuntimeError("Error while processing statistic signal %s (id=%d) from (%s)%s, data type %s: %s", \
                        cComponent::getSignalName(signalID), (int)signalID, \
                        source->getClassName(), source->getFullPath().c_str(), datatype, e.what())

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, bool b)
{
    try
    {
        receiveSignal(NULL, simulation.getSimTime(), b);
    }
    catch (std::exception& e)
    {
        THROW(source, signalID, "bool", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, long l)
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

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l)
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

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, double d)
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

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& v)
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

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
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

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
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
                case SIMSIGNAL_BOOL: receiveSignal(NULL, v->getTimestamp(signalID), v->boolValue(signalID)); break;
                case SIMSIGNAL_LONG: receiveSignal(NULL, v->getTimestamp(signalID), v->longValue(signalID)); break;
                case SIMSIGNAL_ULONG: receiveSignal(NULL, v->getTimestamp(signalID), v->unsignedLongValue(signalID)); break;
                case SIMSIGNAL_DOUBLE: receiveSignal(NULL, v->getTimestamp(signalID), v->doubleValue(signalID)); break;
                case SIMSIGNAL_SIMTIME: receiveSignal(NULL, v->getTimestamp(signalID), v->simtimeValue(signalID)); break;
                case SIMSIGNAL_STRING: receiveSignal(NULL, v->getTimestamp(signalID), v->stringValue(signalID)); break;
                case SIMSIGNAL_OBJECT: receiveSignal(NULL, v->getTimestamp(signalID), v->objectValue(signalID)); break;
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

void cResultListener::subscribedTo(cComponent *component, simsignal_t signalID)
{
    subscribedTo(NULL);
}

void cResultListener::unsubscribedFrom(cComponent *component, simsignal_t signalID)
{
    unsubscribedFrom(NULL);
}

void cResultListener::finish(cComponent *component, simsignal_t signalID)
{
    finish(NULL);
}

NAMESPACE_END

