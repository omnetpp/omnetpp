//==========================================================================
//  CEXPRESSIONBUILDER.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CEXPRESSIONBUILDER_H
#define __CEXPRESSIONBUILDER_H

#include <string>
#include <map>
#include <vector>
#include "nedelements.h"
#include "cdynamicexpression.h"
#include "cexception.h"

NAMESPACE_BEGIN

class cParValue;

/**
 * Utility classes for dynamic NED loading.
 * Assumes object tree has already passed all validation stages (DTD, syntax, semantic).
 *
 * @ingroup NetworkBuilder
 */
class SIM_API cExpressionBuilder
{
  protected:
    // configuration
    bool inSubcomponentScope;

    // state
    cDynamicExpression::Elem *elems;
    int pos;   // length of expression so far
    int limit; // allocated size of the elems[] vector

  protected:
    void doNode(NEDElement *node);
    void doOperator(OperatorNode *node);
    void doFunction(FunctionNode *node);
    void doIdent(IdentNode *node);
    void doLiteral(LiteralNode *node);
    bool isLoopVar(const char *parname);

  public:
    cExpressionBuilder();
    ~cExpressionBuilder();

    cDynamicExpression *process(ExpressionNode *node, bool inSubcomponentScope);

    /**
     * Sets cPar to the given cDynamicExpression, performing basic optimizations:
     * if the expression can be represented in cPar without using dynamic
     * expressions (i.e. it is just a constant number), it does so and deletes
     * the expression.
     */
    static void assign(cParValue *par, cDynamicExpression *expr);
};

NAMESPACE_END


#endif





