//==========================================================================
//   EXPRNODES.CC  - part of
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

#include <cmath>
#include <cinttypes>  // PRId64
#include <sstream>
#include "unitconversion.h"
#include "stlutil.h"
#include "stringutil.h"
#include "pooledstring.h"
#include "exprnodes.h"
#include "expression.h"
#include "patternmatcher.h"

namespace omnetpp {
namespace common {
namespace expression {

LeafNode *ExprNodeFactory::createConstant(const ExprValue& value)
{
    return new ConstantNode(value);
}

UnaryNode *ExprNodeFactory::createUnaryOperator(const char *op_)
{
    std::string op = op_;
    if (op == "-") return new NegateNode();
    if (op == "!") return new NotNode();
    if (op == "~") return new BitwiseNotNode();
    if (op == "_!") throw opp_runtime_error("Not a recognized unary operator: postfix '!' (bang operator)");
    throw opp_runtime_error("Not a recognized unary operator: '%s'", op.c_str());
}

BinaryNode *ExprNodeFactory::createBinaryOperator(const char *op_)
{
    std::string op = op_;
    if (op == "+") return new AddNode();
    if (op == "-") return new SubNode();
    if (op == "*") return new MulNode();
    if (op == "/") return new DivNode();
    if (op == "%") return new ModNode();
    if (op == "^") return new PowNode();
    if (op == "==") return new EqualNode();
    if (op == "!=") return new NotEqualNode();
    if (op == "<") return new LessThanNode();
    if (op == ">") return new GreaterThanNode();
    if (op == "<=") return new LessOrEqualNode();
    if (op == ">=") return new GreaterOrEqualNode();
    if (op == "<=>") return new ThreeWayComparisonNode();
    if (op == "=~") return new MatchNode();
    if (op == "&&") return new AndNode();
    if (op == "||") return new OrNode();
    if (op == "##") return new XorNode();
    if (op == "&") return new BitwiseAndNode();
    if (op == "|") return new BitwiseOrNode();
    if (op == "#") return new BitwiseXorNode();
    if (op == "<<") return new LShiftNode();
    if (op == ">>") return new RShiftNode();
    throw opp_runtime_error("Not a recognized binary operator: '%s'", op.c_str());
}

TernaryNode *ExprNodeFactory::createTernaryOperator(const char *op_)
{
    std::string op = op_;
    if (op == "?:") return new InlineIfNode();
    throw opp_runtime_error("Not a recognized ternary operator: '%s'", op.c_str());
}

bool ExprNodeFactory::supportsStdMathFunction(const char *name)
{
    static std::vector<std::string> names = { "acos","asin","atan","atan2","sin","cos","tan","ceil","floor","exp","pow","sqrt","fabs","fmod","hypot","log","log10" };
    return contains(names, std::string(name));
}

ExprNode *ExprNodeFactory::createStdMathFunction(const char *name_)
{
    std::string name = name_;
    if (name == "acos") return createMathFunction("acos", acos);
    if (name == "asin") return createMathFunction("asin", asin);
    if (name == "atan") return createMathFunction("atan", atan);
    if (name == "atan2") return createMathFunction("atan2", atan2);
    if (name == "sin") return createMathFunction("sin", sin);
    if (name == "cos") return createMathFunction("cos", cos);
    if (name == "tan") return createMathFunction("tan", tan);
    if (name == "ceil") return createMathFunction("ceil", ceil);
    if (name == "floor") return createMathFunction("floor", floor);
    if (name == "exp") return createMathFunction("exp", exp);
    if (name == "pow") return createMathFunction("pow", pow);
    if (name == "sqrt") return createMathFunction("sqrt", sqrt);
    if (name == "fabs") return createMathFunction("fabs", fabs);
    if (name == "fmod") return createMathFunction("fmod", fmod);
    if (name == "hypot") return createMathFunction("hypot", hypot);
    if (name == "log") return createMathFunction("log", log);
    if (name == "log10") return createMathFunction("log10", log10);
    throw opp_runtime_error("Not a standard <cmath> function: '%s'", name_);
}

ExprNode *ExprNodeFactory::createMathFunction(const char *name, double (*f0)())
{
    return new MathFunc0Node(name, f0);
}

ExprNode *ExprNodeFactory::createMathFunction(const char *name, double (*f1)(double))
{
    return new MathFunc1Node(name, f1);
}

ExprNode *ExprNodeFactory::createMathFunction(const char *name, double (*f2)(double,double))
{
    return new MathFunc2Node(name, f2);
}

ExprNode *ExprNodeFactory::createMathFunction(const char *name, double (*f3)(double,double,double))
{
    return new MathFunc3Node(name, f3);
}

//---

void VariableNode::print(std::ostream& out, int spaciousness) const
{
    out << name;
}

ExprValue VariableNode::evaluate(Context *context) const
{
    return getValue(context);
}

//---

void ObjectNode::print(std::ostream& out, int spaciousness) const
{
    if (!typeName.empty())
        out << getName() << " ";
    out << "{ ";
    std::vector<ExprNode*> children = getChildren();
    Assert(children.size() == fieldNames.size());
    for (size_t i = 0; i < children.size(); i++) {
        if (i != 0)
            out << (spaciousness >= LASTPREC-ARITHM_LAST ? ", " : ",");
        out << fieldNames[i] << ": ";
        printChild(out, children[i], spaciousness);
    }
    out << " }";
}

//---

void ArrayNode::print(std::ostream& out, int spaciousness) const
{
    out << "[ ";
    std::vector<ExprNode*> children = getChildren();
    for (size_t i = 0; i < children.size(); i++) {
        if (i != 0)
            out << (spaciousness >= LASTPREC-ARITHM_LAST ? ", " : ",");
        printChild(out, children[i], spaciousness);
    }
    out << " ]";
}

//---

ExprValue IndexedVariableNode::evaluate(Context *context) const
{
    ExprValue indexValue = child->tryEvaluate(context);
    intval_t index = indexValue.intValue();
    if (indexValue.getUnit() != nullptr)
        throw opp_runtime_error("Index must be dimensionless");

    return getValue(context, index);
}

void IndexedVariableNode::print(std::ostream& out, int spaciousness) const
{
    out << name << "[";
    printChild(out, child, spaciousness);
    out<< "]";
}

ExprValue MemberNode::evaluate(Context *context) const
{
    ExprValue object = child->tryEvaluate(context);
    return getValue(context, object);
}

void MemberNode::print(std::ostream& out, int spaciousness) const
{
    printChild(out, child, spaciousness);
    out << "." << name;
}

ExprValue IndexedMemberNode::evaluate(Context *context) const
{
    ExprValue object = child1->tryEvaluate(context);

    ExprValue indexValue = child2->tryEvaluate(context);
    intval_t index = indexValue.intValue();
    if (indexValue.getUnit() != nullptr)
        throw opp_runtime_error("Index must be dimensionless");

    return getValue(context, object, index);
}

void IndexedMemberNode::print(std::ostream& out, int spaciousness) const
{
    printChild(out, child1, spaciousness);
    out << "." << name;
    out << name << "[";
    printChild(out, child2, spaciousness);
    out<< "]";
}

//---

ExprValue NegateNode::evaluate(Context *context) const
{
    ExprValue value = child->tryEvaluate(context);
    if (value.type == ExprValue::INT) {
        ensureNoLogarithmicUnit(value);
        value.intv = -value.intv;
    }
    else if (value.type == ExprValue::DOUBLE) {
        ensureNoLogarithmicUnit(value);
        value.dbl = -value.dbl;
    }
    else if (value.type == ExprValue::UNDEF)
        return value;
    else
        errorNumericArgExpected(value);
    return value;
}

std::string UnaryOperatorNode::str() const
{
    return "operator '" + getName() + "'";
}

void UnaryOperatorNode::print(std::ostream& out, int spaciousness) const
{
    out << getName();
    if (needSpaces(spaciousness))
        out << " ";
    printChild(out, child, spaciousness);
}

std::string BinaryOperatorNode::str() const
{
    return "operator '" + getName() + "'";
}

std::string TernaryOperatorNode::str() const
{
    return "operator '" + getName() + "'";
}

void BinaryOperatorNode::print(std::ostream& out, int spaciousness) const
{
    printChild(out, child1, spaciousness);
    if (needSpaces(spaciousness))
        out << " " << getName() << " ";
    else
        out << getName();
    printChild(out, child2, spaciousness);
}

inline char *concatenate(const char *s1, const char *s2)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *res = new char[len1+len2+1];
    strcpy(res, s1);
    strcpy(res+len1, s2);
    return res;
}

ExprValue AddNode::evaluate(Context *context) const
{
    ExprValue leftValue = child1->tryEvaluate(context);
    ExprValue rightValue = child2->tryEvaluate(context);
    if (leftValue.type == ExprValue::UNDEF || rightValue.type == ExprValue::UNDEF)
        return ExprValue();

    // numeric addition or string concatenation
    if (leftValue.type == ExprValue::INT && rightValue.type == ExprValue::INT) {  // both ints -> integer addition
        ensureNoLogarithmicUnit(rightValue);
        ensureNoLogarithmicUnit(leftValue);
        bringToCommonTypeAndUnit(rightValue, leftValue);
        leftValue.intv = safeAdd(leftValue.intv, rightValue.intv);
        return leftValue;
    }
    else if (leftValue.type == ExprValue::DOUBLE || rightValue.type == ExprValue::DOUBLE) { // at least one is double -> double addition
        ensureNoLogarithmicUnit(rightValue);
        ensureNoLogarithmicUnit(leftValue);
        bringToCommonTypeAndUnit(rightValue, leftValue);
        leftValue.dbl = leftValue.dbl + rightValue.dbl;
        return leftValue;
    }
    else if (leftValue.type == ExprValue::STRING && rightValue.type == ExprValue::STRING) {
        char *res = concatenate(leftValue.s, rightValue.s);
        leftValue.deleteOld();
        leftValue.s = res;
        return leftValue;
    }
    else
        errorNumericArgsExpected(leftValue, rightValue);
}

ExprValue SubNode::evaluate(Context *context) const
{
    ExprValue leftValue = child1->tryEvaluate(context);
    ExprValue rightValue = child2->tryEvaluate(context);
    if (leftValue.type == ExprValue::UNDEF || rightValue.type == ExprValue::UNDEF)
        return ExprValue();

    // numeric addition or string concatenation
    if (leftValue.type == ExprValue::INT && rightValue.type == ExprValue::INT) {  // both ints -> integer subtraction
        ensureNoLogarithmicUnit(rightValue);
        ensureNoLogarithmicUnit(leftValue);
        bringToCommonTypeAndUnit(rightValue, leftValue);
        leftValue.intv = safeSub(leftValue.intv, rightValue.intv);
        return leftValue;
    }
    else if (leftValue.type == ExprValue::DOUBLE || rightValue.type == ExprValue::DOUBLE) { // at least one is double -> double addition
        ensureNoLogarithmicUnit(rightValue);
        ensureNoLogarithmicUnit(leftValue);
        bringToCommonTypeAndUnit(rightValue, leftValue);
        leftValue.dbl = leftValue.dbl - rightValue.dbl;
        return leftValue;
    }
    else
        errorNumericArgsExpected(leftValue, rightValue);
}

ExprValue MulNode::evaluate(Context *context) const
{
    ExprValue leftValue = child1->tryEvaluate(context);
    ExprValue rightValue = child2->tryEvaluate(context);
    if (leftValue.type == ExprValue::UNDEF || rightValue.type == ExprValue::UNDEF)
        return ExprValue();

    if (leftValue.type == ExprValue::INT && rightValue.type == ExprValue::INT) {  // both are integers -> integer multiplication
        if (!rightValue.unit.empty() && !leftValue.unit.empty())
            throw opp_runtime_error("Multiplying two quantities with units is not supported");
        ensureNoLogarithmicUnit(rightValue);
        ensureNoLogarithmicUnit(leftValue);
        leftValue.intv = safeMul(leftValue.intv, rightValue.intv);
        if (leftValue.unit.empty())
            leftValue.unit = rightValue.unit;
        return leftValue;
    }
    else if (leftValue.type == ExprValue::DOUBLE || rightValue.type == ExprValue::DOUBLE) { // at least one is double -> double multiplication
        if (!rightValue.unit.empty() && !leftValue.unit.empty())
            throw opp_runtime_error("Multiplying two quantities with units is not supported");
        ensureNoLogarithmicUnit(rightValue);
        ensureNoLogarithmicUnit(leftValue);
        leftValue.convertToDouble();
        rightValue.convertToDouble();
        leftValue.dbl = leftValue.dbl * rightValue.dbl;
        if (leftValue.unit.empty())
            leftValue.unit = rightValue.unit;
        return leftValue;
    }
    else
        errorNumericArgsExpected(leftValue, rightValue);
}

ExprValue DivNode::evaluate(Context *context) const
{
    ExprValue leftValue = child1->tryEvaluate(context);
    ExprValue rightValue = child2->tryEvaluate(context);
    if (leftValue.type == ExprValue::UNDEF || rightValue.type == ExprValue::UNDEF)
        return ExprValue();

    // even if both args are integer, we perform the division in double, to reduce surprises;
    // for now we only support num/num, unit/num, plus and unit/unit only if the two units are convertible
    leftValue.convertToDouble();
    rightValue.convertToDouble();
    ensureNoLogarithmicUnit(rightValue);
    if (rightValue.dbl != 0)  // allow "0dB/0" as nan for compatibility with INET 3.x
        ensureNoLogarithmicUnit(leftValue);
    if (!rightValue.unit.empty())
        rightValue.dbl = UnitConversion::convertUnit(rightValue.dbl, rightValue.unit.c_str(), leftValue.unit.c_str());
    leftValue.dbl = leftValue.dbl / rightValue.dbl;
    if (!rightValue.unit.empty())
        leftValue.unit = nullptr;
    return leftValue;
}

ExprValue ModNode::evaluate(Context *context) const
{
    ExprValue leftValue = child1->tryEvaluate(context);
    ExprValue rightValue = child2->tryEvaluate(context);
    if (leftValue.type == ExprValue::UNDEF || rightValue.type == ExprValue::UNDEF)
        return ExprValue();

    if (leftValue.type == ExprValue::INT && rightValue.type == ExprValue::INT) {  // both ints -> integer modulo
        ensureNoLogarithmicUnit(rightValue);
        ensureNoLogarithmicUnit(leftValue);
        if (!rightValue.unit.empty() || !leftValue.unit.empty())
            bringToCommonTypeAndUnit(leftValue, rightValue);
        leftValue.intv = leftValue.intv % rightValue.intv;
        return leftValue;
    }
    else
        errorIntegerArgsExpected(leftValue, rightValue);
}

ExprValue PowNode::evaluate(Context *context) const
{
    ExprValue leftValue = child1->tryEvaluate(context);
    ExprValue rightValue = child2->tryEvaluate(context);
    if (leftValue.type == ExprValue::UNDEF || rightValue.type == ExprValue::UNDEF)
        return ExprValue();

    if (leftValue.type == ExprValue::INT && rightValue.type == ExprValue::INT) { // both ints -> integer exponentiation
        if (!rightValue.unit.empty() || !leftValue.unit.empty())
            errorDimlessArgsExpected(leftValue, rightValue);
        if (rightValue.intv < 0)
            throw opp_runtime_error("Negative exponent in integer exponentiation, cast operands to double to allow it");
        return intPow(leftValue.intv, rightValue.intv);
    }
    else {
        leftValue.convertToDouble();
        rightValue.convertToDouble();
        if (!rightValue.unit.empty() || !leftValue.unit.empty())
            errorDimlessArgsExpected(leftValue, rightValue);
        return pow(leftValue.dbl, rightValue.dbl);
    }
}

ExprValue CompareNode::evaluate(Context *context) const
{
    ExprValue leftValue = child1->tryEvaluate(context);
    ExprValue rightValue = child2->tryEvaluate(context);
    if (leftValue.type == ExprValue::UNDEF || rightValue.type == ExprValue::UNDEF)
        return ExprValue();
    double diff = compare(leftValue, rightValue);
    return compute(diff);
}

double CompareNode::compare(ExprValue& leftValue, ExprValue& rightValue) const
{
    if (leftValue.type==ExprValue::INT && rightValue.type==ExprValue::INT) {
        bringToCommonTypeAndUnit(rightValue, leftValue);
        return leftValue.intv - rightValue.intv;
    }
    else if (leftValue.type==ExprValue::DOUBLE || rightValue.type==ExprValue::DOUBLE) {
        leftValue.convertToDouble();
        rightValue.convertToDouble();
        rightValue.dbl = UnitConversion::convertUnit(rightValue.dbl, rightValue.unit.c_str(), leftValue.unit.c_str());
        // Notes:
        // 1. diff type is double so we can return nan if either is nan
        // 2.leftVal==rightVal part is to make inf==inf return 0 (=equals)
        return leftValue.dbl == rightValue.dbl ? 0 : leftValue.dbl - rightValue.dbl;
    }
    else if (leftValue.type==ExprValue::STRING && rightValue.type==ExprValue::STRING)
        return strcmp(leftValue.s, rightValue.s);
    else if (leftValue.type==ExprValue::BOOL && rightValue.type==ExprValue::BOOL)
        return (int)leftValue.bl - (int)rightValue.bl;
    else if (leftValue.type==ExprValue::POINTER && rightValue.type==ExprValue::POINTER) {
        if (!dynamic_cast<const EqualNode*>(this) && !dynamic_cast<const NotEqualNode*>(this))
            throw opp_runtime_error("Arguments of type 'object' may only be compared for equality");
        return leftValue.ptr == rightValue.ptr ? 0 : std::nan("");
    }
    else
        throw opp_runtime_error("Mismatching argument types '%s' vs '%s'",
                                ExprValue::getTypeName(leftValue.getType()),
                                ExprValue::getTypeName(rightValue.getType()));
}

ExprValue MatchNode::evaluate(Context *context) const
{
    ExprValue value = child1->tryEvaluate(context);
    ExprValue pattern = child2->tryEvaluate(context);
    if (value.type == ExprValue::UNDEF || pattern.type == ExprValue::UNDEF)
        return ExprValue();

    if (value.type!=ExprValue::STRING || pattern.type!=ExprValue::STRING)
        throw opp_runtime_error("String operands expected for '=~'");
    PatternMatcher matcher(pattern.s, true/*dottedpath*/, true/*fullstring*/, true/*casesensitive*/);
    return matcher.matches(value.s);
}

void MatchConstPatternNode::print(std::ostream& out, int spaciousness) const
{
    printChild(out, child, spaciousness);
    out << (needSpaces(spaciousness) ? " =~ " : "=~");
    out << matcher.str();
}

ExprValue MatchConstPatternNode::evaluate(Context *context) const
{
    ExprValue value = child->tryEvaluate(context);
    if (value.type == ExprValue::UNDEF)
        return value;
    if (value.type != ExprValue::STRING)
        throw opp_runtime_error("String operand expected for '=~'");
    return matcher.matches(value.s);

}

void InlineIfNode::print(std::ostream& out, int spaciousness) const
{
    printChild(out, child1, spaciousness);
    out << (needSpaces(spaciousness) ? " ? " : "?");
    printChild(out, child2, spaciousness);
    out << (needSpaces(spaciousness) ? " : " : ":");
    printChild(out, child3, spaciousness);
}

ExprValue InlineIfNode::evaluate(Context *context) const
{
    ExprValue cond = child1->tryEvaluate(context);
    if (cond.type == ExprValue::UNDEF)
        return cond;
    if (cond.type != ExprValue::BOOL)
        errorBooleanArgExpected(cond);
    return cond.bl ? child2->tryEvaluate(context) : child3->tryEvaluate(context);
}

ExprValue NotNode::evaluate(Context *context) const
{
    ExprValue value = child->tryEvaluate(context);
    if (value.type == ExprValue::UNDEF)
        return value;
    if (value.type != ExprValue::BOOL)
        errorBooleanArgExpected(value);
    value.bl = !value.bl;
    return value;
}

ExprValue LogicalInfixOperatorNode::evaluate(Context *context) const
{
    ExprValue leftValue = child1->tryEvaluate(context);
    if (leftValue.type == ExprValue::UNDEF)
        return leftValue;
    if (leftValue.type != ExprValue::BOOL)
        errorBooleanArgExpected(leftValue);
    if (shortcut(leftValue.bl))
        return compute(leftValue.bl, false); // value of 2nd arg is irrelevant

    ExprValue rightValue = child2->tryEvaluate(context);
    if (rightValue.type == ExprValue::UNDEF)
        return rightValue;
    if (rightValue.type != ExprValue::BOOL)
        errorBooleanArgExpected(rightValue);
    return compute(leftValue.bl, rightValue.bl);
}

ExprValue BitwiseNotNode::evaluate(Context *context) const
{
    ExprValue value = child->tryEvaluate(context);
    if (value.type == ExprValue::UNDEF)
        return value;
    if (value.type != ExprValue::INT)
        errorIntegerArgExpected(value);
    if (!value.unit.empty())
        errorDimlessArgExpected(value);
    value.intv = ~value.intv;
    return value;
}

ExprValue BitwiseInfixOperatorNode::evaluate(Context *context) const
{
    ExprValue leftValue = child1->tryEvaluate(context);
    ExprValue rightValue = child2->tryEvaluate(context);
    if (leftValue.type == ExprValue::UNDEF || rightValue.type == ExprValue::UNDEF)
        return ExprValue();
    if (rightValue.type != ExprValue::INT || leftValue.type != ExprValue::INT)
        errorIntegerArgsExpected(leftValue, rightValue);
    if (!rightValue.unit.empty() || !leftValue.unit.empty())
        errorDimlessArgsExpected(leftValue, rightValue);
    return compute(leftValue.intv, rightValue.intv);
}

void IntCastNode::print(std::ostream& out, int spaciousness) const
{
    printFunction(out, spaciousness);
}

ExprValue IntCastNode::evaluate(Context *context) const
{
    ExprValue value = child->tryEvaluate(context);
    switch (value.getType()) {
        case ExprValue::UNDEF:
            return value;
        case ExprValue::BOOL:
            return (intval_t)( (bool)value ? 1 : 0 );
        case ExprValue::INT:
            return value;
        case ExprValue::DOUBLE:
            return ExprValue(checked_int_cast<intval_t>(floor(value.doubleValue())), value.getUnit());
        case ExprValue::STRING: {
            std::string unit;
            double d = UnitConversion::parseQuantity(value.stringValue(), unit);
            return ExprValue(checked_int_cast<intval_t>(floor(d)), unit.c_str());
        }
        default:
            throw opp_runtime_error("Cannot cast %s to int", ExprValue::getTypeName(value.getType()));
    }

}

void DoubleCastNode::print(std::ostream& out, int spaciousness) const
{
    printFunction(out, spaciousness);
}

ExprValue DoubleCastNode::evaluate(Context *context) const
{
    ExprValue value = child->tryEvaluate(context);
    switch (value.getType()) {
        case ExprValue::UNDEF:
            return value;
        case ExprValue::BOOL:
            return (bool)value ? 1.0 : 0.0;
        case ExprValue::INT:
            return ExprValue((double)value.intValue(), value.getUnit());
        case ExprValue::DOUBLE:
            return value;
        case ExprValue::STRING: {
            std::string unit;
            double d = UnitConversion::parseQuantity(value.stringValue(), unit);
            return ExprValue(d, unit.c_str());
        }
        default:
            throw opp_runtime_error("Cannot cast %s to double", ExprValue::getTypeName(value.getType()));
    }
}

void UnitConversionNode::print(std::ostream& out, int spaciousness) const
{
    printFunction(out, spaciousness);
}

ExprValue UnitConversionNode::evaluate(Context *context) const
{
    ExprValue arg = child->tryEvaluate(context);
    if (arg.getType() == ExprValue::UNDEF)
        return arg;
    if (arg.getUnit() == nullptr)
        arg.setUnit(name.c_str());
    else {
        arg.convertToDouble();
        arg.convertTo(name.c_str());
    }
    return arg;
}

void MathFunc0Node::print(std::ostream& out, int spaciousness) const
{
    printFunction(out, spaciousness);
}

ExprValue MathFunc0Node::evaluate(Context *context) const
{
    return f();
}

void MathFunc1Node::print(std::ostream& out, int spaciousness) const
{
    printFunction(out, spaciousness);
}

ExprValue MathFunc1Node::evaluate(Context *context) const
{
    ExprValue arg1 = child->tryEvaluate(context);
    if (arg1.type == ExprValue::UNDEF)
        return arg1;
    ensureDimlessDoubleArg(arg1);
    return f(arg1.dbl);
}

void MathFunc2Node::print(std::ostream& out, int spaciousness) const
{
    printFunction(out, spaciousness);
}

ExprValue MathFunc2Node::evaluate(Context *context) const
{
    ExprValue arg1 = child1->tryEvaluate(context);
    ExprValue arg2 = child2->tryEvaluate(context);
    if (arg1.type == ExprValue::UNDEF || arg2.type == ExprValue::UNDEF)
        return ExprValue();
    ensureDimlessDoubleArg(arg1);
    ensureDimlessDoubleArg(arg2);
    return f(arg1.dbl, arg2.dbl);
}

void MathFunc3Node::print(std::ostream& out, int spaciousness) const
{
    printFunction(out, spaciousness);
}

ExprValue MathFunc3Node::evaluate(Context *context) const
{
    ExprValue arg1 = child1->tryEvaluate(context);
    ExprValue arg2 = child2->tryEvaluate(context);
    ExprValue arg3 = child3->tryEvaluate(context);
    if (arg1.type == ExprValue::UNDEF || arg2.type == ExprValue::UNDEF || arg3.type == ExprValue::UNDEF)
        return ExprValue();
    ensureDimlessDoubleArg(arg1);
    ensureDimlessDoubleArg(arg2);
    ensureDimlessDoubleArg(arg3);
    return f(arg1.dbl, arg2.dbl, arg3.dbl);
}

void MathFunc4Node::print(std::ostream& out, int spaciousness) const
{
    printFunction(out, spaciousness);
}

ExprValue MathFunc4Node::evaluate(Context *context) const
{
    Assert(children.size() == 4);
    ExprValue arg1 = children[0]->tryEvaluate(context);
    ExprValue arg2 = children[1]->tryEvaluate(context);
    ExprValue arg3 = children[2]->tryEvaluate(context);
    ExprValue arg4 = children[3]->tryEvaluate(context);
    if (arg1.type == ExprValue::UNDEF || arg2.type == ExprValue::UNDEF || arg3.type == ExprValue::UNDEF || arg4.type == ExprValue::UNDEF)
        return ExprValue();
    ensureDimlessDoubleArg(arg1);
    ensureDimlessDoubleArg(arg2);
    ensureDimlessDoubleArg(arg3);
    ensureDimlessDoubleArg(arg4);
    return f(arg1.dbl, arg2.dbl, arg3.dbl, arg4.dbl);
}

void FunctionNode::print(std::ostream& out, int spaciousness) const
{
    printFunction(out, spaciousness);
}

ExprValue FunctionNode::evaluate(Context *context) const
{
    int n = children.size();
    if (values == nullptr)
        values = new ExprValue[n]; // spare per-call allocation
    int i = 0;
    for (ExprNode *child : children) {
        values[i] = child->tryEvaluate(context);
        if (values[i].type == ExprValue::UNDEF)
            return ExprValue();
        i++;
    }
    return compute(context, values, n);
}

void MethodNode::print(std::ostream& out, int spaciousness) const
{
    printChild(out, children[0], spaciousness);
    out << ".";
    printFunction(out, spaciousness, 1);
}

ExprValue MethodNode::evaluate(Context *context) const
{
    int n = children.size();
    if (values == nullptr)
        values = new ExprValue[n]; // spare per-call allocation
    int i = 0;
    for (ExprNode *child : children) {
        values[i] = child->tryEvaluate(context);
        if (values[i].type == ExprValue::UNDEF)
            return ExprValue();
        i++;
    }
    return compute(context, values[0], values+1, n-1);
}

ExprValue DynamicallyResolvedVariableNode::getValue(Context *context) const
{
    for (auto resolver : context->expression->getDynamicResolvers()) {
        ExprValue result = resolver->readVariable(context, name.c_str());
        if (result.getType() != ExprValue::UNDEF)
            return result;
    }
    throw opp_runtime_error("No such variable: '%s'", name.c_str());
}

ExprValue DynamicallyResolvedIndexedVariableNode::getValue(Context *context, intval_t index) const
{
    for (auto resolver : context->expression->getDynamicResolvers()) {
        ExprValue result = resolver->readVariable(context, name.c_str(), index);
        if (result.getType() != ExprValue::UNDEF)
            return result;
    }
    throw opp_runtime_error("No such variable: '%s[]'", name.c_str());
}

ExprValue DynamicallyResolvedMemberNode::getValue(Context *context, const ExprValue& object) const
{
    for (auto resolver : context->expression->getDynamicResolvers()) {
        ExprValue result = resolver->readMember(context, object, name.c_str());
        if (result.getType() != ExprValue::UNDEF)
            return result;
    }
    throw opp_runtime_error("%s %s has no member named '%s'",
            (object.getType() == ExprValue::POINTER ? "Object" : "Value"),
            object.str().c_str(), name.c_str());
}

ExprValue DynamicallyResolvedIndexedMemberNode::getValue(Context *context, const ExprValue& object, intval_t index) const
{
    for (auto resolver : context->expression->getDynamicResolvers()) {
        ExprValue result = resolver->readMember(context, object, name.c_str(), index);
        if (result.getType() != ExprValue::UNDEF)
            return result;
    }
    throw opp_runtime_error("%s %s has no member named '%s[]'",
            (object.getType() == ExprValue::POINTER ? "Object" : "Value"),
            object.str().c_str(), name.c_str());
}

ExprValue DynamicallyResolvedFunctionNode::compute(Context *context, ExprValue argv[], int argc) const
{
    for (auto resolver : context->expression->getDynamicResolvers()) {
        ExprValue result = resolver->callFunction(context, name.c_str(), argv, argc);
        if (result.getType() != ExprValue::UNDEF)
            return result;
    }
    throw opp_runtime_error("No '%s' function accepting %d argument(s) was found", name.c_str(), argc);
}

ExprValue DynamicallyResolvedMethodNode::compute(Context *context, ExprValue& object, ExprValue argv[], int argc) const
{
    for (auto resolver : context->expression->getDynamicResolvers()) {
        ExprValue result = resolver->callMethod(context, object, name.c_str(), argv, argc);
        if (result.getType() != ExprValue::UNDEF)
            return result;
    }
    throw opp_runtime_error("%s %s has no '%s' method that accepts %d argument(s)",
            (object.getType() == ExprValue::POINTER ? "Object" : "Value"),
            object.str().c_str(), name.c_str(), argc);
}


}  // namespace expression
}  // namespace common
}  // namespace omnetpp
