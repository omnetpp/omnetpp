//==========================================================================
//  RESULTFILTERS.CC - part of
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

#include "omnetpp/cpacket.h"  // PacketBytesFilter
#include "omnetpp/cproperty.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/resultfilters.h"
#include "resultexpr.h"

namespace omnetpp {

using namespace omnetpp::common;  // Expression

#define SIGNALTYPE_TO_NUMERIC_CONVERSIONS \
        "Numeric types are all converted to double, and boolean to 0 and 1. " \
        "Other non-numeric types will cause an error. "

#define NAN_VALUES_IGNORED \
        "NaN values in the input are ignored. "

#define OPTIONALLY_TIMEWEIGHTED \
        "NaN values in the input are ignored, or in the time-weighted case, " \
        "they indicate that the interval up to the next value is to be ignored. " \
        "To turn on time-weighted, specify 'timeWeighted=true' in the @statistic property."

// note: we don't register ExpressionFilter

Register_ResultFilter2("warmup", WarmupPeriodFilter,
        "Ignores all values in the input during the warmup period, "
        "and lets everything through unchanged afterwards. "
        "This filter is inserted automatically whenever a warmup period "
        "is specified in the configuration."
);

Register_ResultFilter2("totalCount", TotalCountFilter,
        "Produces the count of the input values, including NaN and nullptr values. "
        "Signal values do not need to be numeric to be counted. "
);

Register_ResultFilter2("count", CountFilter,
        "Produces the count of the input values. "
        "Signal values do not need to be numeric to be counted. "
        "NaN and nullptr values are ignored."
);

Register_ResultFilter2("constant0", Constant0Filter,
        "Replaces every input value, regardless of its type, with the 0 constant."
);
Register_ResultFilter2("constant1", Constant1Filter,
        "Replaces every value, regardless of its type, with the 1.0 constant."
);
Register_ResultFilter2("time", TimeFilter,
        "Replaces every value with the time of emitting the signal. "
        "Note: unlike the \"count\" filter, this one does not ignore NaN and nullptr values. "
);
Register_ResultFilter2("skipNan", SkipNanFilter,
        "Removes (filters out) NaNs, and lets through all other values."
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
);
Register_ResultFilter2("errorNan", ErrorNanFilter,
        "Raises a runtime error if it encounters a NaN value in the input. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
);
Register_ResultFilter2("nanCount", CountNanFilter, // NOTE: the name "nanCount" (as opposed to "countNan") is better suited for typical use: "record=count,mean,nanCount"
        "Counts the number of NaN values in the input. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
);
Register_ResultFilter2("sum", SumFilter,
        "Produces the sum of the input values. "
        NAN_VALUES_IGNORED
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
);
Register_ResultFilter2("mean", MeanFilter,
        "Produces the (time-weighted or unweighted) mean of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        OPTIONALLY_TIMEWEIGHTED
);
Register_ResultFilter2("min", MinFilter,
        "Produces the minimum of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        NAN_VALUES_IGNORED
);
Register_ResultFilter2("max", MaxFilter,
        "Produces the maximum of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        NAN_VALUES_IGNORED
);
Register_ResultFilter2("last", IdentityFilter,
        "This is actually the identity filter that lets through all input unchanged. "
        "It is useful for expressions like: record=last+5. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
);

Register_ResultFilter2("avg", AverageFilter,
        "Produces the arithmetic mean of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        NAN_VALUES_IGNORED
);
Register_ResultFilter2("timeavg", TimeAverageFilter,
        "Produces the time average of the input values, assuming sample-hold interpolation. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        "A NaN value in the input indicates that the interval up to the next value is to be ignored. "
);
Register_ResultFilter2("removeRepeats", RemoveRepeatsFilter,
        "Removes repeated values from the input. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
);
Register_ResultFilter2("packetBytes", PacketBytesFilter,
        "Expects a cPacket and outputs its length in bytes (getByteLength()). "
        "Null pointers values are ignored. "
        "Non-object signals and objects which are not cPacket will cause an error. "
);
Register_ResultFilter2("packetBits", PacketBitsFilter,
        "Expects a cPacket and outputs its length in bits (getBitLength()). "
        "Null pointers values are ignored. "
        "Non-object signals and objects which are not cPacket will cause an error. "
);
Register_ResultFilter2("sumPerDuration", SumPerDurationFilter,
        "Outputs the sum of the input values divided by the measurement interval "
        "(simtime minus warmupPeriod). NaN values in the input are ignored. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
);


void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, b, details);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, l, details);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, l, details);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, d, details);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, v, details);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, s, details);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details)
{
    // note: cITimestampedValue stuff was already dispatched to (simtime_t,double) method in base class
    if (t >= getEndWarmupPeriod())
        fire(this, t, obj, details);
}

