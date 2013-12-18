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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>
#include <errno.h>
#include <limits.h>  //LONG_MAX
#include <locale.h>
#include "cstringtokenizer.h"
#include "cexception.h"

NAMESPACE_BEGIN


cStringTokenizer::cStringTokenizer(const char *s, const char *delim)
{
    if (!s)
        s = "";
    if (!delim || !*delim)
        delim = " \t\n\r\f";
    delimiter = delim;
    str = new char[strlen(s)+1];
    strcpy(str,s);
    rest = str;
}

cStringTokenizer::cStringTokenizer(const cStringTokenizer& other)
{
    str = NULL;
    copy(other);
}

cStringTokenizer::~cStringTokenizer()
{
    delete [] str;
}

void cStringTokenizer::copy(const cStringTokenizer& other)
{
    delimiter = other.delimiter;
    delete [] str;
    size_t len = (other.rest - other.str) + strlen(other.rest);
    str = new char[len+1];
    memcpy(str, other.str, len+1);
    rest = str + (other.rest - other.str);
}

cStringTokenizer& cStringTokenizer::operator=(const cStringTokenizer& other)
{
    if (this == &other) return *this;
    copy(other);
    return *this;
}

void cStringTokenizer::setDelimiter(const char *delim)
{
    if (!delim || !*delim)
        delim = " ";
    delimiter = delim;
}

inline void skipDelimiters(char *&s, const char *delims)
{
    while (*s && strchr(delims, *s)!=NULL) s++;
}

inline void skipToken(char *&s, const char *delims)
{
    while (*s && strchr(delims, *s)==NULL) s++;
}

const char *cStringTokenizer::nextToken()
{
    skipDelimiters(rest, delimiter.c_str());
    if (!*rest) return NULL;
    const char *token = rest;
    skipToken(rest, delimiter.c_str());
    if (*rest)
        *rest++ = '\0';
    return token;
}

bool cStringTokenizer::hasMoreTokens()
{
    skipDelimiters(rest, delimiter.c_str());
    return *rest;
}

std::vector<std::string> cStringTokenizer::asVector()
{
    const char *s;
    std::vector<std::string> v;
    while ((s=nextToken())!=NULL)
        v.push_back(std::string(s));
    return v;
}

std::vector<int> cStringTokenizer::asIntVector()
{
    const char *s;
    std::vector<int> v;
    while ((s=nextToken())!=NULL)
    {
        char *e;
        errno = 0;
        long d = strtol(s, &e, 10);
        if (*e)
            throw cRuntimeError("Converting string to a vector of ints: error at token '%s'", s);
        if ((int)d != d || (errno==ERANGE && (d==LONG_MAX || d==LONG_MIN)))
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
    while ((s=nextToken())!=NULL)
    {
        char *e;
        double d = strtod(s, &e);
        if (*e)
            throw cRuntimeError("Converting string to a vector of doubles: error at token '%s'", s);
        if (d==HUGE_VAL)
            throw cRuntimeError("Converting string to a vector of doubles: overflow or underflow while converting '%s' to double", s);
        v.push_back(d);
    }
    return v;
}

NAMESPACE_END

