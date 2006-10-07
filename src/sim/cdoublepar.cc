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


cDoublePar::cDoublePar()
{
    val = 0;
}

cDoublePar::~cDoublePar()
{
    beforeChange();
    deleteOld();
}

cDoublePar& cDoublePar::operator=(const cDoublePar& other)
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

std::string cDoublePar::info() const
{
    return toString();
}

void cDoublePar::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cDoublePar::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

cDoublePar& cDoublePar::setBoolValue(bool b)
{
    throw new cRuntimeError(this, eBADCAST, "bool", "double");
}

cDoublePar& cDoublePar::setLongValue(long l)
{
    beforeChange();
    deleteOld();
    val = l;
    afterChange();
    return *this;
}

cDoublePar& cDoublePar::setDoubleValue(double d)
{
    beforeChange();
    deleteOld();
    val = d;
    afterChange();
    return *this;
}

cDoublePar& cDoublePar::setStringValue(const char *s)
{
    throw new cRuntimeError(this, eBADCAST, "string", "double");
}

cDoublePar& cDoublePar::setXMLValue(cXMLElement *node)
{
    throw new cRuntimeError(this, eBADCAST, "XML", "double");
}

cDoublePar& cDoublePar::setExpression(cExpression *e)
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
        val = e->doubleValue();
        delete e;
    }
    afterChange();
    return *this;
}

bool cDoublePar::boolValue() const
{
    throw new cRuntimeError(this, eBADCAST, "double", "bool");
}

long cDoublePar::longValue() const
{
    return double_to_long(evaluate());
}

double cDoublePar::doubleValue() const
{
    return evaluate();
}

std::string cDoublePar::stringValue() const
{
    throw new cRuntimeError(this, eBADCAST, "double", "string");
}

cXMLElement *cDoublePar::xmlValue() const
{
    throw new cRuntimeError(this, eBADCAST, "double", "XML");
}

cExpression *cDoublePar::expression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

double cDoublePar::evaluate() const
{
    return (flags & FL_ISEXPR) ? expr->doubleValue(dynamic_cast<cComponent*>(owner())) : val;
}

void cDoublePar::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

char cDoublePar::type() const
{
    return 'D';
}

bool cDoublePar::isNumeric() const
{
    return true;
}

cDoublePar& cDoublePar::read()
{
    // FIXME TBD
    return *this;
}

void cDoublePar::convertToConst()
{
    setDoubleValue(doubleValue());
}

std::string cDoublePar::toString() const
{
    if (flags & FL_ISEXPR)
        return expr->toString();

    char buf[32];
    sprintf(buf, "%g", val);
    return buf;
}

bool cDoublePar::parse(const char *text)
{
    // maybe it's just a number
    cStringTokenizer tok(text);
    const char *word = tok.nextToken();
    if (word!=NULL && !tok.hasMoreTokens())
    {
        char *endp;
        double num = strtod(word, &endp);  // FIXME TBD try as "units" as well
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

