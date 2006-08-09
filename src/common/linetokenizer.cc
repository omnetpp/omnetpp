//=========================================================================
//  LINETOKENIZER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <assert.h>
#include <sstream>
#include "linetokenizer.h"


LineTokenizer::LineTokenizer()
{
    vecsize = 16000;  // max 16,000 tokens per line (still vec<64K)
    vec = new char *[vecsize];

    errcode = OK;
}

LineTokenizer::~LineTokenizer()
{
    delete [] vec;
}

bool LineTokenizer::tokenize(char *line)
{
    numtokens = 0;
    char *s = line;

    // loop through the tokens on the line
    for (;;)
    {
        // skip spaces before token
        while (*s==' ' || *s=='\t') s++;

        char *token;
        if (!*s)
        {
            // end of line found -- exit loop
            break;
        }
        else if (*s=='"')
        {
            // parse quoted string
            token = s+1;
            s++;
            // try find end of quoted string
            while (*s && (*s!='"' || *(s-1)=='\\')) s++;
            // check we found the close quote
            if (*s!='"')
            {
                errcode = UNMATCHEDQUOTE;
                return false;
            }
            // terminate quoted string with zero, overwriting close quote
            *s++ = 0;
        }
        else
        {
            // parse unquoted string
            token = s;
            // try find end of string
            while (*s && *s!=' ' && *s!='\t') s++;
            // terminate string with zero (if we are not already at end of the line)
            if (*s) *s++ = 0;
        }

        // add token to the array (if there's room); s points to the rest of the string
        if (numtokens==vecsize)
        {
            errcode = TOOMANYTOKENS;
            return false;
        }
        vec[numtokens++] = token;
    }
    return true;
}

std::string LineTokenizer::errorMsg(int linenum) const
{
    std::stringstream out;
    switch (errcode)
    {
        case OK:             out << "OK"; break;
        case UNMATCHEDQUOTE: out << "unmatched quote"; break;
        case TOOMANYTOKENS:  out << "too many tokens per line"; break;
        default:             out << "???";
    }
    out << ", line " << linenum;
    return out.str();
}

/*
Example code:

#include <string.h>
#include <iostream>
using namespace std;

void tok(const char *s)
{
    char *buf = new char[strlen(s)+1];
    strcpy(buf, s);
    cout << buf << " --> ";

    LineTokenizer t;
    bool ok = t.tokenize(buf);
    if (!ok)
        cout << t.errorMsg(1) << endl;

    int numtokens = t.numTokens();
    char **vec = t.tokens();
    for (int i=0; i<numtokens; i++)
        cout << (i==0?"":":") << vec[i];
    cout << "\n";
}

int main(int argc, char **argv)
{
    tok("E 121.1344 e434");
    tok("E \"121.1344 e434\" 222");

    return 0;
}
*/


