//=========================================================================
//  CPAR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cpar.h"
#include "cparimpl.h"
#include "cproperties.h"
#include "cproperty.h"
#include "ccomponent.h"
#include "ccomponenttype.h"
#include "cmodule.h"
#include "csimulation.h"
#include "commonutil.h"
#include "cenvir.h"
#include "cmodelchange.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

NAMESPACE_BEGIN


cPar::~cPar()
{
    if (p && !p->isShared())
        delete p;
}

void cPar::init(cComponent *component, cParImpl *newp)
{
    ASSERT(!p && newp);
    ownercomponent = evalcontext = component;
    p = newp;
}

void cPar::setImpl(cParImpl *newp)
{
    ASSERT(p && newp);
    if (!p->isShared())
        delete p;
    p = newp;
}

void cPar::moveto(cPar& other)
{
    other.ownercomponent = ownercomponent;
    other.p = p;
    other.evalcontext = evalcontext;
    p = NULL;
    evalcontext = NULL;
}

const char *cPar::getName() const
{
    return p->getName();
}

std::string cPar::info() const
{
    return p->info();
}

std::string cPar::detailedInfo() const
{
    return p->detailedInfo() + " " + getProperties()->info();
}

cParImpl *cPar::copyIfShared()
{
    if (p->isShared()) {
        p = p->dup();
        p->setIsShared(false);
    }
    return p;
}

cObject *cPar::getOwner() const
{
    return ownercomponent;
}

void cPar::operator=(const cPar& other)
{
    // this method is not used by the sim kernel, only (rarely) by some
    // simulation models to copy parameters
    if (other.isExpression())
    {
        cComponent *ctx = other.evalcontext;
        if (ctx == other.ownercomponent)
            ctx = ownercomponent;  // best effort to fix up evaluation context
        setExpression(other.getExpression()->dup(), ctx);
    }
    else
    {
        switch (getType())
        {
            case BOOL:   setBoolValue(other.boolValue()); break;
            case DOUBLE: setDoubleValue(other.doubleValue()); break;
            case LONG:   setLongValue(other.longValue()); break;
            case STRING: setStringValue(other.stdstringValue().c_str()); break;
            case XML:    setXMLValue(other.xmlValue()); break;
            default:     ASSERT(false);
        }
    }
}

cProperties *cPar::getProperties() const
{
    cComponent *component = check_and_cast<cComponent *>(getOwner());
    cComponentType *componentType = component->getComponentType();
    cProperties *props = componentType->getParamProperties(getName());
    return props;
}

const char *cPar::getTypeName(Type t)
{
    switch (t)
    {
        case BOOL:   return "bool";
        case DOUBLE: return "double";
        case LONG:   return "long";
        case STRING: return "string";
        case XML:    return "xml";
        default:     return "???";
    }
}

// note: the following one-liners should really be inline functions, but
// they can't be put into cpar.h because of declaration order.

std::string cPar::str() const
{
    return p->str();
}

cPar::Type cPar::getType() const
{
    return p->getType();
}

bool cPar::isShared() const
{
    return p->isShared();
}

bool cPar::isSet() const
{
    return p->isSet();
}

bool cPar::containsValue() const
{
    return p->containsValue();
}

bool cPar::isNumeric() const
{
    return p->isNumeric();
}

bool cPar::isVolatile() const
{
    return p->isVolatile();
}

bool cPar::isExpression() const
{
    return p->isExpression();
}

#define TRY(x) \
    try {x;} catch (std::exception& e) {throw cRuntimeError(ePARAM, getFullName(), e.what());}

bool cPar::boolValue() const
{
    TRY(return p->boolValue(evalcontext));
}

long cPar::longValue() const
{
    TRY(return p->longValue(evalcontext));
}

double cPar::doubleValue() const
{
    TRY(return p->doubleValue(evalcontext));
}

const char *cPar::getUnit() const
{
    return p->getUnit();
}

const char *cPar::stringValue() const
{
    TRY(return p->stringValue(evalcontext));
}

std::string cPar::stdstringValue() const
{
    TRY(return p->stdstringValue(evalcontext));
}

cXMLElement *cPar::xmlValue() const
{
    TRY(return p->xmlValue(evalcontext));
}

cExpression *cPar::getExpression() const
{
    return p->getExpression();
}

cPar& cPar::setBoolValue(bool b)
{
    beforeChange();
    copyIfShared();
    p->setBoolValue(b);
    evalcontext = NULL;
    afterChange();
    return *this;
}

