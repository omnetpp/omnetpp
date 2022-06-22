//==========================================================================
//  CSTRINGTOKENIZER.H - part of
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

#ifndef __OMNETPP_CSTRINGTOKENIZER_H
#define __OMNETPP_CSTRINGTOKENIZER_H

#include <string>
#include <vector>
#include "simkerneldefs.h"

namespace omnetpp {

namespace common {
class StringTokenizer;
}

/**
 * @brief String tokenizer that supports quoted strings and nested parenthesis.
 *
 * It considers the input string to consist of tokens, separated by one or more
 * delimiter characters. Repeated calls to nextToken() will enumerate the tokens
 * in the string, returning nullptr after the last token. The function
 * hasMoreTokens() can be used to find out whether there are more tokens
 * without consuming one. There is also an asVector() method that returns
 * all tokens at once in a string vector.
 *
 * By default, tokens are trimmed of leading and trailing whitespace, and empty
 * tokens are discarded. Trimming and empty token discarding can be turned off by
 * specifying the NO_TRIM and KEEP_EMPTY options to the constructor, respectively.
 *
 * KEEP_EMPTY is advised for delimiters that act as field separators, such as
 * comma, semicolon, colon or tab. When KEEP_EMPTY is enabled, the number of
 * tokens returned is \#separators+1, except for empty input which produces zero
 * items, and blank input which produces zero or one token, depending on whether
 * NO_TRIM was also specified.
 *
 * The tokenizer may be asked not to split up substrings protected by a pair of
 * quote characters ("string constants"). This is enabled with the HONOR_QUOTES option.
 * Use backslash escaping to put quote chars inside strings: "Hello \" World".
 * The list of characters to be treated as quotation marks can be specified,
 * the default is double quote " and apostrophe '.  Unterminated string constants
 * will be reported with an exception.
 *
 * The tokenizer may also be asked not to split up parenthesized expressions.
 * This is enabled with the HONOR_PARENS option. The list of character pairs
 * (open+close) to be treated as parens can be specified, the default is parens,
 * square brackets and curly braces. Proper paren nesting is checked, and
 * mismatches, unterminated and unmatched parens are reported with an exception.
 *
 * Example 1:
 *
 * <pre>
 * const char *str = "one two three four";
 * cStringTokenizer tokenizer(str);
 * while (tokenizer.hasMoreTokens())
 *     EV << " " << tokenizer.nextToken();
 * </pre>
 *
 * Example 2:
 *
 * <pre>
 * const char *str = "42 13 46 72 41";
 * std::vector<string> array = cStringTokenizer(str).asVector();
 * </pre>
 *
 * @ingroup Misc
 */
class SIM_API cStringTokenizer
{
  public:
    enum Options {
        NONE = 0,
        KEEP_EMPTY = 1 << 1,
        NO_TRIM = 1 << 2,
        HONOR_QUOTES = 1 << 3,
        HONOR_PARENS = 1 << 4
    };

  private:
    common::StringTokenizer *impl;

    cStringTokenizer(const cStringTokenizer&) = delete;
    void operator=(const cStringTokenizer&) = delete;

  public:
    /**
     * Sets up a tokenizer to split the input string by whitespace, trim all tokens,
     * and discard empty tokens.
     */
    cStringTokenizer(const char *str, const char *sep=" \t\n\r\f") : cStringTokenizer(str, sep, NONE) {}

    /**
     * Sets up a tokenizer to split the input string by the selected delimiters,
     * and options.
     */
    cStringTokenizer(const char *str, const char *delimiters, int options);

    /**
     * Destructor.
     */
    ~cStringTokenizer();

    /**
     * Sets the set of separator characters.
     */
    void setDelimiterChars(const char *s);

    /**
     * Sets the set of characters to act as quote characters. The default setting is
     * double quote and apostrophe. This is only significant of honoring quotes is
     * enabled.
     */
    void setQuoteChars(const char *quotes);

    /**
     * Sets the set of parenthesis characters. It must be an alternating sequence
     * of open/close characters. The default is "()[]{}". This is only significant
     * if honoring parentheses is enabled.
     */
    void setParenthesisChars(const char *parens);

    /**
     * Returns true if there're more tokens (i.e. the next nextToken()
     * call won't return nullptr).
     */
    bool hasMoreTokens();

    /**
     * Returns the next token. The returned pointers will stay valid as long
     * as the tokenizer object exists. If there're no more tokens, nullptr
     * will be returned.
     */
    const char *nextToken();

    /**
     * Utility function: tokenizes the full input string at once, and
     * returns the tokens in a string vector.
     */
    std::vector<std::string> asVector();

    /**
     * Utility function: converts all tokens to int, and returns them
     * in a vector.
     */
    std::vector<int> asIntVector();

    /**
     * Utility function: converts all tokens to double, and returns them
     * in a vector.
     */
    std::vector<double> asDoubleVector();
};

}  // namespace omnetpp


#endif