std::string WarmupPeriodFilter::str() const
{
    std::stringstream os;
    os << "warmupPeriod = " << getEndWarmupPeriod();
    return os.str();
}

//---

std::string TotalCountFilter::str() const
{
    std::stringstream os;
    os << "count = " << getCount();
    return os.str();
}

//---

std::string ConstantFilter::str() const
{
    std::stringstream os;
    os << "c = " << getConstant();
    return os.str();
}

//---

bool ErrorNanFilter::process(simtime_t& t, double& value, cObject *details)
{
    if (std::isnan(value))
        throw cRuntimeError("errorNan: NaN value detected");
    return true;
}

//---

bool CountNanFilter::process(simtime_t& t, double& value, cObject *details)
{
    if (!std::isnan(value))
        return false;
    count++;
    value = count;
    return true;
}

std::string CountNanFilter::str() const
{
    std::stringstream os;
    os << "count = " << getCount();
    return os.str();
}

//---

bool SumFilter::process(simtime_t& t, double& value, cObject* details)
{
    if (std::isnan(value))
        return false;
    sum += value;
    value = sum;
    return true;
}

std::string SumFilter::str() const
{
    std::stringstream os;
    os << "sum = " << getSum();
    return os.str();
}

//---

void MeanFilter::init(cComponent *component, cProperty *attrsProperty)
{
    cNumericResultFilter::init(component, attrsProperty);
    const char *attr = attrsProperty->getValue("timeWeighted", 0);
    timeWeighted = attr && (std::string)attr != "0" && (std::string)attr != "false";
}

bool MeanFilter::process(simtime_t& t, double& value, cObject *details)
{
    if (!timeWeighted) {
        if (std::isnan(value))
            return false;
        count++;
        weightedSum += value;
        value = weightedSum/count;
        return true;
    }
    else {
        if (!std::isnan(lastValue)) {
            totalTime += t - lastTime;
            weightedSum += lastValue * SIMTIME_DBL(t - lastTime);
        }
        lastTime = t;
        lastValue = value;
        value = weightedSum / totalTime;
        return !std::isnan(value);
    }
}

double MeanFilter::getMean() const
{
    if (!timeWeighted) {
        return weightedSum / count;
    }
    else {
        simtime_t tmpTotalTime = totalTime;
        double tmpWeightedSum = weightedSum;

        if (!std::isnan(lastValue)) {
            simtime_t t = getSimulation()->getSimTime();
            tmpTotalTime += t - lastTime;
            tmpWeightedSum += lastValue * SIMTIME_DBL(t - lastTime);
        }
        return tmpWeightedSum / tmpTotalTime;
    }
}

std::string MeanFilter::str() const
{
    std::stringstream os;
    os << (timeWeighted ? "time-weighted mean" : "mean") << " = " << getMean();
    return os.str();
}

//---

bool MinFilter::process(simtime_t& t, double& value, cObject *details)
{
    if (std::isnan(value))
        return false;
    if (value < min)
        min = value;
    value = min;
    return true;
}

std::string MinFilter::str() const
{
    std::stringstream os;
    os << "min = " << getMin();
    return os.str();
}

//---


bool MaxFilter::process(simtime_t& t, double& value, cObject *details)
{
    if (std::isnan(value))
        return false;
    if (value > max)
        max = value;
    value = max;
    return true;
}

std::string MaxFilter::str() const
{
    std::stringstream os;
    os << "max = " << getMax();
    return os.str();
}

//---


bool AverageFilter::process(simtime_t& t, double& value, cObject *details)
{
    if (std::isnan(value))
        return false;
    count++;
    sum += value;
    value = sum / count;
    return true;
}

std::string AverageFilter::str() const
{
    std::stringstream os;
    os << "average = " << getAverage();
    return os.str();
}

//---

