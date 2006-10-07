//==========================================================================
//   CXMLPAR.CC  - part of
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

#include "cxmlpar.h"
#include "cxmlelement.h"
#include "cstrtokenizer.h"
#include "cdynamicexpression.h"
#include "ccomponent.h"


cXMLPar::cXMLPar()
{
    val = NULL;
}

cXMLPar::~cXMLPar()
{
    beforeChange();
    deleteOld();
}

cXMLPar& cXMLPar::operator=(const cXMLPar& other)
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

std::string cXMLPar::info() const
{
    return toString();
}

void cXMLPar::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cXMLPar::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

cXMLPar& cXMLPar::setBoolValue(bool b)
{
    throw new cRuntimeError(this, eBADCAST, "bool", "XML");
}

cXMLPar& cXMLPar::setLongValue(long l)
{
    throw new cRuntimeError(this, eBADCAST, "int/long", "XML");
}

cXMLPar& cXMLPar::setDoubleValue(double d)
{
    throw new cRuntimeError(this, eBADCAST, "double", "XML");
}

cXMLPar& cXMLPar::setStringValue(const char *s)
{
    throw new cRuntimeError(this, eBADCAST, "string", "XML");
}

cXMLPar& cXMLPar::setXMLValue(cXMLElement *node)
{
    beforeChange();
    deleteOld();
    val = node;
    afterChange();
    return *this;
}

cXMLPar& cXMLPar::setExpression(cExpression *e)
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
        val = e->xmlValue();
        delete e;
    }
    afterChange();
    return *this;
}

bool cXMLPar::boolValue() const
{
    throw new cRuntimeError(this, eBADCAST, "XML", "bool");
}

long cXMLPar::longValue() const
{
    throw new cRuntimeError(this, eBADCAST, "XML", "int/long");
}

double cXMLPar::doubleValue() const
{
    throw new cRuntimeError(this, eBADCAST, "XML", "double");
}

std::string cXMLPar::stringValue() const
{
    throw new cRuntimeError(this, eBADCAST, "XML", "string");
}

cXMLElement *cXMLPar::xmlValue() const
{
    return evaluate();
}

cExpression *cXMLPar::expression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

cXMLElement *cXMLPar::evaluate() const
{
    return (flags & FL_ISEXPR) ? expr->xmlValue(dynamic_cast<cComponent*>(owner())) : val;
}

void cXMLPar::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

char cXMLPar::type() const
{
    return 'X';
}

bool cXMLPar::isNumeric() const
{
    return false;
}

cXMLPar& cXMLPar::read()
{
    // FIXME TBD
    return *this;
}

void cXMLPar::convertToConst()
{
    setXMLValue(xmlValue());
}

std::string cXMLPar::toString() const
{
    if (flags & FL_ISEXPR)
        return expr->toString();

    if (val)
        return std::string("<")+val->getTagName()+"> from "+val->getSourceLocation();
    else
        return std::string("NULL");
}

bool cXMLPar::parse(const char *text)
{
    // maybe it's just xmldoc(...)
    // FIXME TBD
    // if (success)
    // {
    //     setXMLValue(node);
    //     return true;
    // }

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

