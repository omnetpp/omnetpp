//==========================================================================
//  EXPRNODE.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2019 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_EXPRNODE_H
#define __OMNETPP_COMMON_EXPRNODE_H

#include <iostream>
#include <string>
#include <vector>
#include "commonutil.h" //Assert()
#include "exprvalue.h"
#include "intutil.h"
#include "unitconversion.h"

namespace omnetpp { class cObject; };

namespace omnetpp {
namespace common {

class Expression;

namespace expression {

/**
 * Subclass to pass more information into the evaluator.
 */
struct Context {
    const Expression *expression = nullptr;
    cObject *simContext = nullptr; //TODO any_ptr?
    virtual ~Context() {}
};

/**
 * Node in the expression evaluation tree.
 */
class COMMON_API ExprNode {
public:
    enum Precedence {
        ELEM = 0,    // constant, variable, function
        MEMBER = 1,  // . (method, member)
        UNARY = 2,   // - ~ !
        POW = 3,     // ^
        MULDIV = 4,  // * / %
        ADDSUB = 5,  // + -
        SHIFT = 6,   // << >>
        BITWISE_AND = 7,    // &
        BITWISE_XOR = 8,    // #
        BITWISE_OR = 9,     // |
        ARITHM_LAST = BITWISE_OR,
        MATCH = 10,         // =~
        RELATIONAL_LT = 11, // < <= > >=
        RELATIONAL_EQ = 12, // == !=
        LOGICAL_AND = 13,   // &&
        LOGICAL_XOR = 14,   // ##
        LOGICAL_OR = 15,    // ||
        IIF = 16,           // ?:
        LASTPREC = IIF,
    };

    enum {
        SPACIOUSNESS_MIN = 0,
        SPACIOUSNESS_DEFAULT = LASTPREC - ARITHM_LAST,
        SPACIOUSNESS_MAX = LASTPREC
    };

