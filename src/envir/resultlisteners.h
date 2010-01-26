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

//FIXME into sim/!
class ENVIR_API /*SIM_API*/ TimeValue : public cObject, noncopyable
{
    public:
        simtime_t time;
        double value;
};

class ENVIR_API ResultRecorder : public cIListener
{
    protected:
        simtime_t getEndWarmupPeriod() { return 1.0; /*FIXME*/ }
        virtual void unsupportedType(cComponent *component, simsignal_t signalID, const char *dataType);
    public:
        virtual void listenerAdded(cComponent *component, simsignal_t signalID);
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID);
};

void ResultRecorder::listenerAdded(cComponent *component, simsignal_t signalID)
{
    ASSERT(getSubscribeCount() == 1);  // may only be subscribed once (otherwise results get mixed)
}

void ResultRecorder::listenerRemoved(cComponent *component, simsignal_t signalID)
{
    if (getSubscribeCount() == 0)
        delete this;
}

void ResultRecorder::unsupportedType(cComponent *component, simsignal_t signalID, const char *dataType)
{
    const char *signalName = cComponent::getSignalName(signalID);
    throw cRuntimeError("%s: Unsupported signal data type %s for signal %s (id=%d)",
                        opp_typename(typeid(*this)), dataType, signalName, (int)signalID);
}

//---

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

//FIXME warmup period! delegate to common record(d) and record(t,d) methods!!!

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        collect(t, l);
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        collect(t, d);
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t d)
{
    simtime_t t = simulation.getSimTime();
    if (t >= getEndWarmupPeriod())
        collect(t, SIMTIME_DBL(d));
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    const char *signalName = cComponent::getSignalName(signalID);
    throw cRuntimeError("%s: unsupported signal data type 'const char *' for signal %s (id=%d)",
                        opp_typename(typeid(*this)), signalName, (int)signalID);
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    TimeValue *d = dynamic_cast<TimeValue *>(obj);
    if (!d)
        throw cRuntimeError(source, "Wrong object!!!!! must be TimeValue!!!!!FIXME");  //XXX print signalName!
    if (d->time >= getEndWarmupPeriod())
        collect(d->time, d->value);
}


/**
 * Listener for recording a signal to an output vector
 */
class ENVIR_API VectorRecorder : public NumericResultRecorder
{
    protected:
        void *handle;        // identifies output vector for the output vector manager
        simtime_t lastTime;  // last timestamp written, needed to ensure increasing timestamp order (TODO move into manager)
    protected:
        void collect(simtime_t t, double value);
    public:
        VectorRecorder(const char *componentFullPath, const char *signalName); //TODO take signal_t instead? accept vector attributes somehow: unit, data type, interpolation mode, enum etc
        void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
};

VectorRecorder::VectorRecorder(const char *componentFullPath, const char *signalName)
{
    handle = ev.registerOutputVector(componentFullPath, signalName);
    ASSERT(handle!=NULL);
}

void VectorRecorder::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    // copied from base class to add monotonicity check
    TimeValue *d = dynamic_cast<TimeValue *>(obj);
    if (!d)
        throw cRuntimeError(source, "Wrong object!!!!! must be TimeValue!!!!!FIXME");  //XXX print signalName!
    if (d->time < lastTime)
        throw cRuntimeError("Cannot record data with an earlier timestamp (t=%s) than the previously recorded value", SIMTIME_STR(d->time));  //XXX print signalName!
    if (d->time >= getEndWarmupPeriod())
        collect(d->time, d->value);
}

void VectorRecorder::collect(simtime_t t, double value)
{
    lastTime = t;
    ev.recordInOutputVector(handle, t, value);
}

//---

/**
 * Listener for recording the count of signal values. Signal values do not need
 * to be numeric to be counted.
 */
class ENVIR_API CountRecorder : public NumericResultRecorder
{
    protected:
        long count;
    protected:
        void collect(simtime_t t, double value) {
            count++;
        }
    public:
        CountRecorder() {count = 0;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s)  {
            NumericResultRecorder::receiveSignal(source, signalID, (long)0);
        }
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)  {
            NumericResultRecorder::receiveSignal(source, signalID, (long)0); //FIXME cast to TimeValue and check wamup time here !!!
        }
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID); //TODO modify name? like "count(signalName)" or "signalName.count"?
            ev.recordScalar(component, signalName, count, NULL); //FIXME attributes too!!
        }
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
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID);
            ev.recordScalar(component, signalName, sum, NULL); //FIXME attributes; "signal.sum"?
        }
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
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID);
            ev.recordScalar(component, signalName, sum/count, NULL); //FIXME attributes; "signal.mean"?
        }
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
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID);
            ev.recordScalar(component, signalName, min, NULL); //FIXME attributes; "signal.mean"?
        }
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
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID);
            ev.recordScalar(component, signalName, max, NULL); //FIXME attributes; "signal.mean"?
        }
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
        void collect(simtime_t t, double value) {
            if (startTime==-1)
                startTime = t;
            else
                weightedSum += lastValue * SIMTIME_DBL(t - lastTime);
            lastTime = t;
            lastValue = value;
        }
    public:
        TimeAverageRecorder() {startTime = lastTime = -1; lastValue = weightedSum = 0;}
        virtual void finish(cComponent *component, simsignal_t signalID) {
            simtime_t t = simulation.getSimTime();
            collect(t, 0.0); // to get the last interval counted in; value 0.0 is just a dummy
            const char *signalName = cComponent::getSignalName(signalID);
            double interval = SIMTIME_DBL(t - startTime);
            ev.recordScalar(component, signalName, weightedSum / interval, NULL); //FIXME attributes; "signal.mean"?
        }
};

/**
 * Listener for recording signal values via a cStatistic
 */
class ENVIR_API StatisticsRecorder : public NumericResultRecorder
{
    protected:
        cStatistic *statistic;
    protected:
        void collect(simtime_t t, double value) {
            statistic->collect(value);
        }
    public:
        StatisticsRecorder(cStatistic *stat) {statistic = stat;}  //FIXME ownership?
        ~StatisticsRecorder() {delete statistic;}
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID);
            ev.recordStatistic(component, signalName, statistic, NULL); //FIXME attributes; "signal.mean"?
        }
};

#endif

