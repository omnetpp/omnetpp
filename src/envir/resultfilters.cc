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


void ResultFilter::addDelegate(ResultProcessor *delegate)
{
    // reallocate each time
    int n = getNumDelegates();
    ResultProcessor **v = new ResultProcessor*[n+2];
    memcpy(v, delegates, n*sizeof(ResultProcessor*));
    v[n] = delegate;
    v[n+1] = NULL;
    delete [] delegates;
    delegates = v;
    delegate->subscribecount++;
    delegate->listenerAdded(NULL, SIMSIGNAL_NULL);
}

int ResultFilter::getNumDelegates() const
{
    if (!delegates)
        return 0;
    int k = 0;
    while (delegates[k])
        k++;
    return k;
}

std::vector<ResultProcessor*> ResultFilter::getDelegates() const
{
    std::vector<ResultProcessor*> result;
    for (int i = 0; delegates[i]; i++)
        result.push_back(delegates[i]);
    return result;
}

ResultFilter::~ResultFilter()
{
    if (delegates)
    {
        for (int i=0; delegates[i]; i++) {
            delegates[i]->subscribecount--;
            delegates[i]->listenerRemoved(NULL, SIMSIGNAL_NULL);
        }
        delete [] delegates;
    }
}

void ResultFilter::listenerAdded(cComponent *component, simsignal_t signalID)
{
    ASSERT(getSubscribeCount() == 1);  // may only be subscribed once (otherwise results get mixed)
}

void ResultFilter::listenerRemoved(cComponent *component, simsignal_t signalID)
{
    if (getSubscribeCount() == 0)
        delete this;
}

void ResultFilter::fire(cComponent *source, simsignal_t signalID, long l)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(source, signalID, l);
}

void ResultFilter::fire(cComponent *source, simsignal_t signalID, double d)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(source, signalID, d);
}

void ResultFilter::fire(cComponent *source, simsignal_t signalID, simtime_t t)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(source, signalID, t);
}

void ResultFilter::fire(cComponent *source, simsignal_t signalID, const char *s)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(source, signalID, s);
}

void ResultFilter::fire(cComponent *source, simsignal_t signalID, cObject *obj)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(source, signalID, obj);
}

void ResultFilter::finish(cComponent *component, simsignal_t signalID)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->finish(component, signalID);
}

//---

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

void NumericResultFilter::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    double d = l;
    if (process(d))
        fire(source, signalID, d);
}

void NumericResultFilter::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    if (process(d))
        fire(source, signalID, d);
}

void NumericResultFilter::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t v)
{
    double d = SIMTIME_DBL(v);
    if (process(d))
        fire(source, signalID, d);
}

void NumericResultFilter::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    throw cRuntimeError("cannot convert const char * to double"); //FIXME better message
}

void NumericResultFilter::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    cISignalValue *v = dynamic_cast<cISignalValue *>(obj);
    if (!v)
        throw cRuntimeError("cannot convert cObject* to double"); //FIXME better message

    simtime_t t = v->getSignalTime(signalID);
    double d = v->getSignalValue(signalID);
    if (process(t,d))
    {
        cSignalValue tmp(t,d);
        fire(source, signalID, &tmp);
    }
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


