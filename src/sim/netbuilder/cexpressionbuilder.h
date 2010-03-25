//==========================================================================
//  CEXPRESSIONBUILDER.H - part of
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

#ifndef __CEXPRESSIONBUILDER_H
#define __CEXPRESSIONBUILDER_H

#include <string>
#include <map>
#include <vector>
#include "nedelements.h"
#include "cdynamicexpression.h"
#include "cexception.h"

NAMESPACE_BEGIN

class cParImpl;

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
    void doOperator(OperatorElement *node);
    void doFunction(FunctionElement *node);
    void doIdent(IdentElement *node);
    void doLiteral(LiteralElement *node);
    bool isLoopVar(const char *parname);

  public:
    cExpressionBuilder();
    ~cExpressionBuilder();

    /**
     * Translates a NED expression into a cDynamicExpression.
     * Note that the resulting expression object is going to be shared
     * across similar module instances, so it cannot contain concrete
     * cModule or cComponent pointers.
     */
    cDynamicExpression *process(ExpressionElement *node, bool inSubcomponentScope);

    /**
     * Sets cPar to the given cDynamicExpression, performing basic optimizations:
     * if the expression can be represented in cPar without using dynamic
     * expressions (i.e. it is just a constant number), it does so and deletes
     * the expression.
     */
    static void setExpression(cParImpl *par, cDynamicExpression *expr);
};

NAMESPACE_END


#endif





