//==========================================================================
//   CSTRINGPAR.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cstringparimpl.h"
#include "cdynamicexpression.h"
#include "ccomponent.h"
#include "stringutil.h"

USING_NAMESPACE


cStringParImpl::cStringParImpl()
{
}

cStringParImpl::~cStringParImpl()
{
    deleteOld();
}

void cStringParImpl::operator=(const cStringParImpl& other)
{
    if (this==&other) return;

    deleteOld();

    cParImpl::operator=(other);
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
}

void cStringParImpl::parsimPack(cCommBuffer *buffer)
{
    //TBD
}

void cStringParImpl::parsimUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cStringParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, eBADCAST, "bool", "string");
}

void cStringParImpl::setLongValue(long l)
{
    throw cRuntimeError(this, eBADCAST, "int/long", "string");
}

void cStringParImpl::setDoubleValue(double d)
{
    throw cRuntimeError(this, eBADCAST, "double", "string");
}

void cStringParImpl::setStringValue(const char *s)
{
    deleteOld();
    val = (s ? s : "");
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cStringParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, eBADCAST, "XML", "string");
}

void cStringParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cStringParImpl::boolValue(cComponent *context) const
{
    throw cRuntimeError(this, eBADCAST, "string", "bool");
}

long cStringParImpl::longValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "string", "int/long");
}

double cStringParImpl::doubleValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "string", "double");
}

const char *cStringParImpl::stringValue(cComponent *context) const
{
    if (flags & FL_ISEXPR)
        throw cRuntimeError(this, "stringValue() and conversion to `const char *' cannot be invoked "
                                  "on parameters declared `volatile string' in NED -- use stdstringValue() "
                                  "or conversion to `std::string' instead.");
    return val.c_str();
}

std::string cStringParImpl::stdstringValue(cComponent *context) const
{
    return evaluate(context);
}

cXMLElement *cStringParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "string", "XML");
}

cExpression *cStringParImpl::getExpression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

std::string cStringParImpl::evaluate(cComponent *context) const
{
    return (flags & FL_ISEXPR) ? expr->stringValue(context) : val;
}

void cStringParImpl::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
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
    setStringValue(stdstringValue(context).c_str());
}

std::string cStringParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();

    return opp_quotestr(val.c_str());
}

void cStringParImpl::parse(const char *text)
{
    // try parsing it as an expression
    cDynamicExpression *dynexpr = new cDynamicExpression();
    try
    {
        dynexpr->parse(text);
    }
    catch (std::exception& e)
    {
        delete dynexpr;
        throw;
    }
    setExpression(dynexpr);

    // simplify if possible: store as constant instead of expression
    if (dynexpr->isAConstant())
        convertToConst(NULL);
}

int cStringParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret!=0)
        return ret;

    const cStringParImpl *other2 = dynamic_cast<const cStringParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

