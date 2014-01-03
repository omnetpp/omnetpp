//==========================================================================
//  CRESULTFILTER.CC - part of
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
#include "cresultfilter.h"

NAMESPACE_BEGIN


cResultFilter::cResultFilter()
{
    delegates = new cResultListener*[1];
    delegates[0] = NULL;
}

void cResultFilter::addDelegate(cResultListener *delegate)
{
    // reallocate each time
    Assert(delegates!=NULL);
    int n = getNumDelegates();
    cResultListener **v = new cResultListener*[n+2];
    memcpy(v, delegates, n*sizeof(cResultListener*));
    v[n] = delegate;
    v[n+1] = NULL;
    delete [] delegates;
    delegates = v;
    delegate->subscribecount++;
    delegate->subscribedTo(this);
}

int cResultFilter::getNumDelegates() const
{
    int k = 0;
    while (delegates[k])
        k++;
    return k;
}

std::vector<cResultListener*> cResultFilter::getDelegates() const
{
    std::vector<cResultListener*> result;
    for (int i = 0; delegates[i]; i++)
        result.push_back(delegates[i]);
    return result;
}

cResultFilter::~cResultFilter()
{
    for (int i=0; delegates[i]; i++) {
        delegates[i]->subscribecount--;
        delegates[i]->unsubscribedFrom(this);
    }
    delete [] delegates;
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, bool b)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, b);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, long l)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, l);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, unsigned long l)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, l);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, double d)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, d);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, const SimTime& v)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, v);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, const char *s)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, s);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, cObject *obj)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, obj);
}

void cResultFilter::finish(cResultFilter *prev)
{
    for (int i=0; delegates[i]; i++)
        delegates[i]->finish(this);
}

//---

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b)
{
    simtime_t tt = t;
    double d = b;
    if (process(tt, d))
        fire(this, tt, d);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, long l)
{
    simtime_t tt = t;
    double d = l;
    if (process(tt, d))
        fire(this, tt, d);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l)
{
    simtime_t tt = t;
    double d = l;
    if (process(tt, d))
        fire(this, tt, d);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, double d)
{
    simtime_t tt = t;
    if (process(tt, d))
        fire(this, tt, d);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v)
{
    simtime_t tt = t;
    double d = v.dbl();
    if (process(tt, d))
        fire(this, tt, d);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s)
{
    throw cRuntimeError("%s: Cannot convert const char * to double", getClassName());
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj)
{
    // note: cITimestampedValue stuff was already dispatched to (simtime_t,double) method in base class
    throw cRuntimeError("%s: Cannot convert cObject * to double", getClassName());
}

//---

#define THROW(t)  throw opp_runtime_error("%s: received data with wrong type (%s): object expected", getClassName(), t);

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b)
{
    THROW("long");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, long l)
{
    THROW("long");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l)
{
    THROW("unsigned long");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, double d)
{
    THROW("double");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v)
{
    THROW("simtime_t");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s)
{
    THROW("const char *");
}

#undef THROW

//---

cResultFilterDescriptor::cResultFilterDescriptor(const char *name, cResultFilter *(*f)())
  : cNoncopyableOwnedObject(name, false)
{
    creatorfunc = f;
}

cResultFilterDescriptor *cResultFilterDescriptor::find(const char *name)
{
    return dynamic_cast<cResultFilterDescriptor *>(resultFilters.getInstance()->lookup(name));
}

cResultFilterDescriptor *cResultFilterDescriptor::get(const char *name)
{
    cResultFilterDescriptor *p = find(name);
    if (!p)
        throw cRuntimeError("Result filter \"%s\" not found -- perhaps the name is wrong, "
                            "or the filter was not registered with Register_ResultFilter()", name);
    return p;
}

NAMESPACE_END

