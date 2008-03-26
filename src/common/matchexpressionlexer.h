//==========================================================================
//  MATCHEXPRESSIONLEXER.H - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __MATCHEXPRESSIONLEXER_H__
#define __MATCHEXPRESSIONLEXER_H__


NAMESPACE_BEGIN

class MatchExpressionLexer {
	private:
		const char *input;
		const char *ptr;
	public:
		MatchExpressionLexer(const char *input);
		int getNextToken(char **valuep);
};

NAMESPACE_END


#endif


