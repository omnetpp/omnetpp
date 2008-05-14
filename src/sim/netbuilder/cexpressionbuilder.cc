//==========================================================================
//   CEXPRESSIONBUILDER.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

#include "nedelements.h"
#include "nederror.h"
#include "cexpressionbuilder.h"
#include "cmathfunction.h"
#include "cnedfunction.h"
#include "cparimpl.h"
#include "xmlgenerator.h"
#include "nedsupport.h"
#include "stringutil.h"

USING_NAMESPACE

/* for debugging
static void dump(NEDElement *node)
{
    generateXML(std::cout, node, false);
    std::cout.flush();
}
*/

cExpressionBuilder::cExpressionBuilder()
{
    elems = NULL;
}

cExpressionBuilder::~cExpressionBuilder()
{
}

void cExpressionBuilder::doNode(NEDElement *node)
{
    if (pos > limit)
        throw cRuntimeError("Expression too long");
    int tagcode = node->getTagCode();
    switch (tagcode)
    {
        case NED_OPERATOR:
            doOperator((OperatorElement *)node); break;
        case NED_FUNCTION:
            doFunction((FunctionElement *)node); break;
        case NED_IDENT:
            doIdent((IdentElement *)node); break;
        case NED_LITERAL:
            doLiteral((LiteralElement *)node); break;
        default:
            throw cRuntimeError("Unexpected tag in expression: %s", node->getTagName());
    }
}

void cExpressionBuilder::doOperator(OperatorElement *node)
{
    // push args first
    for (NEDElement *op=node->getFirstChild(); op; op=op->getNextSibling())
        doNode(op);

    // determine name and arg count
    const char *name = node->getName();
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    if (!op2)
    {
        // unary:
        if (!strcmp(name,"-"))
            elems[pos++] = cDynamicExpression::NEG;
        else if (!strcmp(name,"!"))
            elems[pos++] = cDynamicExpression::NOT;
        else if (!strcmp(name,"~"))
            elems[pos++] = cDynamicExpression::BIN_NOT;
        else
            throw cRuntimeError("Unexpected operator %s", name);
    }
    else if (!op3)
    {
        // binary:

        // arithmetic
        if (!strcmp(name,"+"))
            elems[pos++] = cDynamicExpression::ADD;
        else if (!strcmp(name,"-"))
            elems[pos++] = cDynamicExpression::SUB;
        else if (!strcmp(name,"*"))
            elems[pos++] = cDynamicExpression::MUL;
        else if (!strcmp(name,"/"))
            elems[pos++] = cDynamicExpression::DIV;
        else if (!strcmp(name,"%"))
            elems[pos++] = cDynamicExpression::MOD;
        else if (!strcmp(name,"^"))
            elems[pos++] = cDynamicExpression::POW;

        // logical
        else if (!strcmp(name,"=="))
            elems[pos++] = cDynamicExpression::EQ;
        else if (!strcmp(name,"!="))
            elems[pos++] = cDynamicExpression::NE;
        else if (!strcmp(name,"<"))
            elems[pos++] = cDynamicExpression::LT;
        else if (!strcmp(name,"<="))
            elems[pos++] = cDynamicExpression::LE;
        else if (!strcmp(name,">"))
            elems[pos++] = cDynamicExpression::GT;
        else if (!strcmp(name,">="))
            elems[pos++] = cDynamicExpression::GE;
        else if (!strcmp(name,"&&"))
            elems[pos++] = cDynamicExpression::AND;
        else if (!strcmp(name,"||"))
            elems[pos++] = cDynamicExpression::OR;
        else if (!strcmp(name,"##"))
            elems[pos++] = cDynamicExpression::XOR;

        // bitwise
        else if (!strcmp(name,"&"))
            elems[pos++] = cDynamicExpression::BIN_AND;
        else if (!strcmp(name,"|"))
            elems[pos++] = cDynamicExpression::BIN_OR;
        else if (!strcmp(name,"#"))
            elems[pos++] = cDynamicExpression::BIN_XOR;
        else if (!strcmp(name,"<<"))
            elems[pos++] = cDynamicExpression::LSHIFT;
        else if (!strcmp(name,">>"))
            elems[pos++] = cDynamicExpression::RSHIFT;
        else
            throw cRuntimeError("Unexpected operator %s", name);
    }
    else
    {
        // tertiary can only be "?:"
        if (!strcmp(name,"?:"))
            elems[pos++] = cDynamicExpression::IIF;
        else
            throw cRuntimeError("Unexpected operator %s", name);
    }
}

