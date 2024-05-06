//   CSTRINGPARIMPL.CC  - part of
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

#include "common/stringutil.h"
#include "omnetpp/cstringparimpl.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/ccomponent.h"
#include "ctemporaryowner.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace internal {

cStringParImpl::~cStringParImpl()
{
    deleteOld();
    delete enumValues;
}

void cStringParImpl::copy(const cStringParImpl& other)
{
    if (flags & FL_ISEXPR)
        expr = other.expr->dup();
    else
        val = other.val;

    delete enumValues;
    enumValues = other.enumValues ? new std::vector<std::string>(*other.enumValues) : nullptr;
}

void cStringParImpl::operator=(const cStringParImpl& other)
{
    if (this == &other)
        return;
    deleteOld();
    cParImpl::operator=(other);
    copy(other);
}

void cStringParImpl::validate(const char *value) const
{
    if (enumValues) {
        bool found = false;
        for (auto& allowedValue : *enumValues) {
            if (opp_streq(allowedValue.c_str(), value)) {
                found = true;
                break;
            }
        }
        if (!found)
            throw cRuntimeError("Value %s is not allowed, must be one of: %s", opp_quotestr(value).c_str(), opp_join(*enumValues, ",", false, '"').c_str());
    }
}

void cStringParImpl::setEnumValues(const std::vector<std::string>& values)
{
    delete enumValues;
    enumValues = values.empty() ? nullptr : new std::vector<std::string>(values);
}

std::vector<std::string> cStringParImpl::getEnumValues() const
{
    return enumValues ? *enumValues : std::vector<std::string>();
}

void cStringParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, E_BADASSIGN, "bool", "string");
}

void cStringParImpl::setIntValue(intval_t l)
{
    throw cRuntimeError(this, E_BADASSIGN, "integer", "string");
}

void cStringParImpl::setDoubleValue(double d)
{
    throw cRuntimeError(this, E_BADASSIGN, "double", "string");
}

void cStringParImpl::setStringValue(const char *s)
{
    validate(s);
    deleteOld();
    val = (s ? s : "");
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cStringParImpl::setObjectValue(cObject *object)
{
    throw cRuntimeError(this, E_BADASSIGN, "object", "string");
}

void cStringParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, E_BADASSIGN, "XML", "string");
}

void cStringParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cStringParImpl::boolValue(cComponent *context) const
{
    throw cRuntimeError(this, E_BADCAST, "string", "bool");
}

intval_t cStringParImpl::intValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "string", "integer");
}

double cStringParImpl::doubleValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "string", "double");
}

const char *cStringParImpl::stringValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(E_PARNOTSET);

    if (flags & FL_ISEXPR)
        throw cRuntimeError(this, "stringValue() and conversion to 'const char *' cannot be invoked "
                                  "on parameters declared 'volatile string' in NED -- use stdstringValue() "
                                  "or conversion to 'std::string' instead.");
    return val.c_str();
}

std::string cStringParImpl::stdstringValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(E_PARNOTSET);

    if ((flags & FL_ISEXPR) == 0)
        return val;
    else {
        try {
            cTemporaryOwner tmp(cTemporaryOwner::DestructorMode::DISPOSE); // eventually dispose of potential object result
            cValue v = evaluate(expr, context);
            if (v.type != cValue::STRING)
                throw cRuntimeError(E_BADCAST, v.getTypeName(), "string");
            const char *s = v.stringValue();
            validate(s);
            return s;
        }
        catch (std::exception& e) {
            throw cRuntimeError(e, expr->getSourceLocation().c_str());
        }
    }
}

cObject *cStringParImpl::objectValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "string", "object");
}

cXMLElement *cStringParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "string", "XML");
}

cExpression *cStringParImpl::getExpression() const
{
    return (flags & FL_ISEXPR) ? expr : nullptr;
}

cPar::Type cStringParImpl::getType() const
{
    return cPar::STRING;
}

bool cStringParImpl::isNumeric() const
{
    return false;
}

void cStringParImpl::convertToConst(cComponent *context)
{
    auto loc = getSourceLocation();
    setStringValue(stdstringValue(context).c_str());
    setSourceLocation(loc);
}

std::string cStringParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();

    return opp_quotestr(val);
}

void cStringParImpl::parse(const char *text, FileLine loc)
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

    setSourceLocation(loc);
}

int cStringParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret != 0)
        return ret;

    const cStringParImpl *other2 = dynamic_cast<const cStringParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

}  // namespace internal
}  // namespace omnetpp

