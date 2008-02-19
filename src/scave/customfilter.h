//=========================================================================
//  CUSTOMFILTER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _CUSTOMFILTER_H_
#define _CUSTOMFILTER_H_

#include "commonnodes.h"
#include "expression.h"

NAMESPACE_BEGIN


/**
 * Processing node which evaluates an arbitrary expression entered
 * by the user.
 *
 * TODO:
 *  - support parameters: $windowsize or $(windowsize) in the expression
 *    would prompt user to enter a value for windowsize;
 *  - extend the expression syntax into a mini imperative language to allow
 *    for writing more powerful filters: introduce variables and assigment operator
 *    (=, +=, *=, ++, --, etc); allow several expressions separated by semicolon;
 *    implement 'for', 'while', 'if'; filternode to keep a variables table;
 *    "init-expression" attr where user could initialize variables;
 *    possibly implement array variables as well.
 */
class SCAVE_API ExpressionFilterNode : public FilterNode
{
    public:
        /**
         * Implements a variable in the expression. Currently just delegates
         * to ExpressionFilterNode::getVariable().
         */
        class NodeVar : public Expression::Functor
        {
          private:
            ExpressionFilterNode *hostnode;
            std::string varname;
          public:
            NodeVar(ExpressionFilterNode *node, const char *name)
                {hostnode = node; varname = name; hostnode->skipFirstDatum |= (varname=="xprev" || varname=="yprev"); }
            virtual ~NodeVar() {}
            virtual Expression::Functor *dup() const {return new NodeVar(hostnode, varname.c_str());}
            virtual const char *name() const {return varname.c_str();}
            virtual const char *argTypes() const {return "";}
            virtual char returnType() const {return Expression::Value::DBL;}
            virtual Expression::Value evaluate(Expression::Value args[], int numargs)
                {return hostnode->getVariable(varname.c_str());}
            virtual std::string toString(std::string args[], int numargs) {return name();}
        };
    private:
        Expression *expr;
        Datum currentDatum, prevDatum;
        bool skipFirstDatum;
    public:
        ExpressionFilterNode(const char *expression);
        virtual ~ExpressionFilterNode();
        virtual bool isReady() const;
        virtual void process();
        double getVariable(const char *varname);
};

class SCAVE_API ExpressionFilterNodeType : public FilterNodeType
{
    public:
        virtual const char *name() const {return "expression";}
        virtual const char *description() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual void mapVectorAttributes(/*inout*/StringMap &attrs) const;
};

NAMESPACE_END


#endif


