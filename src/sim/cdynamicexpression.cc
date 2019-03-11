//==========================================================================
//   CDYNAMICEXPRESSION.CC  - part of
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

#include "common/expression.h"
#include "common/exception.h"
#include "common/unitconversion.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cmodule.h"
#include "nedsupport.h"

using namespace std;
using namespace omnetpp::nedsupport;
using namespace omnetpp::common;
using namespace omnetpp::common::expression;

namespace omnetpp {

cDynamicExpression::cDynamicExpression()
{
    expression = new Expression;
}

cDynamicExpression::~cDynamicExpression()
{
    delete expression;
}

void cDynamicExpression::copy(const cDynamicExpression& other)
{
    delete expression;
    expression = new Expression(*other.expression);
}

cDynamicExpression& cDynamicExpression::operator=(const cDynamicExpression& other)
{
    if (this == &other)
        return *this;
    cExpression::operator=(other);
    copy(other);
    return *this;
}

void cDynamicExpression::parse(const char *text, bool inSubcomponentScope, bool inInifile)
{
    NedOperatorTranslator nedOperatorTranslator(inSubcomponentScope, inInifile);
    NedFunctionTranslator nedFunctionTranslator;
    Expression::MultiAstTranslator translator({ &nedOperatorTranslator, &nedFunctionTranslator, Expression::getDefaultAstTranslator() });
    expression->parse(text, &translator);
}

std::string cDynamicExpression::str() const
{
    return expression->str();
}

cNedValue cDynamicExpression::evaluate(Context *context) const
{
    omnetpp::common::expression::Context tmp;
    tmp.simContext = context;
    ExprValue value = expression->evaluate(&tmp);
    return makeNedValue(value);
}

int cDynamicExpression::compare(const cExpression *other) const
{
    const cDynamicExpression *otherDynamicExpr = dynamic_cast<const cDynamicExpression *>(other);
    if (!otherDynamicExpr)
        return 1;  // for lack of a better option

    std::string thisStr = expression->str();
    std::string otherStr = otherDynamicExpr->expression->str();
    return strcmp(thisStr.c_str(), otherStr.c_str());
}

double cDynamicExpression::convertUnit(double d, const char *unit, const char *targetUnit)
{
    // not inline because simkernel header files cannot refer to common/ headers (unitconversion.h)
    return UnitConversion::convertUnit(d, unit, targetUnit);
}

bool cDynamicExpression::isAConstant() const
{
    return expression->isAConstant();
}

bool cDynamicExpression::boolValue(Context *context) const
{
    cNedValue v = evaluate(context);
    return v.boolValue();
}

intpar_t cDynamicExpression::intValue(Context *context, const char *expectedUnit) const
{
    cNedValue v = evaluate(context);
    return expectedUnit == nullptr ? v.intValue() : (intpar_t)v.doubleValueInUnit(expectedUnit);
}

double cDynamicExpression::doubleValue(Context *context, const char *expectedUnit) const
{
    cNedValue v = evaluate(context);
    return expectedUnit == nullptr ? v.doubleValue() : v.doubleValueInUnit(expectedUnit);
}

std::string cDynamicExpression::stringValue(Context *context) const
{
    cNedValue v = evaluate(context);
    return v.stringValue();
}

cXMLElement *cDynamicExpression::xmlValue(Context *context) const
{
    cNedValue v = evaluate(context);
    return v.xmlValue();
}

//----

}  // namespace omnetpp

