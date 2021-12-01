//==========================================================================
//  SCENARIO.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cassert>
#include <algorithm>
#include <sstream>
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "common/unitconversion.h"
#include "common/stlutil.h"
#include "omnetpp/cexception.h"
#include "scenario.h"
#include "valueiterator.h"

using namespace omnetpp::common;
using namespace omnetpp::common::expression;

namespace omnetpp {
namespace envir {

Scenario::Scenario(const std::vector<IterationVariable>& iterationVariables, const char *constraintText, const char *nestingSpec)
{
    // store the variables along with their value iterators
    for (auto& var : iterationVariables) {
        ASSERT(!var.varName.empty() && !var.value.empty());
        auto& varExt = variablesByName[var.varName]; // add to map
        static_cast<IterationVariable&>(varExt) = var;
        try {
            varExt.iterator.parse(var.value.c_str());
        }
        catch (std::exception& e) {
            throw cRuntimeError("Cannot parse iteration variable '%s' (%s in '%s')", var.varName.c_str(), e.what(), var.value.c_str());
        }
        variables.push_back(&varExt); // point into the map
    }

    for (auto var : variables)
        iteratorsByName[var->varName] = &var->iterator;


    // store the constraint
    constraint = !opp_isempty(constraintText) ? new ValueIterator::Expr(constraintText) : nullptr;

    // check for undefined variables early, for better error reporting
    for (auto iteration : variables) {
        std::string& varName = iteration->varName;
        std::set<std::string> referencedVars = iteration->iterator.getReferencedVariableNames();
        for (auto referencedVar : referencedVars)
            if (!containsKey(variablesByName, referencedVar))
                throw cRuntimeError("Undefined variable $%s, referenced from the definition of $%s (%s)", referencedVar.c_str(), varName.c_str(), iteration->value.c_str());
        std::string& parvar = iteration->parvar;
        if (!parvar.empty()) {
            if (!containsKey(variablesByName, parvar))
                throw cRuntimeError("Undefined variable $%s, referenced from the definition of $%s as parallel variable", parvar.c_str(), varName.c_str());
        }
    }

    // determine sort order (loop nesting order) of the variables
    std::vector<std::string> varNames = resolveNestingOrderSpec(nestingSpec);

    // Reorder variables if needed.
    // Strategy: use index as the pool of variables not yet processed.
    // In each step, pick one variable from the index that does not refer to
    // the ones still in the index, remove it from the index and add
    // it to the sort order as the next innermost loop. If we cannot find
    // any such variable, there is a cycle among the remaining ones.

    std::vector<std::string> orderedVarNames;  // result

    while (!varNames.empty()) {
        // find one variable that references only variables no longer in varNames[] (i.e. outer variables)
        std::string selectedVar;
        for (auto varName : varNames) {
            auto var = variablesByName[varName];
            bool ok = true;
            std::set<std::string> referencedVars = var.iterator.getReferencedVariableNames();
            if (!var.parvar.empty())
                referencedVars.insert(var.parvar);
            for (auto referencedVar : referencedVars)
                if (contains(varNames, referencedVar))
                    ok = false;
            if (ok) {
                selectedVar = varName;
                break;
            }
        }

        if (selectedVar.empty())
            throw cRuntimeError("Cycle detected in iteration variable references");

        orderedVarNames.push_back(selectedVar);
        erase(varNames, indexOf(varNames, selectedVar));
    }

    // rebuild variables[] based on orderedVarNames[]
    variables.clear();
    for (auto varName : orderedVarNames)
        variables.push_back(&variablesByName[varName]);

    restart();
}

Scenario::~Scenario()
{
    delete constraint;
}

std::vector<std::string> Scenario::resolveNestingOrderSpec(const char *orderSpec)
{
    std::vector<std::string> orderedNames = opp_splitandtrim(opp_nulltoempty(orderSpec), ",");

    // check names are valid, and there's only one '*'
    for (std::string orderedName : orderedNames)
        if (orderedName != "*" && !containsKey(variablesByName, orderedName))
            throw cRuntimeError("Undefined variable $%s specified as part of the iteration variable nesting order", orderedName.c_str());
    if (count(orderedNames, std::string("*")) > 1)
        throw cRuntimeError("Iteration variable nesting order may only contain one '*'");

    // insert remaining variables at the "*", or at the end if there is none
    int insertionPoint = indexOf(orderedNames, std::string("*"));
    if (insertionPoint != -1)
        erase(orderedNames, insertionPoint); // remove '*'
    else
        insertionPoint = orderedNames.size();
    for (auto var : variables) {
        std::string varName = var->varName;
        if (!contains(orderedNames, varName))
            insert(orderedNames, insertionPoint++, varName);
    }

    ASSERT(orderedNames.size() == variables.size());

    return orderedNames;
}

std::vector<std::string> Scenario::getIterationVariableNames() const
{
    std::vector<std::string> result;
    for (auto var: variables)
        result.push_back(var->varName);
    return result;
}

ExprValue Scenario::getIterationVariableValue(const char *varName)
{
    std::string value = getVariable(varName);
    try {
        if (value[0] == '"')
            return opp_parsequotedstr(value.c_str());  // strips quotes
        else if (strcmp(value.c_str(), "true") == 0)
            return true;
        else if (strcmp(value.c_str(), "false") == 0)
            return false;
        else
            return UnitConversion::parseQuantity(value.c_str());  // converts to double  FIXME units should NOT be accepted here!!!!!
    }
    catch (std::exception& e) {
        throw cRuntimeError("Wrong value for iteration variable %s: %s", varName, e.what());
    }
}

int Scenario::getNumRuns()
{
    if (!restart())
        return 0;
    int count = 1;
    while (next()) {
        if (count > 1000000)
            throw cRuntimeError("Are you sure you want to generate more than one million runs?");
        count++;
    }
    return count;
}

bool Scenario::restart()
{
    if (variables.empty())
        return true;  // it is valid to have no iterations at all

    // reset all iterators; must be done in the nesting order (innermost
    // loop first), because start values may depend on vars of outer loops
    // If any of them are immediately at end(), step the outer loop and retry.
    // If there is no valid state, return false.
    for (size_t i = 0; i < variables.size(); i++) {
        IterationVariableExt *var = variables[i];
        var->iterator.restart(iteratorsByName);
        if (var->iterator.end()) {
            // step outer vars and retry
            if (i == 0 || !incOuter(i))
                return false;
            --i;
        }
    }

    // if there's a constraint, make sure it holds
    while (constraint && evaluateConstraint() == false)
        if (!inc())
            return false;

    return true;
}

bool Scenario::next()
{
    if (!inc())
        return false;

    // if there's a constraint, make sure it holds
    while (constraint && evaluateConstraint() == false)
        if (!inc())
            return false;

    return true;
}

// inc the outer n iterators, restarts the rest
bool Scenario::incOuter(int n)
{
    // try incrementing iteration variables in inner->outer order
    int i;
    for (i = n-1; i >= 0; i--) {
        IterationVariableExt *var = variables[i];
        if (var->parvar.empty()) {
            var->iterator++;
            if (!var->iterator.end())
                break;  // if incrementing was OK, we're done
        }
    }

    // the ith variable has been incremented successfully
    if (i < 0)
        return false;

    // restart inner variables in outer->inner order, update parallel variables
    for (int j = i+1; j < (int)variables.size(); ++j) {
        IterationVariableExt *var = variables[j];
        if (!var->parvar.empty()) {
            IterationVariableExt *otherIteration = &variablesByName[var->parvar];
            int pos = otherIteration->iterator.getPosition();
            bool ok = var->iterator.gotoPosition(pos, iteratorsByName);
            if (!ok)
                throw cRuntimeError("Parallel iterator ${...!%s} does not have enough values", var->parvar.c_str());
        }
        else {
            var->iterator.restart(iteratorsByName);
            if (var->iterator.end()) {
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
    try {
        constraint->substituteVariables(iteratorsByName);
        constraint->evaluate();
        return constraint->boolValue();
    }
    catch (std::exception& e) {
        throw cRuntimeError("Cannot evaluate constraint expression: %s", e.what());
    }
}

void Scenario::gotoRun(int runNumber)
{
    // spin the iteration variables to the given run number
    if (!restart())
        throw cRuntimeError("Iterators or constraint too restrictive: Not even one run can be generated");
    for (int i = 0; i < runNumber; i++)
        if (!next())
            throw cRuntimeError("Run number %d is out of range", runNumber);
}

std::string Scenario::getVariable(const char *varName) const
{
    auto it = variablesByName.find(varName);
    if (it == variablesByName.end())
        throw cRuntimeError("Unknown iteration variable '%s'", varName);
    const IterationVariableExt& var = it->second;
    if (var.iterator.end())
        throw cRuntimeError("Out of values for iteration variable '%s'", varName);
    return var.iterator.get();
}

int Scenario::getIteratorPosition(const char *varName) const
{
    auto it = variablesByName.find(varName);
    if (it == variablesByName.end())
        throw cRuntimeError("Unknown iteration variable '%s'", varName);
    const IterationVariableExt& var = it->second;
    return var.iterator.getPosition();
}

std::string Scenario::str() const
{
    std::stringstream out;
    for (auto var : variables)
        out << ", $" << var->varName << "=" << var->iterator.get();
    return out.str().substr(2);
}

}  // namespace envir
}  // namespace omnetpp

