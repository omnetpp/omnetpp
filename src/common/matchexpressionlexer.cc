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
			if (len == 2 &&
			    (*start == 'o' && strncmp(start,"or",2) == 0 ||
			     *start == 'O' && strncmp(start,"OR",2) == 0))
				return OR_;
			if (len == 3 &&
			    (*start == 'a' && strncmp(start,"and",3) == 0 ||
			     *start == 'A' && strncmp(start,"AND",3) == 0))
				return AND_;
			if (len == 3 &&
			    (*start == 'n' && strncmp(start,"not",3) == 0 ||
			     *start == 'N' && strncmp(start,"NOT",3) == 0))
				return NOT_;
            std::string str(start, len);
			*valuep = opp_strdup(str.c_str());
			return STRINGLITERAL;
        }
		}
	}
}

