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


CommonStringPool ResultRecorder::statisticNamesPool;

void ResultRecorder::init(const char *statsName)
{
    statisticName = statisticNamesPool.get(statsName);
}

void ResultRecorder::listenerAdded(cComponent *component, simsignal_t signalID)
{
    ASSERT(getSubscribeCount() == 1);  // may only be subscribed once (otherwise results get mixed)
}

void ResultRecorder::listenerRemoved(cComponent *component, simsignal_t signalID)
{
    if (getSubscribeCount() == 0)
        delete this;
}

std::string ResultRecorder::makeName(const char *suffix)
{
    return std::string(statisticName) + ":" + suffix;
}

void ResultRecorder::extractStatisticAttributes(cComponent *component, opp_string_map& result)
{
    cProperty *property = component->getProperties()->get("statistic", getStatisticName());
    if (!property)
        return;

    // fill result[] from the properties
    const std::vector<const char *>& keys = property->getKeys();
    for (int i = 0; i < (int)keys.size(); i++)
    {
        const char *key = keys[i];
        if (!strcmp(key, "record"))
            continue; // no need to save record= key
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
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);

    handle = ev.registerOutputVector(component->getFullPath().c_str(), getStatisticName());
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
            throw cRuntimeError(
                    "%s: cannot record data with an earlier timestamp (t=%s) than "
                    "the previously recorded value, for statistic %s (id=%d)",
                    opp_typename(typeid(*this)), SIMTIME_STR(t), getStatisticName(), (int)signalID);
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
    std::string scalarName = makeName("count");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), count, &attributes);
}

//---

void LastValueRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName("last");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), lastValue, &attributes);
}

//---

void SumRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName("sum");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), sum, &attributes);
}

//---

void MeanRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName("mean");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), sum/count, &attributes); // note: this is NaN if count==0
}

//---

void MinRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName("min");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), isPositiveInfinity(min) ? NaN : min, &attributes);
}

//---

void MaxRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName("max");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
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

    std::string scalarName = makeName("timeavg");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), empty ? NaN : (weightedSum / interval), &attributes);
}

//---

void StatisticsRecorder::finish(cComponent *component, simsignal_t signalID)
{
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordStatistic(component, getStatisticName(), statistic, &attributes);
}

