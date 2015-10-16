//==========================================================================
//  CRESULTLISTENER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/exception.h"
#include "common/stringpool.h"
#include "omnetpp/cresultlistener.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/ctimestampedvalue.h"

using namespace omnetpp::common;

namespace omnetpp {

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
    static StringPool namesPool;
    return namesPool.get(s);
}

// original cIListener API that delegates to the simplified API:

#define THROW(source, signalID, datatype, e) \
    throw cRuntimeError("Error while processing statistic signal %s (id=%d) from (%s)%s, data type %s: %s", \
                        cComponent::getSignalName(signalID), (int)signalID, \
                        source->getClassName(), source->getFullPath().c_str(), datatype, e.what())

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, bool b)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), b);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "bool", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), l);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "long", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), l);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "unsigned long", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), d);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "double", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& v)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), v);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "simtime_t", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), s);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "const char *", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    try {
        cITimestampedValue *v = dynamic_cast<cITimestampedValue *>(obj);
        if (!v)
            receiveSignal(nullptr, getSimulation()->getSimTime(), obj);
        else {
            // dispatch cITimestampedValue by data type
            switch (v->getValueType(signalID)) {
                case SIMSIGNAL_BOOL: receiveSignal(nullptr, v->getTimestamp(signalID), v->boolValue(signalID)); break;
                case SIMSIGNAL_LONG: receiveSignal(nullptr, v->getTimestamp(signalID), v->longValue(signalID)); break;
                case SIMSIGNAL_ULONG: receiveSignal(nullptr, v->getTimestamp(signalID), v->unsignedLongValue(signalID)); break;
                case SIMSIGNAL_DOUBLE: receiveSignal(nullptr, v->getTimestamp(signalID), v->doubleValue(signalID)); break;
                case SIMSIGNAL_SIMTIME: receiveSignal(nullptr, v->getTimestamp(signalID), v->simtimeValue(signalID)); break;
                case SIMSIGNAL_STRING: receiveSignal(nullptr, v->getTimestamp(signalID), v->stringValue(signalID)); break;
                case SIMSIGNAL_OBJECT: receiveSignal(nullptr, v->getTimestamp(signalID), v->objectValue(signalID)); break;
                default: throw opp_runtime_error("got cITimestampedValue with blank or invalid data type");
            }
        }
    }
    catch (std::exception& e) {
        THROW(source, signalID, (obj ? obj->getClassName() : "cObject* (nullptr)"), e);
    }
}

#undef THROW

void cResultListener::subscribedTo(cComponent *component, simsignal_t signalID)
{
    subscribedTo(nullptr);
}

void cResultListener::unsubscribedFrom(cComponent *component, simsignal_t signalID)
{
    unsubscribedFrom(nullptr);
}

void cResultListener::finish(cComponent *component, simsignal_t signalID)
{
    callFinish(nullptr);
}

}  // namespace omnetpp

