//==========================================================================
//  MATCHEXPRESSIONLEXER.H - part of
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


#ifndef __MATCHEXPRESSIONLEXER_H__
#define __MATCHEXPRESSIONLEXER_H__


NAMESPACE_BEGIN

/**
 * Handcoded lexer, because Flex-generated one was not reentrant.
 */
class MatchExpressionLexer
{
    private:
        const char *input;
        const char *ptr;
    public:
        MatchExpressionLexer(const char *input);
        int getNextToken(char **valuep);
};

NAMESPACE_END


#endif


