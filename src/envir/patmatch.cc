//==========================================================================
//  PATMATCH.CC - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//  Implementation of
//         pattern matching stuff
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <assert.h>
#include <string.h>
#include "patmatch.h"
#include "cexception.h"

#ifdef _MSC_VER
#define strncasecmp  strnicmp
#endif


cPatternMatcher::cPatternMatcher()
{
}

cPatternMatcher::~cPatternMatcher()
{
}

void cPatternMatcher::setPattern(const char *patt, bool dottedpath, bool fullstring, bool casesensitive)
{
    pattern.clear();
    iscasesensitive = casesensitive;

    // "tokenize" pattern
    const char *s = patt;
    while (*s!='\0')
    {
        Elem e;
        switch(*s)
        {
           case '?':  e.type = dottedpath ? COMMONCHAR : ANYCHAR; s++; break;
           case '[':  if (pattern.empty() || pattern.back().type!=LITERALSTRING || !parseNumRange(s, ']', e.fromnum, e.tonum))
                          parseLiteralString(s,e);
                      else
                          e.type = NUMRANGE;
                      break;
           case '{':  if (parseNumRange(s, '}', e.fromnum, e.tonum))
                          {e.type = NUMRANGE; s++;}
                      else
                          parseSet(s,e);
                      break;
           case '*':  if (*(s+1)=='*')
                          {e.type = ANYSEQ; s+=2;}
                      else
                          {e.type = dottedpath ? COMMONSEQ : ANYSEQ; s++;}
                      break;
           default:   parseLiteralString(s,e); break;
        }
        pattern.push_back(e);
    }

    if (!fullstring)
    {
        // for substring match, we add "**" at both ends of the pattern (unless already there)
        Elem e;
        e.type = ANYSEQ;
        if (pattern.empty() || pattern.back().type!=ANYSEQ)
            pattern.push_back(e);
        if (pattern.front().type!=ANYSEQ)
            pattern.insert(pattern.begin(),e);
    }
    Elem e;
    e.type = END;
    pattern.push_back(e);
}

void cPatternMatcher::parseSet(const char *&s, Elem& e)
{
    s++; // skip "{"
    e.type = SET;
    if (*s=='^')
    {
        e.type = NEGSET;
        s++;
    }
    // Note: to make "}" part of the set, it must be first within the braces
    const char *sbeg = s;
    while (*s && (*s!='}' || s==sbeg))
    {
        char range[3];
        range[2] = 0;
        if (*(s+1)=='-' && *(s+2) && *(s+2)!='}')
        {
            // store "A-Z" as "AZ"
            range[0] = *s;
            range[1] = *(s+2);
            s+=3;
        }
        else
        {
            // store "X" as "XX"
            range[0] = range[1] = *s;
            s++;
        }
        if (!iscasesensitive)
        {
            // if one end of range is alpha and the other is not, funny things will happen
            range[0] = toupper(range[0]);
            range[1] = toupper(range[1]);
        }
        e.setchars += range;
    }
    if (!*s)
        throw new cException("unmatched '}' in expression");
    s++; // skip "}"
}

void cPatternMatcher::parseLiteralString(const char *&s, Elem& e)
{
    e.type = LITERALSTRING;
    while (*s && *s!='?' && *s!='{' && *s!='*')
    {
        long dummy;
        const char *s1;
        if (*s=='\\')
            e.literalstring += *(++s);
        else
            e.literalstring += *s;
        if (*s=='[' && parseNumRange((s1=s),']',dummy,dummy))
            break;
        s++;
    }
}

bool cPatternMatcher::parseNumRange(const char *&str, char closingchar, long& lo, long& up)
{
    //
    // try to parse "[n..m]" or "{n..m}" and return true on success.
    // str should point at "[" or "{"; on success return it'll point to "]" or "}",
    // and on failure it'll be unchanged. n and m will be stored in lo and up.
    // They are optional -- if missing, lo or up will be set to -1.
    //
    lo = up = -1L;
    const char *s = str+1; // skip "[" or "{"
    if (isdigit(*s))
    {
        lo = atol(s);
        while (isdigit(*s)) s++;
    }
    if (*s!='.' || *(s+1)!='.')
        return false;
    s+=2;
    if (isdigit(*s))
    {
        up = atol(s);
        while (isdigit(*s)) s++;
    }
    if (*s!=closingchar)
        return false;
    str = s;
    return true;
}

void cPatternMatcher::dump(int from)
{
    for (int k=from; k<(int)pattern.size(); k++)
    {
        Elem& e = pattern[k];
        switch (e.type)
        {
            case LITERALSTRING: printf("\"%s\"", e.literalstring.c_str()); break;
            case ANYCHAR: printf("?!"); break;
            case COMMONCHAR: printf("?"); break;
            case SET: printf("SET(%s)", e.setchars.c_str()); break;
            case NEGSET: printf("NEGSET(%s)", e.setchars.c_str()); break;
            case NUMRANGE: printf("%ld..%ld", e.fromnum, e.tonum); break;
            case ANYSEQ: printf("**"); break;
            case COMMONSEQ: printf("*"); break;
            case END: break;
            default: assert(0);
        }
        printf(" ");
    }
}

bool cPatternMatcher::isInSet(char c, const char *set)
{
    assert((strlen(set)&1)==0);
    if (!iscasesensitive)
        c = toupper(c); // set is already uppercase here
    while (*set)
    {
        if (c>=*set && c<=*(set+1))
            return true;
        set+=2;
    }
    return false;
}

bool cPatternMatcher::match(const char *s, int k)
{
    while (true)
    {
        Elem& e = pattern[k];
        long num;
        switch (e.type)
        {
            case LITERALSTRING:
                if (iscasesensitive ?
                    strncmp(s, e.literalstring.c_str(), e.literalstring.length()) :
                    strncasecmp(s, e.literalstring.c_str(), e.literalstring.length())
                   )
                    return false;
                s += e.literalstring.length();
                break;
            case ANYCHAR:
                if (!*s)
                    return false;
                s++;
                break;
            case COMMONCHAR:
                if (!*s || *s=='.')
                    return false;
                s++;
                break;
            case SET:
                if (!*s)
                    return false;
                if (!isInSet(*s, e.setchars.c_str()))
                    return false;
                s++;
                break;
            case NEGSET:
                if (!*s)
                    return false;
                if (isInSet(*s, e.setchars.c_str()))
                    return false;
                s++;
                break;
            case NUMRANGE:
                if (!isdigit(*s))
                    return false;
                num = atol(s);
                while (isdigit(*s)) s++;
                if ((e.fromnum>=0 && num<e.fromnum) || (e.tonum>=0 && num>e.tonum))
                    return false;
                break;
            case ANYSEQ:
                // FIXME shortcut if pattern ends in ANYSEQ or ANYSEQ LITERALSTRING
                do {
                    if (match(s,k+1))
                        return true;
                    s++;
                } while (*s);
                break; // at EOS
            case COMMONSEQ:
                do {
                    if (match(s,k+1))
                        return true;
                    if (!*s || *s=='.')
                        break;
                    s++;
                } while (*s);
                break;
            case END:
                return !*s;
            default:
                assert(0);
        }
        k++;
        assert(k<(int)pattern.size());
    }
}

bool cPatternMatcher::matches(const char *s)
{
    // FIXME todo shortcut: omnetpp.ini keys often begin with "*" or "**"
    // but end in a string literal. So it's usually a performance win to
    // to first check that the last string literal of the pattern matches 
    // the end of the string (unless doing substring search of course)
    
    return match(s,0);
}

