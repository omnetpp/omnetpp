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

#ifndef __ENVIR_RESULTRECORDERS_H
#define __ENVIR_RESULTRECORDERS_H

#include "envirdefs.h"
#include "clistener.h"
#include "cstatistic.h"
#include "ccomponent.h"
#include "commonutil.h"
#include "stringpool.h"

class ResultRecorder;

#define Register_ResultRecorder(NAME, CLASSNAME) \
  static ResultRecorder *__FILEUNIQUENAME__() {return new CLASSNAME;} \
  EXECUTE_ON_STARTUP(resultRecorders.getInstance()->add(new ResultRecorderDescriptor(NAME,__FILEUNIQUENAME__));)

extern cGlobalRegistrationList resultRecorders;

/**
 * Registers a ResultRecorder.
 */
class ENVIR_API ResultRecorderDescriptor : public cNoncopyableOwnedObject
{
  private:
    ResultRecorder *(*creatorfunc)();

  public:
    /**
     * Constructor.
     */
    ResultRecorderDescriptor(const char *name, ResultRecorder *(*f)());

    /**
     * Creates an instance of a particular class by calling the creator
     * function.
     */
    ResultRecorder *create() const  {return creatorfunc();}

    /**
     * Finds the factory object for the given name. The class must have been
     * registered previously with the Register_ResultRecorder() macro.
     */
    static ResultRecorderDescriptor *find(const char *name);

    /**
     * Like find(), but throws an error if the object was not found.
     */
    static ResultRecorderDescriptor *get(const char *name);
};

/**
 * Abstract base class for result recording listeners
 */
class ENVIR_API ResultRecorder : public cIListener
{
    private:
        static CommonStringPool statisticNamesPool;
        const char *statisticName;
    protected:
        simtime_t getEndWarmupPeriod() {return simulation.getWarmupPeriod();}
        virtual std::string makeName(const char *suffix);
        virtual void extractStatisticAttributes(cComponent *component, opp_string_map& result);
        virtual void tweakTitle(opp_string& title) {}
        const char *getStatisticName() {return statisticName;}
    public:
        virtual void init(const char *statisticName);
        virtual void listenerAdded(cComponent *component, simsignal_t signalID);
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID);
};

/**
 * Abstract base class for result recording listeners
 */
class ENVIR_API NumericResultRecorder : public ResultRecorder
{
    protected:
        virtual void collect(simtime_t t, double value) = 0;

        void maybeCollect(double value) {
            simtime_t t = simulation.getSimTime();
            if (t >= getEndWarmupPeriod())
                collect(t, value);
        }
        void maybeCollect(simtime_t t, double value) {
            if (t >= getEndWarmupPeriod())
                collect(t, value);
        }

    public:
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
};

/**
 * Listener for recording a signal to an output vector
 */
class ENVIR_API VectorRecorder : public NumericResultRecorder
{
    protected:
        void *handle;        // identifies output vector for the output vector manager
        simtime_t lastTime;  // to ensure increasing timestamp order
    protected:
        virtual void collect(simtime_t t, double value);
    public:
        VectorRecorder() {handle = NULL; lastTime = 0;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
        virtual void listenerAdded(cComponent *component, simsignal_t signalID);
};

/**
 * Listener for recording the count of signal values. Signal values do not need
 * to be numeric to be counted.
 */
class ENVIR_API CountRecorder : public NumericResultRecorder
{
    protected:
        long count;
    protected:
        virtual void tweakTitle(opp_string& title);
        virtual void collect(simtime_t t, double value) {count++;}
    public:
        CountRecorder() {count = 0;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s); // count strings too
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj); // and all objects
        virtual void finish(cComponent *component, simsignal_t signalID);
};

/**
 * Listener for recording the last signal value
 */
class ENVIR_API LastValueRecorder : public NumericResultRecorder
{
    protected:
        double lastValue;
    protected:
        virtual void tweakTitle(opp_string& title);
        virtual void collect(simtime_t t, double value) {lastValue = value;}
    public:
        LastValueRecorder() {lastValue = NaN;}
        virtual void finish(cComponent *component, simsignal_t signalID);
};

/**
 * Listener for recording the sum of signal values
 */
class ENVIR_API SumRecorder : public NumericResultRecorder
{
    protected:
        double sum;
    protected:
        virtual void tweakTitle(opp_string& title);
        virtual void collect(simtime_t t, double value) {sum += value;}
    public:
        SumRecorder() {sum = 0;}
        virtual void finish(cComponent *component, simsignal_t signalID);
};

/**
 * Listener for recording the mean of signal values
 */
class ENVIR_API MeanRecorder : public NumericResultRecorder
{
    protected:
        long count;
        double sum;
    protected:
        virtual void tweakTitle(opp_string& title);
        virtual void collect(simtime_t t, double value) {count++; sum += value;}
    public:
        MeanRecorder() {count = 0; sum = 0;}
        virtual void finish(cComponent *component, simsignal_t signalID);
};

/**
 * Listener for recording the minimum of signal values
 */
class ENVIR_API MinRecorder : public NumericResultRecorder
{
    protected:
        double min;
    protected:
        virtual void tweakTitle(opp_string& title);
        virtual void collect(simtime_t t, double value) {if (value < min) min = value;}
    public:
        MinRecorder() {min = POSITIVE_INFINITY;}
        virtual void finish(cComponent *component, simsignal_t signalID);
};

/**
 * Listener for recording the maximum of signal values
 */
class ENVIR_API MaxRecorder : public NumericResultRecorder
{
    protected:
        double max;
    protected:
        virtual void tweakTitle(opp_string& title);
        virtual void collect(simtime_t t, double value) {if (value > max) max = value;}
    public:
        MaxRecorder() {max = NEGATIVE_INFINITY;}
        virtual void finish(cComponent *component, simsignal_t signalID);
};

/**
 * Listener for recording the time average of signal values
 */
class ENVIR_API TimeAverageRecorder : public NumericResultRecorder
{
    protected:
        simtime_t startTime;
        simtime_t lastTime;
        double lastValue;
        double weightedSum;
    protected:
        virtual void tweakTitle(opp_string& title);
        virtual void collect(simtime_t t, double value);
    public:
        TimeAverageRecorder() {startTime = lastTime = -1; lastValue = weightedSum = 0;}
        virtual void finish(cComponent *component, simsignal_t signalID);
};

/**
 * Listener for recording signal values via a cStatistic
 */
class ENVIR_API StatisticsRecorder : public NumericResultRecorder, private cObject /*so it can own the statistic object*/
{
    protected:
        cStatistic *statistic;
    protected:
        virtual void collect(simtime_t t, double value) {statistic->collect(value);}
    public:
        StatisticsRecorder(cStatistic *stat) {statistic = stat; take(statistic);}
        ~StatisticsRecorder() {drop(statistic); delete statistic;}
        virtual void finish(cComponent *component, simsignal_t signalID);
};

class ENVIR_API StddevRecorder : public StatisticsRecorder
{
    public:
        StddevRecorder();
};

class ENVIR_API HistogramRecorder : public StatisticsRecorder
{
    public:
        HistogramRecorder();
};

#endif

