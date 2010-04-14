//==========================================================================
//  RESULTFILTERS.H - part of
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

#ifndef __ENVIR_RESULTFILTERS_H
#define __ENVIR_RESULTFILTERS_H

#include "envirdefs.h"
#include "resultfilter.h"
#include "csimulation.h"
#include "cstatistic.h"
#include "expression.h"


class ENVIR_API WarmupPeriodFilter : public ResultFilter
{
    private:
        simtime_t_cref getEndWarmupPeriod() {return simulation.getWarmupPeriod();}
    public:
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, long l);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, unsigned long l);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, double d);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const SimTime& v);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const char *s);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, cObject *obj);
};

/**
 * Result filter for counting signals. Signal values do not need to be numeric
 * to be counted.
 */
class ENVIR_API CountFilter : public ResultFilter
{
    protected:
        long count;
    public:
        CountFilter() {count = 0;}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, long l) {count++; fire(this,t,count);}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, unsigned long l) {count++; fire(this,t,count);}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, double d) {count++; fire(this,t,count);}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const SimTime& v) {count++; fire(this,t,count);}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const char *s) {count++; fire(this,t,count);}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, cObject *obj) {count++; fire(this,t,count);}
};

/**
 * Result filter that replaces every value with a constant. Signal values
 * do not need to be numeric.
 */
class ENVIR_API ConstantFilter : public ResultFilter
{
    protected:
        double c;
    public:
        ConstantFilter(double c) {this->c = c;}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, long l) {fire(this,t,c);}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, unsigned long l) {fire(this,t,c);}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, double d) {fire(this,t,c);}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const SimTime& v) {fire(this,t,c);}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const char *s) {fire(this,t,c);}
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, cObject *obj) {fire(this,t,c);}
};

/**
 * Result filter that replaces every value with zero.
 */
class ENVIR_API Constant0Filter : public ConstantFilter
{
    public:
        Constant0Filter() : ConstantFilter(0) {}
};

/**
 * Result filter that replaces every value with 1.0.
 */
class ENVIR_API Constant1Filter : public ConstantFilter
{
    public:
        Constant1Filter() : ConstantFilter(1) {}
};

/**
 * This class is actually a no-op; provided for symmetry with LastValueRecorder.
 */
class ENVIR_API LastValueFilter : public NumericResultFilter
{
    protected:
        virtual bool process(simtime_t& t, double& value) {return true;}
};

/**
 * Filter that outputs the sum of signal values.
 */
class ENVIR_API SumFilter : public NumericResultFilter
{
    protected:
        double sum;
    protected:
        virtual bool process(simtime_t& t, double& value) {sum += value; value = sum; return true;}
    public:
        SumFilter() {sum = 0;}
};

/**
 * Result filter that computes the mean of signal values
 */
class ENVIR_API MeanFilter : public NumericResultFilter
{
    protected:
        long count;
        double sum;
    protected:
        virtual bool process(simtime_t& t, double& value) {count++; sum += value; value = sum/count; return true;}
    public:
        MeanFilter() {count = 0; sum = 0;}
};

/**
 * Result filter that computes the minimum of signal values
 */
class ENVIR_API MinFilter : public NumericResultFilter
{
    protected:
        double min;
    protected:
        virtual bool process(simtime_t& t, double& value) {if (value < min) min = value; value = min; return true;}
    public:
        MinFilter() {min = POSITIVE_INFINITY;}
};

/**
 * Result filter that computes the maximum of signal values
 */
class ENVIR_API MaxFilter : public NumericResultFilter
{
    protected:
        double max;
    protected:
        virtual bool process(simtime_t& t, double& value) {if (value > max) max = value; value = max; return true;}
    public:
        MaxFilter() {max = NEGATIVE_INFINITY;}
};

/**
 * Result filter that computes the time average of signal values
 */
class ENVIR_API TimeAverageFilter : public NumericResultFilter
{
    protected:
        simtime_t startTime;
        simtime_t lastTime;
        double lastValue;
        double weightedSum;
    protected:
        virtual bool process(simtime_t& t, double& value);
    public:
        TimeAverageFilter() {startTime = lastTime = -1; lastValue = weightedSum = 0;}
};

/**
 * Result filter that computes an expression from signal values
 */
class ENVIR_API ExpressionFilter : public NumericResultFilter
{
    protected:
        Expression expr;
    public:
        // current values, valid inside process() only
        simtime_t currentTime;
        double currentValue;
    protected:
        virtual bool process(simtime_t& t, double& value);
    public:
        ExpressionFilter() {}
        virtual std::string str() const {return expr.str()+" (ExpressionFilter)";}
        Expression& getExpression() {return expr;}
        Expression::Functor *makeValueVariable();
        Expression::Functor *makeTimeVariable();
};

/**
 * Filter that expects a cPacket and outputs its length in bytes (getByteLength()).
 */
class ENVIR_API PacketBytesFilter : public ObjectResultFilter
{
    public:
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, cObject *object);
};

/**
 * Filter that expects a cPacket and outputs its length in bits (getBitLength()).
 */
class ENVIR_API PacketBitsFilter : public ObjectResultFilter
{
    public:
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, cObject *object);
};

/**
 * Filter that outputs the sum of signal values divided by the measurement
 * interval (simtime minus warmupPeriod)
 */
class ENVIR_API SumPerDurationFilter : public NumericResultFilter
{
    protected:
        double sum;
    protected:
        virtual bool process(simtime_t& t, double& value);
    public:
        SumPerDurationFilter() {sum = 0;}
};

#endif


