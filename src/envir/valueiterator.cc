//==========================================================================
//  VALUEITERATOR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cmath>
#include <clocale>
#include <algorithm>
#include <set>
#include "common/opp_ctype.h"
#include "common/stringutil.h"
#include "common/commonutil.h"
#include "common/stringtokenizer.h"
#include "common/exprnodes.h"
#include "common/unitconversion.h"
#include "omnetpp/cexception.h"
#include "valueiterator.h"

using namespace std;
using namespace omnetpp::common;
using namespace omnetpp::common::expression;

namespace omnetpp {
namespace envir {

void ValueIterator::Expr::checkType(ExprValue::Type expected) const
{
    if (value.getType() != expected) {
        if (value.getType() != ExprValue::UNDEF)
            throw opp_runtime_error("%s expected, but %s (%s) found in the expression '%s'",
                    ExprValue::getTypeName(expected), ExprValue::getTypeName(value.getType()),
                    const_cast<ExprValue *>(&value)->str().c_str(),
                    raw.c_str());
        else
            throw opp_runtime_error("%s expected, but nothing found in the expression '%s'",
                    ExprValue::getTypeName(expected), raw.c_str());
    }
}

inline bool isVariableNameChar(char c)
{
    return isalnum(c) || c == '_' || c == '@';  // Note: Expression parser also allows '$' inside names.
                                                //       Not allowed here, because '$x$y' would refer to a variable named 'x$y'
}

void ValueIterator::Expr::collectVariablesInto(set<string>& result) const
{
    string::size_type start = 0, dollarPos;
    while (start < raw.size() && (dollarPos = raw.find('$', start)) != string::npos) {
        start = (dollarPos+1 < raw.size() && raw[dollarPos+1] == '{') ? dollarPos+2 : dollarPos+1;  // support both "$x" and "${x}"
        size_t end = start;
        while (end < raw.size() && isVariableNameChar(raw[end]))
            ++end;
        if (end > start && (start != dollarPos+2 || (end+1 < raw.size() && raw[end+1] == '}')))
            result.insert(raw.substr(start, end-start));
        start = end;
    }
}

namespace {
struct Resolver : public opp_substitutevariables_resolver
{
    const ValueIterator::VariableMap& map;
    Resolver(const ValueIterator::VariableMap& map) : map(map) {}
    virtual bool isVariableNameChar(char c) override { return omnetpp::envir::isVariableNameChar(c); }

