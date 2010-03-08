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
#include "resultlistener.h"
#include "cstatistic.h"
#include "ccomponent.h"
#include "csimulation.h"
#include "commonutil.h"
#include "expression.h"
#include "stringpool.h"
#include "cregistrationlist.h"


class ENVIR_API ResultFilter : public ResultListener
{
    private:
        ResultListener **delegates; // NULL-terminated array
    protected:
        void fire(cComponent *source, simsignal_t signalID, long l);
        void fire(cComponent *source, simsignal_t signalID, double d);
        void fire(cComponent *source, simsignal_t signalID, simtime_t t);
        void fire(cComponent *source, simsignal_t signalID, const char *s);
        void fire(cComponent *source, simsignal_t signalID, cObject *obj);
    public:
        ResultFilter() {delegates = NULL;}
        ~ResultFilter();
        virtual void addDelegate(ResultListener *delegate);
        virtual int getNumDelegates() const;
        std::vector<ResultListener*> getDelegates() const;
        virtual void listenerAdded(cComponent *component, simsignal_t signalID);
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID);
        virtual void finish(cComponent *component, simsignal_t signalID);
};

class ENVIR_API WarmupPeriodFilter : public ResultFilter
{
    private:
        simtime_t getEndWarmupPeriod() {return simulation.getWarmupPeriod();}
    public:
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
};

class ENVIR_API NumericResultFilter : public ResultFilter
{
    protected:
        // return value: whether to delegate (true) or to swallow the value (false)
        virtual bool process(double& value) = 0;
        virtual bool process(simtime_t& t, double& value) = 0;
    public:
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
};

/**
 * Result filter for counting signals. Signal values do not need to be numeric
 * to be counted.
 */
class ENVIR_API CountFilter : public ResultFilter
{
    protected:
        long count;
    protected:
        void doIt(cComponent *source, simsignal_t signalID) {count++; fire(source, signalID, count);}
    public:
        CountFilter() {count = 0;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) {doIt(source, signalID);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d) {doIt(source, signalID);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t) {doIt(source, signalID);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s) {doIt(source, signalID);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
};

/**
 * This class is actually a no-op; provided for symmetry with LastValueRecorder.
 */
class ENVIR_API LastValueFilter : public NumericResultFilter
{
    protected:
        virtual bool process(double& value) {return true;}
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
        virtual bool process(double& value) {sum += value; value = sum; return true;}
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
        virtual bool process(double& value) {count++; sum += value; value = sum/count; return true;}
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
        virtual bool process(double& value) {if (value < min) min = value; value = min; return true;}
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
        virtual bool process(double& value) {if (value > max) max = value; value = max; return true;}
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
        virtual bool process(double& value)  {simtime_t tmp = simulation.getSimTime(); process(tmp, value); return true;}
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
        virtual bool process(double& value)  {simtime_t tmp = simulation.getSimTime(); process(tmp, value); return true;}
        virtual bool process(simtime_t& t, double& value);
    public:
        ExpressionFilter() {}
        virtual std::string str() const {return expr.str();}
        Expression& getExpression() {return expr;}
        Expression::Functor *makeValueVariable();
        Expression::Functor *makeTimeVariable();
};

#endif


