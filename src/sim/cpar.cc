//=========================================================================
//  CPAR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/commonutil.h"
#include "common/stringutil.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cparimpl.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/ccontextswitcher.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cvalue.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/any_ptr.h"
#include "omnetpp/cmodelchange.h"
#include "common/unitconversion.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using namespace omnetpp::common;

namespace omnetpp {

cPar::~cPar()
{
    if (p && !p->isShared())
        delete p;
}

void cPar::init(cComponent *component, cParImpl *newp)
{
    ASSERT(!p && newp);
    ownerComponent = evalContext = component;
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
    other.ownerComponent = ownerComponent;
    other.p = p;
    other.evalContext = evalContext;
    p = nullptr;
    evalContext = nullptr;
}

const char *cPar::getName() const
{
    return p->getName();
}

std::string cPar::str() const
{
    return p->str();
}

internal::cParImpl *cPar::copyIfShared()
{
    if (p->isShared()) {
        p = p->dup();
        p->setIsShared(false);
    }
    return p;
}

cObject *cPar::getOwner() const
{
    return ownerComponent;
}

void cPar::forEachChild(cVisitor *v)
{
    return p->forEachChild(v, ownerComponent);
}

void cPar::operator=(const cPar& other)
{
    // this method is not used by the sim kernel, only (rarely) by some
    // simulation models to copy parameters
    if (other.isExpression()) {
        cComponent *ctx = other.evalContext;
        if (ctx == other.ownerComponent)
            ctx = ownerComponent;  // best effort to fix up evaluation context
        setExpression(other.getExpression()->dup(), ctx);
    }
    else {
        switch (getType()) {
            case BOOL:   setBoolValue(other.boolValue()); break;
            case DOUBLE: setDoubleValue(other.doubleValue()); break;
            case INT:    setIntValue(other.intValue()); break;
            case STRING: setStringValue(other.stdstringValue().c_str()); break;
            case OBJECT: setObjectValue(other.objectValue()); break;
            case XML:    setXMLValue(other.xmlValue()); break;
            default:     ASSERT(false);
        }
    }
}

cProperties *cPar::getProperties() const
{
    cComponent *component = dynamic_cast<cComponent *>(getOwner());
    ASSERT(component != nullptr);
    cComponentType *componentType = component->getComponentType();
    cProperties *props = componentType->getParamProperties(getName());
    return props;
}

const char *cPar::getBaseDirectory() const
{
    return p->getBaseDirectory();
}

std::string cPar::getSourceLocation() const
{
    return p->getSourceLocation().str();
}

const char *cPar::getTypeName(Type t)
{
    switch (t) {
        case BOOL:   return "bool";
        case DOUBLE: return "double";
        case INT:    return "int";
        case STRING: return "string";
        case OBJECT: return "object";
        case XML:    return "xml";
        default:     return "???";
    }
}

// note: the following one-liners should really be inline functions, but
// they can't be put into cpar.h because of declaration order.

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

bool cPar::isMutable() const
{
    return p->isMutable();
}

bool cPar::isExpression() const
{
    return p->isExpression();
}

#define TRY(x) \
    try { x; } catch (std::exception& e) { throw cRuntimeError(E_PARAM, getFullName(), e.what()); }

bool cPar::boolValue() const
{
    TRY(return p->boolValue(evalContext));
}

intval_t cPar::intValue() const
{
    TRY(return p->intValue(evalContext));
}

double cPar::doubleValue() const
{
    TRY(return p->doubleValue(evalContext));
}

double cPar::doubleValueInUnit(const char *targetUnit) const
{
    if (getType() == INT)
        return UnitConversion::convertUnit((double)intValue(), getUnit(), targetUnit); // note: possible precision loss
    else
        return UnitConversion::convertUnit(doubleValue(), getUnit(), targetUnit); // OK for DOUBLE, error for all others
}

const char *cPar::getUnit() const
{
    return p->getUnit();
}

const char *cPar::stringValue() const
{
    TRY(return p->stringValue(evalContext));
}

std::string cPar::stdstringValue() const
{
    TRY(return p->stdstringValue(evalContext));
}

cObject *cPar::objectValue() const
{
    TRY(return p->objectValue(evalContext));
}

cXMLElement *cPar::xmlValue() const
{
    TRY(return p->xmlValue(evalContext));
}

cValue cPar::getValue() const
{
    switch (getType()) {
        case BOOL:   return cValue(boolValue());
        case INT:    return cValue(intValue(), getUnit());
        case DOUBLE: return cValue(doubleValue(), getUnit());
        case STRING: return cValue(stdstringValue());
        case OBJECT: return cValue(objectValue());
        case XML:    return cValue(xmlValue());
        default:     ASSERT(false);
    }
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
    evalContext = nullptr;
    afterChange();
    return *this;
}

cPar& cPar::setIntValue(intval_t l)
{
    beforeChange();
    copyIfShared();
    p->setIntValue(l);
    evalContext = nullptr;
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(double d)
{
    beforeChange();
    copyIfShared();
    p->setDoubleValue(d);
    evalContext = nullptr;
    afterChange();
    return *this;
}

cPar& cPar::setStringValue(const char *s)
{
    beforeChange();
    copyIfShared();
    p->setStringValue(s);
    evalContext = nullptr;
    afterChange();
    return *this;
}

cPar& cPar::setObjectValue(cObject *object)
{
    beforeChange();
    copyIfShared();
    p->setObjectValue(object);
    evalContext = nullptr;
    afterChange();
    return *this;
}

cPar& cPar::setXMLValue(cXMLElement *node)
{
    beforeChange();
    copyIfShared();
    p->setXMLValue(node);
    evalContext = nullptr;
    afterChange();
    return *this;
}

cPar& cPar::setValue(const cValue& value)
{
    switch (getType()) {
        case BOOL:   setBoolValue(value.boolValue()); break;
        case INT:    setIntValue(value.intValueInUnit(getUnit())); break;
        case DOUBLE: setDoubleValue(value.doubleValueInUnit(getUnit())); break;
        case STRING: setStringValue(value.stringValue()); break;
        case OBJECT: setObjectValue(value.objectValue()); break;
        case XML:    setXMLValue(value.xmlValue()); break;
        default:     ASSERT(false);
    }
    return *this;
}

cPar& cPar::setExpression(cExpression *e, cComponent *ctx)
{
    beforeChange();
    copyIfShared();
    p->setExpression(e);
    evalContext = ctx ? ctx : ownerComponent;
    if (ownerComponent->parametersFinalized() && !p->isVolatile()) // note: conversion to const for non-volatile parameters in the setup phase is delayed to parameter finalization, see comment in parse()
        doConvertToConst();
    afterChange();
    return *this;
}

void cPar::beforeChange(bool isInternalChange)
{
    // throw if not mutable
    if (p->isSet() && !p->isMutable() && !isInternalChange && getSimulation()->getParameterMutabilityCheck())
        throw cRuntimeError(this, "Setting the parameter is not allowed at runtime (it is not marked as mutable)");

    // notify pre-change listeners
    if (ownerComponent->hasListeners(PRE_MODEL_CHANGE)) {
        cPreParameterChangeNotification tmp;
        tmp.par = this;
        ownerComponent->emit(PRE_MODEL_CHANGE, &tmp);
    }
}

void cPar::afterChange()
{
    ASSERT(ownerComponent);
#ifdef SIMFRONTEND_SUPPORT
    ownerComponent->updateLastChangeSerial();
#endif

    // call owner's component's handleParameterChange() method, but suppress
    // notifications in the phase when component parameters are still being set up.
    // Reason: component is not yet really prepared to handle the notification at that
    // point, because it hasn't been initialized.
    if (ownerComponent->parametersFinalized()) {
        cContextSwitcher tmp(ownerComponent);
        ownerComponent->handleParameterChange(getFullName());
    }

    // notify post-change listeners
    if (ownerComponent->hasListeners(POST_MODEL_CHANGE)) {
        cPostParameterChangeNotification tmp;
        tmp.par = this;
        ownerComponent->emit(POST_MODEL_CHANGE, &tmp);
    }
}

#ifdef SIMFRONTEND_SUPPORT
bool cPar::hasChangedSince(int64_t lastRefreshSerial)
{
    return ownerComponent->hasChangedSince(lastRefreshSerial);
}

#endif

void cPar::read()
{
    // obtain value if parameter is not set yet
    if (!p->isSet())
        getEnvir()->readParameter(this);
}

void cPar::finalize()
{
    // convert non-volatile expressions to constant
    if (p->isExpression() && !p->isVolatile())
        doConvertToConst();
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
    cComponentType *componentType = ownerComponent->getComponentType();
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

void cPar::doConvertToConst(bool isInternalChange)
{
    copyIfShared();
    beforeChange(isInternalChange);
    try {
        p->convertToConst(evalContext);
    }
    catch (std::exception& e) {
        throw cRuntimeError(E_PARAM, getFullName(), e.what());
    }

    // maybe replace it with a shared copy
    cComponentType *componentType = ownerComponent->getComponentType();
    cParImpl *cachedValue = componentType->getSharedParImpl(p);
    if (cachedValue)
        setImpl(cachedValue);
    else
        componentType->putSharedParImpl(p);
    afterChange();
}

void cPar::parse(const char *text, const char *baseDirectory, FileLine loc, bool resetEvalContext)
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
    cComponentType *componentType = ownerComponent->getComponentType();
    std::string key = std::string(getName()) + "|" + text + "|" + opp_nulltoempty(baseDirectory);
    cParImpl *cachedValue = componentType->getSharedParImpl(key.c_str());
    if (cachedValue) {
        // an identical value found in the map -- use it
        setImpl(cachedValue);
    }
    else {
        // not found: clone existing parameter (to preserve name, type, unit etc), then parse text into it
        cParImpl *tmp = p->dup();
        try {
            tmp->setBaseDirectory(baseDirectory);
            tmp->parse(text, loc);
        }
        catch (std::exception& e) {
            delete tmp;
            throw cRuntimeError("Cannot assign '%s' to parameter '%s': %s", text, getFullPath().c_str(), e.what());
        }

        // successfully parsed: install it
        componentType->putSharedParImpl(key.c_str(), tmp);
        setImpl(tmp);
    }

    if (resetEvalContext)
        setEvaluationContext(getOwnerComponent());

    // Ensure that expressions for non-volatile parameters are evaluated immediately,
    // unless we are in the setup phase when re-using the parsing of similar expressions
    // makes sense. In the latter case, evaluation takes place in finalize() (after which
    // the parametersFinalized flag will be set).
    if (ownerComponent->parametersFinalized() && p->isExpression() && !p->isVolatile())
        doConvertToConst();

    afterChange();
}

}  // namespace omnetpp