    virtual std::string operator()(const std::string& name) override {
        ValueIterator::VariableMap::const_iterator it = map.find(name);
        if (it == map.end())
            throw opp_runtime_error("Unknown iteration variable: $%s", name.c_str());
        return it->second->get();
    }
};
}  // namespace

void ValueIterator::Expr::substituteVariables(const VariableMap& map)
{
    Resolver resolver(map);
    value = opp_substitutevariables(raw, resolver);
}

/**
 * Translate '=' as synonym of '=='. Used for compatibility with OMNeT++ 4.x and 5.x
 */
class EqualSignToEqualsAstTranslator : public Expression::BasicAstTranslator
{
  public:
    virtual ExprNode *createOperatorNode(const char *opName, int argCount) override {
        return (strcmp(opName,"=")==0 && argCount==2) ? ExprNodeFactory::createBinaryOperator("==") : nullptr;
    }
};

void ValueIterator::Expr::evaluate()
{
    Assert(value.getType() == ExprValue::Type::STRING);

    Expression expr;
    EqualSignToEqualsAstTranslator equalsSignTranslator; // accept '=' as 'equals', too
    Expression::MultiAstTranslator translator({ &equalsSignTranslator, Expression::getDefaultAstTranslator() });

    try {
        expr.parse(value.stringValue(), &translator);
    }
    catch (std::exception& e) {
        throw opp_runtime_error("Parse error in expression '%s': %s", (const char *)value, e.what());
    }
    try {
        value = expr.evaluate();
    }
    catch (std::exception& e) {
        throw opp_runtime_error("Cannot evaluate expression '%s': %s", (const char *)value, e.what());
    }
}

void ValueIterator::Item::parse(const char *s)
{
    // parse the <from>[..<to> [step <step>]] syntax
    const char *fromPtr = s;
    const char *toPtr = strstr(fromPtr, "..");
    const char *stepPtr = toPtr ? strstr(toPtr+2, "step") : nullptr;
    while (stepPtr && (opp_isalnumext(*(stepPtr-1)) || opp_isalnumext(*(stepPtr+4))))
        stepPtr = strstr(stepPtr+4, "step");

    std::string fromStr = toPtr ? std::string(fromPtr, toPtr - fromPtr) : std::string(fromPtr);
    std::string toStr = toPtr ? (stepPtr ? std::string(toPtr+2, stepPtr - toPtr - 2) : std::string(toPtr+2)) : "";
    std::string stepStr = stepPtr ? std::string(stepPtr+4) : "1";

    if (!toPtr) {
        type = TEXT;
        text = s;
    }
    else {
        type = FROM_TO_STEP;
        from = fromStr.c_str();
        to = toStr.c_str();
        step = stepStr.c_str();
    }
}

void ValueIterator::Item::collectVariablesInto(std::set<std::string>& result) const
{
    switch (type) {
        case TEXT:
            text.collectVariablesInto(result);
            break;

        case FROM_TO_STEP:
            from.collectVariablesInto(result);
            to.collectVariablesInto(result);
            step.collectVariablesInto(result);
            break;
    }
}

void ValueIterator::Item::restart(const VariableMap& map)
{
    switch (type) {
        case TEXT:
            text.substituteVariables(map);
            // note: no evaluate()! only from-to-step are evaluated, evaluation of other
            // iteration items are left to NED parameter evaluation
            break;

        case FROM_TO_STEP:
            from.substituteVariables(map);
            from.evaluate();
            to.substituteVariables(map);
            to.evaluate();
            step.substituteVariables(map);
            step.evaluate();
            break;
    }
}

int ValueIterator::Item::getNumValues() const
{
    if (type == FROM_TO_STEP) {
        double s = step.dblValue();
        // note 1.000001 below: without it, "1..9 step 0.1" would only go up to 8.9,
        // because floor(8/0.1) = floor(79.9999999999) = 79 not 80!
        return std::max(0, (int)floor((to.dblValue() - from.dblValue() + 1.000001 * s) / s));
    }
    else {
        return 1;
    }
}

std::string ValueIterator::Item::getValueAsString(int k) const
{
    char buf[32];
    switch (type) {
        case TEXT:
            return text.strValue();

        case FROM_TO_STEP:
            sprintf(buf, "%g", from.dblValue() + step.dblValue()*k);
            return buf;
    }
    Assert(false);
    return "";
}

ValueIterator::ValueIterator(const char *s)
{
    Assert(s);
    parse(s);
}

void ValueIterator::parse(const char *s)
{
    items.clear();
    referredVariables.clear();
    StringTokenizer tokenizer(s, ",", StringTokenizer::HONOR_QUOTES | StringTokenizer::HONOR_PARENS);
    while (tokenizer.hasMoreTokens()) {
        const char *token = tokenizer.nextToken();
        Item item;
        item.parse(token);
        item.collectVariablesInto(referredVariables);
        items.push_back(item);
    }
}

int ValueIterator::length() const
{
    int n = 0;
    for (const auto & item : items)
        n += item.getNumValues();
    return n;
}

std::string ValueIterator::get(int index) const
{
    if (index < 0 || index >= length())
        throw cRuntimeError("ValueIterator: Index %d out of bounds", index);

    int k = 0;
    for (const auto & item : items) {
        int n = item.getNumValues();
        if (k <= index && index < k+n)
            return item.getValueAsString(index-k);
        k += n;
    }
    Assert(false);
    return "";
}

void ValueIterator::restart(const VariableMap& vars)
{
    // substitute variables here
    for (auto & item : items)
        item.restart(vars);

    pos = itemIndex = k = 0;
    while (itemIndex < (int)items.size() && items[itemIndex].getNumValues() == 0)
        itemIndex++;
}

void ValueIterator::operator++(int)
{
    if (itemIndex >= (int)items.size())
        return;
    pos++;
    const Item& item = items[itemIndex];
    if (k < item.getNumValues()-1) {
        k++;
    }
    else {
        k = 0;
        while (++itemIndex < (int)items.size() && items[itemIndex].getNumValues() == 0)
            ;
    }
}

bool ValueIterator::gotoPosition(int pos, const VariableMap& vars)
{
    restart(vars);
    for ( ; pos > 0 && !end(); --pos)
        (*this)++;
    return !end();
}

std::string ValueIterator::get() const
{
    if (itemIndex >= (int)items.size())
        return "";
    return items[itemIndex].getValueAsString(k);
}

bool ValueIterator::end() const
{
    return itemIndex >= (int)items.size();
}

void ValueIterator::dump() const
{
    printf("parsed form: ");
    for (int i = 0; i < (int)items.size(); i++) {
        const Item& item = items[i];
        if (i > 0)
            printf(", ");
        if (item.type == Item::TEXT)
            printf("\"%s\"", item.text.strValue().c_str());
        else
            printf("range(%g..%g step %g)", item.from.dblValue(), item.to.dblValue(), item.step.dblValue());
    }
    printf("; enumeration: ");
    int n = length();
    for (int i = 0; i < n; i++) {
        if (i > 0)
            printf(", ");
        printf("%s", get(i).c_str());
    }
    printf(".\n");
}

}  // namespace envir
}  // namespace omnetpp

