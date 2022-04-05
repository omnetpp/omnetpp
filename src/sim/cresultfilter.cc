//==========================================================================
//  CRESULTFILTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm> // copy_n()
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "omnetpp/cresultfilter.h"
#include "omnetpp/checkandcast.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {

cResultFilter::cResultFilter()
{
    delegates = new cResultListener *[1];
    delegates[0] = nullptr;
}

void cResultFilter::init(Context *ctx)
{
    component = ctx->component;
    attrsProperty = ctx->attrsProperty;

#ifdef __GNUC__ /* also affects clang */
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

    // call legacy init() method in case the user redefined it
    init(ctx->component, ctx->attrsProperty);

#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

}

cResultFilter *cResultFilter::clone() const
{
    cResultFilter *copy = (cResultFilter *) createOne(getClassName());
    Context ctx { component, attrsProperty };
    copy->init(&ctx);
    return copy;
}

void cResultFilter::addDelegate(cResultListener *delegate)
{
    if (hasDelegate(delegate))
        throw cRuntimeError("%s: delegate already added", getClassName());

    // reallocate each time
    Assert(delegates != nullptr);
    int n = getNumDelegates();
    cResultListener **v = new cResultListener *[n + 2];
    std::copy_n(delegates, n, v);
    v[n] = delegate;
    v[n+1] = nullptr;
    delete[] delegates;
    delegates = v;
    delegate->delegatedCount++;
    delegate->subscribedTo(this);
}

bool cResultFilter::hasDelegate(cResultListener *delegate)
{
    for (int i = 0; delegates[i]; i++)
        if (delegates[i] == delegate)
            return true;
    return false;
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
        delegates[i]->delegatedCount--;
        delegates[i]->unsubscribedFrom(this);
    }
    delete[] delegates;
}

void cResultFilter::forEachChild(cVisitor *v)
{
    for (int i = 0; delegates[i]; i++)
        if (!v->visit(delegates[i]))
            return;
    cResultListener::forEachChild(v);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details)
{
    for (int i = 0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, b, details);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, intval_t l, cObject *details)
{
    for (int i = 0; delegates[i]; i++)
        delegates[i]->receiveSignal(this, t, l, details);
}

void cResultFilter::fire(cResultFilter *prev, simtime_t_cref t, uintval_t l, cObject *details)
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

void cResultFilter::emitInitialValue()
{
    double initialValue = getInitialDoubleValue();
    if (!std::isnan(initialValue)) {
        // initial value must be sent for the end of the warmup period; except if that time
        // has already passed (e.g. because this filter is part of a dynamically created module)
        cSimulation *simulation = getSimulation();
        simtime_t t = std::max(simulation->getSimTime(), simulation->getWarmupPeriod());
        fire(this, t, initialValue, nullptr);
    }
}

void cResultFilter::callEmitInitialValue()
{
    emitInitialValue();
    for (int i = 0; delegates[i]; i++)
        delegates[i]->callEmitInitialValue();
}

//---

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details)
{
    simtime_t tt = t;
    double d = b;
    if (process(tt, d, details))
        fire(this, tt, d, details);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t l, cObject *details)
{
    simtime_t tt = t;
    double d = l;
    if (process(tt, d, details))
        fire(this, tt, d, details);
}

void cNumericResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t l, cObject *details)
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
    THROW("bool");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t l, cObject *details)
{
    THROW("intval_t");
}

void cObjectResultFilter::receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t l, cObject *details)
{
    THROW("uintval_t");
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

cResultFilterType::cResultFilterType(const char *name, const char *className, const char *description)
: cNoncopyableOwnedObject(name, false), description(opp_nulltoempty(description)), className(className)
{
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

cResultFilter *cResultFilterType::create() const
{
    return check_and_cast<cResultFilter*>(createOne(className.c_str()));
}

}  // namespace omnetpp

