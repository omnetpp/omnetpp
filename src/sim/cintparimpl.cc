//==========================================================================
//   CINTPARIMPL.CC  - part of
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

#include <limits>
#include <cinttypes> // PRId64
#include "omnetpp/cintparimpl.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/ccomponent.h"
#include "ctemporaryowner.h"

namespace omnetpp {
namespace internal {

cIntParImpl::cIntParImpl()
{
    val = 0;
}

cIntParImpl::~cIntParImpl()
{
    deleteOld();
}

void cIntParImpl::copy(const cIntParImpl& other)
{
    if (flags & FL_ISEXPR)
        expr = other.expr->dup();
    else
        val = other.val;
}

void cIntParImpl::operator=(const cIntParImpl& other)
{
    if (this == &other)
        return;
    deleteOld();
    cParImpl::operator=(other);
    copy(other);
}

void cIntParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, E_BADASSIGN, "bool", "integer");
}

void cIntParImpl::setIntValue(intval_t l)
{
    deleteOld();
    val = l;
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cIntParImpl::setDoubleValue(double d)
{
    throw cRuntimeError(this, E_BADASSIGN, "double", "integer");
}

void cIntParImpl::setStringValue(const char *s)
{
    throw cRuntimeError(this, E_BADASSIGN, "string", "integer");
}

void cIntParImpl::setObjectValue(cObject *node)
{
    throw cRuntimeError(this, E_BADASSIGN, "object", "integer");
}

void cIntParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, E_BADASSIGN, "XML", "integer");
}

void cIntParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cIntParImpl::boolValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "integer", "bool");
}

intval_t cIntParImpl::intValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(E_PARNOTSET);

    if ((flags & FL_ISEXPR) == 0)
        return val;
    else {
        try {
            cTemporaryOwner tmp(cTemporaryOwner::DestructorMode::DISPOSE); // eventually dispose of potential object result
            cValue v = evaluate(expr, context);
            return v.intValueInUnit(getUnit());
        }
        catch (std::exception& e) {
            throw cRuntimeError(e, expr->getSourceLocation().c_str());
        }
    }
}

double cIntParImpl::doubleValue(cComponent *context) const
{
    throw cRuntimeError(this, E_BADCAST, "integer", "double");
}

const char *cIntParImpl::stringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "integer", "string");
}

std::string cIntParImpl::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "integer", "string");
}

cObject *cIntParImpl::objectValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "integer", "object");
}

cXMLElement *cIntParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "integer", "XML");
}

cExpression *cIntParImpl::getExpression() const
{
    return (flags & FL_ISEXPR) ? expr : nullptr;
}

void cIntParImpl::deleteOld()
{
    if (flags & FL_ISEXPR) {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cIntParImpl::getType() const
{
    return cPar::INT;
}

bool cIntParImpl::isNumeric() const
{
    return true;
}

void cIntParImpl::convertToConst(cComponent *context)
{
    setIntValue(intValue(context));
}

std::string cIntParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();
    else {
        char buf[32];
        sprintf(buf, "%" PRId64, (int64_t)val);
        const char *unit = getUnit();
        if (!unit)
            return buf;
        else
            return std::string(buf) + unit;
    }
}

void cIntParImpl::parse(const char *text, FileLine loc)
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

int cIntParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret != 0)
        return ret;

    const cIntParImpl *other2 = dynamic_cast<const cIntParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

}  // namespace internal
}  // namespace omnetpp

