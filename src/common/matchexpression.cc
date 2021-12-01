//==========================================================================
//  MATCHEXPRESSION.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stack>
#include "matchexpression.h"
#include "patternmatcher.h"
#include "commonutil.h"
#include "exprnodes.h"
#include "exprvalue.h"

using namespace omnetpp::common::expression;

namespace omnetpp {
namespace common {


class ContextObjectMatchesConstPattern : public LeafNode {
protected:
    PatternMatcher matcher;
public:
    ContextObjectMatchesConstPattern(PatternMatcher matcher) : matcher(matcher) {}
    virtual ExprNode *dup() const override {return new ContextObjectMatchesConstPattern(matcher);}
    virtual std::string getName() const override {return "=~//";}
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
};

class ContextObjectFieldMatchesConstPattern : public LeafNode {
protected:
    std::string fieldToMatch;
    PatternMatcher matcher;
public:
    ContextObjectFieldMatchesConstPattern(PatternMatcher matcher, const char *fieldToMatch) : fieldToMatch(fieldToMatch), matcher(matcher) {}
    virtual ExprNode *dup() const override {return new ContextObjectFieldMatchesConstPattern(matcher, fieldToMatch.c_str());}
    virtual std::string getName() const override {return fieldToMatch + "=~//";};
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
};

void ContextObjectMatchesConstPattern::print(std::ostream& out, int spaciousness) const
{
    out << matcher.str();
}

ExprValue ContextObjectMatchesConstPattern::evaluate(Context *context) const
{
    MatchExpression::Matchable *matchableObject = dynamic_cast<MatchExpression::Matchable*>(context);
    Assert(matchableObject);
    const char *text = matchableObject->getAsString();
    return text == nullptr ? false : matcher.matches(text);
}

void ContextObjectFieldMatchesConstPattern::print(std::ostream& out, int spaciousness) const
{
    out << fieldToMatch << (needSpaces(spaciousness) ? " =~ " : "=~") << matcher.str();
}

ExprValue ContextObjectFieldMatchesConstPattern::evaluate(Context *context) const
{
    MatchExpression::Matchable *matchableObject = dynamic_cast<MatchExpression::Matchable*>(context);
    Assert(matchableObject);
    const char *text = matchableObject->getAsString(fieldToMatch.c_str());
    return text == nullptr ? false : matcher.matches(text);
}

MatchExpression::MatchExpression(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    setPattern(pattern, dottedpath, fullstring, casesensitive);
}

void MatchExpression::setPattern(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    if (opp_isblank(pattern)) {
        delete tree;
        tree = new ConstantNode(ExprValue(false));
    }
    else {
        std::vector<Elem> elems = parsePattern(pattern);
        ExprNode *t = generateEvaluator(elems, dottedpath, fullstring, casesensitive);
        delete tree;
        tree = t;
    }

    this->matchDottedPath = dottedpath;
    this->matchFullString = fullstring;
    this->caseSensitive = casesensitive;
}

ExprNode *MatchExpression::generateEvaluator(const std::vector<Elem>& elems, bool dottedpath, bool fullstring, bool casesensitive)
{
    Assert(!elems.empty());

    std::stack<ExprNode*> stack;

    for (auto & e : elems) {
        switch (e.type) {
            case Elem::PATTERN: {
                PatternMatcher matcher(e.pattern.c_str(), dottedpath, fullstring, casesensitive);
                if (e.fieldname.empty())
                    stack.push(new ContextObjectMatchesConstPattern(matcher));
                else
                    stack.push(new ContextObjectFieldMatchesConstPattern(matcher, e.fieldname.c_str()));
                break;
            }

            case Elem::AND:
            case Elem::OR: {
                Assert(stack.size() >= 2);
                ExprNode *arg2 = stack.top(); stack.pop();
                ExprNode *arg1 = stack.top(); stack.pop();
                BinaryNode *node = (e.type == Elem::AND) ? (BinaryNode *)new AndNode() : (BinaryNode *)new OrNode();
                node->setChildren(arg1, arg2);
                stack.push(node);
                break;
            }

            case Elem::NOT: {
                Assert(!stack.empty());
                ExprNode *arg = stack.top(); stack.pop();
                NotNode *node = new NotNode();
                node->setChild(arg);
                stack.push(node);
                break;
            }

            default:
                throw opp_runtime_error("MatchExpression: Malformed expression: Unknown element type");
        }
    }
    Assert(stack.size() == 1);
    return stack.top();
}

bool MatchExpression::matches(const Matchable *matchableObject)
{
    if (!tree)
        throw opp_runtime_error("MatchExpression: Empty match expression");
    if (!matchableObject)
        throw opp_runtime_error("MatchExpression: Input is nullptr");
    ExprValue value = tree->tryEvaluate(const_cast<Matchable*>(matchableObject));
    if (value.type != ExprValue::BOOL)
        throw opp_runtime_error("MatchExpression: expression did not evaluate to a boolean");
    return value.bl;
}

const char *MatchableStringMap::getAsString(const char *attribute) const
{
    std::map<std::string,std::string>::const_iterator it = stringMap.find(attribute);
    return it == stringMap.end() ? nullptr : it->second.c_str();
}

}  // namespace common
}  // namespace omnetpp

