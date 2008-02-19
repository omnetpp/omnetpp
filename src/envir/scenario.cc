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
#include "stringutil.h"
#include "unitconversion.h"

USING_NAMESPACE


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
    return new Scenario::VariableReference(hostobject, varname);
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

Scenario::Scenario(const std::vector<IterationVariable>& iterationVariables, const char *constraintText) :
vars(iterationVariables)
{
    // store the constraint
    constraint = NULL;
    if (constraintText)
    {
        constraint = new Expression();
        Resolver resolver(this);
        try
        {
            constraint->parse(constraintText, &resolver);
        }
        catch (std::exception& e)
        {
            throw cRuntimeError("Cannot parse constraint expression `%s': %s", constraintText, e.what());
        }
    }

    // fill the variable tables
    variterators.resize(vars.size());
    for (int i=0; i<(int)vars.size(); i++)
    {
        ASSERT(!vars[i].varid.empty() && !vars[i].value.empty());
        variterators[i].parse(vars[i].value.c_str());
        varmap[vars[i].varid] = &variterators[i];
    }
}

Scenario::~Scenario()
{
}

Expression::StkValue Scenario::getIterationVariable(const char *varname)
{
    std::string value = getVariable(varname);
    try
    {
        if (value[0]=='"')
            return opp_parsequotedstr(value.c_str());  // strips quotes
        else if (strcmp(value.c_str(), "true")==0)
            return true;
        else if (strcmp(value.c_str(), "false")==0)
            return false;
        else
            return UnitConversion::parseQuantity(value.c_str()); // converts to double  FIXME units should NOT be accepted here!!!!!
    }
    catch (std::exception& e)
    {
        throw cRuntimeError("Wrong value for iteration variable %s: %s", varname, e.what());
    }
}

int Scenario::getNumRuns()
{
    if (!restart())
        return 0;
    int count = 1;
    while (next())
        count++;  //XXX set some maximum!
    return count;
}

bool Scenario::restart()
{
    if (variterators.size()==0)
        return true;  // it is valid to have no iterations at all

    // reset all iterators. If all of them are immediately at end(), there's no
    // valid state and we must return false
    bool ok = false;
    for (int i=0; i<(int)variterators.size(); i++)
    {
        variterators[i].restart();
        if (!variterators[i].end())
            ok = true;
    }
    if (ok==false)
        return false;

    // if there's a constraint, make sure it holds
    while (constraint && evaluateConstraint()==false)
        if (!inc()) return false;
    return true;
}

bool Scenario::next()
{
    if (!inc()) return false;

    // if there's a constraint, make sure it holds
    while (constraint && evaluateConstraint()==false)
        if (!inc()) return false;
    return true;
}

bool Scenario::inc()
{
    // try incrementing the last iteration variable first
    for (int k=variterators.size()-1; k>=0; k--)
    {
        variterators[k]++;
        if (!variterators[k].end())
            return true; // if incrementing was OK, we're done
        else
            variterators[k].restart(); // reset this counter, and go on incrementing the (k-1)th one
    }
    return false; // no variable could be incremented
}

bool Scenario::evaluateConstraint()
{
    try
    {
        return constraint->boolValue();
    }
    catch (std::exception& e)
    {
        throw cRuntimeError("Cannot evaluate constraint expression: %s", e.what());
    }
}

void Scenario::gotoRun(int runNumber)
{
    // spin the iteration variables to the given run number
    if (!restart())
        throw cRuntimeError("Iterators or constraint too restrictive: not even one run can be generated");
    for (int i=0; i<runNumber; i++)
        if (!next())
            throw cRuntimeError("Run number %d is out of range", runNumber);
}

std::string Scenario::getVariable(const char *varid) const
{
    std::map<std::string,ValueIterator*>::const_iterator it = varmap.find(varid);
    if (it==varmap.end())
        throw cRuntimeError("Unknown iteration variable: %s", varid);
    return it->second->get();
}

int Scenario::getIteratorPosition(const char *varid) const
{
    std::map<std::string,ValueIterator*>::const_iterator it = varmap.find(varid);
    if (it==varmap.end())
        throw cRuntimeError("Unknown iteration variable: %s", varid);
    return it->second->getPosition();
}

std::string Scenario::str() const
{
    std::stringstream out;
    for (int i=0; i<(int)vars.size(); i++)
        out << (i>0?", ":"") << "$" << vars[i].varname << "=" << variterators[i].get();
    return out.str();
}

