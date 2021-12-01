//==========================================================================
//  PATTERNMATCHER.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_PATTERNMATCHER_H
#define __OMNETPP_COMMON_PATTERNMATCHER_H

#include <cstdio>
#include <string>
#include <vector>
#include "commondefs.h"

namespace omnetpp {
namespace common {

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
 *
 *   - dottedpath: dottedpath=true is the mode used in omnetpp.ini for matching
 *     module parameters, like this: "**.mac[*].retries=9". In this mode
 *     mode, '*' cannot "eat" dot, so it can only match one component (module
 *     name) in the path. '**' can be used to match more components.
 *     In dottedpath=false mode, '*' will match anything.
 *
 *   - fullstring: selects between full string and substring match. The pattern
 *     "tev" will match "whatever" in substring mode, but not in full string
 *      mode.
 *
 *   - case sensitive: selects between case sensitive and case insensitive mode.
 *
 * Rule details:
 *
 *   - Sets, negated sets: They can contain several character ranges and also
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
 *
 *   - Numeric ranges: only nonnegative integers can be matched.
 *     The start or the end of the range (or both) can be omitted:
 *     "{10..}", "{..99}" or "{..}" are valid numeric ranges (the last one
 *     matches any number). The specification must use exactly two dots.
 *     Caveat: "*{17..19}" will match "a17","117" and "963217" as well.
 */
class COMMON_API PatternMatcher
{
  private:
    enum ElemType {
      LITERALSTRING = 0,
      ANYCHAR,
      NONDOTCHAR,
      SET,
      NEGSET,
      NUMRANGE,
      ANYSEQ,     // "**": sequence of any chars
      NONDOTSEQ,  // "*": seq of any chars except "."
      END
    };

    struct Elem {
      ElemType type;
      std::string literalString; // if type==LITERALSTRING
      std::string setOfChars; // SET/NEGSET: character pairs (0,1),(2,3) etc denote char ranges
      long numStart, numEnd; // NUMRANGE; -1 means "unset"
    };

    std::vector<Elem> pattern;
    bool caseSensitive = true;

    mutable std::string rest; // used to pass return value from doMatch() to patternPrefixMatches()

  private:
    void parseSet(const char *&s, Elem& e);
    void parseNumRange(const char *&s, Elem& e);
    void parseLiteralString(const char *&s, Elem& e);
    bool parseNumRange(const char *&str, char closingchar, long& lo, long& up);
    std::string debugStrFrom(int from) const;
    bool isInSet(char c, const char *set) const;
    // match line from pattern[patternpos]; with last string literal, ignore last suffixlen of pattern
    bool doMatch(const char *line, int patternpos, int suffixlen) const;

  public:
    /**
     * Constructor
     */
    PatternMatcher() {}

    /**
     * Constructor
     */
    PatternMatcher(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive);

    PatternMatcher(const PatternMatcher& other) = default;
    PatternMatcher(PatternMatcher&& other) = default;

    /**
     * Destructor
     */
    ~PatternMatcher() {}

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
    bool matches(const char *line) const;

    /**
     * Similar to matches(): it returns non-nullptr iif (1) the pattern ends in
     * a string literal (and not, say, '*' or '**') which contains the line suffix
     * (which begins at suffixoffset characters of line) and (2) pattern matches
     * the whole line, except that (3) in matching the pattern's last string literal,
     * it is also accepted if line is shorter than the pattern. If the above
     * conditions hold, it returns the rest of the pattern. The returned
     * pointer is valid until the next call to this method.
     *
     * This method is used by cIniFile's <tt>getEntriesWithPrefix()</tt>, used
     * e.g. to find RNG mapping entries for a module. For that, we have to find
     * all ini file entries (keys) like <tt>"net.host1.gen.rng-NN"</tt>
     * where NN=0,1,2,... In cIniFile, every entry  is a pattern
     * (<tt>"**.host*.gen.rng-1"</tt>, <tt>"**.*.gen.rng-0"</tt>, etc.).
     * So we'd invoke <tt>patternPrefixMatches("net.host1.gen.rng-", 13)</tt>
     * (i.e. suffix=".rng-") to find those entries (patterns) which can expand to
     * <tt>"net.host1.gen.rng-0"</tt>, <tt>"net.host1.gen.rng-1"</tt>, etc.
     *
     * See matches().
     */
    const char *patternPrefixMatches(const char *line, int suffixoffset) const;

    /**
     * Returns the string representation of the pattern.
     */
    std::string str() const;

    /**
     * Returns the internal representation of the pattern as a string.
     * May be useful for debugging purposes.
     */
    std::string debugStr() const {return debugStrFrom(0);}

    /**
     * Prints the internal representation of the pattern on the standard output.
     * May be useful for debugging purposes.
     */
    void dump() const {printf("%s", debugStr().c_str());}

    /**
     * Utility function to determine whether a given string contains wildcards.
     * If it does not, a simple strcmp() might be a faster option than using
     * PatternMatcher.
     */
    static bool containsWildcards(const char *pattern);

};

}  // namespace common
}  // namespace omnetpp


#endif


