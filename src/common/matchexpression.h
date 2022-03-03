//==========================================================================
//  MATCHEXPRESSION.H - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
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
#include "exprnode.h"
#include "exception.h"

namespace omnetpp {
namespace common {

class PatternMatcher;

/**
 * Matches various fields of an object. By default, a pattern must match
 * the "default field" of the object, which will usually be its name.
 * Other fields can be matched with the <fieldname> =~ <pattern> syntax.
 * These elements can be combined with the AND, OR, NOT operators, accepted
 * in both lowercase and uppercase. AND has higher precedence than OR,
 * and parentheses can be used to change the evaluation order.
 *
 * Patterns are those accepted by PatternMatcher, that is, "*", "?",
 * character ranges as "{a-z}", numeric ranges as "{0..999}", or bracketed
 * numeric ranges as "[0..999]" (e.g. "*[90..100] matching "foo[95]")
 * are accepted.
 *
 * Field names and patterns that do not contain whitespace, a left or right
 * parenthesis or the "=~" sequence can be written without quotation marks.
 * When a field name or pattern is enclosed in quotation marks, it is
 * interpreted similar to a string constant in C/C++ regarding the use
 * of backslashes, i.e. "\t" becomes a tab, "\\" a single backslash, etc.
 *
 * Pattern examples:
 *  - "" matches nothing
 *  - "node*"
 *  - "node* or host*"
 *  - "node* or host* or className =~ StandardHost*"
 *  - "packet-* and className=~PPPFrame"
 *  - "className=~TCPSegment and not kind=~{0..2} and SYN or data-*"
 *  - "className=~TCPSegment or byteLength=~{4096..}"
 */
class COMMON_API MatchExpression
{
  public:
    /**
     * Objects to be matched must implement this interface
     */
    class COMMON_API Matchable : public expression::Context
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
     * Element in a (reverse Polish) expression, only used during parsing.
     */
    struct COMMON_API Elem
    {
        enum Type {UNDEF, AND, OR, NOT, PATTERN};
        Type type;
        std::string pattern;
        std::string fieldname;
        Elem(Type type=UNDEF) : type(type) {}
        Elem(const char *pattern, const char *fieldname="") : type(PATTERN), pattern(pattern), fieldname(fieldname) {}
    };

  protected:
    // options
    bool matchDottedPath = false;
    bool matchFullString = true;
    bool caseSensitive = true;

    // stores the expression
    expression::ExprNode *tree = nullptr;

  protected:
    virtual std::vector<Elem>  parsePattern(const char *pattern);
    virtual expression::ExprNode *generateEvaluator(const std::vector<Elem>& elems, bool dottedpath, bool fullstring, bool casesensitive);

  public:
    /**
     * Constructor
     */
    MatchExpression() {}

    /**
     * Constructor, accepts the same args as setPattern().
     */
    MatchExpression(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive);

    MatchExpression(const MatchExpression&) = delete;
    MatchExpression(MatchExpression&&) = delete;

    /**
     * Virtual destructor.
     */
    virtual ~MatchExpression() {delete tree;}

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

}  // namespace common
}  // namespace omnetpp


#endif


