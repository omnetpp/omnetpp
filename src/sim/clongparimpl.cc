//==========================================================================
//   CLONGPAR.CC  - part of
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

#include "clongparimpl.h"
#include "cstringtokenizer.h"
#include "cdynamicexpression.h"
#include "ccomponent.h"

USING_NAMESPACE


cLongParImpl::cLongParImpl()
{
    val = 0;
}

cLongParImpl::~cLongParImpl()
{
    deleteOld();
}

void cLongParImpl::operator=(const cLongParImpl& other)
{
    if (this==&other) return;

    deleteOld();

    cParImpl::operator=(other);
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
}

void cLongParImpl::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cLongParImpl::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cLongParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, eBADCAST, "bool", "int/long");
}

void cLongParImpl::setLongValue(long l)
{
    deleteOld();
    val = l;
    flags |= FL_HASVALUE;
}

void cLongParImpl::setDoubleValue(double d)
{
    deleteOld();
    val = double_to_long(d);
    flags |= FL_HASVALUE;
}

void cLongParImpl::setStringValue(const char *s)
{
    throw cRuntimeError(this, eBADCAST, "string", "int/long");
}

void cLongParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, eBADCAST, "XML", "int/long");
}

void cLongParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_HASVALUE;
}

bool cLongParImpl::boolValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "int/long", "bool");
}

long cLongParImpl::longValue(cComponent *context) const
{
    return evaluate(context);
}

double cLongParImpl::doubleValue(cComponent *context) const
{
    return evaluate(context);
}

const char *cLongParImpl::stringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "int/long", "string");
}

std::string cLongParImpl::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "int/long", "string");
}

cXMLElement *cLongParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "int/long", "XML");
}

cExpression *cLongParImpl::getExpression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

long cLongParImpl::evaluate(cComponent *context) const
{
    return (flags & FL_ISEXPR) ? expr->longValue(context, getUnit()) : val;
}

void cLongParImpl::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
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

    char buf[32];
    sprintf(buf, "%ld", val);
    return buf;
}

void cLongParImpl::parse(const char *text)
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

int cLongParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret!=0)
        return ret;

    const cLongParImpl *other2 = dynamic_cast<const cLongParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

