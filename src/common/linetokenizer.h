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

/**
 * Tokenizes a line.
 */
class LineTokenizer
{
  private:
    // storage of tokens
    char **vec;
    int vecsize;
    int numtokens;

  public:
    /**
     * Constructor.
     */
    LineTokenizer();

    /**
     * Destructor.
     */
    ~LineTokenizer();

    /**
     * Tokenizes the line. Returns the number of tokens on the line.
     * Throws exception on error.
     */
    int tokenize(char *line);

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

#endif

