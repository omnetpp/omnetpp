//==========================================================================
//   CXMLPARIMPL.CC  - part of
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

#include "omnetpp/cxmlparimpl.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/ccomponent.h"
#include "ctemporaryowner.h"

namespace omnetpp {
namespace internal {

cXMLParImpl::~cXMLParImpl()
{
    deleteOld();
}

void cXMLParImpl::copy(const cXMLParImpl& other)
{
    if (flags & FL_ISEXPR)
        expr = other.expr->dup();
    else
        val = other.val;
}

void cXMLParImpl::operator=(const cXMLParImpl& other)
{
    if (this == &other)
        return;
    deleteOld();
    cParImpl::operator=(other);
    copy(other);
}

void cXMLParImpl::forEachChild(cVisitor *v, cComponent *context)
{
    if (isSet() && !isExpression()) {
        if (val)
            v->visit(val);
    }
}

void cXMLParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, E_BADASSIGN, "bool", "XML");
}

void cXMLParImpl::setIntValue(intval_t l)
{
    throw cRuntimeError(this, E_BADASSIGN, "integer", "XML");
}

void cXMLParImpl::setDoubleValue(double d)
{
    throw cRuntimeError(this, E_BADASSIGN, "double", "XML");
}

void cXMLParImpl::setStringValue(const char *s)
{
    throw cRuntimeError(this, E_BADASSIGN, "string", "XML");
}

void cXMLParImpl::setObjectValue(cObject *object)
{
    throw cRuntimeError(this, E_BADASSIGN, "object", "XML");
}

void cXMLParImpl::setXMLValue(cXMLElement *node)
{
    deleteOld();
    val = node;
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cXMLParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cXMLParImpl::boolValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "XML", "bool");
}

intval_t cXMLParImpl::intValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "XML", "integer");
}

double cXMLParImpl::doubleValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "XML", "double");
}

const char *cXMLParImpl::stringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "XML", "string");
}

std::string cXMLParImpl::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "XML", "string");
}

cObject *cXMLParImpl::objectValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "XML", "object");
}

cXMLElement *cXMLParImpl::xmlValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(E_PARNOTSET);

    if ((flags & FL_ISEXPR) == 0)
        return val;
    else {
        try {
            cTemporaryOwner tmp(cTemporaryOwner::DestructorMode::DISPOSE); // eventually dispose of potential object result
            cValue v = evaluate(expr, context);
            if (v.type != cValue::POINTER)
                throw cRuntimeError(E_BADCAST, v.getTypeName(), "XML");
            return v.xmlValue();
        }
        catch (std::exception& e) {
            throw cRuntimeError(e, expr->getSourceLocation().c_str());
        }

    }
}

cExpression *cXMLParImpl::getExpression() const
{
    return (flags & FL_ISEXPR) ? expr : nullptr;
}

void cXMLParImpl::deleteOld()
{
    if (flags & FL_ISEXPR) {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cXMLParImpl::getType() const
{
    return cPar::XML;
}

bool cXMLParImpl::isNumeric() const
{
    return false;
}

void cXMLParImpl::convertToConst(cComponent *context)
{
    setXMLValue(xmlValue(context));
}

std::string cXMLParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();
    else
        return val ? val->str() : "nullptr";
}

void cXMLParImpl::parse(const char *text, FileLine loc)
{
    // try parsing it as an expression
    cDynamicExpression *dynexpr = new cDynamicExpression();
    try {
        dynexpr->parseNedExpr(text);
    }
    catch (std::exception& e) {
        delete dynexpr;
        throw;
    }
    dynexpr->setSourceLocation(loc);
    setExpression(dynexpr);

    // simplify if possible: store as constant instead of expression
    if (dynexpr->isAConstant())
        convertToConst(nullptr);
}

int cXMLParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret != 0)
        return ret;

    const cXMLParImpl *other2 = dynamic_cast<const cXMLParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

}  // namespace internal
}  // namespace omnetpp

