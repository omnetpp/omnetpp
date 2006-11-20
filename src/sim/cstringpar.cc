//==========================================================================
//   CSTRINGPAR.CC  - part of
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

#include "cstringpar.h"
#include "cdynamicexpression.h"
#include "ccomponent.h"


cStringPar::cStringPar()
{
}

cStringPar::~cStringPar()
{
    deleteOld();
}

void cStringPar::operator=(const cStringPar& other)
{
    if (this==&other) return;

    deleteOld();

    cParValue::operator=(other);
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
}

std::string cStringPar::info() const
{
    return toString();
}

void cStringPar::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cStringPar::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cStringPar::setBoolValue(bool b)
{
    throw new cRuntimeError(this, eBADCAST, "bool", "string");
}

void cStringPar::setLongValue(long l)
{
    throw new cRuntimeError(this, eBADCAST, "int/long", "string");
}

void cStringPar::setDoubleValue(double d)
{
    throw new cRuntimeError(this, eBADCAST, "double", "string");
}

void cStringPar::setStringValue(const char *s)
{
    deleteOld();
    val = (s ? s : "");
}

void cStringPar::setXMLValue(cXMLElement *node)
{
    throw new cRuntimeError(this, eBADCAST, "XML", "string");
}

void cStringPar::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR;
}

bool cStringPar::boolValue() const
{
    throw new cRuntimeError(this, eBADCAST, "string", "bool");
}

long cStringPar::longValue() const
{
    throw new cRuntimeError(this, eBADCAST, "string", "int/long");
}

double cStringPar::doubleValue() const
{
    throw new cRuntimeError(this, eBADCAST, "string", "double");
}

const char *cStringPar::stringValue() const
{
    if (flags & FL_ISEXPR)
        throw new cRuntimeError(this, "stringValue() and conversion to `const char *' cannot be invoked "
                                "on parameters declared `volatile string' in NED -- use stdstringValue() "
                                "or conversion to `std::string' instead.");
    return val.c_str();
}

std::string cStringPar::stdstringValue() const
{
    return evaluate();
}

cXMLElement *cStringPar::xmlValue() const
{
    throw new cRuntimeError(this, eBADCAST, "string", "XML");
}

cExpression *cStringPar::expression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

std::string cStringPar::evaluate() const
{
    return (flags & FL_ISEXPR) ? expr->stringValue(dynamic_cast<cComponent*>(owner())) : val;
}

void cStringPar::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cStringPar::type() const
{
    return cPar::STRING;
}

bool cStringPar::isNumeric() const
{
    return false;
}

void cStringPar::convertToConst()
{
    setStringValue(stdstringValue().c_str());
}

std::string cStringPar::toString() const
{
    if (flags & FL_ISEXPR)
        return expr->toString();

    char *d = opp_quotestr(val.c_str());
    std::string ret = d;
    delete [] d;
    return ret;
}

bool cStringPar::parse(const char *text)
{
    // maybe it's just a string literal in quotes
    const char *endp;
    char *str = opp_parsequotedstr(text, endp);
    if (str!=NULL && *endp=='\0')
    {
        setStringValue(str);
        delete [] str;
        return true;
    }

    // try parsing it as an expression
    cDynamicExpression *dynexpr = new cDynamicExpression();
    if (dynexpr->parse(text))
    {
        setExpression(dynexpr);
        return true;
    }

    // bad luck
    return false;
}

