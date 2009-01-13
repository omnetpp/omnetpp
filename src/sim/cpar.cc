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

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE


cPar::~cPar()
{
    if (p && !p->isShared())
        delete p;
}

void cPar::init(cComponent *component, cParImpl *newp)
{
    ASSERT(!p && newp);
    ownercomponent = component;
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
    p = NULL;
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
        setExpression(other.getExpression()->dup());
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
    TRY(return p->boolValue(ownercomponent));
}

long cPar::longValue() const
{
    TRY(return p->longValue(ownercomponent));
}

double cPar::doubleValue() const
{
    TRY(return p->doubleValue(ownercomponent));
}

const char *cPar::getUnit() const
{
    return p->getUnit();
}

const char *cPar::stringValue() const
{
    TRY(return p->stringValue(ownercomponent));
}

std::string cPar::stdstringValue() const
{
    TRY(return p->stdstringValue(ownercomponent));
}

cXMLElement *cPar::xmlValue() const
{
    TRY(return p->xmlValue(ownercomponent));
}

cExpression *cPar::getExpression() const
{
    return p->getExpression();
}

cPar& cPar::setBoolValue(bool b)
{
    copyIfShared();
    p->setBoolValue(b);
    afterChange();
    return *this;
}

cPar& cPar::setLongValue(long l)
{
    copyIfShared();
    p->setLongValue(l);
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(double d)
{
    copyIfShared();
    p->setDoubleValue(d);
    afterChange();
    return *this;
}

cPar& cPar::setStringValue(const char *s)
{
    copyIfShared();
    p->setStringValue(s);
    afterChange();
    return *this;
}

cPar& cPar::setXMLValue(cXMLElement *node)
{
    copyIfShared();
    p->setXMLValue(node);
    afterChange();
    return *this;
}

cPar& cPar::setExpression(cExpression *e)
{
    copyIfShared();
    p->setExpression(e);
    afterChange();
    return *this;
}

void cPar::afterChange()
{
    if (simulation.getContextType()==CTX_EVENT) // don't call during build, initialize or finish
    {
        ASSERT(ownercomponent);
        cContextSwitcher tmp(ownercomponent);
        ownercomponent->handleParameterChange(getFullName());
    }
}

void cPar::read()
{
    //TRACE("read() of par=%s", getFullPath().c_str());

    // obtain value if parameter is not set yet
    if (!p->isSet())
        ev.readParameter(this);

    // convert non-volatile expressions to constant
    if (p->isExpression() && !p->isVolatile())
        convertToConst();

    // convert CONST subexpressions into constants
    if (p->isExpression() && p->containsConstSubexpressions())
    {
        copyIfShared();
        p->evaluateConstSubexpressions(ownercomponent); //XXX sharing?
    }
}

void cPar::acceptDefault()
{
    if (p->isSet())
        throw cRuntimeError(this, "acceptDefault(): Parameter is already set");
    if (!p->containsValue())
        throw cRuntimeError(this, "acceptDefault(): Parameter contains no default value");

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
}

void cPar::convertToConst()
{
    copyIfShared();
    try {
        p->convertToConst(ownercomponent);
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
}

