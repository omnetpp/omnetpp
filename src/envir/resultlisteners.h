//==========================================================================
//  RESULTLISTENERS.H - part of
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

#ifndef __ENVIR_RESULTLISTENERS_H
#define __ENVIR_RESULTLISTENERS_H

#include "envirdefs.h"
#include "clistener.h"
#include "cstatistic.h"
#include "ccomponent.h"

class ENVIR_API ResultRecorder : public cIListener
{
    protected:
        simtime_t getEndWarmupPeriod() { return 1.0; /*FIXME*/ }
        std::string makeName(simsignal_t signalID, const char *opname);
        void extractSignalAttributes(cComponent *component, simsignal_t signalID, opp_string_map& result);
    public:
        virtual void listenerAdded(cComponent *component, simsignal_t signalID);
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID);
};

class ENVIR_API NumericResultRecorder : public ResultRecorder
{
    protected:
        virtual void collect(simtime_t t, double value) = 0;
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
        simtime_t lastTime;  // to ensure increasing timestamp order (TODO move into manager)
    protected:
        void collect(simtime_t t, double value);
    public:
        VectorRecorder(const char *componentFullPath, const char *signalName); //TODO take signal_t instead? accept vector attributes somehow: unit, data type, interpolation mode, enum etc
        void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
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
        void collect(simtime_t t, double value) {count++;}
    public:
        CountRecorder() {count = 0;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s); // count strings too
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj); // and all objects
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
        void collect(simtime_t t, double value) {sum += value;}
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
        void collect(simtime_t t, double value) {count++; sum += value;}
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
        void collect(simtime_t t, double value) {if (value < min) min = value;}
    public:
        MinRecorder() {min = 1e300; /*FIXME positive infinity!*/}
        virtual void finish(cComponent *component, simsignal_t signalID);
};

//FIXME do not record anything (or record nan?) if there was no value

/**
 * Listener for recording the maximum of signal values
 */
class ENVIR_API MaxRecorder : public NumericResultRecorder
{
    protected:
        double max;
    protected:
        void collect(simtime_t t, double value) {if (value > max) max = value;}
    public:
        MaxRecorder() {max = -1e300; /*FIXME negative infinity!*/}
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
        void collect(simtime_t t, double value);
    public:
        TimeAverageRecorder() {startTime = lastTime = -1; lastValue = weightedSum = 0;}
        virtual void finish(cComponent *component, simsignal_t signalID);
};

/**
 * Listener for recording signal values via a cStatistic
 */
//FIXME ownership?
class ENVIR_API StatisticsRecorder : public NumericResultRecorder, private cObject /*so it can own the statistic object*/
{
    protected:
        cStatistic *statistic;
    protected:
        void collect(simtime_t t, double value) {statistic->collect(value);}
    public:
        StatisticsRecorder(cStatistic *stat) {statistic = stat; take(statistic);}
        ~StatisticsRecorder() {drop(statistic); delete statistic;}
        virtual void finish(cComponent *component, simsignal_t signalID);
};

#endif

