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
#include "resultfilters.h"

namespace omnetpp {

using namespace omnetpp::common;  // Expression

// note: we don't register WarmupPeriodFilter and ExpressionFilter
Register_ResultFilter("warmup", WarmupPeriodFilter);
Register_ResultFilter("count", CountFilter);
Register_ResultFilter("constant0", Constant0Filter);
Register_ResultFilter("constant1", Constant1Filter);
Register_ResultFilter("skipNan", SkipNanFilter);
Register_ResultFilter("sum", SumFilter);
Register_ResultFilter("mean", MeanFilter);
Register_ResultFilter("min", MinFilter);
Register_ResultFilter("max", MaxFilter);
Register_ResultFilter("last", IdentityFilter);  // useful for expressions like: record=last+5
Register_ResultFilter("avg", AverageFilter);
Register_ResultFilter("timeavg", TimeAverageFilter);
Register_ResultFilter("removeRepeats", RemoveRepeatsFilter);
Register_ResultFilter("packetBytes", PacketBytesFilter);
Register_ResultFilter("packetBits", PacketBitsFilter);
Register_ResultFilter("sumPerDuration", SumPerDurationFilter);

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

std::string CountFilter::str() const
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
        count++;
        weightedSum += value;
        value = weightedSum/count;
        return true;
    }
    else {
        bool isFirstValue = startTime < SIMTIME_ZERO;
        if (isFirstValue)
            startTime = t;
        else
            weightedSum += lastValue * SIMTIME_DBL(t - lastTime);  // always forward-sample-hold
        lastTime = t;
        lastValue = value;

        if (isFirstValue || t == startTime)
            return true;  // avoid initial 0/0 = NaNs, value is valid as mean

        double interval = SIMTIME_DBL(t - startTime);
        value = weightedSum / interval;
        return true;
    }
}

double MeanFilter::getMean() const
{
    if (!timeWeighted) {
        return weightedSum/count;
    }
    else {
        if (startTime < SIMTIME_ZERO) // empty
            return NaN;

        simtime_t now = getSimulation()->getSimTime();
        double lastInterval = SIMTIME_DBL(now - lastTime);
        double weightedSum = this->weightedSum + lastValue * lastInterval;
        double interval = SIMTIME_DBL(now - startTime);
        return weightedSum / interval;
    }
}

std::string MeanFilter::str() const
{
    std::stringstream os;
    os << (timeWeighted ? "time-weighted mean" : "mean") << " = " << getMean();
    return os.str();
}

//---

std::string MinFilter::str() const
{
    std::stringstream os;
    os << "min = " << getMin();
    return os.str();
}

//---

std::string MaxFilter::str() const
{
    std::stringstream os;
    os << "max = " << getMax();
    return os.str();
}

//---

std::string AverageFilter::str() const
{
    std::stringstream os;
    os << "average = " << getAverage();
    return os.str();
}

//---

bool TimeAverageFilter::process(simtime_t& t, double& value, cObject *details)
{
    bool isFirstValue = startTime < SIMTIME_ZERO;
    if (isFirstValue)
        startTime = t;
    else
        weightedSum += lastValue * SIMTIME_DBL(t - lastTime);  // always forward-sample-hold
    lastTime = t;
    lastValue = value;

    if (isFirstValue || t == startTime)
        return true;  // avoid initial 0/0 = NaNs, value is valid as mean

    double interval = SIMTIME_DBL(t - startTime);
    value = weightedSum / interval;
    return true;
}

double TimeAverageFilter::getTimeAverage() const
{
    if (startTime < SIMTIME_ZERO) // empty
        return NaN;

    simtime_t now = getSimulation()->getSimTime();
    double lastInterval = SIMTIME_DBL(now - lastTime);
    double weightedSum = this->weightedSum + lastValue * lastInterval;
    double interval = SIMTIME_DBL(now - startTime);
    return weightedSum / interval;
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
    lastInputValues[index] = NaN;
    return new ValueVariable(this, &(lastInputValues[index]));
}

//---

void PacketBytesFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (cPacket *packet = dynamic_cast<cPacket *>(object))
        fire(this, t, (double)packet->getByteLength(), details);
}

//---

void PacketBitsFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details)
{
    if (cPacket *packet = dynamic_cast<cPacket *>(object))
        fire(this, t, (double)packet->getBitLength(), details);
}

//---

bool SumPerDurationFilter::process(simtime_t& t, double& value, cObject *details)
{
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

