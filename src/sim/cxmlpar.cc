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
    deleteOld();
}

void cXMLPar::operator=(const cXMLPar& other)
{
    if (this==&other) return;

    deleteOld();

    cParValue::operator=(other);
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
}

std::string cXMLPar::detailedInfo() const
{
    return (flags & FL_ISEXPR) ? "<expression>" : val==NULL ? "NULL" : val->detailedInfo();
}

void cXMLPar::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cXMLPar::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cXMLPar::setBoolValue(bool b)
{
    throw new cRuntimeError(this, eBADCAST, "bool", "XML");
}

void cXMLPar::setLongValue(long l)
{
    throw new cRuntimeError(this, eBADCAST, "int/long", "XML");
}

void cXMLPar::setDoubleValue(double d)
{
    throw new cRuntimeError(this, eBADCAST, "double", "XML");
}

void cXMLPar::setStringValue(const char *s)
{
    throw new cRuntimeError(this, eBADCAST, "string", "XML");
}

void cXMLPar::setXMLValue(cXMLElement *node)
{
    deleteOld();
    val = node;
}

void cXMLPar::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR;
}

bool cXMLPar::boolValue(cComponent *) const
{
    throw new cRuntimeError(this, eBADCAST, "XML", "bool");
}

long cXMLPar::longValue(cComponent *) const
{
    throw new cRuntimeError(this, eBADCAST, "XML", "int/long");
}

double cXMLPar::doubleValue(cComponent *) const
{
    throw new cRuntimeError(this, eBADCAST, "XML", "double");
}

const char *cXMLPar::stringValue(cComponent *) const
{
    throw new cRuntimeError(this, eBADCAST, "XML", "string");
}

std::string cXMLPar::stdstringValue(cComponent *) const
{
    throw new cRuntimeError(this, eBADCAST, "XML", "string");
}

cXMLElement *cXMLPar::xmlValue(cComponent *context) const
{
    return evaluate(context);
}

cExpression *cXMLPar::expression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

cXMLElement *cXMLPar::evaluate(cComponent *context) const
{
    return (flags & FL_ISEXPR) ? expr->xmlValue(context) : val;
}

void cXMLPar::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cXMLPar::type() const
{
    return cPar::XML;
}

bool cXMLPar::isNumeric() const
{
    return false;
}

void cXMLPar::convertToConst(cComponent *context)
{
    setXMLValue(xmlValue(context));
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

