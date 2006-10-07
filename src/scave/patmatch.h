//==========================================================================
//  PATMATCH.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//         pattern matching stuff
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef PATMATCH_H
#define PATMATCH_H

#include <string>

// plain C interface to pattern matcher
bool contains_wildcards(const char *pattern);
bool transform_pattern(const char *from, short *topattern);
bool stringmatch(const short *pattern, const char *line);

/**
 * Pattern matcher class
 */
class PatternMatcher
{
  private:
    std::string str; // only in use of pattern didn't contain wildcard
    short *pattern; // non-null if pattern contained wildcard

  public:
    PatternMatcher();
    PatternMatcher(const PatternMatcher& other);
    PatternMatcher(const char *pattern);
    ~PatternMatcher();
    void operator=(const PatternMatcher&);
    void setPattern(const char *pattern);
    bool matches(const char *line) const;
};

#endif