    class eval_error : public opp_runtime_error {
    public:
        eval_error(const std::string& what) : opp_runtime_error(what) {}
    };

protected:
    virtual ExprValue evaluate(Context *context) const = 0; // do not call directly! only via tryEvaluate()
    virtual std::string makeErrorMessage(std::exception& e) const;
    virtual void printFunction(std::ostream& out, int spaciousness, int startIndex=0) const;
    virtual void printChild(std::ostream& out, ExprNode *child, int spaciousness) const;
    virtual bool needSpaces(int spaciousness) const {return spaciousness >= (LASTPREC-getPrecedence());}
    virtual std::string asPrinted(bool quote=false) const;
    static void bringToCommonTypeAndUnit(ExprValue& a, ExprValue& b);
    inline static void ensureArgType(int index, ExprValue::Type type, const ExprValue& actual);
    inline static void ensureNumericArg(const ExprValue& actual);
    inline static void ensureNoLogarithmicUnit(const ExprValue& value);
    inline static void ensureNumericNoLogarithmicUnit(const ExprValue& value);
    inline static void ensureDimlessDoubleArg(ExprValue& value);
    [[noreturn]] static void errorWrongArgType(int index, ExprValue::Type expected, const ExprValue& actual);
    [[noreturn]] static void errorBooleanArgExpected(const ExprValue& actual);
    [[noreturn]] static void errorBooleanArgsExpected(const ExprValue& actual1, const ExprValue& actual2);
    [[noreturn]] static void errorNumericArgExpected(const ExprValue& actual);
    [[noreturn]] static void errorNumericArgsExpected(const ExprValue& actual1, const ExprValue& actual2);
    [[noreturn]] static void errorIntegerArgExpected(const ExprValue& actual);
    [[noreturn]] static void errorIntegerArgsExpected(const ExprValue& actual1, const ExprValue& actual2);
    [[noreturn]] static void errorDimlessArgExpected(const ExprValue& actual);
    [[noreturn]] static void errorDimlessArgsExpected(const ExprValue& actual1, const ExprValue& actual2);
public:
    virtual ~ExprNode() {}
    inline ExprValue tryEvaluate(Context *context) const;
    virtual std::string getName() const = 0;
    virtual std::string str() const {return getName();}
    virtual ExprNode *dupTree() const;
    virtual ExprNode *dup() const = 0;
    virtual Precedence getPrecedence() const = 0;
    virtual void setChildren(const std::vector<ExprNode*>& children) = 0;
    virtual std::vector<ExprNode*> getChildren() const = 0;
    virtual void print(std::ostream& out, int spaciousness) const = 0; // helper for Expression::str(); in subclasses, call printChild() instead of this
};

inline ExprValue ExprNode::tryEvaluate(Context *context) const
{
    try {
        return evaluate(context);
    }
    catch (const eval_error& e) {
        throw;
    }
    catch (std::exception& e) {
        throw eval_error(makeErrorMessage(e));
    }
}

inline void ExprNode::ensureArgType(int index, ExprValue::Type type, const ExprValue& actual)
{
    if (actual.type != type)
        errorWrongArgType(index, type, actual);
}

inline void ExprNode::ensureNumericArg(const ExprValue& actual)
{
    if (actual.type != ExprValue::INT && actual.type != ExprValue::DOUBLE)
        errorNumericArgExpected(actual);
}

inline void ExprNode::ensureNoLogarithmicUnit(const ExprValue& value)
{
    if (!value.unit.empty() && !UnitConversion::isLinearUnit(value.unit.c_str()))
        throw opp_runtime_error("Refusing to perform computations involving quantities with nonlinear units (%s)", value.str().c_str());
}

inline void ExprNode::ensureNumericNoLogarithmicUnit(const ExprValue& value)
{
    if (value.type != ExprValue::INT && value.type != ExprValue::DOUBLE)
        errorNumericArgExpected(value);
    if (!value.unit.empty() && !UnitConversion::isLinearUnit(value.unit.c_str()))
        throw opp_runtime_error("Refusing to perform computations involving quantities with nonlinear units (%s)", value.str().c_str());
}

inline void ExprNode::ensureDimlessDoubleArg(ExprValue& value)
{
    value.convertToDouble();
    if (!value.unit.empty())
        errorDimlessArgExpected(value);
}


class COMMON_API LeafNode : public ExprNode {
public:
    virtual Precedence getPrecedence() const override {return ELEM;}
    virtual void setChildren(const std::vector<ExprNode*>& children) override {Assert(children.empty());}
    virtual std::vector<ExprNode*> getChildren() const override {return std::vector<ExprNode*>{};}
};

class COMMON_API UnaryNode : public ExprNode {
protected:
    ExprNode *child=nullptr;
public:
    virtual ~UnaryNode() {delete child;}
    void setChild(ExprNode *node) {child = node;}
    virtual void setChildren(const std::vector<ExprNode*>& children) override {Assert(children.size()==1); child = children[0];}
    virtual std::vector<ExprNode*> getChildren() const override {return std::vector<ExprNode*>{child};}
};

class COMMON_API BinaryNode : public ExprNode {
protected:
    ExprNode *child1=nullptr, *child2=nullptr;
public:
    virtual ~BinaryNode() {delete child1; delete child2;}
    void setChildren(ExprNode *left, ExprNode *right) {child1 = left; child2 = right;}
    virtual void setChildren(const std::vector<ExprNode*>& children) override {Assert(children.size()==2); child1 = children[0]; child2 = children[1];}
    virtual std::vector<ExprNode*> getChildren() const override {return std::vector<ExprNode*>{child1,child2};}
};

class COMMON_API TernaryNode : public ExprNode {
protected:
    ExprNode *child1=nullptr, *child2=nullptr, *child3=nullptr;
public:
    virtual ~TernaryNode() {delete child1; delete child2; delete child3;}
    void setChildren(ExprNode *node1, ExprNode *node2, ExprNode *node3) {child1 = node1; child2 = node2; child3 = node3;}
    virtual void setChildren(const std::vector<ExprNode*>& children) override {Assert(children.size()==3); child1 = children[0]; child2 = children[1]; child3 = children[2];}
    virtual std::vector<ExprNode*> getChildren() const override {return std::vector<ExprNode*>{child1,child2,child3};}
};

class COMMON_API NaryNode : public ExprNode {
protected:
    std::vector<ExprNode*> children;
public:
    virtual ~NaryNode() {for (ExprNode *child : children) delete child;}
    virtual void setChildren(const std::vector<ExprNode*>& nodes) override {children=nodes;}
    void appendChild(ExprNode *node) {children.push_back(node);}
    virtual std::vector<ExprNode*> getChildren() const override {return children;}
};

}  // namespace common
}  // namespace common
}  // namespace omnetpp


#endif


