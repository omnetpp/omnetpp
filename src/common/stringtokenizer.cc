//==========================================================================
//  STRINGTOKENIZER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    StringTokenizer  : string tokenizer utility class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stack>
#include "stringutil.h"
#include "opp_ctype.h"
#include "stringtokenizer.h"

namespace omnetpp {
namespace common {

StringTokenizer::StringTokenizer(const char *s, const char *delim, int options) : delimiterChars(delim)
{
    skipEmptyTokens = !(options & KEEP_EMPTY);
    trimTokens = !(options & NO_TRIM);
    honorQuotes = options & HONOR_QUOTES;
    honorParens = options & HONOR_PARENS;

    s = opp_nulltoempty(s);
    buffer = new char[strlen(s)+1];
    strcpy(buffer, s);
    rest = buffer;
}

StringTokenizer::~StringTokenizer()
{
    delete[] buffer;
}

void StringTokenizer::setDelimiterChars(const char *delim)
{
    delimiterChars = delim;
}

void StringTokenizer::setParenthesisChars(const char *parens)
{
    if (strlen(parens) % 2 != 0)
        throw opp_runtime_error("StringTokenizer::setParentheses(): Invalid parens arg: %s", parens);
    parenChars = parens;
}

void StringTokenizer::setQuoteChars(const char *quotes)
{
    quoteChars = quotes;
}

void StringTokenizer::skipToken()
{
    if (!honorQuotes && !honorParens) {
        // fast path: no quote of paren to watch, just stop on first delimiter
        while (*rest && !isDelimiter(*rest))
            rest++;
        return;
    }

    std::stack<char> parenStack;

    while (*rest) {
        char ch = *rest;
        int pos;

        if (isDelimiter(ch) && parenStack.empty()) { // delimiter reached
            return;
        }
        else if (honorQuotes && isQuote(ch)) { // skip string constant
            rest = const_cast<char *>(opp_findclosequote(rest));
            if (!rest)
                throw opp_runtime_error("Unclosed string constant");
        }
        else if (honorParens && (pos = parenChars.find(ch)) != std::string::npos) {  // paren
            if (pos % 2 == 0)  // open
                parenStack.push(ch);
            else {  // close
                if (parenStack.empty() || parenStack.top() != parenChars[pos-1])
                    throw opp_runtime_error("Unmatched close parenthesis '%c'", ch);
                parenStack.pop();
            }
        }

        rest++;
    }

    if (!parenStack.empty())
        throw opp_runtime_error("Unclosed parenthesis '%c'", parenStack.top());

}

char *StringTokenizer::readToken()
{
    if (!*rest) {
        if (delimiterSeen) {
            // generate empty item after last delimiter
            delimiterSeen = false;
            return rest; // == ""
        }
        else {
            return nullptr;
        }
    }

    char *token = rest;
    skipToken();
    char *end = rest;

    if (trimTokens) {
        while (token < end && opp_isspace(*token))
            token++;
        while (end > token && opp_isspace(*(end-1)))
            end--;
    }

    delimiterSeen = (*rest != '\0');
    if (*rest)
        rest++; // skip delimiter

    *end = '\0'; // must be AFTER "if(*rest)" check!

    return token;
}

char *StringTokenizer::readNonemptyToken()
{
    while(true) {
        char *token = readToken();
        if (token == nullptr || *token != '\0')
            return token;
    }
}

const char *StringTokenizer::nextToken()
{
    if (readAheadToken) {
        const char *tmp = readAheadToken;
        readAheadToken = nullptr;
        return tmp;
    }
    return skipEmptyTokens ? readNonemptyToken() : readToken();
}

bool StringTokenizer::hasMoreTokens()
{
    if (readAheadToken)
        return true;
    readAheadToken = skipEmptyTokens ? readNonemptyToken() : readToken();
    return readAheadToken != nullptr;
}

std::vector<std::string> StringTokenizer::asVector()
{
    std::vector<std::string> vec;
    while (const char *token = nextToken())
        vec.push_back(token);
    return vec;
}

}  // namespace common
}  // namespace omnetpp

