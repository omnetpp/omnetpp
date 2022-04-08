//==========================================================================
//  RESULTRECORDERS.H - part of
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

#ifndef __OMNETPP_RESULTRECORDERS_H
#define __OMNETPP_RESULTRECORDERS_H

#include <cmath>  // INFINITY, NAN
#include "omnetpp/cresultrecorder.h"

namespace omnetpp {

class cStatistic;

/**
 * @addtogroup ResultFiltersRecorders
 * @{
 */

/**
 * @brief Listener for recording a signal to an output vector
 */
class SIM_API VectorRecorder : public cNumericResultRecorder
{
    protected:
        void *handle;        // identifies output vector for the output vector manager
        simtime_t lastTime;  // to ensure increasing timestamp order
        double lastValue;    // for getCurrentValue()
    protected:
        virtual void subscribedTo(cResultFilter *prev) override;
        virtual void collect(simtime_t_cref t, double value, cObject *details) override;
    public:
        VectorRecorder() {handle = nullptr; lastTime = 0; lastValue = NAN;}
        virtual ~VectorRecorder();
        virtual simtime_t getLastWriteTime() const {return lastTime;}
        virtual double getLastValue() const {return lastValue;}
        virtual std::string str() const override;
};

/**
 * @brief Listener for recording the count of signal values, including NaN and nullptr.
 */
class SIM_API TotalCountRecorder : public cResultRecorder
{
    protected:
        long count;
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t l, cObject *details) override {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t l, cObject *details) override {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override {if (!std::isnan(d)) count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override {if (s) count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override {if (obj) count++;}
        virtual void finish(cResultFilter *prev) override;
    public:
        TotalCountRecorder() {count = 0;}
        long getCount() const {return count;}
        virtual std::string str() const override;
};

/**
 * @brief Listener for recording the count of signal values. Signal values do not need
 * to be numeric to be counted. NaN and nullptr values are ignored.
 */
class SIM_API CountRecorder : public TotalCountRecorder
{
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override {if (!std::isnan(d)) count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override {if (s) count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override {if (obj) count++;}
    public:
        CountRecorder() {}
};

/**
 * @brief Listener for recording the last non-NaN signal value.
 */
class SIM_API LastValueRecorder : public cNumericResultRecorder
{
    protected:
        double lastValue;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override;
        virtual void finish(cResultFilter *prev) override;
    public:
        LastValueRecorder() {lastValue = NAN;}
        double getLastValue() const {return lastValue;}
        virtual std::string str() const override;
};

/**
 * @brief Recorder that raises a runtime error if it sees a NaN in the input
 * (and never records anything).
 */
class SIM_API ErrorNanRecorder : public cNumericResultRecorder
{
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override;
    public:
        ErrorNanRecorder() {}
};

/**
 * @brief Listener for recording the sum of signal values.
 * NaN values in the input are ignored.
 */
class SIM_API SumRecorder : public cNumericResultRecorder
{
    protected:
        double sum;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override;
        virtual void finish(cResultFilter *prev) override;
    public:
        SumRecorder() {sum = 0;}
        double getSum() const {return sum;}
        virtual std::string str() const override;
};

/**
 * @brief Listener for recording the (time-weighted or unweighted) mean of
 * signal values. NaN values in the input are ignored, or in the time-weighted
 * case, they denote intervals to be ignored.
 */
class SIM_API MeanRecorder : public cNumericResultRecorder
{
    protected:
        bool timeWeighted = false;
        long count = 0;
        double lastValue = NAN;
        simtime_t lastTime = SIMTIME_ZERO;
        double weightedSum = 0;
        simtime_t totalTime = SIMTIME_ZERO;
    protected:
        virtual void init(Context *ctx) override;
        virtual void finish(cResultFilter *prev) override;
    public:
        MeanRecorder() {}
        virtual void collect(simtime_t_cref t, double value, cObject *details) override;
        double getMean() const;
        virtual std::string str() const override;
};

/**
 * @brief Listener for recording the minimum of signal values.
 * NaN values in the input are ignored.
 */
class SIM_API MinRecorder : public cNumericResultRecorder
{
    protected:
        double min;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override;
        virtual void finish(cResultFilter *prev) override;
    public:
        MinRecorder() {min = INFINITY;}
        double getMin() const {return min;}
        virtual std::string str() const override;
};

/**
 * @brief Listener for recording the maximum of signal values.
 * NaN values in the input are ignored.
 */
class SIM_API MaxRecorder : public cNumericResultRecorder
{
    protected:
        double max;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override;
        virtual void finish(cResultFilter *prev) override;
    public:
        MaxRecorder() {max = -INFINITY;}
        double getMax() const {return max;}
        virtual std::string str() const override;
};

/**
 * @brief Listener for recording the arithmetic mean of signal values.
 * NaN values in the input are ignored.
 */
class SIM_API AverageRecorder : public cNumericResultRecorder
{
    protected:
        long count;
        double sum;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override;
        virtual void finish(cResultFilter *prev) override;
    public:
        AverageRecorder() {count = 0; sum = 0;}
        double getAverage() const {return sum/count;}
        virtual std::string str() const override;
};

/**
 * @brief Listener for recording the time average of signal values.
 * NaN values in the input denote intervals to be ignored.
 */
class SIM_API TimeAverageRecorder : public cNumericResultRecorder
{
    protected:
        double lastValue = NAN;
        simtime_t lastTime = SIMTIME_ZERO;
        double weightedSum = 0;
        simtime_t totalTime = SIMTIME_ZERO;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override;
        virtual void finish(cResultFilter *prev) override;
    public:
        TimeAverageRecorder() {}
        double getTimeAverage() const;
        virtual std::string str() const override;
};

/**
 * @brief Listener for recording signal values via a cStatistic.
 * NaN values in the input are ignored, or in the time-weighted
 * case, they denote intervals to be ignored.
 */
class SIM_API StatisticsRecorder : public cNumericResultRecorder
{
    protected:
        cStatistic *statistic = nullptr;
        double lastValue = NAN;
        simtime_t lastTime = SIMTIME_ZERO;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override;
        virtual void finish(cResultFilter *prev) override;
        virtual void forEachChild(cVisitor *v) override;
    public:
        StatisticsRecorder() {}
        ~StatisticsRecorder();
        virtual void setStatistic(cStatistic* stat);
        virtual cStatistic *getStatistic() const {return statistic;}
        virtual std::string str() const override;
};

class SIM_API StatsRecorder : public StatisticsRecorder
{
    public:
        virtual void init(Context *ctx) override;
};

class SIM_API HistogramRecorder : public StatisticsRecorder
{
    public:
        virtual void init(Context *ctx) override;
};

class SIM_API TimeWeightedHistogramRecorder : public StatisticsRecorder
{
    public:
        virtual void init(Context *ctx) override;
};

class SIM_API PSquareRecorder : public StatisticsRecorder
{
    public:
        virtual void init(Context *ctx) override;
};

class SIM_API KSplitRecorder : public StatisticsRecorder
{
    public:
        virtual void init(Context *ctx) override;
};

/** @} */

}  // namespace omnetpp

#endif
