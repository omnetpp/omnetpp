//==========================================================================
//  RESULTFILTERS.CC - part of
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

#include "omnetpp/cpacket.h"  // PacketBytesFilter
#include "resultfilters.h"

namespace omnetpp {

using namespace omnetpp::common;  // Expression

// note: we don't register WarmupPeriodFilter and ExpressionFilter
Register_ResultFilter("count", CountFilter);
Register_ResultFilter("constant0", Constant0Filter);
Register_ResultFilter("constant1", Constant1Filter);
Register_ResultFilter("sum", SumFilter);
Register_ResultFilter("mean", MeanFilter);
Register_ResultFilter("min", MinFilter);
Register_ResultFilter("max", MaxFilter);
Register_ResultFilter("last", IdentityFilter);  // useful for expressions like: record=last+5
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
        return false;  // suppress initial 0/0 = NaNs

    double interval = SIMTIME_DBL(t - startTime);
    value = weightedSum / interval;
    return true;
}

//---

bool UnaryExpressionFilter::process(simtime_t& t, double& value, cObject *details)
{
    currentTime = t;
    currentValue = value;
    value = expr.doubleValue();
    return true;
}

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
    currentTime = t;
    for (int i = 0; i < signalCount; i++) {
        if (prevFilters[i] == prev) {
            currentValues[i] = value;
            value = expr.doubleValue();
            return true;
        }
    }
    throw cRuntimeError("unknown signal");
}

Expression::Functor *NaryExpressionFilter::makeValueVariable(int index, cResultFilter *prevFilter)
{
    Assert(0 <= index && index <= signalCount);
    prevFilters[index] = prevFilter;
    currentValues[index] = NaN;
    return new ValueVariable(this, &(currentValues[index]));
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

}  // namespace omnetpp

