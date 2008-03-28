//==========================================================================
//   CBOOLPAR.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cboolparimpl.h"
#include "cstringtokenizer.h"
#include "cdynamicexpression.h"
#include "ccomponent.h"

USING_NAMESPACE


cBoolParImpl::cBoolParImpl()
{
    val = false;
}

cBoolParImpl::~cBoolParImpl()
{
    deleteOld();
}

void cBoolParImpl::operator=(const cBoolParImpl& other)
{
    if (this==&other) return;

    deleteOld();

    cParImpl::operator=(other);
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
}

void cBoolParImpl::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cBoolParImpl::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cBoolParImpl::setBoolValue(bool b)
{
    deleteOld();
    val = b;
    flags |= FL_HASVALUE;
}

void cBoolParImpl::setLongValue(long l)
{
    throw cRuntimeError(this, eBADCAST, "int/long", "double");
}

void cBoolParImpl::setDoubleValue(double d)
{
    throw cRuntimeError(this, eBADCAST, "double", "bool");
}

void cBoolParImpl::setStringValue(const char *s)
{
    throw cRuntimeError(this, eBADCAST, "string", "bool");
}

void cBoolParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, eBADCAST, "XML", "bool");
}

void cBoolParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_HASVALUE;
}

bool cBoolParImpl::boolValue(cComponent *context) const
{
    return evaluate(context);
}

long cBoolParImpl::longValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "bool", "int/long");
}

double cBoolParImpl::doubleValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "bool", "double");
}

const char *cBoolParImpl::stringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "bool", "string");
}

std::string cBoolParImpl::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "bool", "string");
}

cXMLElement *cBoolParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "bool", "XML");
}

cExpression *cBoolParImpl::expression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

bool cBoolParImpl::evaluate(cComponent *context) const
{
    return (flags & FL_ISEXPR) ? expr->boolValue(context) : val;
}

void cBoolParImpl::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cBoolParImpl::type() const
{
    return cPar::BOOL;
}

bool cBoolParImpl::isNumeric() const
{
    return true;
}

void cBoolParImpl::convertToConst(cComponent *context)
{
    setBoolValue(boolValue(context));
}

std::string cBoolParImpl::toString() const
{
    if (flags & FL_ISEXPR)
        return expr->toString();
    return val ? "true" : "false";
}

void cBoolParImpl::parse(const char *text)
{
    // shortcut: recognize "true" and "false"
    if (strcmp(text, "true")==0 || strcmp(text, "false")==0)
    {
        setBoolValue(text[0]=='t');
        return;
    }

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

int cBoolParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret!=0)
        return ret;

    const cBoolParImpl *other2 = dynamic_cast<const cBoolParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

