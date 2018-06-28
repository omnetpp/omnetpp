//==========================================================================
//  RESULTEXPR.H - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __OMNETPP_RESULTEXPR_H
#define __OMNETPP_RESULTEXPR_H

#include "common/expression.h"
#include "omnetpp/simkerneldefs.h"
#include "omnetpp/cresultfilter.h"
#include "omnetpp/cresultrecorder.h"

namespace omnetpp {

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
        ExpressionFilter() : lastTimestamp(SIMTIME_ZERO), lastOutputValue(NAN) {}
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
        UnaryExpressionFilter() : lastInputValue(NAN) {}
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
 * @brief Result recorder that remembers the last value of a signal,
 * and in finish() it evaluates an expression and saves the result.
 */
//TODO: this is in fact UnaryExpressionRecorder, we also need NaryExpressionRecorder (like filters)
class SIM_API ExpressionRecorder : public cNumericResultRecorder
{
    public:
        typedef omnetpp::common::Expression Expression;
    protected:
        Expression expr;
    public:
        double lastInputValue; // internal, only public for technical reasons
    protected:
        virtual void collect(simtime_t_cref t, double value, cObject *details) override {lastInputValue = value;}
        virtual void finish(cResultFilter *prev) override;
    public:
        // internal methods, only public for technical reasons
        virtual Expression::Functor *makeValueVariable();
        virtual Expression::Functor *makeTimeVariable();
    public:
        ExpressionRecorder() {lastInputValue=NAN;}
        virtual Expression& getExpression() {return expr;}
        double getLastInputValue() const {return lastInputValue;}
        double getCurrentValue() const;
        virtual std::string str() const override;
};

}  // namespace omnetpp


#endif
