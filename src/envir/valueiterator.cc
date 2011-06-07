//==========================================================================
//  VALUEITERATOR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <locale.h>
#include "opp_ctype.h"
#include "valueiterator.h"
#include "stringutil.h"
#include "commonutil.h"
#include "stringtokenizer2.h"
#include "cexception.h"
#include "unitconversion.h"

USING_NAMESPACE

using namespace std;

inline const char* getTypeName(char type)
{
    switch(type)
    {
    case 'B': return "boolean";
    case 'D': return "number";
    case 'S': return "string";
    default: return "<undefined>";
    }
}

void ValueIterator::Expr::checkType(char expected) const
{
    if (currentValue.type != expected)
    {
        if (currentValue.type)
            throw opp_runtime_error("%s expected, but %s (%s) found in the expression '%s'",
                    getTypeName(expected), getTypeName(currentValue.type),
                    const_cast<Expression::Value*>(&currentValue)->str().c_str(),
                    text.c_str());
        else
            throw opp_runtime_error("%s expected, but nothing found in the expression '%s'",
                    getTypeName(expected), text.c_str());
    }
}

inline bool isVariableNameChar(char c)
{
    return isalnum(c) || c == '_' || c == '@'; // Note: Expression parser also allows '$' inside names.
                                               //       Not allowed here, because '$x$y' would refer to a variable named 'x$y'
}

void ValueIterator::Expr::collectVariablesInto(set<string> &result) const
{
    string::size_type start = 0, dollarPos;
    while (start < text.size() && (dollarPos = text.find('$', start)) != string::npos)
    {
        start = (dollarPos+1 < text.size() && text[dollarPos+1]=='{') ? dollarPos+2 : dollarPos+1; // support both "$x" and "${x}"
        size_t end = start;
        while (end < text.size() && isVariableNameChar(text[end]))
            ++end;
        if (end > start && (start!=dollarPos+2 || (end+1 < text.size() && text[end+1]=='}')))
            result.insert(text.substr(start, end-start));
        start = end;
    }
}

class FunctionResolver : public Expression::Resolver
{
  public:
    FunctionResolver() {}
    virtual ~FunctionResolver() {};
    // Variables are substituted textually before parsing the expression
    virtual Expression::Functor *resolveVariable(const char *varname) { Assert(false); return NULL; }
    virtual Expression::Functor *resolveFunction(const char *funcname, int argcount)
    {
        return MathFunction::supports(funcname) ? new MathFunction(funcname) : NULL;  // argcount will be checked by the caller afterwards
    }
};

void ValueIterator::Expr::substituteVariables(const VariableMap &map)
{
    string result;

    string::size_type dollarPos, varStart, varEnd, prevEnd=0;
    while (prevEnd < text.size() && (dollarPos = text.find('$', prevEnd)) != string::npos)
    {
        // copy segment before '$'
        result.append(text.substr(prevEnd, dollarPos-prevEnd));

        varStart = (dollarPos+1<text.size() && text[dollarPos+1]=='{')? dollarPos+2 : dollarPos+1;

        // find variable end
        for (varEnd = varStart; varEnd < text.size() && isVariableNameChar(text[varEnd]); ++varEnd)
            ;

        if (varEnd == varStart) // no varname after "$" or "${"
        {
            result.append(text.substr(dollarPos, varStart-dollarPos)); // copy "$" or "${"
        }
        else if (varStart == dollarPos+2 && (varEnd==text.size() || text[varEnd]!='}')) // started with "${", no closing "}"
        {
            result.append(text.substr(dollarPos, varEnd-dollarPos));
        }
        else
        {
            string name = text.substr(varStart, varEnd-varStart);
            if (varStart == dollarPos+2)
                varEnd++; // skip closing "}"
            VariableMap::const_iterator it = map.find(name);
            if (it == map.end())
            {
                throw opp_runtime_error("unknown iteration variable: $%s", name.c_str());
            }
            result.append(it->second->get());
        }

        // continue from end of the variable
        prevEnd = varEnd;
    }

    // append last segment
    if (prevEnd < text.size())
        result.append(text.substr(prevEnd));

    currentValue = result;
}

