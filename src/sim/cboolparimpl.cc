//==========================================================================
//   CBOOLPAR.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cboolparimpl.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/ccomponent.h"

namespace omnetpp {

cBoolParImpl::cBoolParImpl()
{
    val = false;
}

cBoolParImpl::~cBoolParImpl()
{
    deleteOld();
}

void cBoolParImpl::copy(const cBoolParImpl& other)
{
    if (flags & FL_ISEXPR)
        expr = other.expr->dup();
    else
        val = other.val;
}

void cBoolParImpl::operator=(const cBoolParImpl& other)
{
    if (this == &other)
        return;
    deleteOld();
    cParImpl::operator=(other);
    copy(other);
}

void cBoolParImpl::parsimPack(cCommBuffer *buffer) const
{
    //TBD
}

void cBoolParImpl::parsimUnpack(cCommBuffer *buffer)
{
    //TBD
}

void cBoolParImpl::setBoolValue(bool b)
{
    deleteOld();
    val = b;
    flags |= FL_CONTAINSVALUE | FL_ISSET;
}

void cBoolParImpl::setIntValue(intpar_t l)
{
    throw cRuntimeError(this, E_BADCAST, "integer", "double");
}

void cBoolParImpl::setDoubleValue(double d)
{
    throw cRuntimeError(this, E_BADCAST, "double", "bool");
}

void cBoolParImpl::setStringValue(const char *s)
{
    throw cRuntimeError(this, E_BADCAST, "string", "bool");
}

void cBoolParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, E_BADCAST, "XML", "bool");
}

void cBoolParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cBoolParImpl::boolValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(E_PARNOTSET);

    if ((flags & FL_ISEXPR) == 0)
        return val;
    else {
        cNedValue v = evaluate(expr, context);
        if (v.type != cNedValue::BOOL)
            throw cRuntimeError(E_ECANTCAST, "bool");
        return v.boolValue();
    }
}

intpar_t cBoolParImpl::intValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "bool", "integer");
}

double cBoolParImpl::doubleValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "bool", "double");
}

const char *cBoolParImpl::stringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "bool", "string");
}

std::string cBoolParImpl::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "bool", "string");
}

cXMLElement *cBoolParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "bool", "XML");
}

cExpression *cBoolParImpl::getExpression() const
{
    return (flags | FL_ISEXPR) ? expr : nullptr;
}

void cBoolParImpl::deleteOld()
{
    if (flags & FL_ISEXPR) {
        delete expr;
        flags &= ~FL_ISEXPR;
    }
}

cPar::Type cBoolParImpl::getType() const
{
    return cPar::BOOL;
}

bool cBoolParImpl::isNumeric() const
{
    return false;  // because doubleValue() and intValue() throw error
}

void cBoolParImpl::convertToConst(cComponent *context)
{
    setBoolValue(boolValue(context));
}

std::string cBoolParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();
    return val ? "true" : "false";
}

void cBoolParImpl::parse(const char *text)
{
    // shortcut: recognize "true" and "false"
    if (strcmp(text, "true") == 0 || strcmp(text, "false") == 0) {
        setBoolValue(text[0] == 't');
        return;
    }

    // try parsing it as an expression
    cDynamicExpression *dynexpr = new cDynamicExpression();
    try {
        dynexpr->parse(text);
    }
    catch (std::exception& e) {
        delete dynexpr;
        throw;
    }
    setExpression(dynexpr);

    // simplify if possible: store as constant instead of expression
    if (dynexpr->isAConstant())
        convertToConst(nullptr);
}

int cBoolParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret != 0)
        return ret;

    const cBoolParImpl *other2 = dynamic_cast<const cBoolParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (val == other2->val) ? 0 : (val < other2->val) ? -1 : 1;
}

}  // namespace omnetpp

