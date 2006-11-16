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
#include "exception.h"


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

void MatchExpression::setPattern(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    this->dottedpath = dottedpath;
    this->fullstring = fullstring;
    this->casesensitive = casesensitive;

    ::doParseMatchExpression(elems, pattern, dottedpath, fullstring, casesensitive);
}

bool MatchExpression::matches(const Matchable *object)
{
    const int stksize = 20;
    bool stk[stksize];

    int tos = -1;
    for (int i = 0; i < elems.size(); i++)
    {
        Elem& e = elems[i];
        switch (e.type)
        {
          case Elem::PATTERN:
            if (tos>=stksize-1)
                throw new Exception("MatchExpression overflow");
            stk[++tos] = e.pattern->matches(object->getDefaultAttribute());
            break;
          case Elem::FIELDPATTERN:
            if (tos>=stksize-1)
                throw new Exception("MatchExpression overflow");
            stk[++tos] = e.pattern->matches(object->getAttribute(e.fieldname.c_str()));
            break;
          case Elem::OR:
            if (tos<=2)
                throw new Exception("MatchExpression underflow");
            stk[tos-1] = stk[tos-1] || stk[tos];
            tos--;
            break;
          case Elem::AND:
            if (tos<=2)
                throw new Exception("MatchExpression underflow");
            stk[tos-1] = stk[tos-1] && stk[tos];
            tos--;
            break;
          default:
            throw new Exception("MatchExpression: unknown element type");
       }
    }
    if (tos!=0)
        throw new Exception("MatchExpression: malformed expression");
    return stk[tos];
}

