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
#include "envirdefs.h"

NAMESPACE_BEGIN

/**
 * Support class for parsing simulation scenarios. Given a sequence of
 * values in string form, this class can enumerate those values.
 * Generating numeric sequences with a simple from-to-step syntax
 * is also supported.
 *
 * Syntax: items separated by comma. Quotation marks are respected
 * (that is, commas are ignored within string constants).
 * If an item has the format <tt><i>number..number</i></tt> or
 * <tt><i>number..number</i> step <i>number</i></tt>, it generates
 * a numeric sequence.
 *
 * Example:
 * <pre>
 * ValueIterator v;
 * v.parse("1, 2, foo, bar, 5..8, 10..100 step 10, 10..1 step -1, 1.2e8..1.6e8 step 1e+7");
 * v.dump();
 * </pre>
 */
class ENVIR_API ValueIterator
{
  private:
    // the parsed iteration spec
    struct Item {
        bool isNumeric; // discriminator between the following two
        std::string text;
        double from, to, step;
        int n; // number of steps, calculated from (from,to,step)
    };
    std::vector<Item> items;

    // iteration state
    int itemIndex;  // index into items[]
    int k;          // index within the item

    // counts how many times op++() was invoked on the iterator
    int pos;

  private:
    void parseAsNumericRegion(Item& item);

  public:
    /**
     * Constructor; if the optional string argument is specified,
     * it will get parsed.
     */
    ValueIterator(const char *s = NULL);

    /**
     * Destructor.
     */
    ~ValueIterator();

    /**
     * Parses the given string.
     */
    void parse(const char *s);

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
     * Restarts the iteration.
     */
    void restart();

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
