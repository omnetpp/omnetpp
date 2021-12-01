//==========================================================================
//   EXPRNODE.CC  - part of
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
#include <sstream>
#include "exprnode.h"
#include "stringutil.h"
#include "exception.h"
#include "exprnodes.h" // for makeErrorMessage()

namespace omnetpp {
namespace common {
namespace expression {

ExprNode *ExprNode::dupTree() const
{
    ExprNode *newNode = dup();
    std::vector<ExprNode*> tmp;
    for (ExprNode *child : getChildren())
        tmp.push_back(child->dupTree());
    newNode->setChildren(tmp);
    return newNode;
}

std::string ExprNode::asPrinted(bool quote) const
{
    std::stringstream ss;
    if (quote)
        ss << "'";
    print(ss, 0);
    if (quote)
        ss << "'";
    return ss.str();
}

std::string ExprNode::makeErrorMessage(std::exception& e) const
{
    return str() + ": " + e.what();
}

void ExprNode::bringToCommonTypeAndUnit(ExprValue& a, ExprValue& b)
{
    if (a.getType() == ExprValue::INT && b.getType() == ExprValue::INT) {
        // if conversion factor is an integer, try doing it using integer arithmetic
        // by converting values into the smaller unit; e.g. if one is KiB and the
        // other is MiB, convert MiB value into KiB.
        double c = UnitConversion::getConversionFactor(a.getUnit(), b.getUnit());
        double c2 = UnitConversion::getConversionFactor(b.getUnit(), a.getUnit());
        if (c == 1)
            return; // done
        else if (c > 1 && c == floor(c)) {
            a.intv = safeMul((intval_t)c, a.intv);
            a.unit = b.unit;
            return;
        }
        else if (c2 > 1 && c2 == floor(c2)) {
            b.intv = safeMul((intval_t)c2, b.intv);
            b.unit = a.unit;
            return;
        }
        // else fall through to computing the result in doubles
    }

    // do it in double, or fail due to incompatible units
    a.convertToDouble();
    b.convertToDouble();
    a.dbl = UnitConversion::convertUnit(a.dbl, a.unit.c_str(), b.unit.c_str());
    a.unit = b.unit;
}

void ExprNode::errorBooleanArgExpected(const ExprValue& actual)
{
    throw opp_runtime_error("Boolean argument expected, got %s", ExprValue::getTypeName(actual.getType()));
}

void ExprNode::errorBooleanArgsExpected(const ExprValue& actual1, const ExprValue& actual2)
{
    throw opp_runtime_error("Boolean arguments expected, got %s and %s", ExprValue::getTypeName(actual1.getType()), ExprValue::getTypeName(actual2.getType()));
}

void ExprNode::errorNumericArgExpected(const ExprValue& actual)
{
    throw opp_runtime_error("Numeric argument expected, got %s", ExprValue::getTypeName(actual.getType()));
}

void ExprNode::errorNumericArgsExpected(const ExprValue& actual1, const ExprValue& actual2)
{
    throw opp_runtime_error("Numeric arguments expected, got %s and %s", ExprValue::getTypeName(actual1.getType()), ExprValue::getTypeName(actual2.getType()));
}

void ExprNode::errorIntegerArgExpected(const ExprValue& actual)
{
    const char *hint = actual.getType() == ExprValue::DOUBLE ? " (note: no implicit conversion from double to int)" : "";
    throw opp_runtime_error("Integer argument expected, got %s%s", ExprValue::getTypeName(actual.getType()), hint);
}

void ExprNode::errorIntegerArgsExpected(const ExprValue& actual1, const ExprValue& actual2)
{
    bool hasDouble = actual1.getType() == ExprValue::DOUBLE || actual2.getType() == ExprValue::DOUBLE;
    const char *hint = hasDouble ? " (note: no implicit conversion from double to int)" : "";
    throw opp_runtime_error("Integer arguments expected, got %s and %s%s", ExprValue::getTypeName(actual1.getType()), ExprValue::getTypeName(actual2.getType()), hint);
}

void ExprNode::errorDimlessArgExpected(const ExprValue& actual)
{
    throw opp_runtime_error("Dimensionless argument expected, got %s", actual.str().c_str());
}

void ExprNode::errorDimlessArgsExpected(const ExprValue& actual1, const ExprValue& actual2)
{
    throw opp_runtime_error("Dimensionless arguments expected, got %s and %s", actual1.str().c_str(), actual2.str().c_str());
}

void ExprNode::errorWrongArgType(int index, ExprValue::Type expected, const ExprValue& actual)
{
    const char *note = (expected == ExprValue::INT && actual.getType() == ExprValue::DOUBLE) ?
            " (note: no implicit conversion from double to int)" : "";
    const char *expectedTypeName = (expected == ExprValue::DOUBLE) ? "double or int" :  ExprValue::getTypeName(expected);
    throw opp_runtime_error("%s expected for argument %d, got %s%s",
            expectedTypeName, index, ExprValue::getTypeName(actual.getType()), note);
}

void ExprNode::printFunction(std::ostream& out, int spaciousness, int startIndex) const
{
    out << getName() << "(";
    std::vector<ExprNode*> children = getChildren();
    for (size_t i = startIndex; i < children.size(); i++) {
        if (i != startIndex)
            out << (spaciousness >= LASTPREC-ARITHM_LAST ? ", " : ",");
        children[i]->print(out, spaciousness);
    }
    out << ")";
}

void ExprNode::printChild(std::ostream& out, ExprNode *child, int spaciousness) const
{
    bool paren = (getPrecedence() < child->getPrecedence());
    if (paren)
        out << "(";
    child->print(out, spaciousness);
    if (paren)
        out << ")";
}

}  // namespace expression
}  // namespace common
}  // namespace omnetpp
