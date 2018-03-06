//==========================================================================
//  RESULTRECORDERS.CC - part of
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

#include "omnetpp/cproperty.h"
#include "omnetpp/chistogram.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cpsquare.h"
#include "omnetpp/cksplit.h"
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
Register_ResultRecorder("avg", AverageRecorder);
Register_ResultRecorder("timeavg", TimeAverageRecorder);
Register_ResultRecorder("stats", StatsRecorder);
Register_ResultRecorder("histogram", HistogramRecorder);
Register_ResultRecorder("timeWeightedHistogram", TimeWeightedHistogramRecorder);
Register_ResultRecorder("psquare", PSquareRecorder);
Register_ResultRecorder("ksplit", KSplitRecorder);

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
    for (auto & attribute : attributes)
        getEnvir()->setVectorAttribute(handle, attribute.first.c_str(), attribute.second.c_str());
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

void LastValueRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    if (!std::isnan(value))
        lastValue = value;
}

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

void SumRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    if (!std::isnan(value))
        sum += value;
}


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

void MeanRecorder::init(cComponent *component, const char *statsName, const char *recordingMode, cProperty *attrsProperty, opp_string_map *manualAttrs)
{
    cNumericResultRecorder::init(component, statsName, recordingMode, attrsProperty, manualAttrs);

    omnetpp::opp_string_map attrs = getStatisticAttributes();
    auto it = attrs.find("timeWeighted");
    timeWeighted = it != attrs.end() && (it->second != "0" && it->second != "false");
}

void MeanRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    if (!timeWeighted) {
        if (!std::isnan(value)) {
            count++;
            weightedSum += value;
        }
    }
    else {
        if (!std::isnan(lastValue)) {
            totalTime += t - lastTime;
            weightedSum += lastValue * SIMTIME_DBL(t - lastTime);
        }
        lastTime = t;
        lastValue = value;
    }
}

std::string MeanRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << " = " << getMean();
    return os.str();
}

double MeanRecorder::getMean() const
{
    if (!timeWeighted) {
        return weightedSum / count;
    }
    else {
        simtime_t tmpTotalTime = totalTime;
        double tmpWeightedSum = weightedSum;

        if (!std::isnan(lastValue)) {
            simtime_t t = getSimulation()->getSimTime();
            tmpTotalTime += t - lastTime;
            tmpWeightedSum += lastValue * SIMTIME_DBL(t - lastTime);
        }
        return tmpWeightedSum / tmpTotalTime;
    }
}

void MeanRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordScalar(getComponent(), getResultName().c_str(), getMean(), &attributes);
}

//---


void MinRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    if (!std::isnan(value)) {
        if (value < min)
            min = value;
    }
}

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

void MaxRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    if (!std::isnan(value)) {
        if (value > max)
            max = value;
    }
}

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

void AverageRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    if (!std::isnan(value)) {
        count++;
        sum += value;
    }
}

void AverageRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordScalar(getComponent(), getResultName().c_str(), sum / count, &attributes);  // note: this is NaN if count==0
}

std::string AverageRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << " = " << getAverage();
    return os.str();
}

//---

void TimeAverageRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    if (!std::isnan(lastValue)) {
        totalTime += t - lastTime;
        weightedSum += lastValue * SIMTIME_DBL(t - lastTime);
    }
    lastTime = t;
    lastValue = value;
}

double TimeAverageRecorder::getTimeAverage() const
{
    simtime_t tmpTotalTime = totalTime;
    double tmpWeightedSum = weightedSum;

    if (!std::isnan(lastValue)) {
        simtime_t t = getSimulation()->getSimTime();
        tmpTotalTime += t - lastTime;
        tmpWeightedSum += lastValue * SIMTIME_DBL(t - lastTime);
    }
    return tmpWeightedSum / tmpTotalTime;
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

StatisticsRecorder::StatisticsRecorder()
{
}

StatisticsRecorder::~StatisticsRecorder()
{
    dropAndDelete(statistic);
}

void StatisticsRecorder::forEachChild(cVisitor *v)
{
    v->visit(statistic);
    cNumericResultRecorder::forEachChild(v);
}

void StatisticsRecorder::setStatistic(cStatistic *stat)
{
    ASSERT(statistic == nullptr);
    statistic = stat;
    take(statistic);
}

void StatisticsRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    if (!statistic->isWeighted()) {
        if (!std::isnan(value))
            statistic->collect(value);
    }
    else {
        if (!std::isnan(lastValue))
            statistic->collectWeighted(lastValue, t - lastTime);
        lastTime = t;
        lastValue = value;
    }
}

