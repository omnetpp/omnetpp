//==========================================================================
//   CEXPRESSIONBUILDER.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#include "common/stringutil.h"
#include "common/unitconversion.h"
#include "nedxml/nedelements.h"
#include "nedxml/xmlgenerator.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cparimpl.h"
#include "omnetpp/nedsupport.h"
#include "cexpressionbuilder.h"
#include "nedxml/errorstore.h"

using namespace omnetpp::common;
using namespace omnetpp::nedsupport;

namespace omnetpp {

/* for debugging
static void dump(NedElement *node)
{
    generateXML(std::cout, node, false);
    std::cout.flush();
}
*/

cExpressionBuilder::cExpressionBuilder()
{
    elems = nullptr;
}

cExpressionBuilder::~cExpressionBuilder()
{
}

void cExpressionBuilder::doNode(NedElement *node)
{
    if (pos > limit)
        throw cRuntimeError("Expression too long");
    int tagcode = node->getTagCode();
    switch (tagcode) {
        case NED_OPERATOR: doOperator((OperatorElement *)node); break;
        case NED_FUNCTION: doFunction((FunctionElement *)node); break;
        case NED_IDENT:    doIdent((IdentElement *)node); break;
        case NED_LITERAL:  doLiteral((LiteralElement *)node); break;
        default: throw cRuntimeError("Unexpected tag in expression: %s", node->getTagName());
    }
}

void cExpressionBuilder::doOperator(OperatorElement *node)
{
    // push args first
    for (NedElement *op = node->getFirstChild(); op; op = op->getNextSibling())
        doNode(op);

    // determine name and arg count
    const char *name = node->getName();
    NedElement *op1 = node->getFirstChild();
    NedElement *op2 = op1 ? op1->getNextSibling() : nullptr;
    NedElement *op3 = op2 ? op2->getNextSibling() : nullptr;

    if (!op2) {
        // unary:
        if (!strcmp(name, "-"))
            elems[pos++] = cDynamicExpression::NEG;
        else if (!strcmp(name, "!"))
            elems[pos++] = cDynamicExpression::NOT;
        else if (!strcmp(name, "~"))
            elems[pos++] = cDynamicExpression::BIN_NOT;
        else
            throw cRuntimeError("Unexpected operator '%s'", name);
    }
    else if (!op3) {
        // binary:

        // arithmetic
        if (!strcmp(name, "+"))
            elems[pos++] = cDynamicExpression::ADD;
        else if (!strcmp(name, "-"))
            elems[pos++] = cDynamicExpression::SUB;
        else if (!strcmp(name, "*"))
            elems[pos++] = cDynamicExpression::MUL;
        else if (!strcmp(name, "/"))
            elems[pos++] = cDynamicExpression::DIV;
        else if (!strcmp(name, "%"))
            elems[pos++] = cDynamicExpression::MOD;
        else if (!strcmp(name, "^"))
            elems[pos++] = cDynamicExpression::POW;

        // logical
        else if (!strcmp(name, "=="))
            elems[pos++] = cDynamicExpression::EQ;
        else if (!strcmp(name, "!="))
            elems[pos++] = cDynamicExpression::NE;
        else if (!strcmp(name, "<"))
            elems[pos++] = cDynamicExpression::LT;
        else if (!strcmp(name, "<="))
            elems[pos++] = cDynamicExpression::LE;
        else if (!strcmp(name, ">"))
            elems[pos++] = cDynamicExpression::GT;
        else if (!strcmp(name, ">="))
            elems[pos++] = cDynamicExpression::GE;
        else if (!strcmp(name, "&&"))
            elems[pos++] = cDynamicExpression::AND;
        else if (!strcmp(name, "||"))
            elems[pos++] = cDynamicExpression::OR;
        else if (!strcmp(name, "##"))
            elems[pos++] = cDynamicExpression::XOR;

        // bitwise
        else if (!strcmp(name, "&"))
            elems[pos++] = cDynamicExpression::BIN_AND;
        else if (!strcmp(name, "|"))
            elems[pos++] = cDynamicExpression::BIN_OR;
        else if (!strcmp(name, "#"))
            elems[pos++] = cDynamicExpression::BIN_XOR;
        else if (!strcmp(name, "<<"))
            elems[pos++] = cDynamicExpression::LSHIFT;
        else if (!strcmp(name, ">>"))
            elems[pos++] = cDynamicExpression::RSHIFT;
        else
            throw cRuntimeError("Unexpected operator '%s'", name);
    }
    else {
        // tertiary can only be "?:"
        if (!strcmp(name, "?:"))
            elems[pos++] = cDynamicExpression::IIF;
        else
            throw cRuntimeError("Unexpected operator '%s'", name);
    }
}

void cExpressionBuilder::doFunction(FunctionElement *node)
{
    // get function name, arg count, args
    const char *funcname = node->getName();
    int argcount = node->getNumChildren();

    // operators should be handled specially
    if (!strcmp(funcname, "index")) {
        if (!inSubcomponentScope)
            throw cRuntimeError("'index' operator is only supported in submodule parameters");
        elems[pos++] = new ModuleIndex();
    }
    else if (!strcmp(funcname, "exists")) {
        // operands are in a child "ident" node
        IdentElement *identnode = node->getFirstIdentChild();
        ASSERT(identnode);
        const char *ident = identnode->getName();
        elems[pos++] = new Exists(ident, inSubcomponentScope);
    }
    else if (!strcmp(funcname, "typename")) {
        elems[pos++] = new Typename();
    }
    else if (!strcmp(funcname, "const")) {
        throw cRuntimeError("'const' operator is not yet supported");  // TBD
    }
    else if (!strcmp(funcname, "sizeof")) {
        // operands are in a child "ident" node
        IdentElement *identnode = node->getFirstIdentChild();
        ASSERT(identnode);

        const char *ident = identnode->getName();
        const char *modulename = identnode->getModule();

        // XXX actually we could decide here from the NED declarations
        // if it's sizeof(parentModuleGateVector) or sizeof(submoduleVector),
        // we don't have to do it at runtime in the Sizeof functor class.
        if (opp_isempty(modulename))
            elems[pos++] = new Sizeof(ident, inSubcomponentScope, false);
        else if (strcmp(modulename, "this") == 0)
            elems[pos++] = new Sizeof(ident, false, true);
        else
            throw cRuntimeError("sizeof(module.ident) is not yet supported");  // TBD
    }
    else {  // normal function
            // push args first
        for (NedElement *child = node->getFirstChild(); child; child = child->getNextSibling())
            doNode(child);

        // normal function: find it and add to reverse Polish expression
        cNedMathFunction *functype = cNedMathFunction::find(funcname, argcount);
        cNedFunction *nedfunctype = cNedFunction::find(funcname);
        if (functype)
            elems[pos++] = functype;
        else if (nedfunctype) {
            if (argcount < nedfunctype->getMinArgs() || (argcount > nedfunctype->getMaxArgs() && !nedfunctype->hasVarArgs()))
                throw cRuntimeError("Function '%s' does not accept %d arguments", nedfunctype->getSignature(), argcount);
            elems[pos++].set(nedfunctype, argcount);
        }
        else
            throw cRuntimeError("Function %s() not found (Define_NED_Function() or Define_NED_Math_Function() missing from C++ code?)", funcname);
    }
}

bool cExpressionBuilder::isLoopVar(const char *parname)
{
    const char **varNames = LoopVar::getVarNames();
    int n = LoopVar::getNumVars();
    for (int i = 0; i < n; i++)
        if (strcmp(varNames[i], parname) == 0)
            return true;

    return false;
}

void cExpressionBuilder::doIdent(IdentElement *node)
{
    const char *parname = node->getName();
    const char *modulename = node->getModule();
    bool hasChild = node->getFirstChild() != nullptr;
    if (hasChild)
        doNode(node->getFirstChild());  // push module index

    if (opp_isempty(modulename) && isLoopVar(parname))
        elems[pos++] = new LoopVar(parname);
    else if (opp_isempty(modulename))
        elems[pos++] = new ParameterRef(parname, inSubcomponentScope, false);
    else if (strcmp(modulename, "this") == 0)
        elems[pos++] = new ParameterRef(parname, false, true);
    else
        elems[pos++] = new SiblingModuleParameterRef(modulename, parname, inSubcomponentScope, hasChild);
}

void cExpressionBuilder::doLiteral(LiteralElement *node)
{
    const char *val = node->getValue();
    switch (node->getType()) {
        case LIT_BOOL:   elems[pos++] = !strcmp(val,"true"); break;
        case LIT_INT:    elems[pos++] = (intpar_t)opp_atoll(node->getValue()); break; // this handles hex as well
        case LIT_DOUBLE: elems[pos++] = opp_atof(node->getValue()); break;
        case LIT_STRING: elems[pos++] = node->getValue(); break;
        case LIT_QUANTITY: {
            std::string unit;
            double d = UnitConversion::parseQuantity(node->getValue(), unit);
            bool isInteger = (d == floor(d)) && d >= std::numeric_limits<intpar_t>::min() && d <= std::numeric_limits<intpar_t>::max(); // note: it would be slightly better to try parsing it in integer in the first place
            if (isInteger)
                elems[pos++] = (intpar_t)d;
            else
                elems[pos++] = d;
            elems[pos-1].setUnit(unit.c_str());
            break;
        }
        default: throw cRuntimeError("Internal error: Wrong constant type");
    }
}

cDynamicExpression *cExpressionBuilder::process(ExpressionElement *node, bool inSubcomponentScope)
{
    // create dynamically evaluated expression (reverse Polish).
    // we don't know the size in advance, so first collect it in elems[1000],
    // then make a copy
    this->inSubcomponentScope = inSubcomponentScope;
    elems = new cDynamicExpression::Elem[1000];
    pos = 0;
    limit = 990;

    ASSERT(node != nullptr);
    doNode(node->getFirstChild());

    int n = pos;
    cDynamicExpression::Elem *newElems = new cDynamicExpression::Elem[n];
    for (int i = 0; i < n; i++)
        newElems[i] = elems[i];

    cDynamicExpression *ret = new cDynamicExpression();
    ret->setExpression(newElems, n);

    delete[] elems;
    elems = nullptr;

    // XXX printf("    nedelement to expr returning: %s\n", ret->str().c_str());

    return ret;
}

void cExpressionBuilder::setExpression(cParImpl *par, cDynamicExpression *expr)
{
    par->setExpression(expr);

    // simplify if possible: store as constant instead of expression
    if (expr->isAConstant())
        par->convertToConst(nullptr);
}

}  // namespace omnetpp

