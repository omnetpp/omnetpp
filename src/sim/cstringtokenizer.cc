//==========================================================================
//  CSTRTOKENIZER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cStringTokenizer  : string tokenizer utility class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <climits>  // LONG_MAX
#include <clocale>
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cexception.h"
#include "common/stringtokenizer.h"

namespace omnetpp {

static_assert(cStringTokenizer::NONE == (int)common::StringTokenizer::NONE, "enum value mismatch");
static_assert(cStringTokenizer::KEEP_EMPTY == (int)common::StringTokenizer::KEEP_EMPTY, "enum value mismatch");
static_assert(cStringTokenizer::NO_TRIM == (int)common::StringTokenizer::NO_TRIM, "enum value mismatch");
static_assert(cStringTokenizer::HONOR_QUOTES == (int)common::StringTokenizer::HONOR_QUOTES, "enum value mismatch");
static_assert(cStringTokenizer::HONOR_PARENS == (int)common::StringTokenizer::HONOR_PARENS, "enum value mismatch");

cStringTokenizer::cStringTokenizer(const char *s, const char *delim, int options)
{
    impl = new common::StringTokenizer(s, delim, options);
}

cStringTokenizer::~cStringTokenizer()
{
    delete impl;
}

void cStringTokenizer::setDelimiterChars(const char *delim)
{
    impl->setDelimiterChars(delim);
}

void cStringTokenizer::setParenthesisChars(const char *parens)
{
    impl->setParenthesisChars(parens);
}

void cStringTokenizer::setQuoteChars(const char *quotes)
{
    impl->setQuoteChars(quotes);
}

const char *cStringTokenizer::nextToken()
{
    return impl->nextToken();
}

bool cStringTokenizer::hasMoreTokens()
{
    return impl->hasMoreTokens();
}

std::vector<std::string> cStringTokenizer::asVector()
{
    std::vector<std::string> vec;
    while (const char *token = nextToken())
        vec.push_back(token);
    return vec;
}

std::vector<int> cStringTokenizer::asIntVector()
{
    const char *s;
    std::vector<int> v;
    while ((s = nextToken()) != nullptr) {
        char *e;
        errno = 0;
        long d = strtol(s, &e, 10);
        if (*e)
            throw cRuntimeError("Converting string to a vector of ints: Error at token '%s'", s);
        if ((int)d != d || (errno == ERANGE && (d == LONG_MAX || d == LONG_MIN)))
            throw cRuntimeError("Converting string to a vector of ints: '%s' does not fit into an int", s);
        v.push_back((int)d);
    }
    return v;
}

std::vector<double> cStringTokenizer::asDoubleVector()
{
    const char *s;
    std::vector<double> v;
    setlocale(LC_NUMERIC, "C");
    while ((s = nextToken()) != nullptr) {
        char *e;
        double d = strtod(s, &e);
        if (*e)
            throw cRuntimeError("Converting string to a vector of doubles: Error at token '%s'", s);
        if (d == HUGE_VAL)
            throw cRuntimeError("Converting string to a vector of doubles: Overflow or underflow while converting '%s' to double", s);
        v.push_back(d);
    }
    return v;
}

}  // namespace omnetpp

