//==========================================================================
//  SCENARIO.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

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

NAMESPACE_BEGIN


//----

Scenario::Scenario(const std::vector<IterationVariable>& iterationVariables, const char *constraintText) :
        vars(iterationVariables)
{
    // store the constraint
    constraint = NULL;
    if (constraintText)
    {
        constraint = new ValueIterator::Expr(constraintText);
    }

    // fill the variable tables
    variterators.resize(vars.size());
    for (int i=0; i<(int)vars.size(); i++)
    {
        ASSERT(!vars[i].varid.empty() && !vars[i].value.empty());
        try
        {
            variterators[i].parse(vars[i].value.c_str());
        }
        catch (std::exception& e)
        {
            throw cRuntimeError("Cannot parse iteration variable '%s' (%s in %s)",
                                    vars[i].varname.c_str(), e.what(), vars[i].value.c_str());
        }
    }

    // create map for variable lookup
    for (int i = 0; i < (int)vars.size(); i++)
    {
        varmap[vars[i].varid] = &variterators[i];
    }

    // check for undefined variables early, for better error reporting
    for (int i=0; i < (int)vars.size(); i++)
    {
        const std::set<std::string> &referencedVars = variterators[i].getReferencedVariableNames();
        for (std::set<std::string>::const_iterator varname=referencedVars.begin(); varname!=referencedVars.end(); ++varname)
            if (varmap.find(*varname) == varmap.end())
                throw cRuntimeError("Undefined variable $%s, referenced from the definition of $%s (%s)",
                                        varname->c_str(), vars[i].varname.c_str(), vars[i].value.c_str());

        if (!vars[i].parvar.empty())
        {
            if (varmap.find(vars[i].parvar) == varmap.end())
                throw cRuntimeError("Undefined variable $%s, referenced from the definition of $%s as parallel variable",
                                        vars[i].parvar.c_str(), vars[i].varname.c_str());

        }
    }

    // determine sort order (loop nesting order) of the variables

    // Strategy: use index as the pool of variables not yet processed.
    // In each step, pick one variable from the index that does not refer to
    // the ones still in the index, remove it from the index and add
    // it to the sort order as the next innermost loop. If we cannot find
    // any such variable, there is a cycle among the remaining ones.

    std::map<std::string, int> index;
    for (int i=0; i<(int)vars.size(); i++)
        index[vars[i].varid] = i;
    sortOrder.clear();
    while(!index.empty())
    {
        // find one variable that is references only variables removed from the index (i.e. outer variables)
        int indexOfVar = -1;
        for (int i = 0; i < (int)vars.size(); ++i)
        {
            if (index.find(vars[i].varid) != index.end())
            {
                bool allReferencedVariablesAreAdded = true;
                std::set<std::string> varrefs = variterators[i].getReferencedVariableNames();
                if (!vars[i].parvar.empty())
                    varrefs.insert(vars[i].parvar);
                for (std::set<std::string>::const_iterator v = varrefs.begin(); v != varrefs.end(); ++v)
                    if (index.find(*v) != index.end()) {
                        allReferencedVariablesAreAdded = false;
                        break;
                    }

                if (allReferencedVariablesAreAdded) {
                    indexOfVar = i;
                    break;
                }
            }
        }

        if (indexOfVar >= 0)
        {
            sortOrder.push_back(indexOfVar);
            index.erase(vars[indexOfVar].varid);
        }
        else
            throw cRuntimeError("Cycle detected in iteration variable references");
    }

    restart();
}

Scenario::~Scenario()
{
    delete constraint;
}

Expression::Value Scenario::getIterationVariableValue(const char *varname)
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
    {
        if (count>1000000)
            throw cRuntimeError("Are you sure you want to generate more than one million runs?");
        count++;
    }
    return count;
}

bool Scenario::restart()
{
    if (variterators.size()==0)
        return true;  // it is valid to have no iterations at all

    // reset all iterators; must be done in the nesting order (innermost
    // loop first), because start values may depend on vars of outer loops
    // If any of them are immediately at end(), step the outer loop and retry.
    // If there is no valid state return false.
    for (int i=0; i<(int)sortOrder.size(); i++)
    {
        int j = sortOrder[i];
        variterators[j].restart(varmap);
        if (variterators[j].end())
        {
            // step outer vars and retry
            if (i == 0 || !incOuter(i))
                return false;
            --i;
        }
    }

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

// inc the outer n iterators, restarts the rest
bool Scenario::incOuter(int n)
{
    // try incrementing iteration variables in inner->outer order
    int i;
    for (i=n-1; i>=0; i--)
    {
        int k = sortOrder[i];
        if (!isParallelIteration(k))
        {
            variterators[k]++;
            if (!variterators[k].end())
                break; // if incrementing was OK, we're done
        }
    }

    // the ith variable has been incremented successfully
    if (i < 0)
        return false;

    // restart inner variables in outer->inner order, update parallel variables
    for (int j=i+1; j < (int)sortOrder.size(); ++j)
    {
        int k = sortOrder[j];
        if (isParallelIteration(k))
        {
            int pos = getIteratorPosition(vars[k].parvar.c_str());
            bool ok = variterators[k].gotoPosition(pos, varmap);
            if (!ok)
                throw cRuntimeError("parallel iterator ${...!%s} does not have enough values", vars[k].parvar.c_str());
        }
        else
        {
            variterators[k].restart(varmap);
            if (variterators[k].end())
            {
                if (j == 0 || !incOuter(j))
                    return false;
                --j;
            }
        }
    }

    return true;
}

bool Scenario::evaluateConstraint()
{
    try
    {
        constraint->substituteVariables(varmap);
        constraint->evaluate();
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
    ValueIterator *valueIter = it->second;
    if (valueIter->end())
        throw cRuntimeError("Variable '%s' has no more values", varid);
    return valueIter->get();
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
    for (int i=0; i<(int)sortOrder.size(); i++)
    {
        int j = sortOrder[i];
        out << (i>0?", ":"") << "$" << vars[j].varname << "=" << variterators[j].get();
    }
    return out.str();
}

NAMESPACE_END

