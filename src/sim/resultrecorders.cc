//==========================================================================
//  RESULTRECORDERS.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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
#include "omnetpp/resultrecorders.h"
#include "common/stringutil.h"

namespace omnetpp {

using namespace omnetpp::common;

#define SIGNALTYPE_TO_NUMERIC_CONVERSIONS \
        "Numeric types are all converted to double, and boolean to 0 and 1. " \
        "Other non-numeric types will cause an error. "

#define NAN_VALUES_IGNORED \
        "NaN values in the input are ignored. "

#define OPTIONALLY_TIMEWEIGHTED \
        "NaN values in the input are ignored, or in the time-weighted case, " \
        "they indicate that the interval up to the next value is to be ignored. " \
        "To turn on time-weighted, specify 'timeWeighted=true' in the @statistic property."

Register_ResultRecorder2("vector", VectorRecorder,
        "Records the input values to an output vector. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
);
Register_ResultRecorder2("totalCount", TotalCountRecorder,
        "Records the count of the input values, including NaN and nullptr values. "
        "Signal values do not need to be numeric to be counted. "
);
Register_ResultRecorder2("count", CountRecorder,
        "Records the count of the input values. "
        "Signal values do not need to be numeric to be counted. "
        "NaN and nullptr values are ignored."
);
Register_ResultRecorder2("last", LastValueRecorder,
        "Records the last input value. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        NAN_VALUES_IGNORED
);
Register_ResultRecorder2("errorNan", ErrorNanRecorder,
        "Raises a runtime error if it encounters a NaN value in the input. "
        "Tip: If you want to verify that statistics in a simulation do not "
        "accidentally produce NaN values, add the following entry to the "
        "configuration: '**.result-recording-modes = +errorNan' "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
);
Register_ResultRecorder2("sum", SumRecorder,
        "Records the sum of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        NAN_VALUES_IGNORED
);
Register_ResultRecorder2("mean", MeanRecorder,
        "Records the (time-weighted or unweighted) mean of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        OPTIONALLY_TIMEWEIGHTED
);
Register_ResultRecorder2("min", MinRecorder,
        "Records the minimum of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        NAN_VALUES_IGNORED
);
Register_ResultRecorder2("max", MaxRecorder,
        "Records the maximum of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        NAN_VALUES_IGNORED
);
Register_ResultRecorder2("avg", AverageRecorder,
        "Records the arithmetic mean of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        NAN_VALUES_IGNORED
);
Register_ResultRecorder2("timeavg", TimeAverageRecorder,
        "Records the time average of the input values, assuming sample-hold interpolation. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        "A NaN value in the input indicates that the interval up to the next value is to be ignored. "
);
Register_ResultRecorder2("stats", StatsRecorder,
        "Records basic statistics (count, mean, stddev, min, max, etc.) of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        OPTIONALLY_TIMEWEIGHTED
);
Register_ResultRecorder2("histogram", HistogramRecorder,
        "Records the histogram of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        OPTIONALLY_TIMEWEIGHTED
);
Register_ResultRecorder2("timeWeightedHistogram", TimeWeightedHistogramRecorder,
        "Records the time-weighted histogram of the input values. "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        "A NaN value in the input indicates that the interval up to the next value is to be ignored. "
);
Register_ResultRecorder2("psquare", PSquareRecorder,
        "Records the histogram of the input values using the P^2 algorithm (cPSquare class). "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        OPTIONALLY_TIMEWEIGHTED
);
Register_ResultRecorder2("ksplit", KSplitRecorder,
        "Records the histogram of the input values using the k-split algorithm (cKSplit class). "
        SIGNALTYPE_TO_NUMERIC_CONVERSIONS
        OPTIONALLY_TIMEWEIGHTED
);

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

void TotalCountRecorder::finish(cResultFilter *prev)
{
    opp_string_map attributes = getStatisticAttributes();
    getEnvir()->recordScalar(getComponent(), getResultName().c_str(), count, &attributes);
}

std::string TotalCountRecorder::str() const
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

void ErrorNanRecorder::collect(simtime_t_cref t, double value, cObject *details)
{
    if (std::isnan(value))
        throw cRuntimeError("errorNan: NaN value detected in result '%s'", getResultName().c_str());
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

void MeanRecorder::init(Context *ctx)
{
    cNumericResultRecorder::init(ctx);

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

void StatsRecorder::init(Context *ctx)
{
    StatisticsRecorder::init(ctx);
    bool weighted = getBoolAttr(getStatisticAttributes(), "timeWeighted", false);
    setStatistic(new cStdDev("statistic", weighted));
}

void HistogramRecorder::init(Context *ctx)
{
    StatisticsRecorder::init(ctx);

    omnetpp::opp_string_map attrs = getStatisticAttributes();
    bool weighted = getBoolAttr(getStatisticAttributes(), "timeWeighted", false);
    int numBins = getIntAttr(getStatisticAttributes(), "numBins", -1);
    if (numBins == -1)
        setStatistic(new cHistogram("histogram", weighted));
    else
        setStatistic(new cHistogram("histogram", numBins, weighted));
}

void TimeWeightedHistogramRecorder::init(Context *ctx)
{
    StatisticsRecorder::init(ctx);

    omnetpp::opp_string_map attrs = getStatisticAttributes();
    int numBins = getIntAttr(getStatisticAttributes(), "numBins", -1);
    if (numBins == -1)
        setStatistic(new cHistogram("histogram", true));
    else
        setStatistic(new cHistogram("histogram", numBins, true));
}

void PSquareRecorder::init(Context *ctx)
{
    StatisticsRecorder::init(ctx);
    bool weighted = getBoolAttr(getStatisticAttributes(), "timeWeighted", false);
    if (weighted)
        throw cRuntimeError("%s: cPSquare does not support weighted statistics", getClassName());
    int numBins = getIntAttr(getStatisticAttributes(), "numBins", -1);
    if (numBins == -1)
        setStatistic(new cPSquare("psquare"));
    else
        setStatistic(new cPSquare("psquare", numBins));
}

void KSplitRecorder::init(Context *ctx)
{
    StatisticsRecorder::init(ctx);
    bool weighted = getBoolAttr(getStatisticAttributes(), "timeWeighted", false);
    if (weighted)
        throw cRuntimeError("%s: cKSplit does not support weighted statistics", getClassName());
    setStatistic(new cKSplit("ksplit"));
}

}  // namespace omnetpp

