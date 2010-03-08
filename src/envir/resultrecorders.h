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
#include "resultrecorder.h"
#include "cstatistic.h"


/**
 * Listener for recording a signal to an output vector
 */
class ENVIR_API VectorRecorder : public NumericResultRecorder
{
    protected:
        void *handle;        // identifies output vector for the output vector manager
        simtime_t lastTime;  // to ensure increasing timestamp order
    protected:
        virtual void collect(double value) {collect(simulation.getSimTime(), value);}
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
        virtual void collect(double value)  {count++;}
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
        virtual void collect(double value) {lastValue = value;}
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
        virtual void collect(double value) {sum += value;}
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
        virtual void collect(double value) {count++; sum += value;}
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
        virtual void collect(double value) {if (value < min) min = value;}
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
        virtual void collect(double value) {if (value > max) max = value;}
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
        virtual void collect(double value)  {collect(simulation.getSimTime(), value);}
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
        virtual void collect(double value) {statistic->collect(value);}
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

