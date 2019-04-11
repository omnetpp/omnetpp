//=========================================================================
//  CUSTOMFILTER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_CUSTOMFILTER_H
#define __OMNETPP_SCAVE_CUSTOMFILTER_H

#include "common/expression.h"
#include "common/exprnodes.h"
#include "commonnodes.h"

namespace omnetpp {
namespace scave {

using omnetpp::common::Expression;

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
    private:
        typedef omnetpp::common::Expression Expression;
        typedef omnetpp::common::expression::ExprNode ExprNode;
        typedef omnetpp::common::expression::ExprValue ExprValue;
        typedef omnetpp::common::expression::ValueNode ValueNode;
        typedef omnetpp::common::expression::Context Context;
    public:
        /**
         * Implements a variable in the expression. Currently just delegates
         * to ExpressionFilterNode::getVariable().
         */
        class NodeVar : public ValueNode
        {
          private:
            ExpressionFilterNode *hostnode;
            std::string varname;
          public:
            NodeVar(ExpressionFilterNode *node, const char *name)
                {hostnode = node; varname = name; hostnode->skipFirstDatum |= (varname=="tprev" || varname=="yprev"); }
            virtual ExprNode *dup() const override {return new NodeVar(hostnode, varname.c_str());}
            virtual std::string getName() const override {return varname;}
            virtual void print(std::ostream& out, int spaciousness) const override {out << varname;}
            virtual ExprValue evaluate(Context *context=nullptr) const override
                {return hostnode->getVariable(varname.c_str());}
        };
    private:
        Expression *expr;
        Datum currentDatum, prevDatum;
        bool skipFirstDatum;
    public:
        ExpressionFilterNode(const char *expression);
        virtual ~ExpressionFilterNode();
        virtual bool isReady() const override;
        virtual void process() override;
        double getVariable(const char *varname);
};

class SCAVE_API ExpressionFilterNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "expression";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

} // namespace scave
}  // namespace omnetpp


#endif


