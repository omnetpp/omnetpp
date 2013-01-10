//==========================================================================
//  CMATCHEXPRESSION.H - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __CMATCHEXPRESSION_H__
#define __CMATCHEXPRESSION_H__

#include <string>
#include "simkerneldefs.h"

NAMESPACE_BEGIN

class MatchExpression;


/**
 * Matches various fields of an object. By default, a pattern must match
 * the "default field" of the object, which will usually be its name.
 * Other fields can be matched with the fieldname(pattern) syntax.
 * These elements can be combined with the AND, OR, NOT operators, accepted in
 * both lowercase and uppercase. AND has higher precedence than OR, but
 * parentheses can be used to change the evaluation order.
 *
 * Patterns are those accepted by cPatternMatcher, that is, "*", "?",
 * character ranges as "{a-z}", numeric ranges as "{0..999}", or bracketed
 * numeric ranges as "[0..999]" (e.g. "*[90..100] matching "foo[95]")
 * are accepted.
 *
 * Pattern examples:
 *  - "" matches nothing
 *  - "node*"
 *  - "node* or host*"
 *  - "node* or host* or className(StandardHost*)"
 *  - "packet-* and className(PPPFrame)"
 *  - "className(TCPSegment) and not kind({0..2}) and SYN or data-*"
 *  - "className(TCPSegment) or byteLength({4096..})
 */
class SIM_API cMatchExpression
{
  public:
    /**
     * Objects to be matched must implement this interface
     */
    class SIM_API Matchable
    {
      public:
        /**
         * Return the default string to match. The returned pointer will not be
         * cached by the caller, so it is OK to return a pointer to a static buffer.
         */
        virtual const char *getAsString() const = 0;

        /**
         * Return the string value of the given attribute, or NULL if the object
         * doesn't have an attribute with that name. The returned pointer will not
         * be cached by the caller, so it is OK to return a pointer to a static buffer.
         */
        virtual const char *getAsString(const char *attribute) const = 0;

        /**
         * Virtual destructor, for good manners...
         */
        virtual ~Matchable() {}
    };

  private:
    MatchExpression *impl;

  public:
    /**
     * Constructor
     */
    cMatchExpression();

    /**
     * Constructor, accepts the same args as setPattern().
     */
    cMatchExpression(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive);

    /**
     * Destructor
     */
    ~cMatchExpression();

    /**
     * Sets the pattern to be used by subsequent calls to matches(). See the
     * general class description for the meaning of the rest of the arguments.
     * Throws cException if the pattern is bogus.
     */
    void setPattern(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive);

    /**
     * Returns true if the line matches the pattern with the given settings.
     * See setPattern().
     */
    bool matches(const Matchable *object);
};


/**
 * Wrapper to make a string matchable with cMatchExpression.
 */
class SIM_API cMatchableString : public cMatchExpression::Matchable
{
  private:
    std::string str;
  public:
    cMatchableString(const char *s) {str = s;}
    virtual const char *getAsString() const {return str.c_str();}
    virtual const char *getAsString(const char *attribute) const {return NULL;}
};

NAMESPACE_END


#endif


