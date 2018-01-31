//==========================================================================
//  CRESULTFILTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/commonutil.h"
#include "omnetpp/cresultfilter.h"

using namespace omnetpp::common;

namespace omnetpp {

cResultFilter::cResultFilter()
{
    delegates = new cResultListener *[1];
    delegates[0] = nullptr;
}

void cResultFilter::addDelegate(cResultListener *delegate)
{
    // reallocate each time
    Assert(delegates != nullptr);
    int n = getNumDelegates();
    cResultListener **v = new cResultListener *[n + 2];
    memcpy(v, delegates, n * sizeof(cResultListener *));
    v[n] = delegate;
    v[n+1] = nullptr;
    delete[] delegates;
    delegates = v;
    delegate->subscribeCount++;
    delegate->subscribedTo(this);
}

int cResultFilter::getNumDelegates() const
{
    int k = 0;
    while (delegates[k])
        k++;
    return k;
}

std::vector<cResultListener *> cResultFilter::getDelegates() const
{
    std::vector<cResultListener *> result;
    for (int i = 0; delegates[i]; i++)
        result.push_back(delegates[i]);
    return result;
}

cResultFilter::~cResultFilter()
{
    for (int i = 0; delegates[i]; i++) {
        delegates[i]->subscribeCount--;
        delegates[i]->unsubscribedFrom(this);
    }
    delete[] delegates;
}

void cResultFilter::forEachChild(cVisitor *v)
{
    for (int i = 0; delegates[i]; i++)
        v->visit(delegates[i]);
    cResultListener::forEachChild(v);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details)
{
    for (int i = 0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, b, details);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, long l, cObject *details)
{
    for (int i = 0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, l, details);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details)
{
    for (int i = 0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, l, details);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, double d, cObject *details)
{
    for (int i = 0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, d, details);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details)
{
    for (int i = 0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, v, details);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details)
{
    for (int i = 0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, s, details);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details)
{
    for (int i = 0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, obj, details);
}

void cResultFilter::callFinish(cResultFilter *prev)
{
    finish(prev);
    for (int i = 0; delegates[i]; i++)
        delegates[i]->callFinish(this);
}

//---

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details)
{
    simtime_t tt = t;
    double d = b;
    if (process(tt, d, details))
        fire(this, tt, d, details);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details)
{
    simtime_t tt = t;
    double d = l;
    if (process(tt, d, details))
        fire(this, tt, d, details);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details)
{
    simtime_t tt = t;
    double d = l;
    if (process(tt, d, details))
        fire(this, tt, d, details);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details)
{
    simtime_t tt = t;
    if (process(tt, d, details))
        fire(this, tt, d, details);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details)
{
    simtime_t tt = t;
    double d = v.dbl();
    if (process(tt, d, details))
        fire(this, tt, d, details);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details)
{
    throw cRuntimeError("%s: Cannot convert const char * to double", getClassName());
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details)
{
    // note: cITimestampedValue stuff was already dispatched to (simtime_t,double) method in base class
    throw cRuntimeError("%s: Cannot convert cObject * to double", getClassName());
}

//---

#define THROW(t)    throw opp_runtime_error("%s: Received data with wrong type (%s): Object expected", getClassName(), t);

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details)
{
    THROW("long");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details)
{
    THROW("long");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details)
{
    THROW("unsigned long");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details)
{
    THROW("double");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details)
{
    THROW("simtime_t");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details)
{
    THROW("const char *");
}

#undef THROW

//---

cResultFilterType::cResultFilterType(const char *name, cResultFilter *(*f)())
    : cNoncopyableOwnedObject(name, false)
{
    creatorfunc = f;
}

cResultFilterType *cResultFilterType::find(const char *name)
{
    return dynamic_cast<cResultFilterType *>(resultFilters.getInstance()->lookup(name));
}

cResultFilterType *cResultFilterType::get(const char *name)
{
    cResultFilterType *p = find(name);
    if (!p)
        throw cRuntimeError("Result filter \"%s\" not found -- perhaps the name is wrong, "
                            "or the filter was not registered with Register_ResultFilter()", name);
    return p;
}

}  // namespace omnetpp