void ValueIterator::Expr::evaluate()
{
    Assert(currentValue.type == 'S');

    FunctionResolver resolver;
    Expression expr;
    try
    {
        expr.parse(currentValue.s.c_str(), &resolver);
    }
    catch (std::exception &e)
    {
        throw opp_runtime_error("Parse error in expression: %s", currentValue.s.c_str(), e.what());
    }

    try
    {
        currentValue = expr.evaluate();
    }
    catch (std::exception &e)
    {
        throw opp_runtime_error("Cannot evaluate expression: %s (%s)", currentValue.s.c_str(), e.what());
    }
}

void ValueIterator::Item::parse(const char *s)
{
    // parse the <from>[..<to> [step <step>]] syntax
    const char *fromPtr = s;
    const char *toPtr = strstr(fromPtr, "..");
    const char *stepPtr = toPtr ? strstr(toPtr+2, "step") : NULL;
    while (stepPtr && (opp_isalnum(*(stepPtr-1)) || opp_isalnum(*(stepPtr+4))))
        stepPtr = strstr(stepPtr+4, "step");

    std::string fromStr = toPtr ? std::string(fromPtr, toPtr - fromPtr) : std::string(fromPtr);
    std::string toStr = toPtr ? (stepPtr ? std::string(toPtr+2, stepPtr - toPtr - 2) : std::string(toPtr+2)) : "";
    std::string stepStr = stepPtr ? std::string(stepPtr+4) : "1";

    if (!toPtr)
    {
        type = TEXT;
        text = s;
    }
    else
    {
        type = FROM_TO_STEP;
        from = fromStr.c_str();
        to = toStr.c_str();
        step = stepStr.c_str();
    }
}

void ValueIterator::Item::collectVariablesInto(std::set<std::string> &result) const
{
    switch (type)
    {
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

void ValueIterator::Item::restart(const VariableMap &map)
{
    switch (type)
    {
    case TEXT:
        text.substituteVariables(map);
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
    if (type == FROM_TO_STEP)
    {
        double s = step.dblValue();
        // note 1.000001 below: without it, "1..9 step 0.1" would only go up to 8.9,
        // because floor(8/0.1) = floor(79.9999999999) = 79 not 80!
        return std::max(0, (int)floor((to.dblValue() - from.dblValue() + 1.000001 * s) / s));
    }
    else
    {
        return 1;
    }
}

std::string ValueIterator::Item::getValueAsString(int k) const
{
    char buf[32];
    switch (type)
    {
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
    while (tokenizer.hasMoreTokens())
    {
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
    for (int i=0; i<(int)items.size(); i++)
        n += items[i].getNumValues();
    return n;
}

std::string ValueIterator::get(int index) const
{
    if (index<0 || index>=length())
        throw cRuntimeError("ValueIterator: index %d out of bounds", index);

    int k = 0;
    for (int i=0; i<(int)items.size(); i++)
    {
        const Item& item = items[i];
        int n = item.getNumValues();
        if (k <= index && index < k+n)
            return item.getValueAsString(index-k);
        k+=n;
    }
    Assert(false);
    return "";
}

void ValueIterator::restart(const VariableMap &vars)
{
    // substitute variables here
    for (std::vector<Item>::iterator it = items.begin(); it != items.end(); ++it)
        it->restart(vars);

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
        while (++itemIndex < (int)items.size() && items[itemIndex].getNumValues() == 0);
    }
}

bool ValueIterator::gotoPosition(int pos, const VariableMap &vars)
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
    for (int i=0; i<(int)items.size(); i++)
    {
        const Item& item = items[i];
        if (i>0) printf(", ");
        if (item.type == Item::TEXT)
            printf("\"%s\"", item.text.strValue().c_str());
        else
            printf("range(%g..%g step %g)", item.from.dblValue(), item.to.dblValue(), item.step.dblValue());
    }
    printf("; enumeration: ");
    int n = length();
    for (int i=0; i<n; i++)
    {
        if (i>0) printf(", ");
        printf("%s", get(i).c_str());
    }
    printf(".\n");
}
