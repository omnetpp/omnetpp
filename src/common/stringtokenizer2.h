//==========================================================================
//  STRINGTOKENIZER2.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_STRINGTOKENIZER2_H
#define __OMNETPP_COMMON_STRINGTOKENIZER2_H

#include <string>
#include <vector>
#include "commondefs.h"
#include "exception.h"

namespace omnetpp {
namespace common {

/**
 * Exception thrown when a syntax error found while tokenizing a string.
 */
class COMMON_API StringTokenizerException : public opp_runtime_error
{
  public:
    /**
     * Constructor with error message generated in a printf-like manner.
     */
    StringTokenizerException(const char *msg, ...);

    /**
     * Destructor with throw clause required by gcc.
     */
    virtual ~StringTokenizerException() throw() {}
};

/**
 * Similar to StringTokenizer, but supports nested parenthesis, quoted
 * strings and backslash quoted characters in tokens
 * (tokens do not end with unbalanced parentheses and can contain strings constants).
 *
 * Example1:
 *
 * <pre>
 * const char *str = "This 'string contains' {a lot} (of {[{irritating}]}) 'parentheses: {,[ and ('";
 * StringTokenizer tokenizer(str, " ", "()[]{}", "'");
 * while (tokenizer.hasMoreTokens())
 *     EV << tokenizer.nextToken() << "\n";
 * </pre>
 *
 * Output:
 * <pre>
 * This
 * 'string contains'
 * {a lot}
 * (of {[{irritating}]})
 * 'parentheses: {,[ and ('
 * </pre>
 *
 * Example2:
 *
 * <pre>
 * const char *str = "\\\\ \\  \\{ \\} \\( \\) \\[ \\] \\' \\n";
 * StringTokenizer tokenizer(str, " ", "()[]{}", "'");
 * while (tokenizer.hasMoreTokens())
 *     EV << tokenizer.nextToken() << "|";
 * </pre>
 *
 * Output:
 * <pre>
 * \| |{|}|(|)|[|]|'|\n|
 * </pre>
 *
 *
 * @ingroup SimSupport
 */
class COMMON_API StringTokenizer2
{
  private:
    char *str; // copy of the input string, modified during the process
    char *end; // points to the ending '\0' of str
    char *dst; // points to the next output character str
    char *src; // points to the next input character str
    int tokenStart; // index of the first character of the current token in the original string
    int tokenEnd; // index of the first character after the current token in the original string
    std::string delimiterChars;
    std::string parenChars;
    std::string quoteChars;

    void skipDelimiters();
    char *readToken();
    bool isQuote(char ch) { return quoteChars.find(ch) != std::string::npos; }
    bool isParen(char ch) { return parenChars.find(ch) != std::string::npos; }
    bool isDelimiter(char ch) { return delimiterChars.find(ch) != std::string::npos; }


  public:
    /**
     * Constructor.
     * All input parameters (str, delimiters, parens, and quotes) are copied in the constructor,
     * so they can safely be deleted after the call.
     * The delimiters default to all whitespace characters (space, tab, CR, LF, FF).
     * The parens must be an alternating sequence of open/close characters and
     * it is default to "()".
     * The quotes default to the double quote character.
     */
    StringTokenizer2(const char *str, const char *delimiters=nullptr, const char *parens=nullptr, const char *quotes=nullptr);

    /**
     * Destructor.
     */
    ~StringTokenizer2();

    /**
     * Change delimiters. This allows for switching delimiters during
     * tokenization.
     */
    void setDelimiter(const char *s);

    /**
     * Change parenthesis characters.
     * It must be an alternating sequence of open/close characters.
     */
    void setParentheses(const char *parens);

    /**
     * Change quote characters.
     */
    void setQuotes(const char *quotes);


    /**
     * Returns true if there're more tokens (i.e. the next nextToken()
     * call won't return nullptr).
     */
    bool hasMoreTokens();

    /**
     * Returns the next token and moves forward the iteration.
     * The returned pointer will point to the beginning
     * of the next token in the original input string, 'end' will be set
     * to the end of the token (exclusive).
     * If there're no more tokens, nullptr will be returned and 'end'
     * will not be set.
     */
    const char *nextToken();

    /**
     * Returns the character index of the current token in the original input string.
     */
    int getTokenStart() { return tokenStart; }

    /**
     * Returns the length of the current token in the original input string.
     * It can differ from strlen(nextToken()), due to \ quoting of characters.
     */
    int getTokenLength() { return tokenEnd - tokenStart; }
};

inline void StringTokenizer2::skipDelimiters()
{
    while (src < end && (*src=='\0' || isDelimiter(*src)))
        src++;
}


} // namespace common
}  // namespace omnetpp


#endif

