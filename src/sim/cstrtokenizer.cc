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
#include "cstrtokenizer.h"
#include "cexception.h"


cStringTokenizer::cStringTokenizer(const char *s, const char *delim)
{
    if (!s)
        s = "";
    if (!delim || !*delim)
        delim = " ";
    delimiter = delim;
    str = new char[strlen(s)+1];
    strcpy(str,s);
    strend = str+strlen(str);
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

const char *cStringTokenizer::nextToken()
{
    if (!rest)
        return NULL;
    char *token = strtok(rest, delimiter.c_str());
    rest = token ? token+strlen(token)+1 : NULL;
    if (rest && rest>=strend)
        rest = NULL;
    return token;

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
            throw new cRuntimeError("Converting string to integer vector: error at token '%s'", s);
        if (errno==ERANGE || (int)d != d)
            throw new cRuntimeError("Converting string to integer vector: '%s' does not fit into an int", s);
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
            throw new cRuntimeError("Converting string to double vector: error at token '%s'", s);
        if (errno==ERANGE)
            throw new cRuntimeError("Converting string to double vector: overflow or underflow while converting '%s' to double", s);
        v.push_back(d);
    }
    return v;
}


