//==========================================================================
//  FNAMELISTTOKENIZER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_FNAMELISTTOKENIZER_H
#define __OMNETPP_COMMON_FNAMELISTTOKENIZER_H

#include <string>
#include <vector>
#include "commondefs.h"
#include "commonutil.h"

namespace omnetpp {
namespace common {


/**
 * A string tokenizer class which honors quotation marks but not
 * backslashes. It is designed to split up filename lists returned by
 * cInifile::getAsFilenames().
 *
 * It considers the string as consisting of tokens separated by tab or space
 * characters. If a token contains space or tab ("C:\Program Files\X"),
 * it has to be surrounded with quotation marks. A token may NOT contain
 * quotation marks. (Rationale: filenames do not contain quotes but they
 * may well contain backslashes, so interpreting backslashes would not be
 * very practical.)
 */
class COMMON_API FilenamesListTokenizer
{
  private:
    char *str; // copy of full string (will be corrupted during tokenization)
    char *rest; // rest of string (to be tokenized)

  public:
    /**
     * Constructor. The class will make its own copy of the input string.
     */
    FilenamesListTokenizer(const char *str);

    /**
     * Destructor.
     */
    ~FilenamesListTokenizer();

    /**
     * Returns true if there're more tokens (i.e. the next nextToken()
     * call won't return nullptr).
     */
    bool hasMoreTokens();

    /**
     * Returns the next token. The returned pointers will stay valid as long
     * as the tokenizer object exists. If there're no more tokens, nullptr
     * will be returned.
     */
    const char *nextToken();

    /**
     * Utility function: tokenizes the full input string at once, and
     * returns the tokens in a string vector.
     */
    std::vector<std::string> asVector();
};

}  // namespace common
}  // namespace omnetpp


#endif

