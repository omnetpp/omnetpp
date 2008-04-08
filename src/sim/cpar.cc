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
  Copyright (C) 1992-2005 Andras Varga

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

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE


cPar::~cPar()
{
    if (p && !p->isShared())
        delete p;
}

void cPar::assign(cComponent *component, cParImpl *newp)
{
    ASSERT(!p && newp);
    ownercomponent = component;
    p = newp;
}

void cPar::reassign(cParImpl *newp)
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

const char *cPar::name() const
{
    return p->name();
}

std::string cPar::info() const
{
    return p->info();
}

std::string cPar::detailedInfo() const
{
    return p->detailedInfo() + " " + properties()->info();
}

void cPar::copyIfShared()
{
    if (p->isShared())
    {
        p = p->dup();
        p->setIsShared(false);
    }
}

cObject *cPar::owner() const
{
    return ownercomponent;
}

void cPar::operator=(const cPar& other)
{
    // this method is not used by the sim kernel, only (rarely) by some
    // simulation models to copy parameters
    if (other.isExpression())
    {
        setExpression(other.expression()->dup());
    }
    else
    {
        switch (type())
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

cProperties *cPar::properties() const
{
    cComponent *component = check_and_cast<cComponent *>(owner());
    cComponentType *componentType = component->componentType();
    cProperties *props = componentType->paramProperties(name());
    return props;
}

const char *cPar::typeName(Type t)
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

std::string cPar::toString() const
{
    return p->toString();
}

cPar::Type cPar::type() const
{
    return p->type();
}

bool cPar::isShared() const
{
    return p->isShared();
}

bool cPar::hasValue() const
{
    return p->hasValue();
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
    try {x;} catch (std::exception& e) {throw cRuntimeError(ePARAM, fullName(), e.what());}

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

const char *cPar::unit() const
{
    return p->unit();
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

cExpression *cPar::expression() const
{
    return p->expression();
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
    if (simulation.contextType()==CTX_EVENT) // don't call during build, initialize or finish
    {
        ASSERT(ownercomponent);
        cContextSwitcher tmp(ownercomponent);
        ownercomponent->handleParameterChange(fullName());
    }
}

void cPar::read()
{
    //TRACE("read() of par=%s", fullPath().c_str());

    // obtain value if parameter is not set yet
    if (p->isInput())
    {
        //printf("           before ev.readParameter(), p=%p shared=%d input=%d\n", p, p->isShared(), p->isInput());
        ev.readParameter(this);
        //printf("           after ev.readParameter(), p=%p shared=%d input=%d\n", p, p->isShared(), p->isInput());
    }

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
    // basically we only need to set the isInput flag to false, but only
    // for ourselves, without affecting the shared parameter prototype.
    if (p->isInput())
    {
        // try to look up the value in the value cache (temporarily setting isInput=false)
        p->setIsInput(false);
        cComponentType *componentType = ownercomponent->componentType();
        cParImpl *cachedValue = componentType->parImplCache2()->get(p);
        p->setIsInput(true);

        // use the cached value, or create a value and put it into the cache
        if (cachedValue)
            reassign(cachedValue);
        else {
            copyIfShared();
            p->setIsInput(false);
            componentType->parImplCache2()->put(p);
        }
    }
}

void cPar::convertToConst()
{
    copyIfShared();
    try {
        p->convertToConst(ownercomponent);
    } catch (std::exception& e) {
        throw cRuntimeError(ePARAM, fullName(), e.what());
    }

    // maybe replace it with a shared copy
    cComponentType *componentType = ownercomponent->componentType();
    cParImpl *cachedValue = componentType->parImplCache2()->get(p);
    if (cachedValue)
        reassign(cachedValue);
    else
        componentType->parImplCache2()->put(p);
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
    cComponentType *componentType = ownercomponent->componentType();
    std::string key = std::string(componentType->name()) + ":" + name() + ":" + text;
    cParImpl *cachedValue = componentType->parImplCache()->get(key.c_str());
    if (cachedValue)
    {
        // an identical value found in the map -- use it
        reassign(cachedValue);
    }
    else
    {
        // not found: clone existing parameter (to preserve name, type, unit etc), then parse text into it
        cParImpl *tmp = p->dup();
        tmp->setIsInput(false);
        try
        {
            tmp->parse(text);
        }
        catch (std::exception& e)
        {
            delete tmp;
            throw cRuntimeError("Wrong value `%s' for parameter `%s': %s", text, fullPath().c_str(), e.what());
        }

        // successfully parsed: install it
        componentType->parImplCache()->put(key.c_str(), tmp);
        reassign(tmp);
    }
}

//---

cParImplCache::~cParImplCache()
{
    for (StringToParMap::iterator it = parMap.begin(); it!=parMap.end(); ++it)
        delete it->second;
    parMap.clear();
}

cParImpl *cParImplCache::get(const char *key) const
{
    StringToParMap::const_iterator it = parMap.find(key);
    return it==parMap.end() ? NULL : it->second;
}

void cParImplCache::put(const char *key, cParImpl *value)
{
    ASSERT(parMap.find(key)==parMap.end()); // not yet in there
    value->setIsShared(true);
    parMap[key] = value;
}

//---

// cannot go inline due to declaration order
bool cParImplCache2::Less::operator()(cParImpl *a, cParImpl *b) const
{
    return a->compare(b) < 0;
}

cParImplCache2::~cParImplCache2()
{
    for (ParImplSet::iterator it = parSet.begin(); it!=parSet.end(); ++it)
        delete *it;
    parSet.clear();
}

cParImpl *cParImplCache2::get(cParImpl *value) const
{
    ParImplSet::const_iterator it = parSet.find(value);
    return it==parSet.end() ? NULL : *it;
}

void cParImplCache2::put(cParImpl *value)
{
    ASSERT(parSet.find(value)==parSet.end()); // not yet in there
    value->setIsShared(true);
    parSet.insert(value);
}


