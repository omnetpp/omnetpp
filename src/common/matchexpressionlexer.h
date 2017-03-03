//==========================================================================
//  MATCHEXPRESSIONLEXER.H - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __OMNETPP_COMMON_MATCHEXPRESSIONLEXER_H
#define __OMNETPP_COMMON_MATCHEXPRESSIONLEXER_H


namespace omnetpp {
namespace common {

/**
 * Handcoded lexer, because Flex-generated one was not reentrant.
 */
class MatchExpressionLexer
{
    private:
        const char *ptr;
    public:
        MatchExpressionLexer(const char *input);
        int getNextToken(char **valuep);
};

} // namespace common
}  // namespace omnetpp


#endif


