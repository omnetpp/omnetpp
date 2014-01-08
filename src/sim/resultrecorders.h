//==========================================================================
//  RESULTRECORDERS.H - part of
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

#ifndef __RESULTRECORDERS_H
#define __RESULTRECORDERS_H

#include "cresultrecorder.h"
#include "expression.h"
#include "cstatistic.h"

NAMESPACE_BEGIN

/**
 * Listener for recording a signal to an output vector
 */
class SIM_API VectorRecorder : public cNumericResultRecorder
{
    protected:
        void *handle;        // identifies output vector for the output vector manager
        simtime_t lastTime;  // to ensure increasing timestamp order
    protected:
        virtual void collect(simtime_t_cref t, double value);
    public:
        VectorRecorder() {handle = NULL; lastTime = 0;}
        virtual ~VectorRecorder();
        virtual void subscribedTo(cResultFilter *prev);
};

/**
 * Listener for recording the count of signal values. Signal values do not need
 * to be numeric to be counted.
 */
class SIM_API CountRecorder : public cResultRecorder
{
    protected:
        long count;
    public:
        CountRecorder() {count = 0;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b) {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, long l) {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l) {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d) {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v) {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s) {count++;}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj) {count++;}
        virtual void finish(cResultFilter *prev);
};

/**
 * Listener for recording the last signal value
 */
class SIM_API LastValueRecorder : public cNumericResultRecorder
{
    protected:
        double lastValue;
    protected:
        virtual void collect(simtime_t_cref t, double value) {lastValue = value;}
    public:
        LastValueRecorder() {lastValue = NaN;}
        virtual void finish(cResultFilter *prev);
};

/**
 * Listener for recording the sum of signal values
 */
class SIM_API SumRecorder : public cNumericResultRecorder
{
    protected:
        double sum;
    protected:
        virtual void collect(simtime_t_cref t, double value) {sum += value;}
    public:
        SumRecorder() {sum = 0;}
        virtual void finish(cResultFilter *prev);
};

/**
 * Listener for recording the mean of signal values
 */
class SIM_API MeanRecorder : public cNumericResultRecorder
{
    protected:
        long count;
        double sum;
    protected:
        virtual void collect(simtime_t_cref t, double value) {count++; sum += value;}
    public:
        MeanRecorder() {count = 0; sum = 0;}
        virtual void finish(cResultFilter *prev);
};

/**
 * Listener for recording the minimum of signal values
 */
class SIM_API MinRecorder : public cNumericResultRecorder
{
    protected:
        double min;
    protected:
        virtual void collect(simtime_t_cref t, double value) {if (value < min) min = value;}
    public:
        MinRecorder() {min = POSITIVE_INFINITY;}
        virtual void finish(cResultFilter *prev);
};

/**
 * Listener for recording the maximum of signal values
 */
class SIM_API MaxRecorder : public cNumericResultRecorder
{
    protected:
        double max;
    protected:
        virtual void collect(simtime_t_cref t, double value) {if (value > max) max = value;}
    public:
        MaxRecorder() {max = NEGATIVE_INFINITY;}
        virtual void finish(cResultFilter *prev);
};

/**
 * Listener for recording the time average of signal values
 */
class SIM_API TimeAverageRecorder : public cNumericResultRecorder
{
    protected:
        simtime_t startTime;
        simtime_t lastTime;
        double lastValue;
        double weightedSum;
    protected:
        virtual void collect(simtime_t_cref t, double value);
    public:
        TimeAverageRecorder() {startTime = lastTime = -1; lastValue = weightedSum = 0;}
        virtual void finish(cResultFilter *prev);
};

/**
 * Listener for recording signal values via a cStatistic
 */
class SIM_API StatisticsRecorder : public cNumericResultRecorder, private cObject /*so it can own the statistic object*/
{
    protected:
        cStatistic *statistic;
    protected:
        virtual void collect(double value) {statistic->collect(value);}
        virtual void collect(simtime_t_cref t, double value) {statistic->collect(value);}
    public:
        StatisticsRecorder(cStatistic *stat) {statistic = stat; take(statistic);}
        ~StatisticsRecorder() {drop(statistic); delete statistic;}
        virtual void finish(cResultFilter *prev);
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
 * Result recorder that remembers the last value of a signal,
 * and in finish() it evaluates an expression and saves the result.
 */
//TODO: make variants that eat 2 signals, and N signals
class SIM_API ExpressionRecorder : public cNumericResultRecorder
{
    protected:
        Expression expr;
    public:
        // current values, valid inside process() only
        double lastValue;
    protected:
        virtual void collect(simtime_t_cref t, double value) {lastValue = value;}
    public:
        ExpressionRecorder() {lastValue=NaN;}
        virtual std::string str() const {return expr.str()+" (ExpressionRecorder)";}
        virtual Expression& getExpression() {return expr;}
        virtual Expression::Functor *makeValueVariable();
        virtual Expression::Functor *makeTimeVariable();
        virtual void finish(cResultFilter *prev);
};

NAMESPACE_END

#endif

