//==========================================================================
//   CDOUBLEPAR.CC  - part of
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

#include "cdoubleparimpl.h"
#include "cstringtokenizer.h"
#include "cdynamicexpression.h"
#include "ccomponent.h"

NAMESPACE_BEGIN


cDoubleParImpl::cDoubleParImpl()
{
    val = 0;
}

cDoubleParImpl::~cDoubleParImpl()
{
    deleteOld();
}

void cDoubleParImpl::copy(const cDoubleParImpl& other)
{
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
}

void cDoubleParImpl::operator=(const cDoubleParImpl& other)
{
    if (this==&other) return;
    deleteOld();
    cParImpl::operator=(other);
    copy(other);
}

void cDoubleParImpl::parsimPack(cCommBuffer *buffer)
{
    //TBD
}

void cDoubleParImpl::parsimUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cDoubleParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, eBADCAST, "bool", "double");
}

void cDoubleParImpl::setLongValue(long l)
{
    deleteOld();
    val = l;
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cDoubleParImpl::setDoubleValue(double d)
{
    deleteOld();
    val = d;
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cDoubleParImpl::setStringValue(const char *s)
{
    throw cRuntimeError(this, eBADCAST, "string", "double");
}

void cDoubleParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, eBADCAST, "XML", "double");
}

void cDoubleParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cDoubleParImpl::boolValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "double", "bool");
}

long cDoubleParImpl::longValue(cComponent *context) const
{
    return double_to_long(doubleValue(context));
}

double cDoubleParImpl::doubleValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(ePARNOTSET);

    if ((flags & FL_ISEXPR) == 0)
        return val;
    else {
        cNEDValue v = evaluate(expr, context);
        if (v.type != cNEDValue::DBL)
            throw cRuntimeError(eECANTCAST, "double");
        return v.doubleValueInUnit(getUnit());
    }
}

const char *cDoubleParImpl::stringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "double", "string");
}

std::string cDoubleParImpl::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "double", "string");
}

cXMLElement *cDoubleParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "double", "XML");
}

cExpression *cDoubleParImpl::getExpression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

void cDoubleParImpl::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cDoubleParImpl::getType() const
{
    return cPar::DOUBLE;
}

bool cDoubleParImpl::isNumeric() const
{
    return true;
}

void cDoubleParImpl::convertToConst(cComponent *context)
{
    setDoubleValue(doubleValue(context));
}

std::string cDoubleParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();

    char buf[32];
    sprintf(buf, "%g", val);
    return buf;
}

void cDoubleParImpl::parse(const char *text)
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

int cDoubleParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret!=0)
        return ret;

    const cDoubleParImpl *other2 = dynamic_cast<const cDoubleParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

NAMESPACE_END

