//==========================================================================
//  PATTERNMATCHER.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "omnetpp/platdep/platmisc.h"
#include "opp_ctype.h"
#include "patternmatcher.h"
#include "stringutil.h"
#include "exception.h"

namespace omnetpp {
namespace common {

PatternMatcher::PatternMatcher(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    setPattern(pattern, dottedpath, fullstring, casesensitive);
}

void PatternMatcher::setPattern(const char *patt, bool dottedpath, bool fullstring, bool casesensitive)
{
    pattern.clear();
    caseSensitive = casesensitive;

    // "tokenize" pattern
    const char *s = patt;
    while (*s != '\0') {
        Elem e;
        switch (*s) {
            case '?':
                e.type = dottedpath ? NONDOTCHAR : ANYCHAR;
                s++;
                break;

            case '[':
                if (pattern.empty() || pattern.back().type != LITERALSTRING || !parseNumRange(s, ']', e.numStart, e.numEnd))
                    parseLiteralString(s, e);
                else
                    e.type = NUMRANGE;
                break;

            case '{':
                if (parseNumRange(s, '}', e.numStart, e.numEnd)) {
                    e.type = NUMRANGE;
                    s++;
                }
                else
                    parseSet(s, e);
                break;

            case '*':
                if (*(s+1) == '*') {
                    e.type = ANYSEQ;
                    s += 2;
                }
                else {
                    e.type = dottedpath ? NONDOTSEQ : ANYSEQ;
                    s++;
                }
                break;

            default:
                parseLiteralString(s, e);
                break;
        }
        pattern.push_back(e);
    }

    if (!fullstring) {
        // for substring match, we add "**" at both ends of the pattern (unless already there)
        Elem e;
        e.type = ANYSEQ;
        if (pattern.empty() || pattern.back().type != ANYSEQ)
            pattern.push_back(e);
        if (pattern.front().type != ANYSEQ)
            pattern.insert(pattern.begin(), e);
    }
    Elem e;
    e.type = END;
    pattern.push_back(e);
}

void PatternMatcher::parseSet(const char *& s, Elem& e)
{
    s++;  // skip "{"
    e.type = SET;
    if (*s == '^') {
        e.type = NEGSET;
        s++;
    }
    // Note: to make "}" part of the set, it must be first within the braces
    const char *sbeg = s;
    while (*s && (*s != '}' || s == sbeg)) {
        char range[3];
        range[2] = 0;
        if (*(s+1) == '-' && *(s+2) && *(s+2) != '}') {
            // store "A-Z" as "AZ"
            range[0] = *s;
            range[1] = *(s+2);
            s += 3;
        }
        else {
            // store "X" as "XX"
            range[0] = range[1] = *s;
            s++;
        }
        if (!caseSensitive) {
            // if one end of range is alpha and the other is not, funny things will happen
            range[0] = opp_toupper(range[0]);
            range[1] = opp_toupper(range[1]);
        }
        e.setOfChars += range;
    }
    if (!*s)
        throw opp_runtime_error("Unmatched '}' in expression");
    s++;  // skip "}"
}

void PatternMatcher::parseLiteralString(const char *& s, Elem& e)
{
    e.type = LITERALSTRING;
    while (*s && *s != '?' && *s != '{' && *s != '*') {
        long dummy;
        const char *s1;
        if (*s == '\\')
            e.literalString += *(++s);
        else
            e.literalString += *s;
        if (*s == '[' && parseNumRange((s1 = s), ']', dummy, dummy))
            break;
        s++;
    }
}

bool PatternMatcher::parseNumRange(const char *& str, char closingchar, long& lo, long& up)
{
    //
    // try to parse "[n..m]" or "{n..m}" and return true on success.
    // str should point at "[" or "{"; on success return it'll point to "]" or "}",
    // and on failure it'll be unchanged. n and m will be stored in lo and up.
    // They are optional -- if missing, lo or up will be set to -1.
    //
    lo = up = -1L;
    const char *s = str+1;  // skip "[" or "{"
    if (opp_isdigit(*s)) {
        lo = atol(s);
        while (opp_isdigit(*s))
            s++;
    }
    if (*s != '.' || *(s+1) != '.')
        return false;
    s += 2;
    if (opp_isdigit(*s)) {
        up = atol(s);
        while (opp_isdigit(*s))
            s++;
    }
    if (*s != closingchar)
        return false;
    str = s;
    return true;
}

inline void printSetRanges(std::stringstream& out, const char *set)
{
    while (*set) {
        out << *set << "-" << *(set+1);
        set += 2;
    }
}

std::string PatternMatcher::str() const
{
    std::stringstream out;
    for (const Elem& e : pattern) {
        switch (e.type) {
            case LITERALSTRING: out << e.literalString; break;
            case ANYCHAR: out << "?"; break;
            case NONDOTCHAR: out << "?"; break;
            case ANYSEQ: out << "**"; break;
            case NONDOTSEQ: out << "*"; break;
            case SET: out << "["; printSetRanges(out, e.setOfChars.c_str()); out << "]"; break;
            case NEGSET: out << "[^"; printSetRanges(out, e.setOfChars.c_str()); out << "]"; break;
            case NUMRANGE: {
                out << "{";
                if (e.numStart >= 0)
                    out << e.numStart;
                out << "..";
                if (e.numEnd >= 0)
                    out << e.numEnd;
                out << "}";
                break;
            }
            case END: break;
            default: assert(0);
        }
    }
    return out.str();
}

std::string PatternMatcher::debugStrFrom(int from) const
{
    std::string result;
    for (int k = from; k < (int)pattern.size(); k++) {
        const Elem& e = pattern[k];
        switch (e.type) {
            case LITERALSTRING:
                result += opp_quotestr(e.literalString);
                break;

            case ANYCHAR:
                result += "ANYCHAR";
                break;

            case NONDOTCHAR:
                result += "NONDOTCHAR";
                break;

            case SET:
                result += opp_stringf("SET(%s)", e.setOfChars.c_str());
                break;

            case NEGSET:
                result += opp_stringf("NEGSET(%s)", e.setOfChars.c_str());
                break;

            case NUMRANGE:
                result += opp_stringf("NUMRANGE(%ld..%ld)", e.numStart, e.numEnd);
                break;

            case ANYSEQ:
                result += "ANYSEQ";
                break;

            case NONDOTSEQ:
                result += "NONDOTSEQ";
                break;

            case END:
                break;

            default:
                assert(0);
        }
        result += " ";
    }
    return result;
}

bool PatternMatcher::isInSet(char c, const char *set) const
{
    assert((strlen(set)&1) == 0);
    if (!caseSensitive)
        c = opp_toupper(c);  // set is already uppercase here
    while (*set) {
        if (c >= *set && c <= *(set+1))
            return true;
        set += 2;
    }
    return false;
}

bool PatternMatcher::doMatch(const char *s, int k, int suffixlen) const
{
    while (true) {
        const Elem& e = pattern[k];
        long num;  // case NUMRANGE
        int len;  // case LITERALSTRING
        switch (e.type) {
            case LITERALSTRING:
                len = e.literalString.length();
                // special case: last string literal with prefix match: allow s to be shorter
                if (suffixlen > 0 && k == (int)pattern.size()-2)
                    len -= suffixlen;
                // compare
                if (caseSensitive ?
                    strncmp(s, e.literalString.c_str(), len) :
                    strncasecmp(s, e.literalString.c_str(), len)
                    )
                    return false;
                s += len;
                break;

            case ANYCHAR:
                if (!*s)
                    return false;
                s++;
                break;

            case NONDOTCHAR:
                if (!*s || *s == '.')
                    return false;
                s++;
                break;

            case SET:
                if (!*s)
                    return false;
                if (!isInSet(*s, e.setOfChars.c_str()))
                    return false;
                s++;
                break;

            case NEGSET:
                if (!*s)
                    return false;
                if (isInSet(*s, e.setOfChars.c_str()))
                    return false;
                s++;
                break;

            case NUMRANGE:
                if (!opp_isdigit(*s))
                    return false;
                num = atol(s);
                while (opp_isdigit(*s))
                    s++;
                if ((e.numStart >= 0 && num < e.numStart) || (e.numEnd >= 0 && num > e.numEnd))
                    return false;
                break;

            case ANYSEQ:
                // potential shortcuts: if pattern ends in ANYSEQ, rest of the input
                // can be anything; if pattern ends in ANYSEQ LITERAL, it's enough if
                // input ends in the literal string
                if (k == (int)pattern.size()-2)
                    return true;
                if (k == (int)pattern.size()-3 && pattern[k+1].type == LITERALSTRING)
                    return opp_stringendswith(s, pattern[k+1].literalString.c_str());

                // general case
                while (true) {
                    if (doMatch(s, k+1, suffixlen))
                        return true;
                    if (!*s)
                        return false;
                    s++;
                }
                break;  // at EOS

            case NONDOTSEQ:
                while (true) {
                    if (doMatch(s, k+1, suffixlen))
                        return true;
                    if (!*s || *s == '.')
                        return false;
                    s++;
                }
                break;

            case END:
                return !*s;

            default:
                assert(0);
        }
        k++;
        assert(k < (int)pattern.size());
    }
}

bool PatternMatcher::matches(const char *line) const
{
    assert(pattern[pattern.size()-1].type == END);

    // shortcut: omnetpp.ini keys often begin with "*" or "**"
    // but end in a string literal. So it's usually a performance win to
    // to first check that the last string literal of the pattern matches
    // the end of the string. (We do the shortcut only in the case-sensitive
    // case. omnetpp.ini is case sensitive.)

    if (pattern.size() >= 2 && caseSensitive) {
        const Elem& e = pattern[pattern.size()-2];
        if (e.type == LITERALSTRING) {
            // return if last 2 chars don't match
            int pattlen = e.literalString.size();
            int linelen = strlen(line);
            if (pattlen >= 2 && linelen >= 2 && (line[linelen-1] != e.literalString.at(pattlen-1) ||
                                                 line[linelen-2] != e.literalString.at(pattlen-2)))  // FIXME why doesn't work for pattlen==1 ?
                return false;
        }
    }

    // perform full-blown pattern matching
    return doMatch(line, 0, 0);
}

bool PatternMatcher::covers(const char *pattern) const
{
    //TODO This is an approximate solution, it doesn't work with numeric ranges.
    // For example, "*.node[0..10].foo" covers "*.node[3..6].foo", but matches() says no
    return matches(pattern);
}

const char *PatternMatcher::patternPrefixMatches(const char *line, int suffixoffset) const
{
    if (!caseSensitive)
        throw opp_runtime_error("PatternMatcher: patternPrefixMatches() doesn't support case-insensitive match");

    // pattern must end in a literal string...
    assert(pattern[pattern.size()-1].type == END);
    if (pattern.size() < 2)
        return nullptr;
    const Elem& e = pattern[pattern.size()-2];
    if (e.type != LITERALSTRING)
        return nullptr;

    // ...with the suffixlen characters at the end of 'line'
    const char *pattstring = e.literalString.c_str();
    const char *p = strstr(pattstring, line+suffixoffset);
    if (!p)
        return nullptr;
    p += strlen(line+suffixoffset);
    rest = p;
    int pattsuffixlen = e.literalString.size() - (p-pattstring);

    // pattern, if we cut off the 'rest', must exactly match 'line'
    return doMatch(line, 0, pattsuffixlen) ? rest.c_str() : nullptr;
}

bool PatternMatcher::containsWildcards(const char *pattern)
{
    return strchr(pattern, '?') || strchr(pattern, '*') ||
           strchr(pattern, '\\') || strchr(pattern, '{') ||
           strstr(pattern, "..");
}

}  // namespace common
}  // namespace omnetpp

