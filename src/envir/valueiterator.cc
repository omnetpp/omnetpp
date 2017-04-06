//==========================================================================
//  VALUEITERATOR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
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
#include "common/opp_ctype.h"
#include "common/stringutil.h"
#include "common/commonutil.h"
#include "common/stringtokenizer2.h"
#include "common/unitconversion.h"
#include "omnetpp/cexception.h"
#include "valueiterator.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

inline const char *getTypeName(char type)
{
    switch (type) {
        case 'B': return "boolean";
        case 'D': return "number";
        case 'S': return "string";
        default: return "<undefined>";
    }
}

void ValueIterator::Expr::checkType(char expected) const
{
    if (value.type != expected) {
        if (value.type)
            throw opp_runtime_error("%s expected, but %s (%s) found in the expression '%s'",
                    getTypeName(expected), getTypeName(value.type),
                    const_cast<Expression::Value *>(&value)->str().c_str(),
                    raw.c_str());
        else
            throw opp_runtime_error("%s expected, but nothing found in the expression '%s'",
                    getTypeName(expected), raw.c_str());
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

class FunctionResolver : public Expression::Resolver
{
  public:
    FunctionResolver() {}
    virtual ~FunctionResolver() {};
    // Variables are substituted textually before parsing the expression
    virtual Expression::Functor *resolveVariable(const char *varName) override { return nullptr; }
    virtual Expression::Functor *resolveFunction(const char *functionName, int argCount) override {
        return MathFunction::supports(functionName) ? new MathFunction(functionName) : nullptr;  // argcount will be checked by the caller afterwards
    }
};

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
} // namespace {

void ValueIterator::Expr::substituteVariables(const VariableMap& map)
{
    Resolver resolver(map);
    value = opp_substitutevariables(raw, resolver);
}

void ValueIterator::Expr::evaluate()
{
    Assert(value.type == 'S');

    FunctionResolver resolver;
    Expression expr;
    try {
        expr.parse(value.s.c_str(), &resolver);
    }
    catch (std::exception& e) {
        throw opp_runtime_error("Parse error in expression '%s': %s", value.s.c_str(), e.what());
    }
    try {
        value = expr.evaluate();
    }
    catch (std::exception& e) {
        throw opp_runtime_error("Cannot evaluate expression '%s': %s", value.s.c_str(), e.what());
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

ValueIterator::ValueIterator()
{
    pos = itemIndex = k = 0;
}

ValueIterator::ValueIterator(const char *s)
{
    Assert(s);
    pos = itemIndex = k = 0;
    parse(s);
}

ValueIterator::~ValueIterator()
{
}

void ValueIterator::parse(const char *s)
{
    items.clear();
    referredVariables.clear();
    StringTokenizer2 tokenizer(s, ",", "()[]{}", "\"");
    while (tokenizer.hasMoreTokens()) {
        Item item;
        std::string token = opp_trim(tokenizer.nextToken());
        item.parse(token.c_str());
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

