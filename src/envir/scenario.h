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


/**
 * Helper class for SectionBasedConfiguration: Resolves iterations in a scenario.
 */
class Scenario
{
  public:
    typedef SectionBasedConfiguration::IterationSpec IterationSpec;

  private:
    const std::vector<IterationSpec>& iterspecs;
    Expression *condition;

  public:
    /**
     * Used in resolving and generating scenarios, it implements a $x iteration variable.
     * Currently just delegates SectonBasedConfiguration::getIterationVariable().
     */
    class NodeVar : public Expression::Functor
    {
      private:
        Scenario *hostnode;
        std::string varname;
      public:
        NodeVar(Scenario *node, const char *name) {hostnode = node; varname = name;}
        virtual ~NodeVar() {}
        virtual Expression::Functor *dup() const {return new NodeVar(hostnode, varname.c_str());}
        virtual const char *name() const {return varname.c_str();}
        virtual const char *argTypes() const {return "";}
        virtual char returnType() const {return Expression::StkValue::DBL;}
        virtual Expression::StkValue evaluate(Expression::StkValue args[], int numargs)
            {return hostnode->getVariable(varname.c_str());}
        virtual std::string toString(std::string args[], int numargs) {return name();}
    };

  protected:
    Expression::StkValue getVariable(const char *varname);

  public:
    Scenario(const std::vector<IterationSpec>& iterationSpecs, const char *condition);
    ~Scenario();
    int getNumRuns();
    std::vector<std::string> generate(int runNumber);
    std::string unroll();
};

#endif


