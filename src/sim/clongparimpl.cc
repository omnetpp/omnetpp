//==========================================================================
//   CLONGPAR.CC  - part of
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

#include "omnetpp/clongparimpl.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/ccomponent.h"

namespace omnetpp {

cLongParImpl::cLongParImpl()
{
    val = 0;
}

cLongParImpl::~cLongParImpl()
{
    deleteOld();
}

void cLongParImpl::copy(const cLongParImpl& other)
{
    if (flags & FL_ISEXPR)
        expr = other.expr->dup();
    else
        val = other.val;
}

void cLongParImpl::operator=(const cLongParImpl& other)
{
    if (this == &other)
        return;
    deleteOld();
    cParImpl::operator=(other);
    copy(other);
}

void cLongParImpl::parsimPack(cCommBuffer *buffer) const
{
    //TBD
}

void cLongParImpl::parsimUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cLongParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, E_BADCAST, "bool", "int/long");
}

void cLongParImpl::setLongValue(long l)
{
    deleteOld();
    val = l;
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cLongParImpl::setDoubleValue(double d)
{
    deleteOld();
    val = double_to_long(d);
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cLongParImpl::setStringValue(const char *s)
{
    throw cRuntimeError(this, E_BADCAST, "string", "int/long");
}

void cLongParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, E_BADCAST, "XML", "int/long");
}

void cLongParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cLongParImpl::boolValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "int/long", "bool");
}

long cLongParImpl::longValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(E_PARNOTSET);

    if ((flags & FL_ISEXPR) == 0)
        return val;
    else {
        cNEDValue v = evaluate(expr, context);
        if (v.type != cNEDValue::DBL)
            throw cRuntimeError(E_ECANTCAST, "long");
        return double_to_long(v.doubleValueInUnit(getUnit()));
    }
}

double cLongParImpl::doubleValue(cComponent *context) const
{
    return (double)longValue(context);
}

const char *cLongParImpl::stringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "int/long", "string");
}

std::string cLongParImpl::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "int/long", "string");
}

cXMLElement *cLongParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "int/long", "XML");
}

cExpression *cLongParImpl::getExpression() const
{
    return (flags | FL_ISEXPR) ? expr : nullptr;
}

void cLongParImpl::deleteOld()
{
    if (flags & FL_ISEXPR) {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cLongParImpl::getType() const
{
    return cPar::LONG;
}

bool cLongParImpl::isNumeric() const
{
    return true;
}

void cLongParImpl::convertToConst(cComponent *context)
{
    setLongValue(longValue(context));
}

std::string cLongParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();
    else {
        char buf[32];
        sprintf(buf, "%ld", val);
        const char *unit = getUnit();
        if (!unit)
            return buf;
        else
            return std::string(buf) + unit;
    }
}

void cLongParImpl::parse(const char *text)
{
    // try parsing it as an expression
    cDynamicExpression *dynexpr = new cDynamicExpression();
    try {
        dynexpr->parse(text);
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

int cLongParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret != 0)
        return ret;

    const cLongParImpl *other2 = dynamic_cast<const cLongParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

}  // namespace omnetpp

