//==========================================================================
//  RESULTRECORDERS.CC - part of
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

#include "omnetpp/cproperty.h"
#include "omnetpp/chistogram.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cstatistic.h"
#include "resultrecorders.h"

namespace omnetpp {

using namespace omnetpp::common;  // Expression

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
    if (handle != nullptr)
        getEnvir()->deregisterOutputVector(handle);
}

void VectorRecorder::subscribedTo(cResultFilter *prev)
{
    cNumericResultRecorder::subscribedTo(prev);

    // we can register the vector here, because base class ensures we are subscribed only at once place
    opp_string_map attributes = getStatisticAttributes();

    handle = getEnvir()->registerOutputVector(getComponent()->getFullPath().c_str(), getResultName().c_str());
    ASSERT(handle != nullptr);
    for (opp_string_map::iterator it = attributes.begin(); it != attributes.end(); ++it)
        getEnvir()->setVectorAttribute(handle, it->first.c_str(), it->second.c_str());
}

void VectorRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    if (t < lastTime) {
        throw cRuntimeError("%s: Cannot record data with an earlier timestamp (t=%s) "
                            "than the previously recorded value (t=%s)",
                getClassName(), SIMTIME_STR(t), SIMTIME_STR(lastTime));
    }

    lastTime = t;
    lastValue = value;
    getEnvir()->recordInOutputVector(handle, t, value);
}

std::string VectorRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << ": ";
    if (std::isnan(lastValue))
        os << "(empty)";
    else
        os << "last write: t=" << getLastWriteTime() << " value=" << getLastValue();
    return os.str();
}

//---

void CountRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordScalar(getComponent(), getResultName().c_str(), count, &attributes);
}

std::string CountRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << " = " << getCount();
    return os.str();
}

//---

void LastValueRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordScalar(getComponent(), getResultName().c_str(), lastValue, &attributes);
}

std::string LastValueRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << " = " << getLastValue();
    return os.str();
}

//---

void SumRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordScalar(getComponent(), getResultName().c_str(), sum, &attributes);
}

std::string SumRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << " = " << getSum();
    return os.str();
}

//---

void MeanRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordScalar(getComponent(), getResultName().c_str(), sum / count, &attributes);  // note: this is NaN if count==0
}

std::string MeanRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << " = " << getMean();
    return os.str();
}

//---

void MinRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordScalar(getComponent(), getResultName().c_str(), isPositiveInfinity(min) ? NaN : min, &attributes);
}

std::string MinRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << " = " << getMin();
    return os.str();
}

//---

void MaxRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordScalar(getComponent(), getResultName().c_str(), isNegativeInfinity(max) ? NaN : max, &attributes);
}

std::string MaxRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << " = " << getMax();
    return os.str();
}

//---

void TimeAverageRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    if (startTime < SIMTIME_ZERO)  // uninitialized
        startTime = t;
    else
        weightedSum += lastValue * SIMTIME_DBL(t - lastTime);
    lastTime = t;
    lastValue = value;
}

double TimeAverageRecorder::getTimeAverage() const
{
    if (startTime < SIMTIME_ZERO) // empty
        return NaN;

    simtime_t now = getSimulation()->getSimTime();
    double lastInterval = SIMTIME_DBL(now - lastTime);
    double weightedSum = this->weightedSum + lastValue * lastInterval;
    double interval = SIMTIME_DBL(now - startTime);
    return weightedSum / interval;
}

void TimeAverageRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordScalar(getComponent(), getResultName().c_str(), getTimeAverage(), &attributes);
}

std::string TimeAverageRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << " = " << getTimeAverage();
    return os.str();
}

//---

StatisticsRecorder::StatisticsRecorder(cStatistic *stat)
{
    statistic = stat;
    statistic->removeFromOwnershipTree();
}

StatisticsRecorder::~StatisticsRecorder()
{
    delete statistic;
}

void StatisticsRecorder::collect(double value)
{
    statistic->collect(value);
}

void StatisticsRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    statistic->collect(value);
}

void StatisticsRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordStatistic(getComponent(), getResultName().c_str(), statistic, &attributes);
}

std::string StatisticsRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << ": " << getStatistic()->info();
    return os.str();
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
    RecValueVariable(ExpressionRecorder *recorder) { owner = recorder; }
    virtual Functor *dup() const override { return new RecValueVariable(owner); }
    virtual const char *getName() const override { return "<lastsignalvalue>"; }
    virtual char getReturnType() const override { return Expression::Value::DBL; }
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) override { return owner->lastInputValue; }
};

//XXX currently unused
class RecTimeVariable : public Expression::Variable
{
  public:
    virtual Functor *dup() const override { return new RecTimeVariable(); }
    virtual const char *getName() const override { return "<simtime>"; }
    virtual char getReturnType() const override { return Expression::Value::DBL; }
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) override { return SIMTIME_DBL(getSimulation()->getSimTime()); }
};

Expression::Functor *ExpressionRecorder::makeValueVariable()
{
    return new RecValueVariable(this);
}

Expression::Functor *ExpressionRecorder::makeTimeVariable()
{
    return new RecTimeVariable();
}

double ExpressionRecorder::getCurrentValue() const
{
    return expr.doubleValue();
}

void ExpressionRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordScalar(getComponent(), getResultName().c_str(), expr.doubleValue(), &attributes);
}

std::string ExpressionRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << " = " << getCurrentValue();
    return os.str();
}

}  // namespace omnetpp

