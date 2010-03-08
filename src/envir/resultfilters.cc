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

// note: we don't register WarmupPeriodFilter and ExpressionFilter
Register_ResultFilter("count", CountFilter);
Register_ResultFilter("last", LastValueFilter);
Register_ResultFilter("sum", SumFilter);
Register_ResultFilter("mean", MeanFilter);
Register_ResultFilter("min", MinFilter);
Register_ResultFilter("max", MaxFilter);
Register_ResultFilter("timeavg", TimeAverageFilter);


void WarmupPeriodFilter::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        fire(source, signalID, l);
}

void WarmupPeriodFilter::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        fire(source, signalID, d);
}

void WarmupPeriodFilter::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t v)
{
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        fire(source, signalID, v);
}

void WarmupPeriodFilter::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        fire(source, signalID, s);
}

void WarmupPeriodFilter::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    cISignalValue *v = dynamic_cast<cISignalValue *>(obj);
    simtime_t t = v ? v->getSignalTime(signalID) : simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        fire(source, signalID, obj);
}

//---

void CountFilter::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    cISignalValue *v = dynamic_cast<cISignalValue *>(obj);
    if (!v)
        doIt(source, signalID);
    else {
        count++;
        cSignalValue tmp(v->getSignalTime(signalID), count);
        fire(source, signalID, &tmp);
    }
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
    virtual const char *getName() const {return "$value";}
    virtual char getReturnType() const {return 'D';}
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) {return owner->currentValue;}
};

class TimeVariable : public Expression::Variable
{
  private:
    ExpressionFilter *owner;
  public:
    TimeVariable(ExpressionFilter *filter) {owner = filter;}
    virtual Functor *dup() const {return new TimeVariable(owner);}
    virtual const char *getName() const {return "$time";}
    virtual char getReturnType() const {return 'D';}
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


