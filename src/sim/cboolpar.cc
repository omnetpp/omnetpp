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

USING_NAMESPACE


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

    cParImpl::operator=(other);
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
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
    flags |= FL_HASVALUE;
}

void cBoolPar::setLongValue(long l)
{
    throw cRuntimeError(this, eBADCAST, "int/long", "double");
}

void cBoolPar::setDoubleValue(double d)
{
    throw cRuntimeError(this, eBADCAST, "double", "bool");
}

void cBoolPar::setStringValue(const char *s)
{
    throw cRuntimeError(this, eBADCAST, "string", "bool");
}

void cBoolPar::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, eBADCAST, "XML", "bool");
}

void cBoolPar::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_HASVALUE;
}

bool cBoolPar::boolValue(cComponent *context) const
{
    return evaluate(context);
}

long cBoolPar::longValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "bool", "int/long");
}

double cBoolPar::doubleValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "bool", "double");
}

const char *cBoolPar::stringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "bool", "string");
}

std::string cBoolPar::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "bool", "string");
}

cXMLElement *cBoolPar::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "bool", "XML");
}

cExpression *cBoolPar::expression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

bool cBoolPar::evaluate(cComponent *context) const
{
    return (flags & FL_ISEXPR) ? expr->boolValue(context) : val;
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
    return cPar::BOOL;
}

bool cBoolPar::isNumeric() const
{
    return true;
}

void cBoolPar::convertToConst(cComponent *context)
{
    setBoolValue(boolValue(context));
}

std::string cBoolPar::toString() const
{
    if (flags & FL_ISEXPR)
        return expr->toString();
    return val ? "true" : "false";
}

void cBoolPar::parse(const char *text)
{
/*XXX not really needed
    // maybe it's a single word, "true" or "false"
    cStringTokenizer tok(text);
    const char *word = tok.nextToken();
    if (word!=NULL && !tok.hasMoreTokens())
    {
        if (!strcmp(word, "true"))
        {
            setBoolValue(true);
            return;
        }
        else if (!strcmp(word, "false"))
        {
            setBoolValue(false);
            return;
        }
    }
*/

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

int cBoolPar::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret!=0)
        return ret;

    const cBoolPar *other2 = dynamic_cast<const cBoolPar *>(other);
    if (flags & FL_ISEXPR)
        throw cRuntimeError(this, "cannot compare expressions yet"); //FIXME
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