bool TimeAverageFilter::process(simtime_t& t, double& value, cObject *details)
{
    if (!std::isnan(lastValue)) {
        totalTime += t - lastTime;
        weightedSum += lastValue * SIMTIME_DBL(t - lastTime);
    }
    lastTime = t;
    lastValue = value;
    value = weightedSum / totalTime;
    return !std::isnan(value);
}

double TimeAverageFilter::getTimeAverage() const
{
    simtime_t tmpTotalTime = totalTime;
    double tmpWeightedSum = weightedSum;

    if (!std::isnan(lastValue)) {
        simtime_t t = getSimulation()->getSimTime();
        tmpTotalTime += t - lastTime;
        tmpWeightedSum += lastValue * SIMTIME_DBL(t - lastTime);
    }
    return tmpWeightedSum / tmpTotalTime;
}

std::string TimeAverageFilter::str() const
{
    std::stringstream os;
    os << "timeAverage = " << getTimeAverage();
    return os.str();
}

//---

std::string RemoveRepeatsFilter::str() const
{
    std::stringstream os;
    os << "lastValue = " << getLastValue();
    return os.str();
}

bool RemoveRepeatsFilter::process(simtime_t& t, double& value, cObject *details)
{
    bool repeated = std::isnan(value) ? std::isnan(prev) : value==prev;
    prev = value;
    return !repeated;
}

//---

std::string ExpressionFilter::str() const
{
    std::stringstream os;
    os << expr.str() << " = " << getLastValue();
    return os.str();
}

//---

bool UnaryExpressionFilter::process(simtime_t& t, double& value, cObject *details)
{
    lastTimestamp = t;
    lastInputValue = value;
    lastOutputValue = value = expr.doubleValue();
    return true;
}

//---

void NaryExpressionFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details)
{
    simtime_t tt = t;
    double d = b;
    if (process(prev, tt, d))
        fire(this, tt, d, details);
}

void NaryExpressionFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details)
{
    simtime_t tt = t;
    double d = l;
    if (process(prev, tt, d))
        fire(this, tt, d, details);
}

void NaryExpressionFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details)
{
    simtime_t tt = t;
    double d = l;
    if (process(prev, tt, d))
        fire(this, tt, d, details);
}

void NaryExpressionFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details)
{
    simtime_t tt = t;
    if (process(prev, tt, d))
        fire(this, tt, d, details);
}

void NaryExpressionFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details)
{
    simtime_t tt = t;
    double d = v.dbl();
    if (process(prev, tt, d))
        fire(this, tt, d, details);
}

bool NaryExpressionFilter::process(cResultFilter *prev, simtime_t& t, double& value)
{
    lastTimestamp = t;
    for (int i = 0; i < signalCount; i++) {
        if (prevFilters[i] == prev) {
            lastInputValues[i] = value;
            lastOutputValue = value = expr.doubleValue();
            return true;
        }
    }
    throw cRuntimeError("Unknown signal");
}

Expression::Functor *NaryExpressionFilter::makeValueVariable(int index, cResultFilter *prevFilter)
{
    Assert(0 <= index && index <= signalCount);
    prevFilters[index] = prevFilter;
    lastInputValues[index] = prevFilter->getInitialDoubleValue();
    return new ValueVariable(this, &(lastInputValues[index]));
}

//---

void PacketBytesFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (object)
        if (cPacket *packet = check_and_cast<cPacket *>(object))
            fire(this, t, (double)packet->getByteLength(), details);
}

//---

void PacketBitsFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (object)
        if (cPacket *packet = check_and_cast<cPacket *>(object))
            fire(this, t, (double)packet->getBitLength(), details);
}

//---

bool SumPerDurationFilter::process(simtime_t& t, double& value, cObject *details)
{
    if (std::isnan(value))
        return false;
    sum += value;
    value = sum / (t - getSimulation()->getWarmupPeriod());
    return true;
}

double SumPerDurationFilter::getSumPerDuration() const
{
    cSimulation *simulation = getSimulation();
    simtime_t now = simulation->getSimTime();
    simtime_t interval = now - simulation->getWarmupPeriod();
    return sum / interval;
}

std::string SumPerDurationFilter::str() const
{
    std::stringstream os;
    os << "sum/duration = " << getSumPerDuration();
    return os.str();
}

}  // namespace omnetpp

