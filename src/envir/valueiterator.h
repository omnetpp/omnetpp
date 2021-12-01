//==========================================================================
//  VALUEITERATOR.H - part of
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

#ifndef __OMNETPP_ENVIR_VALUEITERATOR_H
#define __OMNETPP_ENVIR_VALUEITERATOR_H

#include <string>
#include <vector>
#include <map>
#include "common/expression.h"
#include "envirdefs.h"

namespace omnetpp {
namespace envir {

/**
 * Support class for parsing simulation scenarios. Given a sequence of
 * values in string form, this class can enumerate those values.
 * Generating numeric sequences with a simple from-to-step syntax
 * is also supported.
 *
 * Syntax: items separated by comma. Quotation marks are respected
 * (that is, commas are ignored within string constants).
 * If an item has the format <tt><i>expr..expr</i></tt> or
 * <tt><i>expr..expr</i> step <i>expr</i></tt>, it generates
 * a numeric sequence.
 *
 * Items may contain variable references, in the $var or ${var} syntax.
 * For single items, variables will be textually substituted; for numeric
 * sequences (from/to/step), variables will be textually substituted, then
 * the from/to/step expressions will be evaluated using the Expression class
 * in src/common.
 *
 * Usage: the string should be passed to the constructor or the parse() method.
 * Before iteration, the restart() method MUST be called, with a map of variables
 * to be substituted as argument. Then iteration may begin, using get() and
 * operator++. Iteration may be restarted any number of times.
 */
class ENVIR_API ValueIterator
{
  public:
    typedef omnetpp::common::Expression Expression;
    typedef omnetpp::common::expression::ExprValue ExprValue;
    typedef std::map<std::string,ValueIterator*> VariableMap; // maps variable names to their iterators

    /**
     * Represents an element in the iteration string (a single item, or one of
     * the from/to/step expressions); stores both the original ("raw") string,
     * and the result after variable substitution and optional evaluation.
     * Currently also used for the constraint= expression.
     */
    class Expr {
      private:
        std::string raw; // original text of item; may contain variable references
        ExprValue value;  // value after variable substitution and optional evaluation; filled in by substituteVariables() and evaluate()
        void checkType(ExprValue::Type expected) const;
      public:
        Expr() {}
        Expr(const char *text) : raw(text) {}
        void collectVariablesInto(std::set<std::string>& vars) const;
        void substituteVariables(const VariableMap& map);  // raw -> value
        void evaluate(); // value -> value
        double dblValue() const { if (value.getType() == ExprValue::INT) return (intval_t)value; checkType(ExprValue::DOUBLE); return (double)value; }
        bool boolValue() const { checkType(ExprValue::BOOL); return (bool)value; }
        std::string strValue() const { checkType(ExprValue::STRING); return (std::string)value; }
    };

  private:
    /**
     * The parsed iteration spec; it can be
     *   - constant text
     *  - '<from>..<to>' where <from> and <to> are either contants or numeric expressions
     *  - '<from>..<to> step <step>' where <from>, <to> and <step> are either constants or numeric expressions
     */
    struct Item {
        enum Type { TEXT, FROM_TO_STEP};
        Type type = TEXT;
        Expr text;
        Expr from, to, step;

        void parse(const char *s);
        void collectVariablesInto(std::set<std::string>& result) const;
        void restart(const VariableMap& map);

        int getNumValues() const;
        std::string getValueAsString(int k) const;
    };

    std::vector<Item> items;

    // iteration state
    int itemIndex = 0;  // index into items[]
    int k = 0;          // index within the item

    // counts how many times op++() was invoked on the iterator
    int pos = 0;

    // names of variables referenced in this value iterator
    std::set<std::string> referredVariables;

  public:
    /**
     * Default constructor.
     */
    ValueIterator() {}

    /**
     * Constructor; the iteration is parsed from the string argument.
     */
    ValueIterator(const char *s);

    /**
     * Destructor.
     */
    ~ValueIterator() {}

    /**
     * Parses the given string.
     */
    void parse(const char *s);

    /**
     * Returns the variable names referenced in the iteration string, e.g. "x", "a", "b"
     * in "${1,5,$x+2,$a..$b}". Used by Scenario to calculate iteration variable ordering.
     * (Referenced variables should become outer loops to this one.)
     */
    std::set<std::string> getReferencedVariableNames() const { return referredVariables; }

    /**
     * Stateless access: returns the length of the sequence, with numeric
     * ranges expanded. Does not change the state of the iterator.
     *
     * Note: restart() MUST BE CALLED before iteration!
     */
    int length() const;

    /**
     * Stateless access: returns the ith value in the sequence.
     * If i<0 or i>=length(), an error is thrown.
     * Does not change the state of the iterator.
     *
     * Note: restart() MUST BE CALLED before iteration!
     */
    std::string get(int k) const;

    /**
     * Restarts the iteration. It substitutes variables in the expression
     * defining this iterator and re-evaluates the <from>, <to>, <step> expressions.
     */
    void restart(const VariableMap& vars);

    /**
     * Moves the iterator to the next element.
     *
     * Note: restart() MUST BE CALLED before iteration!
     */
    void operator++(int);

    /**
     * Returns the current element. Result is undefined when the iteration is
     * over (end() returns true).
     *
     * Note: restart() MUST BE CALLED before iteration!
     */
    std::string get() const;

    /**
     * Same as get().
     */
    std::string operator()() const  {return get();}

    /**
     * Returns how many times operator++ was invoked on the iterator
     * since construction or the last restart() call.
     */
    int getPosition() const  {return pos;}

    /**
     * Restart the iteration, and go to the ith position in the sequence.
     * Returns false if there is no ith element. The vars argument will
     * be passed to restart().
     */
    bool gotoPosition(int pos, const VariableMap& vars);

    /**
     * Returns true when the iteration is over, that is, after invoking
     * operator++() after the last element.
     */
    bool end() const;

    /**
     * For debugging purposes
     */
    void dump() const;
};

}  // namespace envir
}  // namespace omnetpp


#endif
