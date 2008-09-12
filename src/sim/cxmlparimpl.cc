//==========================================================================
//   CXMLPAR.CC  - part of
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

#include "cxmlparimpl.h"
#include "cxmlelement.h"
#include "cstringtokenizer.h"
#include "cdynamicexpression.h"
#include "ccomponent.h"

USING_NAMESPACE


cXMLParImpl::cXMLParImpl()
{
    val = NULL;
}

cXMLParImpl::~cXMLParImpl()
{
    deleteOld();
}

void cXMLParImpl::operator=(const cXMLParImpl& other)
{
    if (this==&other) return;

    deleteOld();

    cParImpl::operator=(other);
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
}

std::string cXMLParImpl::detailedInfo() const
{
    return (flags & FL_ISEXPR) ? "<expression>" : val==NULL ? "NULL" : val->detailedInfo();
}

void cXMLParImpl::parsimPack(cCommBuffer *buffer)
{
    //TBD
}

void cXMLParImpl::parsimUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cXMLParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, eBADCAST, "bool", "XML");
}

void cXMLParImpl::setLongValue(long l)
{
    throw cRuntimeError(this, eBADCAST, "int/long", "XML");
}

void cXMLParImpl::setDoubleValue(double d)
{
    throw cRuntimeError(this, eBADCAST, "double", "XML");
}

void cXMLParImpl::setStringValue(const char *s)
{
    throw cRuntimeError(this, eBADCAST, "string", "XML");
}

void cXMLParImpl::setXMLValue(cXMLElement *node)
{
    deleteOld();
    val = node;
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cXMLParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cXMLParImpl::boolValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "XML", "bool");
}

long cXMLParImpl::longValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "XML", "int/long");
}

double cXMLParImpl::doubleValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "XML", "double");
}

const char *cXMLParImpl::stringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "XML", "string");
}

std::string cXMLParImpl::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "XML", "string");
}

cXMLElement *cXMLParImpl::xmlValue(cComponent *context) const
{
    return evaluate(context);
}

cExpression *cXMLParImpl::getExpression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

cXMLElement *cXMLParImpl::evaluate(cComponent *context) const
{
    return (flags & FL_ISEXPR) ? expr->xmlValue(context) : val;
}

void cXMLParImpl::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cXMLParImpl::getType() const
{
    return cPar::XML;
}

bool cXMLParImpl::isNumeric() const
{
    return false;
}

void cXMLParImpl::convertToConst(cComponent *context)
{
    setXMLValue(xmlValue(context));
}

std::string cXMLParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();

    if (val)
        return std::string("<")+val->getTagName()+"> from "+val->getSourceLocation();
    else
        return std::string("NULL");
}

void cXMLParImpl::parse(const char *text)
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

int cXMLParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret!=0)
        return ret;

    const cXMLParImpl *other2 = dynamic_cast<const cXMLParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

