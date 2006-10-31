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
#include "exception.h"
#include "linetokenizer.h"


LineTokenizer::LineTokenizer()
{
    vecsize = 16000;  // max 16,000 tokens per line (still vec<64K)
    vec = new char *[vecsize];

    lineBufferSize = 1000;
    lineBuffer = new char[lineBufferSize];
}

LineTokenizer::~LineTokenizer()
{
    delete [] vec;
    delete lineBuffer;
}

int LineTokenizer::tokenize(char *line, int length)
{
    if (length > lineBufferSize)
        throw new Exception("Cannot tokenize lines longer than %d", lineBufferSize);

    strncpy(lineBuffer, line, lineBufferSize);

    char *s = lineBuffer + length - 1;
    while (s >= lineBuffer && (*s == '\r' || *s == '\n'))
        *s-- = '\0';

    numtokens = 0;
    s = lineBuffer;

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
                throw new Exception("Unmatched quote in file");
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
            throw new Exception("Too many tokens on a line, max %d allowed", vecsize-1);
        vec[numtokens++] = token;
    }
    return numtokens;
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


