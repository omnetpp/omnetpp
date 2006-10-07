//==========================================================================
//   CLONGPAR.CC  - part of
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

#include "clongpar.h"
#include "cstrtokenizer.h"
#include "cdynamicexpression.h"
#include "ccomponent.h"


cLongPar::cLongPar()
{
    val = 0;
}

cLongPar::~cLongPar()
{
    beforeChange();
    deleteOld();
}

cLongPar& cLongPar::operator=(const cLongPar& other)
{
    if (this==&other) return *this;

    beforeChange();
    deleteOld();

    cPar::operator=(other);
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;

    afterChange();
    return *this;
}

std::string cLongPar::info() const
{
    return toString();
}

void cLongPar::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cLongPar::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

cLongPar& cLongPar::setBoolValue(bool b)
{
    throw new cRuntimeError(this, eBADCAST, "bool", "int/long");
}

cLongPar& cLongPar::setLongValue(long l)
{
    beforeChange();
    deleteOld();
    val = l;
    afterChange();
    return *this;
}

cLongPar& cLongPar::setDoubleValue(double d)
{
    beforeChange();
    deleteOld();
    val = double_to_long(d);
    afterChange();
    return *this;
}

cLongPar& cLongPar::setStringValue(const char *s)
{
    throw new cRuntimeError(this, eBADCAST, "string", "int/long");
}

cLongPar& cLongPar::setXMLValue(cXMLElement *node)
{
    throw new cRuntimeError(this, eBADCAST, "XML", "int/long");
}

cLongPar& cLongPar::setExpression(cExpression *e)
{
    beforeChange();
    deleteOld();
    if (flags & FL_ISVOLATILE)
    {
        expr = e;
        flags |= FL_ISEXPR;
    }
    else
    {
        // not a "function" param: evaluate expression once, and store the result
        val = e->longValue();
        delete e;
    }
    afterChange();
    return *this;
}

bool cLongPar::boolValue() const
{
    throw new cRuntimeError(this, eBADCAST, "int/long", "bool");
}

long cLongPar::longValue() const
{
    return evaluate();
}

double cLongPar::doubleValue() const
{
    return evaluate();
}

std::string cLongPar::stringValue() const
{
    throw new cRuntimeError(this, eBADCAST, "int/long", "string");
}

cXMLElement *cLongPar::xmlValue() const
{
    throw new cRuntimeError(this, eBADCAST, "int/long", "XML");
}

cExpression *cLongPar::expression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

long cLongPar::evaluate() const
{
    return (flags & FL_ISEXPR) ? expr->longValue(dynamic_cast<cComponent*>(owner())) : val;
}

void cLongPar::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

char cLongPar::type() const
{
    return 'L';
}

bool cLongPar::isNumeric() const
{
    return true;
}

cLongPar& cLongPar::read()
{
    // FIXME TBD
    return *this;
}

void cLongPar::convertToConst()
{
    setLongValue(longValue());
}

std::string cLongPar::toString() const
{
    if (flags & FL_ISEXPR)
        return expr->toString();

    char buf[32];
    sprintf(buf, "%ld", val);
    return buf;
}

bool cLongPar::parse(const char *text)
{
    // maybe it's just a number
    cStringTokenizer tok(text);
    const char *word = tok.nextToken();
    if (word!=NULL && !tok.hasMoreTokens())
    {
        char *endp;
        long num = strtol(word, &endp, 10); // FIXME TBD try as "units" as well
        if (*endp == '\0')
        {
            val = num;
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

