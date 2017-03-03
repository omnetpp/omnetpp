//==========================================================================
//  MATCHEXPRESSION.H - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __OMNETPP_COMMON_MATCHEXPRESSION_H
#define __OMNETPP_COMMON_MATCHEXPRESSION_H

#include <string>
#include <vector>
#include <map>
#include "commondefs.h"
#include "exception.h"

namespace omnetpp {
namespace common {

class PatternMatcher;


/**
 * Matches various fields of an object. By default, a pattern must match
 * the "default field" of the object, which will usually be its name.
 * Other fields can be matched with the fieldname(pattern) syntax.
 * These elements can be combined with the AND, OR, NOT operators, accepted in
 * both lowercase and uppercase. AND has higher precedence than OR, but
 * parentheses can be used to change the evaluation order.
 *
 * Patterns are those accepted by PatternMatcher, that is, "*", "?",
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
class COMMON_API MatchExpression
{
  public:
    /**
     * Objects to be matched must implement this interface
     */
    class COMMON_API Matchable
    {
      public:
        /**
         * Return the default string to match. The returned pointer will not be
         * cached by the caller, so it is OK to return a pointer to a static buffer.
         */
        virtual const char *getAsString() const = 0;

        /**
         * Return the string value of the given attribute, or nullptr if the object
         * doesn't have an attribute with that name. The returned pointer will not
         * be cached by the caller, so it is OK to return a pointer to a static buffer.
         */
        virtual const char *getAsString(const char *attribute) const = 0;

        /**
         * Virtual destructor, for good manners...
         */
        virtual ~Matchable() {}
    };

    /**
     * One element in a (reverse Polish) expression
     */
    class COMMON_API Elem
    {
      public:
        friend class MatchExpression;
        enum Type {UNDEF, AND, OR, NOT, PATTERN, FIELDPATTERN};
      private:
        Type type;
        std::string fieldname;
        PatternMatcher *pattern;
      public:
        /** Ctor for AND, OR, NOT */
        Elem(Type type=UNDEF)  {this->type = type;}

        /** The given field of the object must match pattern */
        Elem(PatternMatcher *pattern, const char *fieldname=nullptr);

        /** Copy ctor */
        Elem(const Elem& other)  {type=UNDEF; operator=(other);}

        /** Dtor */
        ~Elem();

        /** Assignment */
        void operator=(const Elem& other);
    };

  protected:
    // options
    bool matchDottedPath;
    bool matchFullString;
    bool caseSensitive;

    // stores the expression
    std::vector<Elem> elems;

  protected:
    // internal: access to the parser
    static void parsePattern(std::vector<MatchExpression::Elem>& elems, const char *pattern,
                             bool dottedpath, bool fullstring, bool casesensitive);

  public:
    /**
     * Constructor
     */
    MatchExpression();

    /**
     * Constructor, accepts the same args as setPattern().
     */
    MatchExpression(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive);

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
 * Wrapper to make a string matchable with MatchExpression.
 */
class COMMON_API MatchableString : public MatchExpression::Matchable
{
  private:
    std::string str;
  public:
    MatchableString(const char *s) {str = s;}
    virtual const char *getAsString() const override {return str.c_str();}
    virtual const char *getAsString(const char *attribute) const override {return nullptr;}
};

/**
 * Wrapper to make a string map matchable with MatchExpression.
 */
class COMMON_API MatchableStringMap : public MatchExpression::Matchable
{
  private:
    std::string defaultValue;
    std::map<std::string,std::string> stringMap;
  public:
    MatchableStringMap(const std::string& defaultValue, const std::map<std::string,std::string>& map) : defaultValue(defaultValue), stringMap(map) {}
    virtual const char *getAsString() const override {return defaultValue.c_str();}
    virtual const char *getAsString(const char* attribute) const override;
};

} // namespace common
}  // namespace omnetpp


#endif


