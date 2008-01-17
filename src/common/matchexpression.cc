//==========================================================================
// MATCHEXPRESSION.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "matchexpression.h"
#include "patternmatcher.h"
#include "commonutil.h"

USING_NAMESPACE


MatchExpression::Elem::Elem(PatternMatcher *pattern, const char *fieldname)
{
    type = fieldname==NULL ? PATTERN : FIELDPATTERN;
    this->fieldname = fieldname==NULL ? "" : fieldname;
    this->pattern = pattern;
}

MatchExpression::Elem::~Elem()
{
    if (type==PATTERN || type==FIELDPATTERN)
        delete pattern;
}

void MatchExpression::Elem::operator=(const Elem& other)
{
    type = other.type;
    if (type==PATTERN || type==FIELDPATTERN)
    {
        fieldname = other.fieldname;
        pattern = new PatternMatcher(*(other.pattern));
    }
}

MatchExpression::MatchExpression()
{
}

MatchExpression::MatchExpression(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    setPattern(pattern, dottedpath, fullstring, casesensitive);
}

void MatchExpression::setPattern(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    this->dottedpath = dottedpath;
    this->fullstring = fullstring;
    this->casesensitive = casesensitive;

    elems.clear();

    Assert(pattern);
    if (*pattern)
        parsePattern(elems, pattern, dottedpath, fullstring, casesensitive);
}

bool MatchExpression::matches(const Matchable *object)
{
    if (elems.empty())
        return false;

    const int stksize = 20;
    bool stk[stksize];

    int tos = -1;
    for (int i = 0; i < (int)elems.size(); i++)
    {
        Elem& e = elems[i];
        const char *attr;
        switch (e.type)
        {
          case Elem::PATTERN:
            if (tos>=stksize-1)
                throw opp_runtime_error("MatchExpression: malformed expression: stack overflow");
            attr = object->getDefaultAttribute();
            stk[++tos] = attr==NULL ? false : e.pattern->matches(attr);
            break;
          case Elem::FIELDPATTERN:
            if (tos>=stksize-1)
                throw opp_runtime_error("MatchExpression: malformed expression: stack overflow");
            attr = object->getAttribute(e.fieldname.c_str());
            stk[++tos] = attr==NULL ? false : e.pattern->matches(attr);
            break;
          case Elem::OR:
            if (tos<1)
                throw opp_runtime_error("MatchExpression: malformed expression: stack underflow");
            stk[tos-1] = stk[tos-1] || stk[tos];
            tos--;
            break;
          case Elem::AND:
            if (tos<1)
                throw opp_runtime_error("MatchExpression: malformed expression: stack underflow");
            stk[tos-1] = stk[tos-1] && stk[tos];
            tos--;
            break;
          case Elem::NOT:
            if (tos<0)
                throw opp_runtime_error("MatchExpression: malformed expression: stack underflow");
            stk[tos] = !stk[tos];
            break;
          default:
            throw opp_runtime_error("MatchExpression: malformed expression: unknown element type");
       }
    }
    if (tos!=0)
        throw opp_runtime_error("MatchExpression: malformed expression: %d items left on stack", tos);

    return stk[tos];
}

