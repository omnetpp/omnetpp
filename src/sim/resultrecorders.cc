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

#include "resultrecorders.h"
#include "cproperty.h"
#include "chistogram.h"

NAMESPACE_BEGIN


Register_ResultRecorder("vector", VectorRecorder);
Register_ResultRecorder("count", CountRecorder);
Register_ResultRecorder("last", LastValueRecorder);
Register_ResultRecorder("sum", SumRecorder);
Register_ResultRecorder("mean", MeanRecorder);
Register_ResultRecorder("min", MinRecorder);
Register_ResultRecorder("max", MaxRecorder);
Register_ResultRecorder("timeavg", TimeAverageRecorder);
Register_ResultRecorder("stats", StatsRecorder);
Register_ResultRecorder("histogram", HistogramRecorder);


VectorRecorder::~VectorRecorder()
{
    if (handle != NULL)
        ev.deregisterOutputVector(handle);
}

void VectorRecorder::subscribedTo(cResultFilter *prev)
{
    cNumericResultRecorder::subscribedTo(prev);

    // we can register the vector here, because base class ensures we are subscribed only at once place
    opp_string_map attributes = getStatisticAttributes();

    handle = ev.registerOutputVector(getComponent()->getFullPath().c_str(), getResultName().c_str());
    ASSERT(handle != NULL);
    for (opp_string_map::iterator it = attributes.begin(); it != attributes.end(); ++it)
        ev.setVectorAttribute(handle, it->first.c_str(), it->second.c_str());
}

void VectorRecorder::collect(simtime_t_cref t, double value)
{
    if (t < lastTime)
    {
        throw cRuntimeError("%s: Cannot record data with an earlier timestamp (t=%s) "
                            "than the previously recorded value (t=%s)",
                            getClassName(), SIMTIME_STR(t), SIMTIME_STR(lastTime));
    }

    lastTime = t;
    ev.recordInOutputVector(handle, t, value);
}

//---

void CountRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    ev.recordScalar(getComponent(), getResultName().c_str(), count, &attributes);
}

//---

void LastValueRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    ev.recordScalar(getComponent(), getResultName().c_str(), lastValue, &attributes);
}

//---

void SumRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    ev.recordScalar(getComponent(), getResultName().c_str(), sum, &attributes);
}

//---

void MeanRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    ev.recordScalar(getComponent(), getResultName().c_str(), sum/count, &attributes); // note: this is NaN if count==0
}

//---

void MinRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    ev.recordScalar(getComponent(), getResultName().c_str(), isPositiveInfinity(min) ? NaN : min, &attributes);
}

//---

void MaxRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    ev.recordScalar(getComponent(), getResultName().c_str(), isNegativeInfinity(max) ? NaN : max, &attributes);
}

//---

void TimeAverageRecorder::collect(simtime_t_cref t, double value)
{
    if (startTime < SIMTIME_ZERO) // uninitialized
        startTime = t;
    else
        weightedSum += lastValue * SIMTIME_DBL(t - lastTime);
    lastTime = t;
    lastValue = value;
}

void TimeAverageRecorder::finish(cResultFilter *prev)
{
    bool empty = (startTime < SIMTIME_ZERO);
    simtime_t t = simulation.getSimTime();
    collect(t, NaN); // to get the last interval counted in; the value is just a dummy
    double interval = SIMTIME_DBL(t - startTime);

    opp_string_map attributes = getStatisticAttributes();
    ev.recordScalar(getComponent(), getResultName().c_str(), empty ? NaN : (weightedSum / interval), &attributes);
}

//---

void StatisticsRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    ev.recordStatistic(getComponent(), getResultName().c_str(), statistic, &attributes);
}

StatsRecorder::StatsRecorder() : StatisticsRecorder(new cStdDev())
{
}

HistogramRecorder::HistogramRecorder() : StatisticsRecorder(new cHistogram())
{
}

//---

class RecValueVariable : public Expression::Variable
{
  private:
    ExpressionRecorder *owner;
  public:
    RecValueVariable(ExpressionRecorder *recorder) {owner = recorder;}
    virtual Functor *dup() const {return new RecValueVariable(owner);}
    virtual const char *getName() const {return "<lastsignalvalue>";}
    virtual char getReturnType() const {return Expression::Value::DBL;}
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) {return owner->lastValue;}
};

//XXX currently unused
class RecTimeVariable : public Expression::Variable
{
  public:
    virtual Functor *dup() const {return new RecTimeVariable();}
    virtual const char *getName() const {return "<simtime>";}
    virtual char getReturnType() const {return Expression::Value::DBL;}
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) {return SIMTIME_DBL(simulation.getSimTime());}
};

Expression::Functor *ExpressionRecorder::makeValueVariable()
{
    return new RecValueVariable(this);
}

Expression::Functor *ExpressionRecorder::makeTimeVariable()
{
    return new RecTimeVariable();
}

void ExpressionRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    ev.recordScalar(getComponent(), getResultName().c_str(), expr.doubleValue(), &attributes);
}

NAMESPACE_END

