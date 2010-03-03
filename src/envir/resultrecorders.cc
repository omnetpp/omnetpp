//==========================================================================
//  RESULTRECORDERS.CC - part of
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

#include "cproperties.h"
#include "cproperty.h"
#include "resultrecorders.h"


void ResultRecorder::listenerAdded(cComponent *component, simsignal_t signalID)
{
    ASSERT(getSubscribeCount() == 1);  // may only be subscribed once (otherwise results get mixed)
}

void ResultRecorder::listenerRemoved(cComponent *component, simsignal_t signalID)
{
    if (getSubscribeCount() == 0)
        delete this;
}

std::string ResultRecorder::makeName(simsignal_t signalID, const char *opname)
{
    const char *signalName = cComponent::getSignalName(signalID);
    if (!signalName)
        signalName = "<unnamed>";
    return std::string(signalName) + ":" + opname;
}

void ResultRecorder::extractSignalAttributes(cComponent *component, simsignal_t signalID, opp_string_map& result)
{
    const char *signalName = cComponent::getSignalName(signalID);
    if (signalName)
        extractSignalAttributes(component, signalName, result);
}

void ResultRecorder::extractSignalAttributes(cComponent *component, const char *signalName, opp_string_map& result)
{
    cProperty *property = component->getProperties()->get("statistic", signalName);
    if (!property)
        return;

    // fill result[] from the properties
    const std::vector<const char *>& keys = property->getKeys();
    for (int i = 0; i < (int)keys.size(); i++)
    {
        const char *key = keys[i];
        int numValues = property->getNumValues(key);
        if (numValues == 0)
            result[key] = "";
        else if (numValues == 1)
            result[key] = property->getValue(key, 0);
        else {
            std::string buf;
            for (int j = 0; j < numValues; j++) {
                if (j > 0) buf += ",";
                buf += property->getValue(key, j);
            }
            result[key] = buf;
        }
    }
}

//---

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    maybeCollect(l);
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    maybeCollect(d);
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t d)
{
    maybeCollect(SIMTIME_DBL(d));
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    maybeCollect(s!=NULL); // record 0 or 1
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    cISignalValue *v = dynamic_cast<cISignalValue *>(obj);
    if (v)
        maybeCollect(v->getSignalTime(signalID), v->getSignalValue(signalID));
    else
        maybeCollect(obj!=NULL); // record 0 or 1
}

//---

void VectorRecorder::listenerAdded(cComponent *component, simsignal_t signalID)
{
    NumericResultRecorder::listenerAdded(component, signalID);

    // we can register the vector here, because base class ensures we are subscribed only at once place
    const char *signalName = cComponent::getSignalName(signalID);
    ASSERT(signalName != NULL);
    opp_string_map attributes;
    extractSignalAttributes(component, signalName, attributes);

    handle = ev.registerOutputVector(component->getFullPath().c_str(), signalName);
    ASSERT(handle != NULL);
    for (opp_string_map::iterator it = attributes.begin(); it != attributes.end(); ++it)
        ev.setVectorAttribute(handle, it->first.c_str(), it->second.c_str());
}

void VectorRecorder::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    // copied from base class to add monotonicity check
    cISignalValue *v = dynamic_cast<cISignalValue *>(obj);
    if (v) {
        simtime_t t = v->getSignalTime(signalID);
        if (t < lastTime) {
            const char *signalName = cComponent::getSignalName(signalID);
            throw cRuntimeError(
                    "%s: cannot record data with an earlier timestamp (t=%s) than "
                    "the previously recorded value, for signal %s (id=%d)",
                    opp_typename(typeid(*this)), SIMTIME_STR(t), signalName, (int)signalID);
        }
        maybeCollect(t, v->getSignalValue(signalID));
    }
    else {
        maybeCollect(obj!=NULL); // record 0 or 1
    }
}

void VectorRecorder::collect(simtime_t t, double value)
{
    lastTime = t;
    ev.recordInOutputVector(handle, t, value);
}

//---

void CountRecorder::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    maybeCollect(0.0); // dummy value
}

void CountRecorder::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    // if it is a cISignalValue, we should use its time for checking warm-up period;
    // base class method overridden to spare call to getSignalValue()
    cISignalValue *v = dynamic_cast<cISignalValue *>(obj);
    if (v)
        maybeCollect(v->getSignalTime(signalID), 0.0); // dummy value
    else
        maybeCollect(0.0); // dummy value
}

void CountRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName(signalID, "count");
    opp_string_map attributes;
    extractSignalAttributes(component, signalID, attributes);
    ev.recordScalar(component, scalarName.c_str(), count, &attributes);
}

//---

void LastValueRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName(signalID, "last");
    opp_string_map attributes;
    extractSignalAttributes(component, signalID, attributes);
    ev.recordScalar(component, scalarName.c_str(), lastValue, &attributes);
}

//---

void SumRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName(signalID, "sum");
    opp_string_map attributes;
    extractSignalAttributes(component, signalID, attributes);
    ev.recordScalar(component, scalarName.c_str(), sum, &attributes);
}

//---

void MeanRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName(signalID, "mean");
    opp_string_map attributes;
    extractSignalAttributes(component, signalID, attributes);
    ev.recordScalar(component, scalarName.c_str(), sum/count, &attributes); // note: this is NaN if count==0
}

//---

void MinRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName(signalID, "min");
    opp_string_map attributes;
    extractSignalAttributes(component, signalID, attributes);
    ev.recordScalar(component, scalarName.c_str(), isPositiveInfinity(min) ? NaN : min, &attributes);
}

//---

void MaxRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName(signalID, "max");
    opp_string_map attributes;
    extractSignalAttributes(component, signalID, attributes);
    ev.recordScalar(component, scalarName.c_str(), isNegativeInfinity(max) ? NaN : max, &attributes);
}

//---

void TimeAverageRecorder::collect(simtime_t t, double value)
{
    if (startTime < SIMTIME_ZERO)
        startTime = t;
    else
        weightedSum += lastValue * SIMTIME_DBL(t - lastTime);
    lastTime = t;
    lastValue = value;
}

void TimeAverageRecorder::finish(cComponent *component, simsignal_t signalID)
{
    bool empty = (startTime < SIMTIME_ZERO);
    simtime_t t = simulation.getSimTime();
    collect(t, 0.0); // to get the last interval counted in; value 0.0 is just a dummy
    double interval = SIMTIME_DBL(t - startTime);

    std::string scalarName = makeName(signalID, "timeavg");
    opp_string_map attributes;
    extractSignalAttributes(component, signalID, attributes);
    ev.recordScalar(component, scalarName.c_str(), empty ? NaN : (weightedSum / interval), &attributes);
}

//---

void StatisticsRecorder::finish(cComponent *component, simsignal_t signalID)
{
    const char *signalName = cComponent::getSignalName(signalID);
    opp_string_map attributes;
    extractSignalAttributes(component, signalID, attributes);
    ev.recordStatistic(component, signalName, statistic, &attributes);
}

