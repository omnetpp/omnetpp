//==========================================================================
//  STRINGTOKENIZER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    StringTokenizer  : string tokenizer utility class
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_STRINGTOKENIZER_H
#define __OMNETPP_COMMON_STRINGTOKENIZER_H

#include <string>
#include <vector>
#include "commondefs.h"

namespace omnetpp {
namespace common {

/**
 * String tokenizer class, modelled after strtok(). It considers the input
 * string to consist of tokens, separated by one or more delimiter characters.
 * Repeated calls to nextToken() will enumerate the tokens in the string,
 * returning nullptr after the last token. The function hasMoreTokens() can be
 * used to find out whether there are more tokens without consuming one.
 *
 * Limitations: this class does not honor quotes, apostrophes or backslash
 * quoting; nor does it return empty tokens if it encounters multiple
 * delimiter characters in a row (so setting the delimiter to "," does not
 * produce the desired results). This behaviour is consistent with strtok().
 *
 * Example 1:
 *
 * <pre>
 * const char *str = "one two three four";
 * StringTokenizer tokenizer(str);
 * while (tokenizer.hasMoreTokens())
 *     EV << " [" << tokenizer.nextToken() << "]";
 * </pre>
 *
 * Example 2:
 *
 * <pre>
 * const char *str = "42 13 46 72 41";
 * std::vector<int> array = cStringTokenizer(str).asIntVector();
 * </pre>
 *
 * @ingroup SimSupport
 */
class COMMON_API StringTokenizer
{
  private:
    char *str; // copy of full string
    char *rest; // rest of string (to be tokenized)
    std::string delimiter;

  public:
    /**
     * Constructor. The class will make its own copy of the input string
     * and of the delimiters string. The delimiters default to all whitespace
     * characters (space, tab, CR, LF, FF).
     */
    StringTokenizer(const char *str, const char *delimiters=nullptr);

    /**
     * Destructor.
     */
    ~StringTokenizer();

    /**
     * Change delimiters. This allows for switching delimiters during
     * tokenization.
     */
    void setDelimiter(const char *s);

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
};

} // namespace common
}  // namespace omnetpp


#endif

