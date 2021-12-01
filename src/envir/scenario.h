//==========================================================================
//  SCENARIO.H - part of
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

#ifndef __OMNETPP_ENVIR_SCENARIO_H
#define __OMNETPP_ENVIR_SCENARIO_H

#include <map>
#include <vector>
#include <set>
#include <string>
#include "common/expression.h"
#include "envirdefs.h"
#include "valueiterator.h"

namespace omnetpp {
namespace envir {

/**
 * Helper class for SectionBasedConfiguration: Resolves iterations in a scenario.
 * May be used as an iterator as well.
 */
class ENVIR_API Scenario
{
  public:
    typedef omnetpp::common::Expression Expression;
    typedef omnetpp::common::expression::ExprValue ExprValue;

    /**
     * Used during scenario resolution: an iteration variable in the
     * configuration. An iteration spec may be in one of the following forms:
     * ${1,2,5,10}; ${x=1,2,5,10}. (Note: ${x} is just an iteration variable
     * _reference_, not an an iteration variable itself.
     *
     * It is also possible to do "parallel iterations": the ${1..9 ! varname}
     * notation means that "if variable varname is at its kth iteration,
     * take the kth value from 0..9 as well". That is, this iteration and
     * varname's iterator are advanced in lockstep.
     */
    struct IterationVariable {
        std::string varName; // printable variable name ("x"); may be a generated one like "0"; never empty
        std::string value;   // "1,2,5..10"; never empty
        std::string parvar;  // "in parallel to" variable", as in the ${1,2,5..10 ! var} notation
    };

  private:
    struct IterationVariableExt : IterationVariable {
        ValueIterator iterator;
    };
    std::map<std::string,IterationVariableExt> variablesByName;
    std::vector<IterationVariableExt*> variables; // in nesting order, innermost first
    ValueIterator::Expr *constraint;

    std::map<std::string,ValueIterator*> iteratorsByName;  // only for IterationVariable

  private:
    bool inc() { return incOuter(variables.size()); }
    bool incOuter(int n);
    bool evaluateConstraint();
    ExprValue getIterationVariableValue(const char *varname);
    int getIteratorPosition(const char *varid) const;
    std::vector<std::string> resolveNestingOrderSpec(const char *orderSpec);

  public:
    Scenario(const std::vector<IterationVariable>& iterationVariables, const char *constraint, const char *nestingSpec);
    ~Scenario();

    /**
     * Returns the iteration variable names.
     */
    std::vector<std::string> getIterationVariableNames() const;

    /**
     * Counts the number of runs this scenario generates.
     *
     * The current iteration state is NOT preserved.
     */
    int getNumRuns();

    /**
     * Spins the iteration variables to the given run.
     */
    void gotoRun(int runNumber);

    /**
     * Restarts the iteration. Returns false if there is no valid config at all,
     * that is, get() and next() may not be called.
     */
    bool restart();

    /**
     * Advances to the next valid run. When it returns false, that means the
     * the iteration finished, and get() and next() may not be invoked any more.
     */
    bool next();

    /**
     * Returns the value of the given variable.
     */
    std::string getVariable(const char *varid) const;

    /**
     * Returns the current iteration state as a string ("$x=100, $y=3, $2=.4").
     * Used for the value of ${iterationvars} as well.
     */
    std::string str() const;
};

}  // namespace envir
}  // namespace omnetpp


#endif


