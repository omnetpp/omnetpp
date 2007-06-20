//==========================================================================
//  SCENARIO.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef SCENARIO_H_
#define SCENARIO_H_

#include <map>
#include <vector>
#include <set>
#include <string>
#include "expression.h"
#include "sectionbasedconfig.h"
#include "valueiterator.h"


/**
 * Helper class for SectionBasedConfiguration: Resolves iterations in a scenario.
 * May be used as an iterator as well.
 */
class Scenario
{
  public:
    typedef SectionBasedConfiguration::IterationSpec IterationSpec;

  private:
    // the input
    const std::vector<IterationSpec>& iterspecs;
    Expression *condition;

    // the iteration variables, named (${x=1..5}) and unnamed (${1..5})
    std::vector<ValueIterator> itervars; // indices correspond to iterspecs[]
    std::map<std::string, ValueIterator*> namedvars;

  private:
    bool inc();
    bool evaluateCondition();
    std::string getVar(const char *varname) const;

  public:
    /**
     * Used in resolving and generating scenarios, it implements a $x iteration variable.
     * Currently just delegates SectonBasedConfiguration::getIterationVariable().
     */
    class IterationVariable : public Expression::Functor
    {
      private:
        Scenario *hostnode;
        std::string varname;
      public:
        IterationVariable(Scenario *node, const char *name) {hostnode = node; varname = name;}
        virtual ~IterationVariable() {}
        virtual Expression::Functor *dup() const {return new IterationVariable(hostnode, varname.c_str());}
        virtual const char *name() const {return varname.c_str();}
        virtual const char *argTypes() const {return "";}
        virtual char returnType() const {return Expression::StkValue::DBL;}
        virtual Expression::StkValue evaluate(Expression::StkValue args[], int numargs)
            {return hostnode->getIterationVariable(varname.c_str());}
        virtual std::string toString(std::string args[], int numargs) {return name();}
    };

  protected:
    Expression::StkValue getIterationVariable(const char *varname);

  public:
    Scenario(const std::vector<IterationSpec>& iterationSpecs, const char *condition);
    ~Scenario();

    /**
     * Counts the number of runs this scenario generates.
     *
     * The current iteration state is NOT preserved.
     */
    int getNumRuns();

    /**
     * Returns the values of the iteration variables for the given run.
     * The returned vector is the same size as the input iterationSpecs,
     * and each element contains the value for the corresponding iteration
     * variable.
     *
     * The current iteration state is NOT preserved.
     */
    std::vector<std::string> generate(int runNumber);

    /**
     * Generates all runs in the given scenario, and returns them as strings.
     * The current iteration state is NOT preserved.
     */
    std::vector<std::string> unroll();

    /**
     * Restarts the iteration. Returns false if there's no valid config at all,
     * that is, get() and next() may not be called.
     */
    bool restart();

    /**
     * Advances to the next valid run. When it returns false, that means the
     * the iteration finished, and get() and next() may not be invoked any more.
     */
    bool next();

    /**
     * During iteration (see restart() and next()), it returns the values
     * for the current run.
     */
    std::vector<std::string> get() const;

    /**
     * Returns the current iteration state as a string ("$x=100, $y=3, $2=.4")
     */
    std::string str() const;
};

#endif


