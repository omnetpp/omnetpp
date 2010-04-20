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


// note: we don't register WarmupPeriodFilter and ExpressionFilter
Register_ResultFilter("count", CountFilter);
Register_ResultFilter("constant0", Constant0Filter);
Register_ResultFilter("constant1", Constant1Filter);
Register_ResultFilter("last", LastValueFilter);
Register_ResultFilter("sum", SumFilter);
Register_ResultFilter("mean", MeanFilter);
Register_ResultFilter("min", MinFilter);
Register_ResultFilter("max", MaxFilter);
Register_ResultFilter("timeavg", TimeAverageFilter);
Register_ResultFilter("packetBytes", PacketBytesFilter);
Register_ResultFilter("packetBits", PacketBitsFilter);
Register_ResultFilter("sumPerDuration", SumPerDurationFilter);


void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, simtime_t_cref t, long l)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, l);
}

void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, simtime_t_cref t, unsigned long l)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, l);
}

void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, simtime_t_cref t, double d)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, d);
}

void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, simtime_t_cref t, const SimTime& v)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, v);
}

void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, simtime_t_cref t, const char *s)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, s);
}

void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, simtime_t_cref t, cObject *obj)
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

class ValueVariable : public Expression::Variable
{
  private:
    ExpressionFilter *owner;
  public:
    ValueVariable(ExpressionFilter *filter) {owner = filter;}
    virtual Functor *dup() const {return new ValueVariable(owner);}
    virtual const char *getName() const {return "<signalvalue>";}
    virtual char getReturnType() const {return Expression::Value::DBL;}
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) {return owner->currentValue;}
};

//XXX currently unused
class TimeVariable : public Expression::Variable
{
  private:
    ExpressionFilter *owner;
  public:
    TimeVariable(ExpressionFilter *filter) {owner = filter;}
    virtual Functor *dup() const {return new TimeVariable(owner);}
    virtual const char *getName() const {return "<signaltime>";}
    virtual char getReturnType() const {return Expression::Value::DBL;}
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) {return SIMTIME_DBL(owner->currentTime);}
};

bool ExpressionFilter::process(simtime_t& t, double& value)
{
    currentTime = t;
    currentValue = value;
    value = expr.doubleValue();
    return true;
}

Expression::Functor *ExpressionFilter::makeValueVariable()
{
    return new ValueVariable(this);
}

Expression::Functor *ExpressionFilter::makeTimeVariable()
{
    return new TimeVariable(this);
}

//---

void PacketBytesFilter::receiveSignal(ResultFilter *prev, simtime_t_cref t, cObject *object)
{
    if (dynamic_cast<cPacket *>(object))
    {
        cPacket *pk = (cPacket *)object;
        fire(this, t, (double)pk->getByteLength());
    }
}

//---

void PacketBitsFilter::receiveSignal(ResultFilter *prev, simtime_t_cref t, cObject *object)
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
    value = sum / (simTime() - simulation.getWarmupPeriod());
    return true;
}

