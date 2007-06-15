//=========================================================================
//  FILTERNODES.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <math.h>
#include "channel.h"
#include "customfilter.h"


/**
 * Function object to implement all math.h functions.
 */
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
    virtual Expression::StkValue evaluate(Expression::StkValue args[], int numargs) {
        return 3.14; //FIXME
    }
    virtual std::string toString(std::string args[], int numargs) {return varname.c_str();}
};



class Resolver : public Expression::Resolver
{
  public:
    virtual ~Resolver() {};
    virtual Expression::Functor *resolveVariable(const char *varname);
    virtual Expression::Functor *resolveFunction(const char *funcname, int argcount);
};

Expression::Functor *Resolver::resolveVariable(const char *varname)
{
    if (strcmp(varname, "x")==0)
        return NULL; //XXX
    else if (strcmp(varname, "y")==0)
        return NULL; //XXX
    else
        throw opp_runtime_error("Unrecognized variable: %s", varname);
}

Expression::Functor *Resolver::resolveFunction(const char *funcname, int argcount)
{
    //FIXME check argcount!
    if (MathFunction::supports(funcname))
        return new MathFunction(funcname);
    else
        throw opp_runtime_error("Unrecognized function: %s()", funcname);
}

CustomFilterNode::CustomFilterNode(const char *text)
{
    expr = new Expression();
    Resolver resolver;
    expr->parse(text, &resolver);
}

CustomFilterNode::~CustomFilterNode()
{
    delete expr;
}

bool CustomFilterNode::isReady() const
{
    return in()->length()>0;
}

void CustomFilterNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        d.y = expr->doubleValue();
        out()->write(&d,1);
    }
}

//--

const char *CustomFilterNodeType::description() const
{
    return "Evaluates an arbitrary expression";
}

void CustomFilterNodeType::getAttributes(StringMap& attrs) const
{
    attrs["expression"] = "The expression to evaluate. Use x for time, and y for value."; //FIXME use "t" and "x" instead?
}

Node *CustomFilterNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new CustomFilterNode(attrs["expression"].c_str());
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}


