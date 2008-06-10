//==========================================================================
// MATCHEXPRESSIONLEXER.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "stringutil.h"
// include token values
#include "matchexpression.tab.h"
#include "matchexpressionlexer.h"

USING_NAMESPACE

MatchExpressionLexer::MatchExpressionLexer(const char *input)
    : input(input), ptr(input)
{
}

int MatchExpressionLexer::getNextToken(char **valuep) {
    const char *start;

    *valuep = NULL;
    while (true) {
        start = ptr;
        switch (*ptr++) {
        case '\0': return -1;
        case '(': return '(';
        case ')': return ')';
        case '"':
        {
            std::string str = opp_parsequotedstr(start,ptr);
            *valuep = opp_strdup(str.c_str());
            return STRINGLITERAL;
        }
        case ' ': case '\n': case '\t': continue;
        default:
        {
            for (;;++ptr) {
                switch (*ptr) {
                case '\0':
                case ' ': case '\t': case '\n': case '(': case ')':
                    goto END;
                }
            };
            END:
            int len = ptr - start;
            if (len == 2 && strncasecmp(start, "or", 2) == 0)
                return OR_;
            if (len == 3 && strncasecmp(start, "and", 3) == 0)
                return AND_;
            if (len == 3 && strncasecmp(start, "not", 3) == 0)
                return NOT_;
            std::string str(start, len);
            *valuep = opp_strdup(str.c_str());
            return STRINGLITERAL;
        }
        }
    }
}

