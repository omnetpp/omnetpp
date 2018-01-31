//==========================================================================
//  CRESULTLISTENER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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
    cComponent::invalidateCachedResultRecorderLists();
}

void cResultListener::unsubscribedFrom(cResultFilter *prev)
{
    cComponent::invalidateCachedResultRecorderLists();
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
    throw cRuntimeError("%s while processing statistic signal %s (id=%d) with data type %s, emitted from (%s)%s", \
                        e.what(), cComponent::getSignalName(signalID), (int)signalID, datatype, \
                        source->getClassName(), source->getFullPath().c_str())

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), b, details);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "bool", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, long l, cObject *details)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), l, details);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "long", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l, cObject *details)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), l, details);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "unsigned long", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, double d, cObject *details)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), d, details);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "double", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& v, cObject *details)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), v, details);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "simtime_t", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, const char *s, cObject *details)
{
    try {
        receiveSignal(nullptr, getSimulation()->getSimTime(), s, details);
    }
    catch (std::exception& e) {
        THROW(source, signalID, "const char *", e);
    }
}

void cResultListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details)
{
    try {
        cITimestampedValue *v = dynamic_cast<cITimestampedValue *>(obj);
        if (!v)
            receiveSignal(nullptr, getSimulation()->getSimTime(), obj, details);
        else {
            // dispatch cITimestampedValue by data type
            switch (v->getValueType(signalID)) {
                case SIMSIGNAL_BOOL: receiveSignal(nullptr, v->getTimestamp(signalID), v->boolValue(signalID), details); break;
                case SIMSIGNAL_LONG: receiveSignal(nullptr, v->getTimestamp(signalID), v->longValue(signalID), details); break;
                case SIMSIGNAL_ULONG: receiveSignal(nullptr, v->getTimestamp(signalID), v->unsignedLongValue(signalID), details); break;
                case SIMSIGNAL_DOUBLE: receiveSignal(nullptr, v->getTimestamp(signalID), v->doubleValue(signalID), details); break;
                case SIMSIGNAL_SIMTIME: receiveSignal(nullptr, v->getTimestamp(signalID), v->simtimeValue(signalID), details); break;
                case SIMSIGNAL_STRING: receiveSignal(nullptr, v->getTimestamp(signalID), v->stringValue(signalID), details); break;
                case SIMSIGNAL_OBJECT: receiveSignal(nullptr, v->getTimestamp(signalID), v->objectValue(signalID), details); break;
                default: throw opp_runtime_error("Got cITimestampedValue with blank or invalid data type");
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

