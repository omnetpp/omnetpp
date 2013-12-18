//==========================================================================
//  STRINGTOKENIZER.CC - part of
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
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "stringtokenizer.h"

NAMESPACE_BEGIN


StringTokenizer::StringTokenizer(const char *s, const char *delim)
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

StringTokenizer::~StringTokenizer()
{
    delete [] str;
}

void StringTokenizer::setDelimiter(const char *delim)
{
    if (!delim || !*delim)
        delim = " \t\n\r\f";
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

const char *StringTokenizer::nextToken()
{
    skipDelimiters(rest, delimiter.c_str());
    if (!*rest) return NULL;
    const char *token = rest;
    skipToken(rest, delimiter.c_str());
    if (*rest)
        *rest++ = '\0';
    return token;
}

bool StringTokenizer::hasMoreTokens()
{
    skipDelimiters(rest, delimiter.c_str());
    return *rest;
}

std::vector<std::string> StringTokenizer::asVector()
{
    const char *s;
    std::vector<std::string> v;
    while ((s=nextToken())!=NULL)
        v.push_back(std::string(s));
    return v;
}

NAMESPACE_END

