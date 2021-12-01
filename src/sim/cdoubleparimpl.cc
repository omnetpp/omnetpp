//==========================================================================
//   CDOUBLEPARIMPL.CC  - part of
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
#include "omnetpp/cdoubleparimpl.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/ccomponent.h"
#include "common/stringutil.h"
#include "ctemporaryowner.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace internal {

cDoubleParImpl::cDoubleParImpl()
{
    val = 0;
}

cDoubleParImpl::~cDoubleParImpl()
{
    deleteOld();
}

void cDoubleParImpl::copy(const cDoubleParImpl& other)
{
    if (flags & FL_ISEXPR)
        expr = other.expr->dup();
    else
        val = other.val;
}

void cDoubleParImpl::operator=(const cDoubleParImpl& other)
{
    if (this == &other)
        return;
    deleteOld();
    cParImpl::operator=(other);
    copy(other);
}

void cDoubleParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, E_BADASSIGN, "bool", "double");
}

void cDoubleParImpl::setIntValue(intval_t l)
{
    throw cRuntimeError(this, E_BADASSIGN, "integer", "double");
}

void cDoubleParImpl::setDoubleValue(double d)
{
    deleteOld();
    val = d;
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cDoubleParImpl::setStringValue(const char *s)
{
    throw cRuntimeError(this, E_BADASSIGN, "string", "double");
}

void cDoubleParImpl::setObjectValue(cObject *object)
{
    throw cRuntimeError(this, E_BADASSIGN, "object", "double");
}

void cDoubleParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, E_BADASSIGN, "XML", "double");
}

void cDoubleParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cDoubleParImpl::boolValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "double", "bool");
}

intval_t cDoubleParImpl::intValue(cComponent *context) const
{
    throw cRuntimeError(this, E_BADCAST, "double", "integer");
}

double cDoubleParImpl::doubleValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(E_PARNOTSET);

    if ((flags & FL_ISEXPR) == 0)
        return val;
    else {
        try {
            cTemporaryOwner tmp(cTemporaryOwner::DestructorMode::DISPOSE); // eventually dispose of potential object result
            cValue v = evaluate(expr, context);
            return v.doubleValueInUnit(getUnit()); // allows conversion from INT
        }
        catch (std::exception& e) {
            throw cRuntimeError(e, expr->getSourceLocation().c_str());
        }
    }
}

const char *cDoubleParImpl::stringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "double", "string");
}

std::string cDoubleParImpl::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "double", "string");
}

cObject *cDoubleParImpl::objectValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "double", "object");
}

cXMLElement *cDoubleParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "double", "XML");
}

cExpression *cDoubleParImpl::getExpression() const
{
    return (flags & FL_ISEXPR) ? expr : nullptr;
}

void cDoubleParImpl::deleteOld()
{
    if (flags & FL_ISEXPR) {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cDoubleParImpl::getType() const
{
    return cPar::DOUBLE;
}

bool cDoubleParImpl::isNumeric() const
{
    return true;
}

void cDoubleParImpl::convertToConst(cComponent *context)
{
    setDoubleValue(doubleValue(context));
}

std::string cDoubleParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();
    else {
        char buf[32];
        opp_dtoa(buf, "%g", val);
        if (!std::isfinite(val))
            strcat(buf, " ");
        const char *unit = getUnit();
        if (!opp_isempty(unit))
            strcat(buf, unit);
        return buf;
    }
}

void cDoubleParImpl::parse(const char *text, FileLine loc)
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

int cDoubleParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret != 0)
        return ret;

    const cDoubleParImpl *other2 = dynamic_cast<const cDoubleParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

}  // namespace internal
}  // namespace omnetpp

