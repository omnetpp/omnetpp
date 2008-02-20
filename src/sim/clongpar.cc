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

USING_NAMESPACE


cLongPar::cLongPar()
{
    val = 0;
}

cLongPar::~cLongPar()
{
    deleteOld();
}

void cLongPar::operator=(const cLongPar& other)
{
    if (this==&other) return;

    deleteOld();

    cParImpl::operator=(other);
    if (flags & FL_ISEXPR)
        expr = (cExpression *) other.expr->dup();
    else
        val = other.val;
}

void cLongPar::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cLongPar::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cLongPar::setBoolValue(bool b)
{
    throw cRuntimeError(this, eBADCAST, "bool", "int/long");
}

void cLongPar::setLongValue(long l)
{
    deleteOld();
    val = l;
    flags |= FL_HASVALUE;
}

void cLongPar::setDoubleValue(double d)
{
    deleteOld();
    val = double_to_long(d);
    flags |= FL_HASVALUE;
}

void cLongPar::setStringValue(const char *s)
{
    throw cRuntimeError(this, eBADCAST, "string", "int/long");
}

void cLongPar::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, eBADCAST, "XML", "int/long");
}

void cLongPar::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_HASVALUE;
}

bool cLongPar::boolValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "int/long", "bool");
}

long cLongPar::longValue(cComponent *context) const
{
    return evaluate(context);
}

double cLongPar::doubleValue(cComponent *context) const
{
    return evaluate(context);
}

const char *cLongPar::stringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "int/long", "string");
}

std::string cLongPar::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "int/long", "string");
}

cXMLElement *cLongPar::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, eBADCAST, "int/long", "XML");
}

cExpression *cLongPar::expression() const
{
    return (flags | FL_ISEXPR) ? expr : NULL;
}

long cLongPar::evaluate(cComponent *context) const
{
    return (flags & FL_ISEXPR) ? expr->longValue(context, unit()) : val;
}

void cLongPar::deleteOld()
{
    if (flags & FL_ISEXPR)
    {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cLongPar::type() const
{
    return cPar::LONG;
}

bool cLongPar::isNumeric() const
{
    return true;
}

void cLongPar::convertToConst(cComponent *context)
{
    setLongValue(longValue(context));
}

std::string cLongPar::toString() const
{
    if (flags & FL_ISEXPR)
        return expr->toString();

    char buf[32];
    sprintf(buf, "%ld", val);
    return buf;
}

void cLongPar::parse(const char *text)
{
/*XXX not really needed
    // maybe it's just a number
    cStringTokenizer tok(text);
    const char *word = tok.nextToken();
    if (word!=NULL && !tok.hasMoreTokens())
    {
        char *endp;
        long num = strtol(word, &endp, 10); // FIXME TBD try as "units" as well
        if (*endp == '\0')
        {
            setLongValue(num);
            return true;
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

int cLongPar::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret!=0)
        return ret;

    const cLongPar *other2 = dynamic_cast<const cLongPar *>(other);
    if (flags & FL_ISEXPR)
        throw cRuntimeError(this, "cannot compare expressions yet"); //FIXME
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

