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
class SCAVE_API CustomFilterNode : public FilterNode
{
    public:
        class NodeVar : public Expression::Functor
        {
          private:
            CustomFilterNode *hostnode;
            std::string varname;
          public:
            NodeVar(CustomFilterNode *node, const char *name) {hostnode = node; varname = name;}
            virtual ~NodeVar() {}
            virtual Expression::Functor *dup() const {return new NodeVar(hostnode, varname.c_str());}
            virtual const char *name() const {return varname.c_str();}
            virtual const char *argTypes() const {return "";}
            virtual char returnType() const {return Expression::StkValue::DBL;}
            virtual Expression::StkValue evaluate(Expression::StkValue args[], int numargs)
                {return hostnode->getVariable(varname.c_str());}
            virtual std::string toString(std::string args[], int numargs) {return name();}
        };
    private:
        Expression *expr;
        Datum currentDatum;
    public:
        CustomFilterNode(const char *expression);
        virtual ~CustomFilterNode();
        virtual bool isReady() const;
        virtual void process();
        double getVariable(const char *varname);
};

class SCAVE_API CustomFilterNodeType : public FilterNodeType
{
    public:
        virtual const char *name() const {return "custom";}
        virtual const char *description() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
};

#endif


