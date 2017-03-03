//=========================================================================
//  CUSTOMFILTER.CC - part of
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

#include <cmath>
#include "channel.h"
#include "customfilter.h"
#include "scaveutils.h"

namespace omnetpp {
namespace scave {

using namespace omnetpp::common;

/**
 * Resolves variables (x, y) and functions (sin, fabs, etc) in
 * ExpressionFilterNode expressions.
 */
class Resolver : public Expression::Resolver
{
  private:
    ExpressionFilterNode *hostnode;

  public:
    Resolver(ExpressionFilterNode *node) { hostnode = node; }
    virtual ~Resolver() {};
    virtual Expression::Functor *resolveVariable(const char *varname) override;
    virtual Expression::Functor *resolveFunction(const char *funcname, int argcount) override;
};

Expression::Functor *Resolver::resolveVariable(const char *varname)
{
    if (strcmp(varname, "t") == 0 || strcmp(varname, "y") == 0 || strcmp(varname, "tprev") == 0 || strcmp(varname, "yprev") == 0)
        return new ExpressionFilterNode::NodeVar(hostnode, varname);
    else
        throw opp_runtime_error("Unrecognized variable: %s", varname);
}

Expression::Functor *Resolver::resolveFunction(const char *funcname, int argcount)
{
    // FIXME check argcount!
    if (MathFunction::supports(funcname))
        return new MathFunction(funcname);
    else
        throw opp_runtime_error("Unrecognized function: %s()", funcname);
}

ExpressionFilterNode::ExpressionFilterNode(const char *text)
{
    skipFirstDatum = false;
    expr = new Expression();
    Resolver resolver(this);
    expr->parse(text, &resolver);
    prevDatum.x = prevDatum.y = NaN;
}

ExpressionFilterNode::~ExpressionFilterNode()
{
    delete expr;
}

bool ExpressionFilterNode::isReady() const
{
    return in()->length() > 0;
}

void ExpressionFilterNode::process()
{
    int n = in()->length();
    if (skipFirstDatum && n > 0) {
        in()->read(&prevDatum, 1);
        n--;
        skipFirstDatum = false;
    }
    for (int i = 0; i < n; i++) {
        in()->read(&currentDatum, 1);
        double value = expr->doubleValue();
        prevDatum = currentDatum;
        currentDatum.y = value;
        out()->write(&currentDatum, 1);
    }
}

double ExpressionFilterNode::getVariable(const char *varname)
{
    if (varname[0] == 't' && varname[1] == 0)
        return currentDatum.x;
    else if (varname[0] == 'y' && varname[1] == 0)
        return currentDatum.y;
    else if (varname[0] == 't' && strcmp(varname, "tprev") == 0)
        return prevDatum.x;
    else if (varname[0] == 'y' && strcmp(varname, "yprev") == 0)
        return prevDatum.y;
    else
        return 0.0;  // cannot happen, as validation has already taken place in Resolver
}

// --

const char *ExpressionFilterNodeType::getDescription() const
{
    return "Evaluates an arbitrary expression. Use t for time, y for value, and tprev, yprev for the previous values.";  // FIXME use "t" and "y" instead?
}

void ExpressionFilterNodeType::getAttributes(StringMap& attrs) const
{
    attrs["expression"] = "The expression to evaluate.";
}

Node *ExpressionFilterNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new ExpressionFilterNode(attrs["expression"].c_str());
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void ExpressionFilterNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    attrs["type"] = "double";
}

}  // namespace scave
}  // namespace omnetpp

