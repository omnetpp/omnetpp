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
  public:
    enum StatusCode {OK, UNMATCHEDQUOTE, TOOMANYTOKENS};

  private:
    // storage of tokens
    char **vec;
    int vecsize;
    int numtokens;

    // status
    StatusCode errcode;

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
     * Tokenizes the line. Returns false on error.
     */
    bool tokenize(char *line);

    /**
     * Number of tokens read by the last readAndTokenizeLine() call.
     */
    int numTokens() {return numtokens;}

    /**
     * Array that holds the tokens read by the last readAndTokenizeLine() call;
     * contents will be overwritten with the next call.
     */
    char **tokens() {return vec;}

    /**
     * Detailed error code after tokenizeLine() returned false.
     */
    StatusCode errorCode() const {return errcode;}

    /**
     * Produces a textual error message, based on the error code
     * and the line number (to be passed into this function).
     */
    std::string errorMsg(int linenum) const;
};

#endif

