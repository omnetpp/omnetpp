//=========================================================================
//  LINETOKENIZER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __LINETOKENIZER_H_
#define __LINETOKENIZER_H_

#include <vector>
#include <string>
#include "commondefs.h"

NAMESPACE_BEGIN

/**
 * Tokenizes a line. Uses space as separator, and also honors quoted substrings
 * (that is, `one "two or three" four' is returned as 3 tokens `one',
 * `two or three', `four'). If a quote appears within a token, it needs to
 * be preceded by backslash. A backslash also needs to be quoted (ie.
 * duplicated) if it appears within a token.
 */
class COMMON_API LineTokenizer
{
  private:
    int lineBufferSize;
    char *lineBuffer;

    // storage of tokens
    char **vec;
    int vecsize;
    int numtokens;

  public:
    /**
     * Constructor.
     */
    LineTokenizer(int lineBufferSize = 1000, int maxTokenNum=-1 /*=lineBufferSize/4*/);

    /**
     * Destructor.
     */
    ~LineTokenizer();

    /**
     * Tokenizes the line. Returns the number of tokens on the line.
     * Throws exception on error.
     */
    int tokenize(const char *line, int length);

    /**
     * Number of tokens read by the last tokenize() call.
     */
    int numTokens() {return numtokens;}

    /**
     * Array that holds the tokens read by the last tokenize() call;
     * contents will be overwritten with the next call.
     */
    char **tokens() {return vec;}
};

NAMESPACE_END


#endif

