//==========================================================================
//  RESULTRECORDERS.H - part of
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

#ifndef __OMNETPP_RESULTRECORDERS_H
#define __OMNETPP_RESULTRECORDERS_H

#include "common/expression.h"
#include "omnetpp/cresultrecorder.h"
#include "omnetpp/cstatistic.h"

namespace omnetpp {

using omnetpp::common::NaN;
using omnetpp::common::POSITIVE_INFINITY;
using omnetpp::common::NEGATIVE_INFINITY;

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
        VectorRecorder() {handle = nullptr; lastTime = 0; lastValue = NaN;}
        virtual ~VectorRecorder();
        virtual simtime_t getLastWriteTime() const {return lastTime;}
        virtual double getLastValue() const {return lastValue;}
};

/**
 * @brief Listener for recording the count of signal values. Signal values do not need
 * to be numeric to be counted.
 */
class SIM_API CountRecorder : public cResultRecorder
{
    protected:
        long count;
    public:
        CountRecorder() {count = 0;}
        long getCount() const {return count;}
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details) override {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details) override {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override {count++;}
        virtual void finish(cResultFilter *prev) override;
};

/**
 * @brief Listener for recording the last signal value
 */
class SIM_API LastValueRecorder : public cNumericResultRecorder
{
    protected:
        double lastValue;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override {lastValue = value;}
        virtual void finish(cResultFilter *prev) override;
    public:
        LastValueRecorder() {lastValue = NaN;}
        double getLastValue() const {return lastValue;}
};

/**
 * @brief Listener for recording the sum of signal values
 */
class SIM_API SumRecorder : public cNumericResultRecorder
{
    protected:
        double sum;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override {sum += value;}
        virtual void finish(cResultFilter *prev) override;
    public:
        SumRecorder() {sum = 0;}
        double getSum() const {return sum;}
};

/**
 * @brief Listener for recording the mean of signal values
 */
class SIM_API MeanRecorder : public cNumericResultRecorder
{
    protected:
        long count;
        double sum;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override {count++; sum += value;}
        virtual void finish(cResultFilter *prev) override;
    public:
        MeanRecorder() {count = 0; sum = 0;}
        double getMean() const {return sum/count;}
};

/**
 * @brief Listener for recording the minimum of signal values
 */
class SIM_API MinRecorder : public cNumericResultRecorder
{
    protected:
        double min;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override {if (value < min) min = value;}
        virtual void finish(cResultFilter *prev) override;
    public:
        MinRecorder() {min = POSITIVE_INFINITY;}
        double getMin() const {return min;}
};

/**
 * @brief Listener for recording the maximum of signal values
 */
class SIM_API MaxRecorder : public cNumericResultRecorder
{
    protected:
        double max;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override {if (value > max) max = value;}
        virtual void finish(cResultFilter *prev) override;
    public:
        MaxRecorder() {max = NEGATIVE_INFINITY;}
        double getMax() const {return max;}
};

/**
 * @brief Listener for recording the time average of signal values
 */
class SIM_API TimeAverageRecorder : public cNumericResultRecorder
{
    protected:
        simtime_t startTime;
        simtime_t lastTime;
        double lastValue;
        double weightedSum;
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override;
        virtual void finish(cResultFilter *prev) override;
    public:
        TimeAverageRecorder() {startTime = lastTime = -1; lastValue = weightedSum = 0;}
        double getTimeAverage() const;
};

/**
 * @brief Listener for recording signal values via a cStatistic
 */
class SIM_API StatisticsRecorder : public cNumericResultRecorder, private cObject /*so it can own the statistic object*/
{
    protected:
        cStatistic *statistic;
    protected:
        virtual void collect(double value) {statistic->collect(value);}
        virtual void collect(simtime_t_cref t, double value, cObject *details) override {statistic->collect(value);}
        virtual void finish(cResultFilter *prev) override;
    public:
        StatisticsRecorder(cStatistic *stat) {statistic = stat; take(statistic);}
        ~StatisticsRecorder() {drop(statistic); delete statistic;}
        virtual cStatistic *getStatistic() const {return statistic;}
};

class SIM_API StatsRecorder : public StatisticsRecorder
{
    public:
        StatsRecorder();
};

class SIM_API HistogramRecorder : public StatisticsRecorder
{
    public:
        HistogramRecorder();
};

/**
 * @brief Result recorder that remembers the last value of a signal,
 * and in finish() it evaluates an expression and saves the result.
 */
//TODO: make variants that eat 2 signals, and N signals
class SIM_API ExpressionRecorder : public cNumericResultRecorder
{
    public:
        typedef omnetpp::common::Expression Expression;
    protected:
        Expression expr;
    public:
        double lastValue; // current values, valid inside process() only, only public for technical reasons
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override {lastValue = value;}
        virtual void finish(cResultFilter *prev) override;
    public:
        // internal methods, only public for technical reasons
        virtual Expression::Functor *makeValueVariable();
        virtual Expression::Functor *makeTimeVariable();
    public:
        ExpressionRecorder() {lastValue=NaN;}
        virtual std::string str() const override {return expr.str()+" (ExpressionRecorder)";}
        virtual Expression& getExpression() {return expr;}
        virtual double getCurrentValue() const;
};

}  // namespace omnetpp

#endif

