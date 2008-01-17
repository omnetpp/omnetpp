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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>
#include <errno.h>
#include "cstrtokenizer.h"
#include "cexception.h"

USING_NAMESPACE


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

cStringTokenizer::~cStringTokenizer()
{
    delete [] str;
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
        long d = strtol(s, &e, 10);
        if (*e)
            throw cRuntimeError("Converting string to integer vector: error at token '%s'", s);
        if (errno==ERANGE || (int)d != d)
            throw cRuntimeError("Converting string to integer vector: '%s' does not fit into an int", s);
        v.push_back((int)d);
    }
    return v;
}

std::vector<double> cStringTokenizer::asDoubleVector()
{
    const char *s;
    std::vector<double> v;
    while ((s=nextToken())!=NULL)
    {
        char *e;
        double d = strtod(s, &e);
        if (*e)
            throw cRuntimeError("Converting string to double vector: error at token '%s'", s);
        if (errno==ERANGE)
            throw cRuntimeError("Converting string to double vector: overflow or underflow while converting '%s' to double", s);
        v.push_back(d);
    }
    return v;
}


