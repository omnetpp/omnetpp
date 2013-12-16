//==========================================================================
//  RESULTFILTERS.CC - part of
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

#include "resultfilters.h"
#include "cmessage.h"  // PacketBytesFilter

NAMESPACE_BEGIN


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


void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, b);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, long l)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, l);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, l);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, double d)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, d);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, v);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, s);
}

void WarmupPeriodFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj)
{
    // note: cITimestampedValue stuff was already dispatched to (simtime_t,double) method in base class
    if (t >= getEndWarmupPeriod())
        fire(this, t, obj);
}

//---

bool TimeAverageFilter::process(simtime_t& t, double& value)
{
    bool isFirstValue = startTime < SIMTIME_ZERO;
    if (isFirstValue)
        startTime = t;
    else
        weightedSum += lastValue * SIMTIME_DBL(t - lastTime);  // always forward-sample-hold
    lastTime = t;
    lastValue = value;

    if (isFirstValue || t==startTime)
        return false;  // suppress initial 0/0 = NaNs

    double interval = SIMTIME_DBL(t - startTime);
    value = weightedSum / interval;
    return true;
}

//---

bool UnaryExpressionFilter::process(simtime_t& t, double& value)
{
    currentTime = t;
    currentValue = value;
    value = expr.doubleValue();
    return true;
}

void NaryExpressionFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b)
{
    simtime_t tt = t;
    double d = b;
    if (process(prev, tt, d))
        fire(this, tt, d);
}

void NaryExpressionFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, long l)
{
    simtime_t tt = t;
    double d = l;
    if (process(prev, tt, d))
        fire(this, tt, d);
}

void NaryExpressionFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l)
{
    simtime_t tt = t;
    double d = l;
    if (process(prev, tt, d))
        fire(this, tt, d);
}

void NaryExpressionFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, double d)
{
    simtime_t tt = t;
    if (process(prev, tt, d))
        fire(this, tt, d);
}

void NaryExpressionFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v)
{
    simtime_t tt = t;
    double d = v.dbl();
    if (process(prev, tt, d))
        fire(this, tt, d);
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

void PacketBytesFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object)
{
    if (dynamic_cast<cPacket *>(object))
    {
        cPacket *pk = (cPacket *)object;
        fire(this, t, (double)pk->getByteLength());
    }
}

//---

void PacketBitsFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object)
{
    if (dynamic_cast<cPacket *>(object))
    {
        cPacket *pk = (cPacket *)object;
        fire(this, t, (double)pk->getBitLength());
    }
}

//---

bool SumPerDurationFilter::process(simtime_t& t, double& value)
{
    sum += value;
    value = sum / (t - simulation.getWarmupPeriod());
    return true;
}

NAMESPACE_END

