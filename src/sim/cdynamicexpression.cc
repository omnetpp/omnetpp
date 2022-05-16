//==========================================================================
//   CDYNAMICEXPRESSION.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <memory>
#include "common/expression.h"
#include "common/exception.h"
#include "common/unitconversion.h"
#include "omnetpp/cdynamicexpression.h"

#include "ctemporaryowner.h"
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

Register_Class(cOwnedDynamicExpression);

inline cExpression::Context *ctx(Context *context) {return dynamic_cast<cExpression::Context*>(context->simContext);}

cValue cDynamicExpression::SymbolTable::readVariable(Context *context, const char *name)
{
    const auto it = variables.find(name);
    if (it == variables.end())
        return IResolver::readVariable(context, name);
    return it->second;
}

cValue cDynamicExpression::SymbolTable::readVariable(Context *context, const char *name, intval_t index)
{
    const auto it = arrays.find(name);
    if (it == arrays.end())
        return IResolver::readVariable(context, name, index);
    auto array = it->second;
    if (index < 0 || index >= array.size())
        throw cRuntimeError("Index %ld out of bounds for array %s[%ld]", (long)index, name, (long)array.size());
    return array[index];
}

//----

cDynamicExpression::cDynamicExpression() : expression(new Expression())
{
}

cDynamicExpression::~cDynamicExpression()
{
    auto resolvers = expression->getDynamicResolvers();
    for (auto resolver: resolvers)
        delete resolver;
    delete expression;
    delete resolver;
}

void cDynamicExpression::copy(const cDynamicExpression& other)
{
    delete expression;
    delete resolver;
    expression = new Expression(*other.expression);
    resolver = other.resolver ? other.resolver->dup() : nullptr;
    sourceLoc = other.sourceLoc;
}

cDynamicExpression& cDynamicExpression::operator=(const cDynamicExpression& other)
{
    if (this == &other)
        return *this;
    cExpression::operator=(other);
    copy(other);
    return *this;
}

void cDynamicExpression::parse(const char *text)
{
    NedFunctionTranslator nedFunctionTranslator;
    Expression::MultiAstTranslator translator({ &nedFunctionTranslator, Expression::getDefaultAstTranslator()});
    expression->parse(text, &translator);
}

// Adapts cDynamicExpression::IResolver to Expression::DynamicResolver
class DynamicResolver : public Expression::DynamicResolver {
private:
    cDynamicExpression::IResolver *resolver;
    cDynamicExpression::Context *c(Expression::Context *ctx) {return dynamic_cast<cDynamicExpression::Context*>(ctx->simContext);}
    std::unique_ptr<cValue[]> cvtArgs(ExprValue argv[], int argc) {std::unique_ptr<cValue[]> a(new cValue[argc]); for (int i=0; i<argc; i++) a[i] = makeNedValue(argv[i]); return a;}
public:
    DynamicResolver(cDynamicExpression::IResolver *resolver) : resolver(resolver) {}
    virtual ~DynamicResolver() {}
    virtual ExprValue readVariable(Expression::Context *context, const char *name) {return makeExprValue(resolver->readVariable(c(context), name));}
    virtual ExprValue readVariable(Expression::Context *context, const char *name, intval_t index) {return makeExprValue(resolver->readVariable(c(context), name, index));}
    virtual ExprValue callFunction(Expression::Context *context, const char *name, ExprValue argv[], int argc)  {return makeExprValue(resolver->callFunction(c(context), name, cvtArgs(argv,argc).get(), argc));}
    virtual ExprValue readMember(Expression::Context *context, const ExprValue& object, const char *name) {return makeExprValue(resolver->readMember(c(context), makeNedValue(object), name));}
    virtual ExprValue readMember(Expression::Context *context, const ExprValue& object, const char *name, intval_t index) {return makeExprValue(resolver->readMember(c(context), makeNedValue(object), name, index));}
    virtual ExprValue callMethod(Expression::Context *context, const ExprValue& object, const char *name, ExprValue argv[], int argc) {return makeExprValue(resolver->callMethod(c(context), makeNedValue(object), name, cvtArgs(argv,argc).get(), argc));}
};

void cDynamicExpression::setResolver(IResolver *res)
{
    if (resolver == res)
        return;

    auto resolvers = expression->getDynamicResolvers();
    expression->clearDynamicResolvers();
    for (auto resolver: resolvers)
        delete resolver;
    delete resolver;

    resolver = res;
    expression->addDynamicResolver(new DynamicResolver(res));
}

void cDynamicExpression::parseNedExpr(const char *text)
{
    NedOperatorTranslator nedOperatorTranslator;
    NedFunctionTranslator nedFunctionTranslator;
    Expression::MultiAstTranslator translator({ &nedOperatorTranslator, &nedFunctionTranslator, Expression::getDefaultAstTranslator() });
    expression->parse(text, &translator);
}

std::string cDynamicExpression::str() const
{
    return expression->str();
}

inline cOwnedObject *ownedObjectIn(cValue& value)
{
    if (value.getType() == cValue::POINTER) {
        cObject *obj = value.objectValue();
        if (obj && obj->isOwnedObject())
            return (cOwnedObject*)obj;
    }
    return nullptr;
}

cValue cDynamicExpression::evaluate(Context *context) const
{
    // collect objects created during expression evaluation
    // and delete all of them on leaving this function (except the result, see later)
    cTemporaryOwner tmpOwner(cTemporaryOwner::DestructorMode::DISPOSE);

    omnetpp::common::expression::Context exprContext;
    exprContext.simContext = context;
    cValue value = makeNedValue(expression->evaluate(&exprContext)); // evaluate expr in the given context

    tmpOwner.restoreOriginalOwner();
    if (cOwnedObject *obj = ownedObjectIn(value))
        if (obj->getOwner() == &tmpOwner)
            tmpOwner.drop(obj); // don't let tmpOwner delete the object we are returning

    return value;
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
    cValue v = evaluate(context);
    return v.boolValue();
}

intval_t cDynamicExpression::intValue(Context *context, const char *expectedUnit) const
{
    cValue v = evaluate(context);
    return expectedUnit == nullptr ? v.intValue() : (intval_t)v.doubleValueInUnit(expectedUnit);
}

double cDynamicExpression::doubleValue(Context *context, const char *expectedUnit) const
{
    cValue v = evaluate(context);
    return expectedUnit == nullptr ? v.doubleValue() : v.doubleValueInUnit(expectedUnit);
}

std::string cDynamicExpression::stringValue(Context *context) const
{
    cValue v = evaluate(context);
    return v.stringValue();
}

cXMLElement *cDynamicExpression::xmlValue(Context *context) const
{
    cValue v = evaluate(context);
    return v.xmlValue();
}

}  // namespace omnetpp

