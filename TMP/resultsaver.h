//==========================================================================
//  RESULTSAVER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __RESULTSAVER_H
#define __RESULTSAVER_H

#include <omnetpp.h>

/**
 * A cIListener that saves results into an output vector.
 */
class cVectorRecorder : public cIListener {
  protected:
     void *vectorHandle;
  public:
    cVectorRecorder(const char *componentName, const char *vectorName);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
};

cVectorRecorder::cVectorRecorder(const char *componentName, const char *vectorName)
{
    vectorHandle = ev.registerOutputVector(componentName, vectorName);
    //XXX setVectorAttribute(void *vechandle, const char *name, const char *value);
    //XXX deregisterOutputVector(vectorHandle);
}

void cVectorRecorder::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    ev.recordInOutputVector(vectorHandle, simTime(), (double)l);
}

void cVectorRecorder::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    ev.recordInOutputVector(vectorHandle, simTime(), d);
}

void cVectorRecorder::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)
{
    ev.recordInOutputVector(vectorHandle, simTime(), SIMTIME_DBL(t));
}

void cVectorRecorder::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    throw cRuntimeError("cannot record strings as output vectors");
}

void cVectorRecorder::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    //XXX may save a *field* of the object (reflection!)
    //XXX may save a TimestampedDatum struct!
    throw cRuntimeError("cannot record objects as output vectors");  //XXX refine message
}

//----

/**
 * A cIListener that saves the last value as a scalar.
 */
class cLastValueScalarRecorder : public cIListener {
  protected:
     double lastValue;

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) {
        lastValue = l;
    }
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d) {
        lastValue = d;
    }
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t) {
        lastValue = SIMTIME_DBL(t);
    }
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s) {
        throw cRuntimeError("cannot record strings as output vectors");
    }

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) {
        throw cRuntimeError("cannot record strings as output vectors");
    }

    virtual void finish() { //XXX should be called on exit!!!
        ev.recordScalar(component, scalarName, lastValue, NULL);
    }
};

#endif

