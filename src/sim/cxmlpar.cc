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

USING_NAMESPACE


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
    throw cRuntimeError(this, eBADCAST, "bool", "XML");
}

void cXMLPar::setLongValue(long l)
{
    throw cRuntimeError(this, eBADCAST, "int/long", "XML");
}

void cXMLPar::setDoubleValue(double d)
{
    throw cRuntimeError(this, eBADCAST, "double", "XML");
}

void cXMLPar::setStringValue(const char *s)
{
    throw cRuntimeError(this, eBADCAST, "string", "XML");
}

void cXMLPar::setXMLValue(cXMLElement *node)
{
    deleteOld();
    val = node;
    flags |= FL_HASVALUE;
}

void cXMLPar::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_HASVALUE;
}

bool cXMLPar::boolValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "XML", "bool");
}

long cXMLPar::longValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "XML", "int/long");
}

double cXMLPar::doubleValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "XML", "double");
}

const char *cXMLPar::stringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "XML", "string");
}

std::string cXMLPar::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "XML", "string");
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

void cXMLPar::parse(const char *text)
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

int cXMLPar::compare(const cParValue *other) const
{
    int ret = cParValue::compare(other);
    if (ret!=0)
        return ret;

    const cXMLPar *other2 = dynamic_cast<const cXMLPar *>(other);
    if (flags & FL_ISEXPR)
        throw cRuntimeError(this, "cannot compare expressions yet"); //FIXME
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

