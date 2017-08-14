//==========================================================================
//  RESULTFILTERS.H - part of
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

#ifndef __OMNETPP_RESULTFILTERS_H
#define __OMNETPP_RESULTFILTERS_H

#include "common/expression.h"
#include "omnetpp/simkerneldefs.h"
#include "omnetpp/cresultfilter.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cstatistic.h"

namespace omnetpp {

using omnetpp::common::NaN;
using omnetpp::common::POSITIVE_INFINITY;
using omnetpp::common::NEGATIVE_INFINITY;

class SIM_API WarmupPeriodFilter : public cResultFilter
{
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override;
    public:
        virtual simtime_t_cref getEndWarmupPeriod() const {return getSimulation()->getWarmupPeriod();}
        virtual std::string str() const override;
};

/**
 * @brief Result filter for counting signals. Signal values do not need to be numeric
 * to be counted. NaN and nullptr values are ignored.
 */
class SIM_API CountFilter : public cResultFilter
{
    protected:
        long count;
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override {count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details) override {count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details) override {count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override {if (!std::isnan(d)) count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override {count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override {if (s) count++; fire(this,t,count,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override {if (obj) count++; fire(this,t,count,details);}
    public:
        CountFilter() {count = 0;}
        long getCount() const {return count;}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that replaces every value with a constant. Signal values
 * do not need to be numeric.
 */
class SIM_API ConstantFilter : public cResultFilter
{
    protected:
        double c;
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details) override {fire(this,t,c,details);}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details) override {fire(this,t,c,details);}
    public:
        ConstantFilter(double c) {this->c = c;}
        double getConstant() const {return c;}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that replaces every value with zero.
 */
class SIM_API Constant0Filter : public ConstantFilter
{
    public:
        Constant0Filter() : ConstantFilter(0) {}
};

/**
 * @brief Result filter that replaces every value with 1.0.
 */
class SIM_API Constant1Filter : public ConstantFilter
{
    public:
        Constant1Filter() : ConstantFilter(1) {}
};

/**
 * @brief This class is a no-op filter.
 */
class SIM_API IdentityFilter : public cNumericResultFilter
{
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override {return true;}
};

/**
 * @brief Filter that removes (filters out) NaNs, and lets through all other values.
 */
class SIM_API SkipNanFilter : public cNumericResultFilter
{
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override {return !std::isnan(value);}
};

/**
 * @brief Filter that outputs the sum of signal values. NaN values in the input are ignored.
 */
class SIM_API SumFilter : public cNumericResultFilter
{
    protected:
        double sum;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        SumFilter() {sum = 0;}
        double getSum() const {return sum;}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that computes the (time-weighted or unweighted) mean
 * of signal values. NaN values in the input are ignored, or in the time-weighted
 * case, they denote intervals to be ignored.
 */
class SIM_API MeanFilter : public cNumericResultFilter
{
    protected:
        bool timeWeighted = false;
        long count = 0;
        double lastValue = NaN;
        simtime_t lastTime = SIMTIME_ZERO;
        double weightedSum = 0;
        simtime_t totalTime = SIMTIME_ZERO;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        MeanFilter() {}
        virtual void init(cComponent *component, cProperty *attrsProperty) override;
        double getMean() const;
        virtual std::string str() const override;
};

/**
 * @brief Result filter that computes the minimum of signal values.
 * NaN values in the input are ignored.
 */
class SIM_API MinFilter : public cNumericResultFilter
{
    protected:
        double min;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        MinFilter() {min = POSITIVE_INFINITY;}
        double getMin() const {return min;}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that computes the maximum of signal values.
 * NaN values in the input are ignored.
 */
class SIM_API MaxFilter : public cNumericResultFilter
{
    protected:
        double max;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        MaxFilter() {max = NEGATIVE_INFINITY;}
        double getMax() const {return max;}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that computes the arithmetic mean of signal values.
 * NaN values in the input are ignored.
 */
class SIM_API AverageFilter : public cNumericResultFilter
{
    protected:
        long count;
        double sum;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        AverageFilter() {count = 0; sum = 0;}
        double getAverage() const {return sum/count;}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that computes the time average of signal values.
 * NaN values in the input denote intervals to be ignored.
 */
class SIM_API TimeAverageFilter : public cNumericResultFilter
{
    protected:
        double lastValue = NaN;
        simtime_t lastTime = SIMTIME_ZERO;
        double weightedSum = 0;
        simtime_t totalTime = SIMTIME_ZERO;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        TimeAverageFilter() {}
        double getTimeAverage() const;
        virtual std::string str() const override;
};

/**
 * @brief Result filter that removes repeated values
 */
class SIM_API RemoveRepeatsFilter : public cNumericResultFilter
{
    protected:
        double prev;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override {bool repeated = (value==prev); prev = value; return !repeated;}
    public:
        RemoveRepeatsFilter() {prev = NaN;}
        double getLastValue() const {return prev;}
        virtual std::string str() const override;
};

class SIM_API ExpressionFilter : public cNumericResultFilter
{
    public:
        typedef omnetpp::common::Expression Expression;
    protected:
        class ValueVariable : public Expression::Variable
        {
          private:
            ExpressionFilter *owner;
            double *currentValue;
          public:
            ValueVariable(ExpressionFilter *owner, double *currentValue) {this->owner = owner; this->currentValue = currentValue;}
            virtual Functor *dup() const override {return new ValueVariable(owner, currentValue);}
            virtual const char *getName() const override {return "<signalvalue>";}
            virtual char getReturnType() const override {return Expression::Value::DBL;}
            virtual Expression::Value evaluate(Expression::Value args[], int numargs) override {return *currentValue;}
        };
        //XXX currently unused
        class TimeVariable : public Expression::Variable
        {
          private:
            ExpressionFilter *owner;
          public:
            TimeVariable(ExpressionFilter *filter) {owner = filter;}
            virtual Functor *dup() const override {return new TimeVariable(owner);}
            virtual const char *getName() const override {return "<signaltime>";}
            virtual char getReturnType() const override {return Expression::Value::DBL;}
            virtual Expression::Value evaluate(Expression::Value args[], int numargs) override {return SIMTIME_DBL(owner->lastTimestamp);}
        };

    protected:
        Expression expr;
        simtime_t lastTimestamp;
        double lastOutputValue;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override {lastTimestamp = t; value = expr.doubleValue(); return true;}
    public:
        // internal methods, only public for technical reasons
        virtual Expression::Functor *makeValueVariable(int index, cResultFilter *prevFilter) {throw cRuntimeError("constant expression cannot have variables");};
        virtual Expression::Functor *makeTimeVariable() {return new TimeVariable(this);} // XXX: unused
    public:
        ExpressionFilter() : lastTimestamp(SIMTIME_ZERO), lastOutputValue(NaN) {}
        Expression& getExpression() {return expr;}
        simtime_t getLastTimestamp() const {return lastTimestamp;}
        double getLastValue() const {return lastOutputValue;}
        virtual std::string str() const override;
};

/**
 * @brief Result filter that computes an expression from a single input.
 */
class SIM_API UnaryExpressionFilter : public ExpressionFilter
{
    protected:
        double lastInputValue;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        // internal method, only public for technical reasons
        virtual Expression::Functor *makeValueVariable(int index, cResultFilter *prevFilter) override {ASSERT(index == 0); return new ValueVariable(this, &lastInputValue);}
    public:
        UnaryExpressionFilter() : lastInputValue(NaN) {}
        double getLastInputValue() const {return lastInputValue;}
};

/**
 * @brief Result filter that computes an expression from multiple inputs.
 */
class SIM_API NaryExpressionFilter : public ExpressionFilter
{
    protected:
        int signalCount;
        cResultFilter **prevFilters;
        double *lastInputValues;
    protected:
        using ExpressionFilter::process;
        virtual Expression::Functor *makeValueVariable(int index, cResultFilter *prevFilter) override;
        virtual void subscribedTo(cResultFilter *prev) override {}
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details) override;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details) override;
        virtual bool process(cResultFilter *prev, simtime_t& t, double& value);
    public:
        NaryExpressionFilter(int signalCount) {this->signalCount = signalCount; prevFilters = new cResultFilter*[signalCount]; lastInputValues = new double[signalCount];}
        virtual ~NaryExpressionFilter() {delete [] prevFilters; delete [] lastInputValues; }
};

/**
 * @brief Filter that expects a cPacket and outputs its length in bytes (getByteLength()).
 * Null (nullptr) values are ignored; non-cPacket objects are an error.
 */
class SIM_API PacketBytesFilter : public cObjectResultFilter
{
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details) override;
    public:
        using cObjectResultFilter::receiveSignal;
};

/**
 * @brief Filter that expects a cPacket and outputs its length in bits (getBitLength()).
 * Null (nullptr) values are ignored; non-cPacket objects are an error.
 */
class SIM_API PacketBitsFilter : public cObjectResultFilter
{
    protected:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *object, cObject *details) override;
    public:
        using cObjectResultFilter::receiveSignal;
};

/**
 * @brief Filter that outputs the sum of signal values divided by the measurement
 * interval (simtime minus warmupPeriod). NaN values in the input are ignored.
 */
class SIM_API SumPerDurationFilter : public cNumericResultFilter
{
    protected:
        double sum;
    protected:
        virtual bool process(simtime_t& t, double& value, cObject *details) override;
    public:
        SumPerDurationFilter() {sum = 0;}
        double getSumPerDuration() const;
        virtual std::string str() const override;
};

}  // namespace omnetpp

#endif


