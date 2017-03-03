//==========================================================================
//  STRINGTOKENIZER2.CC - part of
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

#include <cstdio>
#include <cstdarg>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stack>

#include "exception.h"
#include "commonutil.h"
#include "stringtokenizer2.h"

namespace omnetpp {
namespace common {

StringTokenizerException::StringTokenizerException(const char *messagefmt, ...) : opp_runtime_error("")
{
    char buf[1024];
    VSNPRINTF(buf, 1024, messagefmt);
    errormsg = buf;
}

StringTokenizer2::StringTokenizer2(const char *s, const char *delim, const char *parens, const char *quotes)
    : tokenStart(-1), tokenEnd(-1)
{
    if (!s)
        s = "";
    int len = strlen(s);
    str = new char[len+1];
    end = str + len;
    strcpy(str, s);
    dst = src = str;

    if (!delim || !*delim)
        delim = " \t\n\r\f";
    delimiterChars = delim;

    if (!parens)
        parens = "()";
    else if (strlen(parens) % 2 != 0)
        throw StringTokenizerException("StringTokenizer2(): Invalid parens arg: %s", parens);
    parenChars = parens;

    if (!quotes)
        quotes = "\"";
    quoteChars = quotes;
}

StringTokenizer2::~StringTokenizer2()
{
    delete[] str;
}

void StringTokenizer2::setDelimiter(const char *delim)
{
    if (!delim || !*delim)
        delim = " \t\n\r\f";
    delimiterChars = delim;
}

void StringTokenizer2::setParentheses(const char *parens)
{
    if (!parens)
        parens = "()";
    else if (strlen(parens) % 2 != 0)
        throw StringTokenizerException("StringTokenizer2::setParentheses(): Invalid parens arg: %s", parens);
    parenChars = parens;
}

void StringTokenizer2::setQuotes(const char *quotes)
{
    if (!quotes)
        quotes = "\"";
    quoteChars = quotes;
}

char *StringTokenizer2::readToken()
{
    char quoteChar = 0;
    std::stack<char> parens;
    size_t pos;

    // copy the token to the beginning of buffer
    dst = str;

    while (src < end) {
        char ch = *src;

        if (ch == '\\') {
            char next = *(src+1);
            if (next && (next == '\\' || isQuote(next) || isParen(next) || isDelimiter(next))) {
                if (quoteChar)
                    *dst++ = ch;  // preserve \ within quotes
                *dst++ = next;
                src += 2;
                continue;
            }
        }

        if (quoteChar) {  // within quote
            if (ch == quoteChar)
                quoteChar = 0;
        }
        else if (isQuote(ch))  // start quote
            quoteChar = ch;
        else if ((pos = parenChars.find(ch)) != std::string::npos) {  // paren
            if (pos % 2 == 0)  // open
                parens.push(ch);
            else {  // close
                if (parens.empty() || parens.top() != parenChars[pos-1])
                    throw StringTokenizerException("Unmatched closing parenthesis '%c'", ch);
                parens.pop();
            }
        }
        if (parens.empty() && !quoteChar && isDelimiter(ch)) {  // delim
            break;
        }

        *dst++ = *src++;
    }

    *dst = '\0';

    if (quoteChar)
        throw StringTokenizerException("Unclosed string constant");
    if (!parens.empty())
        throw StringTokenizerException("Unclosed parenthesis '%c'", parens.top());

    return str;
}

bool StringTokenizer2::hasMoreTokens()
{
    skipDelimiters();
    return *src;
}

const char *StringTokenizer2::nextToken()
{
    skipDelimiters();
    if (*src) {
        tokenStart = src - str;
        char *ptr = readToken();
        tokenEnd = src - str;
        return ptr;
    }
    else {
        tokenStart = -1;
        tokenEnd = -1;
        return nullptr;
    }
}

}  // namespace common
}  // namespace omnetpp

