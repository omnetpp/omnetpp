//==========================================================================
//  PATMATCH.H - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//         pattern matching stuff
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPATTERNMATCHER_H
#define __CPATTERNMATCHER_H

#include <string>
#include <vector>

#include "defs.h"
#include "envdefs.h"


/**
 * Glob-style pattern matching class, adopted to special OMNeT++ requirements.
 * One instance represents a pattern to match.
 *
 * Pattern syntax:
 *   - ? : matches any character except '.'
 *   - * : matches zero or more characters except '.'
 *   - ** : matches zero or more character (any character)
 *   - {a-z} : matches a character in range a-z
 *   - {^a-z} : matches a character NOT in range a-z
 *   - {32..255} : any number (ie. sequence of digits) in range 32..255  (e.g. "99")
 *   - [32..255] : any number in square brackets in range 32..255 (e.g. "[99]")
 *   - backslash \ : takes away the special meaning of the subsequent character
 *
 * The "except '.'" phrases in the above rules apply only in "dottedpath" mode (see below).
 *
 * There are three option switches (see setPattern() method):
 *   - dottedpath: dottedpath=yes is the mode used in omnetpp.ini for matching
 *     module parameters, like this: "**.mac[*].retries=9". In this mode
 *     mode, '*' cannot "eat" dot, so it can only match one component (module
 *     name) in the path. '**' can be used to match more components.
 *     (This is similar to e.g. Java Ant's usage of the asterisk.)
 *     In dottedpath=false mode, '*' will match anything.
 *   - fullstring: selects between full string and substring match. The pattern
 *     "ate" will match "whatever" in substring mode, but not in full string
 *      mode.
 *   - case sensitive: selects between case sensitive and case insensitive mode.
 *
 * Rule details:
 *   - sets, negated sets: They can contain several character ranges and also
 *     enumeration of characters. For example: "{_a-zA-Z0-9}","{xyzc-f}". To
 *     include '-' in the set, put it at a position where it cannot be
 *     interpreted as character range, for example: "{a-z-}" or "{-a-z}".
 *     If you want to include '}' in the set, it must be the first
 *     character: "{}a-z}", or as a negated set: "{^}a-z}". A backslash
 *     is always taken as literal backslash (and NOT as escape character)
 *     within set definitions.
 *     When doing case-insensitive match, avoid ranges that include both
 *     alpha (a-zA-Z) and non-alpha characters, because they might cause
 *     funny results.
 *   - numeric ranges: only nonnegative integers can be matched.
 *     The start or the end of the range (or both) can be omitted:
 *     "{10..}", "{..99}" or "{..}" are valid numeric ranges (the last one
 *     matches any number). The specification must use exactly two dots.
 *     Caveat: "*{17..19}" will match "a17","117" and "963217" as well.
 */
class ENVIR_API cPatternMatcher
{
  private:
    enum ElemType {
      LITERALSTRING = 0,
      ANYCHAR,
      COMMONCHAR, // any char except "."
      SET,
      NEGSET,
      NUMRANGE,
      ANYSEQ,     // "**": sequence of any chars
      COMMONSEQ,  // "*": seq of any chars except "."
      END
    };

    struct Elem {
      ElemType type;
      std::string literalstring; // FIXME use one string -- maybe opp_string (std:: is too big)!
      std::string setchars; // SET/NEGSET: character pairs (0,1),(2,3) etc denote char ranges
      long fromnum, tonum; // NUMRANGE; -1 means "unset"
    };

    std::vector<Elem> pattern;
    bool iscasesensitive;

  private:
    void parseSet(const char *&s, Elem& e);
    void parseNumRange(const char *&s, Elem& e);
    void parseLiteralString(const char *&s, Elem& e);
    bool parseNumRange(const char *&str, char closingchar, long& lo, long& up);
    void dump(int from);
    bool isInSet(char c, const char *set);
    bool match(const char *line, int patternpos);

  public:
    /**
     * Constructor
     */
    cPatternMatcher();

    /**
     * Destructor
     */
    ~cPatternMatcher();

    /**
     * Sets the pattern to be used by subsequent calls to matches(). See the
     * general class description for the meaning of the rest of the arguments.
     * Throws cException if the pattern is bogus.
     */
    void setPattern(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive);

    /**
     * Returns true if the line matches the pattern with the given settings.
     * See setPattern().
     */
    bool matches(const char *line);

    /**
     * Prints the internal representation of the pattern on the standard output.
     * May be useful for debugging purposes.
     */
    void dump()  {dump(0);}

};

#endif


