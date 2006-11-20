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

#include "cboolpar.h"
#include "cstrtokenizer.h"
#include "cdynamicexpression.h"
#include "ccomponent.h"


cBoolPar::cBoolPar()
{
    val = false;
}

cBoolPar::~cBoolPar()
{
    deleteOld();
}

void cBoolPar::operator=(const cBoolPar& other)
{
    if (this==&other) return;

    deleteOld();

    cParValue::operator=(other);
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
}

std::string cBoolPar::info() const
{
    return toString();
}

void cBoolPar::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cBoolPar::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cBoolPar::setBoolValue(bool b)
{
    deleteOld();
    val = b;
}

void cBoolPar::setLongValue(long l)
{
    throw new cRuntimeError(this, eBADCAST, "int/long", "double");
}

void cBoolPar::setDoubleValue(double d)
{
    throw new cRuntimeError(this, eBADCAST, "double", "bool");
}

void cBoolPar::setStringValue(const char *s)
{
    throw new cRuntimeError(this, eBADCAST, "string", "bool");
}

void cBoolPar::setXMLValue(cXMLElement *node)
{
    throw new cRuntimeError(this, eBADCAST, "XML", "bool");
}

void cBoolPar::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR;
}

bool cBoolPar::boolValue() const
{
    return evaluate();
}

long cBoolPar::longValue() const
{
    throw new cRuntimeError(this, eBADCAST, "bool", "int/long");
}

double cBoolPar::doubleValue() const
{
    throw new cRuntimeError(this, eBADCAST, "bool", "double");
}

const char *cBoolPar::stringValue() const
{
    throw new cRuntimeError(this, eBADCAST, "bool", "string");
}

std::string cBoolPar::stdstringValue() const
{
    throw new cRuntimeError(this, eBADCAST, "bool", "string");
}

cXMLElement *cBoolPar::xmlValue() const
{
    throw new cRuntimeError(this, eBADCAST, "bool", "XML");
}

cExpression *cBoolPar::expression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

bool cBoolPar::evaluate() const
{
    return (flags & FL_ISEXPR) ? expr->boolValue(dynamic_cast<cComponent*>(owner())) : val;
}

void cBoolPar::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cBoolPar::type() const
{
    return cPar::Type::BOOL;
}

bool cBoolPar::isNumeric() const
{
    return true;
}

void cBoolPar::convertToConst()
{
    setBoolValue(boolValue());
}

std::string cBoolPar::toString() const
{
    if (flags & FL_ISEXPR)
        return expr->toString();
    return val ? "true" : "false";
}

bool cBoolPar::parse(const char *text)
{
    // maybe it's a single word, "true" or "false"
    cStringTokenizer tok(text);
    const char *word = tok.nextToken();
    if (word!=NULL && !tok.hasMoreTokens())
    {
        if (!strcmp(word,"true"))
        {
            setBoolValue(true);
            return true;
        }
        else if (!strcmp(word,"false"))
        {
            setBoolValue(false);
            return true;
        }
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

