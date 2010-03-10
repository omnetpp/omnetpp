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
Register_ResultFilter("pkBytes", PacketBytesFilter);


void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, long l)
{
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        fire(this, l);
}

void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, double d)
{
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        fire(this, d);
}

void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, simtime_t t, double d)
{
    if (t >= getEndWarmupPeriod())
        fire(this, t, d);
}

void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, simtime_t v)
{
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        fire(this, v);
}

void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, const char *s)
{
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        fire(this, s);
}

void WarmupPeriodFilter::receiveSignal(ResultFilter *prev, cObject *obj)
{
    // note: cISignalValue stuff was already dispatched to (simtime_t,double) method in base class
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        fire(this, obj);
}

//---

bool TimeAverageFilter::process(simtime_t& t, double& value)
{
    if (startTime < SIMTIME_ZERO) // uninitialized
        startTime = t;
    else
        weightedSum += lastValue * SIMTIME_DBL(t - lastTime);
    lastTime = t;
    lastValue = value;

    double interval = SIMTIME_DBL(t - startTime);
    value = weightedSum / interval;  // if interval==0, result will be 0/0=NaN
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

void PacketBytesFilter::receiveSignal(ResultFilter *prev, cObject *object)
{
    cPacket *pk = check_and_cast<cPacket *>(object);
    fire(this, (double)pk->getByteLength());
}



