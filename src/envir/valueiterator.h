//==========================================================================
//  VALUEITERATOR.H - part of
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

#ifndef VALUEITERATOR_H_
#define VALUEITERATOR_H_

#include <string>
#include <vector>
#include <map>
#include "envirdefs.h"
#include "expression.h"

NAMESPACE_BEGIN

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
 * a numeric sequence. Expression syntax is that accepted by
 * the Expression class in src/common. References to other iteration
 * variables are also accepted in expressions (e.g. '$x'); see
 * the Resolve class in scenario.cc.
 *
 * Example:
 * <pre>
 * ValueIterator v;
 * Expression::Resolver resolver = ...;
 * v.parse("1, 2, foo, bar, 5..8, 10..100 step 10, 10..1 step -1, 1.2e8..1.6e8 step 1e+7", &resolver);
 * v.dump();
 * </pre>
 */
class ENVIR_API ValueIterator
{
  public:
    typedef std::map<std::string,ValueIterator*> VariableMap; // maps variable names to their iterators

    class Expr {
        std::string text; // may contain variables
        Expression::Value currentValue;
        void checkType(char expected) const;
    public:
        Expr() {}
        Expr(const char *text) : text(text) {}
        void collectVariablesInto(std::set<std::string>& vars) const;
        void substituteVariables(const VariableMap& map);
        void evaluate();
        double dblValue() const { checkType('D'); return currentValue.dbl; }
        bool boolValue() const { checkType('B'); return currentValue.bl; }
        std::string strValue() const { checkType('S'); return currentValue.s; }
    };

  private:
    // the parsed iteration spec; it can be
    //   - constant text
    //   - '<from>..<to>' where <from> and <to> are either contants or numeric expressions
    //   - '<from>..<to> step <step>' where <from>, <to> and <step> are either constants or numeric expressions
    struct Item {
        enum Type { TEXT, FROM_TO_STEP};
        Type type;
        Expr text;
        Expr from, to, step;

        Item() : type(TEXT) {}
        void parse(const char *s);
        void collectVariablesInto(std::set<std::string>& result) const;
        void restart(const VariableMap& map);

        int getNumValues() const;
        std::string getValueAsString(int k) const;
    };

    std::vector<Item> items;

    // iteration state
    int itemIndex;  // index into items[]
    int k;          // index within the item

    // counts how many times op++() was invoked on the iterator
    int pos;

    // names of variables referenced in this value iterator
    std::set<std::string> referredVariables;

  public:
    /**
     * Default constructor.
     */
    ValueIterator();

    /**
     * Constructor; the iteration is parsed from the string argument.
     */
    ValueIterator(const char *s);

    /**
     * Destructor.
     */
    ~ValueIterator();

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
     */
    int length() const;


    /**
     * Stateless access: returns the ith value in the sequence.
     * If i<0 or i>=length(), an error is thrown.
     * Does not change the state of the iterator.
     */
    std::string get(int k) const;

    /**
     * Restarts the iteration. It substitutes variables in the expression
     * defining this iterator and re-evaluates the <from>, <to>, <step> expressions.
     */
    void restart(const VariableMap& vars);

    /**
     * Moves the iterator to the next element.
     */
    void operator++(int);

    /**
     * Returns the current element. Result is undefined when the iteration is
     * over (end() returns true).
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
     * Go to the ith position in the sequence.
     * Returns false if there is no ith element.
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

NAMESPACE_END


#endif
