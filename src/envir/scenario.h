//==========================================================================
//  SCENARIO.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

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
#include "sectionbasedconfig.h"
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
    typedef SectionBasedConfiguration::IterationVariable IterationVariable;

  private:
    struct IterationVariableExt : IterationVariable {
        ValueIterator iterator;
    };
    std::map<std::string,IterationVariableExt> variablesByName;  // varId-to-iteration
    std::vector<IterationVariableExt*> variables; // in nesting order, innermost first
    ValueIterator::Expr *constraint;

    std::map<std::string,ValueIterator*> iteratorsByName;  // only for IterationVariable

  private:
    bool inc() { return incOuter(variables.size()); }
    bool incOuter(int n);
    bool evaluateConstraint();
    Expression::Value getIterationVariableValue(const char *varname);
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

} // namespace envir
}  // namespace omnetpp


#endif


