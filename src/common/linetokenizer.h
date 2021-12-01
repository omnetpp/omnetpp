//=========================================================================
//  LINETOKENIZER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_LINETOKENIZER_H
#define __OMNETPP_COMMON_LINETOKENIZER_H

#include <vector>
#include <string>
#include "commondefs.h"

namespace omnetpp {
namespace common {

/**
 * Tokenizes a line. Uses space as default separator, and also honors quoted substrings
 * (that is, `one "two or three" four' is returned as 3 tokens `one',
 * `two or three', `four'). If a quote appears within a token, it needs to
 * be preceded by backslash. A backslash also needs to be quoted (ie.
 * duplicated) if it appears within a token.
 */
class COMMON_API LineTokenizer
{
  private:
    // separators
    char sep1;
    char sep2;

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
    LineTokenizer(int initialBufferSize = 1024, int maxTokenNum=-1 /*=initialBufferSize/4*/, char sep1=' ', char sep2='\t');

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

    std::vector<const char *> tokensVector() {
        std::vector<const char *> result;
        for (int i = 0; i < vecsize; i++)
            result.push_back(*(vec + i));
        return result;
    }
};

}  // namespace common
}  // namespace omnetpp


#endif