cPar& cPar::setLongValue(long l)
{
    beforeChange();
    copyIfShared();
    p->setLongValue(l);
    evalcontext = NULL;
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(double d)
{
    beforeChange();
    copyIfShared();
    p->setDoubleValue(d);
    evalcontext = NULL;
    afterChange();
    return *this;
}

cPar& cPar::setStringValue(const char *s)
{
    beforeChange();
    copyIfShared();
    p->setStringValue(s);
    evalcontext = NULL;
    afterChange();
    return *this;
}

cPar& cPar::setXMLValue(cXMLElement *node)
{
    beforeChange();
    copyIfShared();
    p->setXMLValue(node);
    evalcontext = NULL;
    afterChange();
    return *this;
}

cPar& cPar::setExpression(cExpression *e, cComponent *ctx)
{
    beforeChange();
    copyIfShared();
    p->setExpression(e);
    evalcontext = ctx ? ctx : ownercomponent;
    afterChange();
    return *this;
}

void cPar::beforeChange()
{
    // notify pre-change listeners
    if (ownercomponent->hasListeners(PRE_MODEL_CHANGE)) {
        cPreParameterChangeNotification tmp;
        tmp.par = this;
        ownercomponent->emit(PRE_MODEL_CHANGE, &tmp);
    }
}

void cPar::afterChange()
{
    ASSERT(ownercomponent);
    // call owner's component's handleParameterChange() method,
    // i.e. parameter change notification is allowed only on fully initialized components
    if (ownercomponent->initialized())
    {
        cContextSwitcher tmp(ownercomponent);
        ownercomponent->handleParameterChange(getFullName());
    }

    // notify post-change listeners
    if (ownercomponent->hasListeners(POST_MODEL_CHANGE)) {
        cPostParameterChangeNotification tmp;
        tmp.par = this;
        ownercomponent->emit(POST_MODEL_CHANGE, &tmp);
    }

}

void cPar::read()
{
    // obtain value if parameter is not set yet
    if (!p->isSet())
        ev.readParameter(this);
}

void cPar::finalize()
{
    // convert non-volatile expressions to constant
    if (p->isExpression() && !p->isVolatile())
        convertToConst();

    // convert CONST subexpressions into constants
    if (p->isExpression() && p->containsConstSubexpressions())
    {
        beforeChange();
        copyIfShared();
        p->evaluateConstSubexpressions(evalcontext); //XXX sharing?
        afterChange();
    }
}

void cPar::acceptDefault()
{
    if (p->isSet())
        throw cRuntimeError(this, "acceptDefault(): Parameter is already set");
    if (!p->containsValue())
        throw cRuntimeError(this, "acceptDefault(): Parameter contains no default value");

    beforeChange();

    // basically we only need to set the isSet flag to true, but only
    // for ourselves, without affecting the shared parameter prototype.

    // try to look up the value in the value cache (temporarily setting
    // isSet=true so that we can use this object as key)
    p->setIsSet(true);
    cComponentType *componentType = ownercomponent->getComponentType();
    cParImpl *cachedValue = componentType->getSharedParImpl(p);
    p->setIsSet(false);

    // use the cached value, or create a value and put it into the cache
    if (cachedValue)
        setImpl(cachedValue);
    else {
        copyIfShared();
        p->setIsSet(true);
        componentType->putSharedParImpl(p);
    }
    afterChange();
}

void cPar::convertToConst()
{
    beforeChange();
    copyIfShared();
    try {
        p->convertToConst(evalcontext);
    }
    catch (std::exception& e) {
        throw cRuntimeError(ePARAM, getFullName(), e.what());
    }

    // maybe replace it with a shared copy
    cComponentType *componentType = ownercomponent->getComponentType();
    cParImpl *cachedValue = componentType->getSharedParImpl(p);
    if (cachedValue)
        setImpl(cachedValue);
    else
        componentType->putSharedParImpl(p);
    afterChange();
}

void cPar::parse(const char *text)
{
    // Implementation note: we are trying to share cParImpl objects for
    // values coming from the configuration. This is possible because an
    // expression's representation does not contain pointers to concrete
    // modules or other parameters. The context is always passed in separately
    // when the expression gets evaluated.
    //    For sharing parameter values, we use a map stored in cComponentType,
    // which we index with "parametername:textualvalue" as key. Per-componentType
    // storage ensures that parameters of identical name but different types
    // don't cause trouble.
    //
    // Note: text may not contain "ask" or "default"! This is ensured by
    // cParImpl::parse() which throws an error on them.
    //
    beforeChange();
    cComponentType *componentType = ownercomponent->getComponentType();
    std::string key = std::string(componentType->getName()) + ":" + getName() + ":" + text;
    cParImpl *cachedValue = componentType->getSharedParImpl(key.c_str());
    if (cachedValue)
    {
        // an identical value found in the map -- use it
        setImpl(cachedValue);
    }
    else
    {
        // not found: clone existing parameter (to preserve name, type, unit etc), then parse text into it
        cParImpl *tmp = p->dup();
        try {
            tmp->parse(text);
        }
        catch (std::exception& e) {
            delete tmp;
            throw cRuntimeError("Wrong value `%s' for parameter `%s': %s", text, getFullPath().c_str(), e.what());
        }

        // successfully parsed: install it
        componentType->putSharedParImpl(key.c_str(), tmp);
        setImpl(tmp);
    }
    afterChange();
}

NAMESPACE_END