void cExpressionBuilder::doFunction(FunctionElement *node)
{
    // get function name, arg count, args
    const char *funcname = node->getName();
    int argcount = node->getNumChildren();

    // operators should be handled specially
    if (!strcmp(funcname, "index"))
    {
        if (!inSubcomponentScope)
            throw cRuntimeError("`index' operator is only supported in submodule parameters");
        elems[pos++] = new NEDSupport::ModuleIndex();
    }
    else if (!strcmp(funcname, "const"))
    {
        throw cRuntimeError("`const' operator: not yet supported"); //TBD
    }
    else if (!strcmp(funcname, "sizeof"))
    {
        // operands are in a child "ident" node
        IdentElement *identnode = node->getFirstIdentChild();
        ASSERT(identnode);

        const char *ident = identnode->getName();
        const char *modulename = identnode->getModule();

        // XXX actually we could decide here from the NED declarations
        // if it's sizeof(parentModuleGateVector) or sizeof(submoduleVector),
        // we don't have to do it at runtime in the Sizeof functor class.
        if (opp_isempty(modulename))
            elems[pos++] = new NEDSupport::Sizeof(ident, inSubcomponentScope, false);
        else if (strcmp(modulename, "this")==0)
            elems[pos++] = new NEDSupport::Sizeof(ident, false, true);
        else
            //XXX elems[pos++] = new NEDSupport::Sizeof(modulename, ident, inSubcomponentScope, hasChild);
            throw cRuntimeError("dynamic module builder: sizeof(module.ident): not yet supported"); //TBD
    }
    else if (!strcmp(funcname, "xmldoc"))
    {
        // push args first
        for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
            doNode(child);

        // syntax is xmldoc(file) or xmldoc(file, xpath); both args are string expressions
        ASSERT(argcount==1 || argcount==2);
        elems[pos++] = new NEDSupport::XMLDoc(argcount==2);
    }
    else // normal function
    {
        // push args first
        for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
            doNode(child);

        // normal function: find it and add to reverse Polish expression
        cMathFunction *functype = cMathFunction::find(funcname,argcount);
        cNEDFunction *nedfunctype = cNEDFunction::find(funcname,argcount);
        if (functype)
            elems[pos++] = functype;
        else if (nedfunctype)
            elems[pos++].set(nedfunctype, argcount);
        else
            throw cRuntimeError("Function %s with %d args not found (Define_NED_Function() missing from C++ code?)", funcname, argcount);
    }
}

bool cExpressionBuilder::isLoopVar(const char *parname)
{
    const char **varNames = NEDSupport::LoopVar::getVarNames();
    int n = NEDSupport::LoopVar::getNumVars();
    for (int i=0; i<n; i++)
        if (strcmp(varNames[i], parname)==0)
            return true;
    return false;
}

void cExpressionBuilder::doIdent(IdentElement *node)
{
    const char *parname = node->getName();
    const char *modulename = node->getModule();
    bool hasChild = node->getFirstChild()!=NULL;
    if (hasChild)
        doNode(node->getFirstChild()); // push module index

    if (opp_isempty(modulename) && isLoopVar(parname))
        elems[pos++] = new NEDSupport::LoopVar(parname);
    else if (opp_isempty(modulename))
        elems[pos++] = new NEDSupport::ParameterRef(parname, inSubcomponentScope, false);
    else if (strcmp(modulename, "this")==0)
        elems[pos++] = new NEDSupport::ParameterRef(parname, false, true);
    else
        elems[pos++] = new NEDSupport::SiblingModuleParameterRef(modulename, parname, inSubcomponentScope, hasChild);
}

void cExpressionBuilder::doLiteral(LiteralElement *node)
{
    const char *val = node->getValue();
    switch (node->getType())
    {
        case NED_CONST_BOOL:   elems[pos++] = !strcmp(val,"true"); break;
        case NED_CONST_INT:    elems[pos++] = opp_atol(node->getValue()); break; // this handles hex as well
        case NED_CONST_DOUBLE: elems[pos++] = opp_atof(node->getValue());
                               elems[pos-1].setUnit(node->getUnit());
                               break;
        case NED_CONST_STRING: elems[pos++] = node->getValue(); break;
        default: throw cRuntimeError("Internal error: wrong constant type");
    }
}

cDynamicExpression *cExpressionBuilder::process(ExpressionElement *node,
                                                bool inSubcomponentScope)
{
    // create dynamically evaluated expression (reverse Polish).
    // we don't know the size in advance, so first collect it in elems[1000],
    // then make a copy
    this->inSubcomponentScope = inSubcomponentScope;
    elems = new cDynamicExpression::Elem[1000];
    pos = 0;
    limit = 990;

    ASSERT(node!=NULL);
    doNode(node->getFirstChild());

    int n = pos;
    cDynamicExpression::Elem *newElems = new cDynamicExpression::Elem[n];
    for (int i=0; i<n; i++)
        newElems[i] = elems[i];

    cDynamicExpression *ret = new cDynamicExpression();
    ret->setExpression(newElems, n);

    delete [] elems;
    elems = NULL;

    //XXX printf("    nedelement to expr returning: %s\n", ret->str().c_str());

    return ret;
}

void cExpressionBuilder::setExpression(cParImpl *par, cDynamicExpression *expr)
{
    par->setExpression(expr);

    // simplify if possible: store as constant instead of expression
    if (expr->isAConstant())
        par->convertToConst(NULL);
}