void StatisticsRecorder::finish(cResultFilter *prev)
{
    if (statistic->isWeighted() && !std::isnan(lastValue))
        statistic->collectWeighted(lastValue, simTime() - lastTime);

    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordStatistic(getComponent(), getResultName().c_str(), statistic, &attributes);
}

std::string StatisticsRecorder::str() const
{
    std::stringstream os;
    os << getResultName() << ": " << getStatistic()->str();
    return os.str();
}

inline bool getBoolAttr(const opp_string_map& attrs, const char *name, bool defaultValue)
{
    auto it = attrs.find(name);
    return it == attrs.end() ? defaultValue : (it->second != "0" && it->second != "false");
}

inline int getIntAttr(const opp_string_map& attrs, const char *name, int defaultValue)
{
    auto it = attrs.find(name);
    return it == attrs.end() ? defaultValue : opp_atol(it->second.c_str());
}

void StatsRecorder::init(cComponent *component, const char *statsName, const char *recordingMode, cProperty *attrsProperty, opp_string_map *manualAttrs)
{
    StatisticsRecorder::init(component, statsName, recordingMode, attrsProperty, manualAttrs);
    bool weighted = getBoolAttr(getStatisticAttributes(), "timeWeighted", false);
    setStatistic(new cStdDev("statistic", weighted));
}

void HistogramRecorder::init(cComponent *component, const char *statsName, const char *recordingMode, cProperty *attrsProperty, opp_string_map *manualAttrs)
{
    StatisticsRecorder::init(component, statsName, recordingMode, attrsProperty, manualAttrs);

    omnetpp::opp_string_map attrs = getStatisticAttributes();
    bool weighted = getBoolAttr(getStatisticAttributes(), "timeWeighted", false);
    int numBins = getIntAttr(getStatisticAttributes(), "numBins", -1);
    if (numBins == -1)
        setStatistic(new cHistogram("histogram", weighted));
    else
        setStatistic(new cHistogram("histogram", numBins, weighted));
}

void TimeWeightedHistogramRecorder::init(cComponent *component, const char *statsName, const char *recordingMode, cProperty *attrsProperty, opp_string_map *manualAttrs)
{
    StatisticsRecorder::init(component, statsName, recordingMode, attrsProperty, manualAttrs);

    omnetpp::opp_string_map attrs = getStatisticAttributes();
    int numBins = getIntAttr(getStatisticAttributes(), "numBins", -1);
    if (numBins == -1)
        setStatistic(new cHistogram("histogram", true));
    else
        setStatistic(new cHistogram("histogram", numBins, true));
}

void PSquareRecorder::init(cComponent *component, const char *statsName, const char *recordingMode, cProperty *attrsProperty, opp_string_map *manualAttrs)
{
    StatisticsRecorder::init(component, statsName, recordingMode, attrsProperty, manualAttrs);
    bool weighted = getBoolAttr(getStatisticAttributes(), "timeWeighted", false);
    if (weighted)
        throw cRuntimeError("%s: cPSquare does not support weighted statistics", getClassName());
    int numBins = getIntAttr(getStatisticAttributes(), "numBins", -1);
    if (numBins == -1)
        setStatistic(new cPSquare("psquare"));
    else
        setStatistic(new cPSquare("psquare", numBins));
}

void KSplitRecorder::init(cComponent *component, const char *statsName, const char *recordingMode, cProperty *attrsProperty, opp_string_map *manualAttrs)
{
    StatisticsRecorder::init(component, statsName, recordingMode, attrsProperty, manualAttrs);
    bool weighted = getBoolAttr(getStatisticAttributes(), "timeWeighted", false);
    if (weighted)
        throw cRuntimeError("%s: cKSplit does not support weighted statistics", getClassName());
    setStatistic(new cKSplit("ksplit"));
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

