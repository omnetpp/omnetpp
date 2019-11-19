//==========================================================================
//   COBJECTPARIMPL.CC  - part of
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

#include "omnetpp/cobjectparimpl.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/cobjectfactory.h"
#include "omnetpp/ccomponent.h"

namespace omnetpp {

cObjectParImpl::cObjectParImpl()
{
    obj = nullptr;
    expr = nullptr;
    expectedType = nullptr;
}

cObjectParImpl::~cObjectParImpl()
{
    deleteExpression();
    deleteObject();
}

void cObjectParImpl::copy(const cObjectParImpl& other)
{
    if (flags & FL_ISEXPR)
        expr = other.expr->dup();

    expectedType = other.expectedType;

    obj = other.obj;
    if (obj) {
        if (!obj->isOwnedObject())
            obj = obj->dup();
        else if (obj->getOwner() == &other)
            take(static_cast<cOwnedObject*>(obj = obj->dup()));
    }
}

void cObjectParImpl::operator=(const cObjectParImpl& other)
{
    if (this == &other)
        return;
    deleteExpression();
    deleteObject();
    cParImpl::operator=(other);
    copy(other);
}

void cObjectParImpl::forEachChild(cVisitor *v, cComponent *context)
{
    if (isSet()) { // because xmlValue() throws otherwise
        cObject *element = objectValue(context);
        if (element)
            v->visit(element);
    }
}

void cObjectParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, E_BADCAST, "bool", "object");
}

void cObjectParImpl::setIntValue(intval_t l)
{
    throw cRuntimeError(this, E_BADCAST, "integer", "object");
}

void cObjectParImpl::setDoubleValue(double d)
{
    throw cRuntimeError(this, E_BADCAST, "double", "object");
}

void cObjectParImpl::setStringValue(const char *s)
{
    throw cRuntimeError(this, E_BADCAST, "string", "object");
}

void cObjectParImpl::setObjectValue(cObject *object)
{
    deleteExpression();
    doSetObject(object);
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cObjectParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, E_BADCAST, "XML", "object");
}

void cObjectParImpl::setExpression(cExpression *e)
{
    deleteExpression();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cObjectParImpl::boolValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "bool");
}

intval_t cObjectParImpl::intValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "integer");
}

double cObjectParImpl::doubleValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "double");
}

const char *cObjectParImpl::stringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "string");
}

std::string cObjectParImpl::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "string");
}

cObject *cObjectParImpl::objectValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(E_PARNOTSET);

    if ((flags & FL_ISEXPR) == 0)
        return obj;
    else {
        cValue v = evaluate(expr, context);
        if (v.type != cValue::OBJECT)
            throw cRuntimeError(E_ECANTCAST, "object");

        cObjectParImpl *mutableThis = const_cast<cObjectParImpl*>(this);
        mutableThis->doSetObject(v.objectValue());
        return obj;
    }
}

cXMLElement *cObjectParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "XML");
}

cExpression *cObjectParImpl::getExpression() const
{
    return (flags | FL_ISEXPR) ? expr : nullptr;
}

void cObjectParImpl::deleteExpression()
{
    if (flags & FL_ISEXPR) {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

void cObjectParImpl::deleteObject()
{
    if (obj) {
        if (!obj->isOwnedObject())
            delete obj;
        else if (obj->getOwner() == this)
            dropAndDelete(static_cast<cOwnedObject *>(obj));
        obj = nullptr;
    }
}

void cObjectParImpl::doSetObject(cObject *object)
{
    checkType(object);
    deleteObject();
    obj = object;
    if (cOwnedObject *ownedObj = dynamic_cast<cOwnedObject*>(obj))
        if (ownedObj->getOwner()->isSoftOwner())
            take(ownedObj);
}

void cObjectParImpl::checkType(cObject *object)
{
    if (!expectedType)
        return;

    bool optional = expectedType[strlen(expectedType)-1] == '?';
    if (!object) {
        if (optional)
            return;
        else
            throw cRuntimeError("nullptr is not an allowed value (append '?' to the name in @class() to allow it)");
    }

    std::string typeName = optional ? std::string(expectedType, strlen(expectedType)-1) : expectedType; // remove trailing '?'
    cObjectFactory *factory = cObjectFactory::find(typeName.c_str());
    if (!factory)
        throw cRuntimeError("'%s' is not a registered class (missing Register_Class() or Register_Abstract_Class() macro?)", typeName.c_str());
    if (!factory->isInstance(object))
        throw cRuntimeError("Cannot cast object (%s)%s to '%s', the C++ type declared for the parameter",
                object->getClassName(), object->getFullName(), factory->getFullName());

}

cPar::Type cObjectParImpl::getType() const
{
    return cPar::OBJECT;
}

bool cObjectParImpl::isNumeric() const
{
    return false;
}

void cObjectParImpl::setExpectedType(const char *s)
{
    stringPool.release(expectedType);
    expectedType = stringPool.get(s);
}

void cObjectParImpl::convertToConst(cComponent *context)
{
    cObject *saved = objectValue(context);
    obj = nullptr;
    setObjectValue(saved);
}

std::string cObjectParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();
    else if (obj)
        return std::string("(") + obj->getClassName() + ")" + obj->getFullName() + ": " + obj->str();
    else
        return "nullptr";
}

void cObjectParImpl::parse(const char *text)
{
    // try parsing it as an expression
    cDynamicExpression *dynexpr = new cDynamicExpression();
    try {
        dynexpr->parseNedExpr(text, true, true);
    }
    catch (std::exception& e) {
        delete dynexpr;
        throw;
    }
    setExpression(dynexpr);

    // simplify if possible: store as constant instead of expression
    if (dynexpr->isAConstant())
        convertToConst(nullptr);
}

int cObjectParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret != 0)
        return ret;

    const cObjectParImpl *other2 = dynamic_cast<const cObjectParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (obj == other2->obj) ? 0 : (obj < other2->obj) ? -1 : 1;
}

}  // namespace omnetpp

