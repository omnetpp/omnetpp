//==========================================================================
//  CSTRTOKENIZER2.H - part of
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

#ifndef __STRINGTOKENIZER2_H
#define __STRINGTOKENIZER2_H

#include <string>
#include <vector>
#include "defs.h"

/**
 * A string tokenizer class which honors quotation marks but not
 * backslashes. It is designed to split up filename lists returned by
 * cInifile::getAsFilenames().
 *
 * It considers the string as consisting of tokens separated by tab or space
 * charaters. If a token contains space or tab, it has to be surrounded
 * with quotation marks. A token may NOT contain quotation mark.
 *
 * Rationale: filenames do not contain quotes but they may well contain
 * backslashes, so escaping with backslashes would not be feasible.
 */
class SIM_API cStringTokenizer2
{
  private:
    char *str; // copy of full string (will be corrupted during tokenization)
    char *rest; // rest of string (to be tokenized)

  public:
    /**
     * Constructor. The class will make its own copy of the input string.
     */
    cStringTokenizer2(const char *str);

    /**
     * Destructor.
     */
    ~cStringTokenizer2();

    /**
     * Returns true if there're more tokens (i.e. the next nextToken()
     * call won't return NULL).
     */
    bool hasMoreTokens();

    /**
     * Returns the next token. The returned pointers will stay valid as long
     * as the tokenizer object exists. If there're no more tokens,
     * a NULL pointer will be returned.
     */
    const char *nextToken();

    /**
     * Utility function: tokenizes the full input string at once, and
     * returns the tokens in a string vector.
     */
    std::vector<std::string> asVector();
};

#endif

