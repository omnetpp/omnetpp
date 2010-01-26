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

/**
 * Listener for recording a signal to an output vector
 */
class ENVIR_API VectorRecordingListener : public cIListener
{
    protected:
        void *handle;        // identifies output vector for the output vector manager
        simtime_t last_t;    // last timestamp written, needed to ensure increasing timestamp order (TODO move into manager)
    protected:
        /** Utility function, throws a "data type not supported" error. */
        virtual void unsupportedType(simsignal_t signalID, const char *dataType);
        void record(simtime_t t, double value);
    public:
        VectorRecordingListener(const char *componentFullPath, const char *signalName); //TODO accept vector attributes somehow: unit, data type, interpolation mode, enum etc
        virtual ~VectorRecordingListener();
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID);
};

VectorRecordingListener::VectorRecordingListener(const char *componentFullPath, const char *signalName)
{
    handle = ev.registerOutputVector(componentFullPath, signalName);
    ASSERT(handle!=NULL);
}

VectorRecordingListener::~VectorRecordingListener()
{
}

void VectorRecordingListener::record(simtime_t t, double value)
{
    last_t = t;
    ev.recordInOutputVector(handle, t, value);
}

void VectorRecordingListener::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    record(simulation.getSimTime(), l);
}

void VectorRecordingListener::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    record(simulation.getSimTime(), d);
}

void VectorRecordingListener::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)
{
    record(simulation.getSimTime(), SIMTIME_DBL(t));
}

void VectorRecordingListener::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    //FIXME unsupported
}

void VectorRecordingListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    //FIXME cast to some (t,value) tuple
    simtime_t t = 0; //FIXME
    double value = 0; //FIXME

    if (t < last_t)
        throw cRuntimeError(source, "Cannot record data with an earlier timestamp (t=%s) "
                                 "than the previously recorded value", SIMTIME_STR(t));  //XXX signalName!
    record(t, value);
}

void VectorRecordingListener::listenerRemoved(cComponent *component, simsignal_t signalID)
{
    if (getSubscribeCount() == 0)
        delete this;
}

//---

/**
 * Listener for recording a signal to an output vector XXX
 */
class ENVIR_API CountRecorder : public cIListener
{
    protected:
        long count;
    public:
        CountRecorder() {count = 0;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) {count++;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d)  {count++;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)  {count++;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s)  {count++;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)  {count++;}
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID); //TODO modify name? like "count(signalName)" or "signalName.count"?
            ev.recordScalar(component, signalName, count, NULL); //FIXME attributes too!!
        }
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID) {
            if (getSubscribeCount() == 0)
                delete this;
        }
};

/**
 * Listener for recording a signal to an output vector XXX
 */
class ENVIR_API SumRecorder : public cIListener
{
    protected:
        double sum;
    public:
        SumRecorder() {sum = 0;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) {sum += l;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d)  {sum += d;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)  {sum += SIMTIME_DBL(t);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s)  {ASSERT(FALSE);} //FIXME
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)  {ASSERT(FALSE);} //FIXME
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID);
            ev.recordScalar(component, signalName, sum, NULL); //FIXME attributes; "signal.sum"?
        }
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID) {
            if (getSubscribeCount() == 0)
                delete this;
        }
};

/**
 * Listener for recording a signal to an output vector XXX
 */
class ENVIR_API MeanRecorder : public cIListener
{
    protected:
        long count;
        double sum;
    public:
        MeanRecorder() {count = 0; sum = 0;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) {count++; sum += l;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d)  {count++; sum += d;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)  {count++; sum += SIMTIME_DBL(t);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s)  {ASSERT(FALSE);} //FIXME
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)  {ASSERT(FALSE);} //FIXME
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID);
            ev.recordScalar(component, signalName, sum/count, NULL); //FIXME attributes; "signal.mean"?
        }
        //TODO listenerAdded: ASSERT(getSubscribeCount()==1)  --only once!
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID) {
            if (getSubscribeCount() == 0)
                delete this;
        }
};

/**
 * Listener for recording a signal to an output vector XXX
 */
class ENVIR_API MinRecorder : public cIListener
{
    protected:
        double min;
    public:
        MinRecorder() {min = 1e300; /*FIXME positive infinity!*/}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) {if (l < min) min = l;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d)  {if (d < min) min = d;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)  {double d = SIMTIME_DBL(t); if (d < min) min = d;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s)  {ASSERT(FALSE);} //FIXME
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)  {ASSERT(FALSE);} //FIXME
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID);
            ev.recordScalar(component, signalName, min, NULL); //FIXME attributes; "signal.mean"?
        }
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID) {
            if (getSubscribeCount() == 0)
                delete this;
        }
};

/**
 * Listener for recording a signal to an output vector XXX
 */
class ENVIR_API MaxRecorder : public cIListener
{
    protected:
        double max;
    public:
        MaxRecorder() {max = -1e300; /*FIXME negative infinity!*/}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) {if (l > max) max = l;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d)  {if (d > max) max = d;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)  {double d = SIMTIME_DBL(t); if (d > max) max = d;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s)  {ASSERT(FALSE);} //FIXME
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)  {ASSERT(FALSE);} //FIXME
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID);
            ev.recordScalar(component, signalName, max, NULL); //FIXME attributes; "signal.mean"?
        }
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID) {
            if (getSubscribeCount() == 0)
                delete this;
        }
};

/**
 * Listener for recording a signal to an output vector XXX
 */
class ENVIR_API TimeAverageRecorder : public cIListener
{
    protected:
        simtime_t startTime;
        simtime_t lastTime;
        double wsum;
    public:
        TimeAverageRecorder() {startTime = lastTime = -1; wsum = 0;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) {receiveSignal(source, signalID, (double)l);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d)  {
            simtime_t t = simulation.getSimTime();
            if (startTime==-1)
                startTime = t;
            else
                wsum += d * SIMTIME_DBL(t - lastTime);
            lastTime = t;
        }
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)  {receiveSignal(source, signalID, SIMTIME_DBL(t));}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s)  {ASSERT(FALSE);} //FIXME
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)  {ASSERT(FALSE);} //FIXME
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID);
            double interval = SIMTIME_DBL(simulation.getSimTime() - startTime);
            ev.recordScalar(component, signalName, interval, NULL); //FIXME attributes; "signal.mean"?
        }
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID) {
            if (getSubscribeCount() == 0)
                delete this;
        }
};

/**
 * Listener for recording a signal to an output vector XXX
 */
class ENVIR_API StatisticsRecorder : public cIListener
{
    protected:
        cStatistic *statistic;
    public:
        StatisticsRecorder(cStatistic *stat) {statistic = stat;}  //FIXME ownership?
        ~StatisticsRecorder() {delete statistic;}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) {statistic->collect(l);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d)  {statistic->collect(d);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)  {statistic->collect(t);}
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s)  {ASSERT(FALSE);} //FIXME
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)  {ASSERT(FALSE);} //FIXME
        virtual void finish(cComponent *component, simsignal_t signalID) {
            const char *signalName = cComponent::getSignalName(signalID);
            ev.recordStatistic(component, signalName, statistic, NULL); //FIXME attributes; "signal.mean"?
        }
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID) {
            if (getSubscribeCount() == 0)
                delete this;
        }
};

#endif

