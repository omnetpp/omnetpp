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

#include "commonutil.h"
#include "resultfilter.h"

cGlobalRegistrationList resultFilters("resultFilters");


ResultFilter::ResultFilter()
{
    delegates = new ResultListener*[1];
    delegates[0] = NULL;
}

void ResultFilter::addDelegate(ResultListener *delegate)
{
    // reallocate each time
    Assert(delegates!=NULL);
    int n = getNumDelegates();
    ResultListener **v = new ResultListener*[n+2];
    memcpy(v, delegates, n*sizeof(ResultListener*));
    v[n] = delegate;
    v[n+1] = NULL;
    delete [] delegates;
    delegates = v;
    delegate->subscribecount++;
    delegate->listenerAdded(this);
}

int ResultFilter::getNumDelegates() const
{
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
    for (int i=0; delegates[i]; i++) {
        delegates[i]->subscribecount--;
        delegates[i]->listenerRemoved(this);
    }
    delete [] delegates;
}

void ResultFilter::fire(ResultFilter *prev, long l)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, l);
}

void ResultFilter::fire(ResultFilter *prev, double d)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, d);
}

void ResultFilter::fire(ResultFilter *prev, simtime_t t, double d)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, d);
}

void ResultFilter::fire(ResultFilter *prev, simtime_t t)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t);
}

void ResultFilter::fire(ResultFilter *prev, const char *s)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, s);
}

void ResultFilter::fire(ResultFilter *prev, cObject *obj)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, obj);
}

void ResultFilter::finish(ResultFilter *prev)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->finish(this);
}

//---

void NumericResultFilter::receiveSignal(ResultFilter *prev, long l)
{
    double d = l;
    if (process(d))
        fire(this, d);
}

void NumericResultFilter::receiveSignal(ResultFilter *prev, double d)
{
    if (process(d))
        fire(this, d);
}

void NumericResultFilter::receiveSignal(ResultFilter *prev, simtime_t t, double d)
{
    if (process(t, d))
        fire(this, t, d);
}

void NumericResultFilter::receiveSignal(ResultFilter *prev, simtime_t v)
{
    double d = SIMTIME_DBL(v);
    if (process(d))
        fire(this, d);
}

void NumericResultFilter::receiveSignal(ResultFilter *prev, const char *s)
{
    throw cRuntimeError("%s: Cannot convert const char * to double", getClassName());
}

void NumericResultFilter::receiveSignal(ResultFilter *prev, cObject *obj)
{
    // note: cISignalValue stuff was already dispatched to (simtime_t,double) method in base class
    throw cRuntimeError("%s: Cannot convert cObject * to double", getClassName());
}

//---

ResultFilterDescriptor::ResultFilterDescriptor(const char *name, ResultFilter *(*f)())
  : cNoncopyableOwnedObject(name, false)
{
    creatorfunc = f;
}

ResultFilterDescriptor *ResultFilterDescriptor::find(const char *name)
{
    return dynamic_cast<ResultFilterDescriptor *>(resultFilters.getInstance()->lookup(name));
}

ResultFilterDescriptor *ResultFilterDescriptor::get(const char *name)
{
    ResultFilterDescriptor *p = find(name);
    if (!p)
        throw cRuntimeError("Result filter \"%s\" not found -- perhaps the name is wrong, "
                            "or the filter was not registered with Register_ResultFilter()", name);
    return p;
}


