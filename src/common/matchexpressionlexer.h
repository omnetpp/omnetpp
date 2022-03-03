//==========================================================================
//  MATCHEXPRESSIONLEXER.H - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
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
 *
 * Token constants produced: STRINGLITERAL, MATCHES, AND_, OR_, NOT_, '(', ')'.
 *
 * MATCHES is "=~". AND_, OR_, NOT_ corresponds to keywords for logical
 * operations (and, or, not -- case insensitive!).
 *
 * Non-whitespace character sequences that do not begin with a quote and
 * are not other tokens (=~, and, or, not) are returned as STRINGLITERAL.
 * The following things terminate such an unquoted string literal:
 * whitespace, "=~", "(", ")". All other characters are taken as part of
 * the string.
 *
 * Quoted string literals begin with a double quote, then parsed with
 * opp_parsequotedstr() which understands the usual escape sequences
 * (\n,\t,\",\\,etc.). The parsed value is returned, i.e. without quotes.
 *
 * foo=~bar --> STRINGLITERAL(foo), MATCHES, STRINGLITERAL(bar)
 * fo(o --> STRINGLITERAL(fo), '(', STRINGLITERAL(o)
 * foo =~ "foo" --> STRINGLITERAL(foo), MATCHES, STRINGLITERAL(foo)
 */
class MatchExpressionLexer
{
    private:
        const char *ptr;
    public:
        MatchExpressionLexer(const char *input);
        int getNextToken(char **valuep);
};

}  // namespace common
}  // namespace omnetpp


#endif


