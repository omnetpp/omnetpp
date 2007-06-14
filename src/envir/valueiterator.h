//==========================================================================
//  VALUEITERATOR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef VALUEITERATOR_H_
#define VALUEITERATOR_H_

#include <vector>

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
 * v.dump();
 * </pre>
 */
class ValueIterator
{
  private:
    struct Item {
        bool isNumeric; // discriminator between the following two
        std::string text;
        double from, to, step;
    };
    std::vector<Item> items;

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
     * The length of the sequence, with numeric ranges expanded.
     */
    int length();

    /**
     * Returns the ith value in the sequence. If i<0 or i>=length(),
     * an error is thrown.
     */
    std::string get(int k);

    /**
     * For debugging purposes
     */
    void dump();
};

#endif
