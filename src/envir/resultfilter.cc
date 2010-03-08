//==========================================================================
//  RESULTFILTER.CC - part of
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

#include "resultfilter.h"


void ResultFilter::addDelegate(ResultListener *delegate)
{
    // reallocate each time
    int n = getNumDelegates();
    ResultListener **v = new ResultListener*[n+2];
    memcpy(v, delegates, n*sizeof(ResultListener*));
    v[n] = delegate;
    v[n+1] = NULL;
    delete [] delegates;
    delegates = v;
    delegate->subscribecount++;
    delegate->listenerAdded(NULL, SIMSIGNAL_NULL);
}

int ResultFilter::getNumDelegates() const
{
    if (!delegates)
        return 0;
    int k = 0;
    while (delegates[k])
        k++;
    return k;
}

std::vector<ResultListener*> ResultFilter::getDelegates() const
{
    std::vector<ResultListener*> result;
    for (int i = 0; delegates[i]; i++)
        result.push_back(delegates[i]);
    return result;
}

ResultFilter::~ResultFilter()
{
    if (delegates)
    {
        for (int i=0; delegates[i]; i++) {
            delegates[i]->subscribecount--;
            delegates[i]->listenerRemoved(NULL, SIMSIGNAL_NULL);
        }
        delete [] delegates;
    }
}

void ResultFilter::listenerAdded(cComponent *component, simsignal_t signalID)
{
    ASSERT(getSubscribeCount() == 1);  // may only be subscribed once (otherwise results get mixed)
}

void ResultFilter::listenerRemoved(cComponent *component, simsignal_t signalID)
{
    if (getSubscribeCount() == 0)
        delete this;
}

void ResultFilter::fire(cComponent *source, simsignal_t signalID, long l)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(source, signalID, l);
}

void ResultFilter::fire(cComponent *source, simsignal_t signalID, double d)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(source, signalID, d);
}

void ResultFilter::fire(cComponent *source, simsignal_t signalID, simtime_t t)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(source, signalID, t);
}

void ResultFilter::fire(cComponent *source, simsignal_t signalID, const char *s)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(source, signalID, s);
}

void ResultFilter::fire(cComponent *source, simsignal_t signalID, cObject *obj)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(source, signalID, obj);
}

void ResultFilter::finish(cComponent *component, simsignal_t signalID)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->finish(component, signalID);
}

//---

void NumericResultFilter::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    double d = l;
    if (process(d))
        fire(source, signalID, d);
}

void NumericResultFilter::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    if (process(d))
        fire(source, signalID, d);
}

void NumericResultFilter::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t v)
{
    double d = SIMTIME_DBL(v);
    if (process(d))
        fire(source, signalID, d);
}

void NumericResultFilter::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    throw cRuntimeError("cannot convert const char * to double"); //FIXME better message
}

void NumericResultFilter::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    cISignalValue *v = dynamic_cast<cISignalValue *>(obj);
    if (!v)
        throw cRuntimeError("cannot convert cObject* to double"); //FIXME better message

    simtime_t t = v->getSignalTime(signalID);
    double d = v->getSignalValue(signalID);
    if (process(t,d))
    {
        cSignalValue tmp(t,d);
        fire(source, signalID, &tmp);
    }
}


