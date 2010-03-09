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


Register_ResultRecorder("vector", VectorRecorder);
Register_ResultRecorder("count", CountRecorder);
Register_ResultRecorder("last", LastValueRecorder);
Register_ResultRecorder("sum", SumRecorder);
Register_ResultRecorder("mean", MeanRecorder);
Register_ResultRecorder("min", MinRecorder);
Register_ResultRecorder("max", MaxRecorder);
Register_ResultRecorder("timeavg", TimeAverageRecorder);
Register_ResultRecorder("histogram", HistogramRecorder);


void VectorRecorder::listenerAdded(cComponent *x, simsignal_t signalID)
{
    NumericResultRecorder::listenerAdded(x, signalID);

    // we can register the vector here, because base class ensures we are subscribed only at once place
    opp_string_map attributes;
    extractStatisticAttributes(getComponent(), attributes);

    handle = ev.registerOutputVector(getComponent()->getFullPath().c_str(), getStatisticName());
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
        collect(t, v->getSignalValue(signalID));
    }
    else {
        collect(obj!=NULL); // record 0 or 1
    }
}

void VectorRecorder::collect(simtime_t t, double value)
{
    lastTime = t;
    ev.recordInOutputVector(handle, t, value);
}

//---

void CountRecorder::tweakTitle(opp_string& title)
{
    title = opp_string("count of ") + title;
}

void CountRecorder::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    collect(0.0); // dummy value
}

void CountRecorder::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    // if it is a cISignalValue, we should use its time for checking warm-up period;
    // base class method overridden to spare call to getSignalValue()
    cISignalValue *v = dynamic_cast<cISignalValue *>(obj);
    if (v)
        collect(v->getSignalTime(signalID), 0.0); // dummy value
    else
        collect(0.0); // dummy value
}

void CountRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName("count");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), count, &attributes);
}

//---

void LastValueRecorder::tweakTitle(opp_string& title)
{
    title = opp_string("last value of ") + title;
}

void LastValueRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName("last");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), lastValue, &attributes);
}

//---

void SumRecorder::tweakTitle(opp_string& title)
{
    title = opp_string("sum of ") + title;
}

void SumRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName("sum");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), sum, &attributes);
}

//---

void MeanRecorder::tweakTitle(opp_string& title)
{
    title = opp_string("mean of ") + title;
}

void MeanRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName("mean");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), sum/count, &attributes); // note: this is NaN if count==0
}

//---

void MinRecorder::tweakTitle(opp_string& title)
{
    title = opp_string("minimum of ") + title;
}

void MinRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName("min");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), isPositiveInfinity(min) ? NaN : min, &attributes);
}

//---

void MaxRecorder::tweakTitle(opp_string& title)
{
    title = opp_string("maximum of ") + title;
}

void MaxRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName("max");
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), isNegativeInfinity(max) ? NaN : max, &attributes);
}

//---

void TimeAverageRecorder::tweakTitle(opp_string& title)
{
    title = opp_string("time average of ") + title;
}

void TimeAverageRecorder::collect(simtime_t t, double value)
{
    if (startTime < SIMTIME_ZERO) // uninitialized
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
    collect(t, NaN); // to get the last interval counted in; the value is just a dummy
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

StddevRecorder::StddevRecorder() : StatisticsRecorder(new cStdDev())
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

void ExpressionRecorder::finish(cComponent *component, simsignal_t signalID)
{
    std::string scalarName = makeName(expr.str().c_str());
    opp_string_map attributes;
    extractStatisticAttributes(component, attributes);
    ev.recordScalar(component, scalarName.c_str(), expr.doubleValue(), &attributes);
}

