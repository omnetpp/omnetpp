//==========================================================================
//   CDOUBLEPAR.CC  - part of
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

#include "cdoublepar.h"
#include "cstrtokenizer.h"
#include "cdynamicexpression.h"
#include "ccomponent.h"

USING_NAMESPACE


cDoublePar::cDoublePar()
{
    val = 0;
}

cDoublePar::~cDoublePar()
{
    deleteOld();
}

void cDoublePar::operator=(const cDoublePar& other)
{
    if (this==&other) return;

    deleteOld();

    cParImpl::operator=(other);
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
}

void cDoublePar::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cDoublePar::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cDoublePar::setBoolValue(bool b)
{
    throw cRuntimeError(this, eBADCAST, "bool", "double");
}

void cDoublePar::setLongValue(long l)
{
    deleteOld();
    val = l;
    flags |= FL_HASVALUE;
}

void cDoublePar::setDoubleValue(double d)
{
    deleteOld();
    val = d;
    flags |= FL_HASVALUE;
}

void cDoublePar::setStringValue(const char *s)
{
    throw cRuntimeError(this, eBADCAST, "string", "double");
}

void cDoublePar::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, eBADCAST, "XML", "double");
}

void cDoublePar::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_HASVALUE;
}

bool cDoublePar::boolValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "double", "bool");
}

long cDoublePar::longValue(cComponent *context) const
{
    return double_to_long(evaluate(context));
}

double cDoublePar::doubleValue(cComponent *context) const
{
    return evaluate(context);
}

const char *cDoublePar::stringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "double", "string");
}

std::string cDoublePar::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "double", "string");
}

cXMLElement *cDoublePar::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "double", "XML");
}

cExpression *cDoublePar::expression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

double cDoublePar::evaluate(cComponent *context) const
{
    return (flags & FL_ISEXPR) ? expr->doubleValue(context, unit()) : val;
}

void cDoublePar::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cDoublePar::type() const
{
    return cPar::DOUBLE;
}

bool cDoublePar::isNumeric() const
{
    return true;
}

void cDoublePar::convertToConst(cComponent *context)
{
    setDoubleValue(doubleValue(context));
}

std::string cDoublePar::toString() const
{
    if (flags & FL_ISEXPR)
        return expr->toString();

    char buf[32];
    sprintf(buf, "%g", val);
    return buf;
}

void cDoublePar::parse(const char *text)
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

int cDoublePar::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret!=0)
        return ret;

    const cDoublePar *other2 = dynamic_cast<const cDoublePar *>(other);
    if (flags & FL_ISEXPR)
        throw cRuntimeError(this, "cannot compare expressions yet"); //FIXME
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

