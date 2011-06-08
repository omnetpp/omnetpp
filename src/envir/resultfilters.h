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

class ENVIR_API ExpressionFilter : public NumericResultFilter
{
    protected:
        class ValueVariable : public Expression::Variable
        {
          private:
            ExpressionFilter *owner;
            double *currentValue;
          public:
            ValueVariable(ExpressionFilter *owner, double *currentValue) {this->owner = owner; this->currentValue = currentValue;}
            virtual Functor *dup() const {return new ValueVariable(owner, currentValue);}
            virtual const char *getName() const {return "<signalvalue>";}
            virtual char getReturnType() const {return Expression::Value::DBL;}
            virtual Expression::Value evaluate(Expression::Value args[], int numargs) {return *currentValue;}
        };
        //XXX currently unused
        class TimeVariable : public Expression::Variable
        {
          private:
            ExpressionFilter *owner;
          public:
            TimeVariable(ExpressionFilter *filter) {owner = filter;}
            virtual Functor *dup() const {return new TimeVariable(owner);}
            virtual const char *getName() const {return "<signaltime>";}
            virtual char getReturnType() const {return Expression::Value::DBL;}
            virtual Expression::Value evaluate(Expression::Value args[], int numargs) {return SIMTIME_DBL(owner->currentTime);}
        };

    protected:
        Expression expr;
        simtime_t currentTime;
    protected:
        virtual bool process(simtime_t& t, double& value) {currentTime = t; value = expr.doubleValue(); return true;}
    public:
        Expression& getExpression() {return expr;}
        virtual Expression::Functor *makeValueVariable(int index, ResultFilter *prevFilter) {throw cRuntimeError("constant expression cannot have variables");};
        Expression::Functor *makeTimeVariable() {return new TimeVariable(this);} // XXX: unused
};

/**
 * Result filter that computes an expression from signal values
 */
class ENVIR_API UnaryExpressionFilter : public ExpressionFilter
{
    protected:
        double currentValue;
    protected:
        virtual bool process(simtime_t& t, double& value);
    public:
        UnaryExpressionFilter() {}
        virtual std::string str() const {return expr.str()+" (UnaryExpressionFilter)";}
        virtual Expression::Functor *makeValueVariable(int index, ResultFilter *prevFilter) {Assert(index == 0); return new ValueVariable(this, &currentValue);}
};

/**
 * Result filter that computes an expression from multiple signal values.
 */
class ENVIR_API NaryExpressionFilter : public ExpressionFilter
{
    protected:
        int signalCount;
        ResultFilter **prevFilters;
        double *currentValues;
    protected:
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, long l);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, unsigned long l);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, double d);
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const SimTime& v);
        virtual bool process(ResultFilter *prev, simtime_t& t, double& value);
        virtual void subscribedTo(ResultFilter *prev) {}
    public:
        NaryExpressionFilter(int signalCount) {this->signalCount = signalCount; prevFilters = new ResultFilter*[signalCount]; currentValues = new double[signalCount];}
        virtual ~NaryExpressionFilter() {delete [] prevFilters; delete [] currentValues; }
        virtual std::string str() const {return expr.str()+" (NaryExpressionFilter)";}
        virtual Expression::Functor *makeValueVariable(int index, ResultFilter *prevFilter);
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


