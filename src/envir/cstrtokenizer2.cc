//==========================================================================
//  CSTRTOKENIZER2.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cStringTokenizer2  : string tokenizer utility class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <assert.h>
#include <stdlib.h>
#include "cstrtokenizer2.h"


cStringTokenizer2::cStringTokenizer2(const char *s)
{
    if (!s)
        s = "";
    str = new char[strlen(s)+1];
    strcpy(str,s);
    rest = str;
}

cStringTokenizer2::~cStringTokenizer2()
{
    delete [] str;
}

bool cStringTokenizer2::hasMoreTokens()
{
    while (*rest==' ' || *rest=='\t') rest++;
    return *rest != '\0';
}

const char *cStringTokenizer2::nextToken()
{
    // we want to write "s" instead of "rest"
    char *&s = rest;

    // skip spaces before token
    while (*s==' ' || *s=='\t') s++;
    if (!*s)
        return NULL;

    char *token;
    if (*s=='"')
    {
        // parse quoted string
        token = s+1;
        s++;
        // try to find end-quote, or end of the string
        while (*s && *s!='"') s++;
        // terminate quoted string with zero, overwriting close quote (if it was there)
        if (*s)
            *s++ = 0;
    }
    else
    {
        // parse unquoted string
        token = s;
        // try find end of string
        while (*s && *s!=' ' && *s!='\t') s++;
        // terminate string with zero (if we are not already at end of the line)
        if (*s)
            *s++ = 0;
    }
    return token;
}

std::vector<std::string> cStringTokenizer2::asVector()
{
    const char *s;
    std::vector<std::string> v;
    while ((s=nextToken())!=NULL)
        v.push_back(std::string(s));
    return v;
}


