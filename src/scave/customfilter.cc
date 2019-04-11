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
using namespace omnetpp::common::expression;

/**
 * Resolves variables (x, y) in ExpressionFilterNode expressions.
 */
class VariableResolver : public Expression::BasicAstTranslator
{
  private:
    ExpressionFilterNode *hostNode;

  public:
    VariableResolver(ExpressionFilterNode *node) : hostNode(node) {}
    virtual ExprNode *createIdentNode(const char *varName, bool withIndex) override;
};

ExprNode *VariableResolver::createIdentNode(const char *varName, bool withIndex)
{
    if (strcmp(varName, "t") == 0 || strcmp(varName, "y") == 0 || strcmp(varName, "tprev") == 0 || strcmp(varName, "yprev") == 0)
        return new ExpressionFilterNode::NodeVar(hostNode, varName);
    return nullptr;
}

ExpressionFilterNode::ExpressionFilterNode(const char *text)
{
    skipFirstDatum = false;
    expr = new Expression();
    VariableResolver resolver(this);
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

