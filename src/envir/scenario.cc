//==========================================================================
//  SCENARIO.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include <algorithm>
#include <sstream>
#include "scenario.h"
#include "valueiterator.h"
#include "cexception.h"


/**
 * Resolves variables ($x, $y) and functions (sin, fabs, etc) in expressions.
 */
class Resolver : public Expression::Resolver
{
  private:
    Scenario *hostobject;
  public:
    Resolver(Scenario *object) {hostobject = object;}
    virtual ~Resolver() {};
    virtual Expression::Functor *resolveVariable(const char *varname);
    virtual Expression::Functor *resolveFunction(const char *funcname, int argcount);
};

Expression::Functor *Resolver::resolveVariable(const char *varname)
{
    return new Scenario::NodeVar(hostobject, varname);
}

Expression::Functor *Resolver::resolveFunction(const char *funcname, int argcount)
{
    //FIXME check argcount!
    if (MathFunction::supports(funcname))
        return new MathFunction(funcname);
    else
        throw opp_runtime_error("Unrecognized function: %s()", funcname);
}

//----

Scenario::Scenario(const std::vector<IterationSpec>& iterationSpecs, const char *conditionText) :
iterspecs(iterationSpecs)
{
    condition = NULL;
    if (conditionText)
    {
        condition = new Expression();
        Resolver resolver(this);
        condition->parse(conditionText, &resolver);
    }
}

Scenario::~Scenario()
{
}

int Scenario::getNumRuns()
{
    // and calculate Cartesian product
    double count = 1; // we use double to eliminate risk of overflow
    //XXX

    if (count > 1000000000)
        throw cRuntimeError("Scenario generates too many runs: %g", count);
    return (int) count;
}

std::vector<std::string> Scenario::generate(int runNumber)
{
    //XXX
    return std::vector<std::string>();
}

std::string Scenario::unroll()
{
    //XXX
    return "";
}


