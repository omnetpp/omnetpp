//==========================================================================
//  EXPRNODES.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2019 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_EXPRNODES_H
#define __OMNETPP_COMMON_EXPRNODES_H

#include <cmath>
#include <iostream>
#include "exprnode.h"
#include "patternmatcher.h" // MatchConstPattern

namespace omnetpp {
namespace common {
namespace expression {

class ValueNode : public LeafNode {
public:
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class UnaryOperatorNode : public UnaryNode {
public:
    virtual void print(std::ostream& out, int spaciousness) const override;
};

class BinaryOperatorNode : public BinaryNode {
public:
    virtual void print(std::ostream& out, int spaciousness) const override;
};

class TernaryOperatorNode : public TernaryNode {
};

class ExprNodeFactory {
public:
    static LeafNode *createConstant(const ExprValue& value);
    static UnaryNode *createUnaryOperator(const char *op);
    static BinaryNode *createBinaryOperator(const char *op);
    static TernaryNode *createTernaryOperator(const char *op);
    static bool supportsStdMathFunction(const char *name);
    static ExprNode *createStdMathFunction(const char *name);
    static ExprNode *createMathFunction(const char *name, double (*f0)());
    static ExprNode *createMathFunction(const char *name, double (*f1)(double));
    static ExprNode *createMathFunction(const char *name, double (*f2)(double,double));
    static ExprNode *createMathFunction(const char *name, double (*f3)(double,double,double));
};

//--------

class ConstantNode : public ValueNode {
protected:
    ExprValue value;
protected:
    virtual void print(std::ostream& out, int spaciousness) const override {out << value.str();}
    virtual ExprValue evaluate(Context *context) const override {return value;}
public:
    ConstantNode(const ExprValue& value) : value(value) {}
    virtual ExprNode *dup() const override {return new ConstantNode(value);}
    virtual std::string getName() const override {return value.str();}
};

class NegateNode : public UnaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new NegateNode;}
    virtual std::string getName() const override {return "-";}
    virtual Precedence getPrecedence() const override {return UNARY;}
};


class AddNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new AddNode;}
    virtual std::string getName() const override {return "+";}
    virtual Precedence getPrecedence() const override {return ADDSUB;}
};

class SubNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new SubNode;}
    virtual std::string getName() const override {return "-";}
    virtual Precedence getPrecedence() const override {return ADDSUB;}
};

class MulNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new MulNode;}
    virtual std::string getName() const override {return "*";}
    virtual Precedence getPrecedence() const override {return MULDIV;}
};

class DivNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new DivNode;}
    virtual std::string getName() const override {return "/";}
    virtual Precedence getPrecedence() const override {return MULDIV;}
};

class ModNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new ModNode;}
    virtual std::string getName() const override {return "%";}
    virtual Precedence getPrecedence() const override {return MULDIV;}
};

class PowNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new PowNode;}
    virtual std::string getName() const override {return "^";}
    virtual Precedence getPrecedence() const override {return POW;}
};

class CompareNode : public BinaryOperatorNode {
protected:
    virtual ExprValue compute(double diff) const = 0;
    virtual ExprValue evaluate(Context *context) const override;
};

class ThreeWayComparisonNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return std::isnan(diff) ? diff : double((0<diff) - (diff<0)); }
public:
    virtual ExprNode *dup() const override {return new ThreeWayComparisonNode;}
    virtual std::string getName() const override {return "<=>";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_EQ;}
};

class EqualNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff == 0; }
public:
    virtual ExprNode *dup() const override {return new EqualNode;}
    virtual std::string getName() const override {return "==";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_EQ;}
};

class NotEqualNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff != 0; }
public:
    virtual ExprNode *dup() const override {return new NotEqualNode;}
    virtual std::string getName() const override {return "!=";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_EQ;}
};

class GreaterThanNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff > 0; }
public:
    virtual ExprNode *dup() const override {return new GreaterThanNode;}
    virtual std::string getName() const override {return ">";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_LT;}
};

class LessThanNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff < 0; }
public:
    virtual ExprNode *dup() const override {return new LessThanNode;}
    virtual std::string getName() const override {return "<";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_LT;}
};

class LessOrEqualNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff <= 0; }
public:
    virtual ExprNode *dup() const override {return new LessOrEqualNode;}
    virtual std::string getName() const override {return "<=";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_LT;}
};

class GreaterOrEqualNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff >= 0; }
public:
    virtual ExprNode *dup() const override {return new GreaterOrEqualNode;}
    virtual std::string getName() const override {return ">=";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_LT;}
};

class MatchNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new MatchNode;}
    virtual std::string getName() const override {return "=~";}
    virtual Precedence getPrecedence() const override {return MATCH;}
};

class MatchConstPatternNode : public UnaryNode {
protected:
    PatternMatcher matcher;
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    MatchConstPatternNode(const PatternMatcher& matcher) : matcher(matcher) {}
    MatchConstPatternNode(const char *pattern, bool dottedpath=true, bool fullstring=true, bool casesensitive=true) :
        matcher(pattern,dottedpath, fullstring, casesensitive) {}
    virtual ExprNode *dup() const override {return new MatchConstPatternNode(matcher);}
    virtual std::string getName() const override {return "=~<PATTERN>";}
    virtual Precedence getPrecedence() const override {return MATCH;}
};

class InlineIfNode : public TernaryOperatorNode {
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new InlineIfNode;}
    virtual std::string getName() const override {return "?:";}
    virtual Precedence getPrecedence() const override {return IIF;}
};

class NotNode : public UnaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new NotNode;}
    virtual std::string getName() const override {return "!";}
    virtual Precedence getPrecedence() const override {return UNARY;}
};

class LogicalInfixOperatorNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual bool shortcut(bool left) const = 0;
    virtual bool compute(bool a, bool b) const = 0;
};

class AndNode : public LogicalInfixOperatorNode {
protected:
    virtual bool shortcut(bool a) const override {return a == false;}
    virtual bool compute(bool a, bool b) const override {return a && b; }
public:
    virtual ExprNode *dup() const override {return new AndNode;}
    virtual std::string getName() const override {return "&&";}
    virtual Precedence getPrecedence() const override {return LOGICAL_AND;}
};

class OrNode : public LogicalInfixOperatorNode {
protected:
    virtual bool shortcut(bool a) const override {return a == true;}
    virtual bool compute(bool a, bool b) const override {return a || b; }
public:
    virtual ExprNode *dup() const override {return new OrNode;}
    virtual std::string getName() const override {return "||";}
    virtual Precedence getPrecedence() const override {return LOGICAL_OR;}
};

class XorNode : public LogicalInfixOperatorNode {
protected:
    virtual bool shortcut(bool a) const override {return false;}
    virtual bool compute(bool a, bool b) const override {return a != b; }
public:
    virtual ExprNode *dup() const override {return new XorNode;}
    virtual std::string getName() const override {return "##";}
    virtual Precedence getPrecedence() const override {return LOGICAL_XOR;}
};

class BitwiseNotNode : public UnaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new BitwiseNotNode;}
    virtual std::string getName() const override {return "~";}
    virtual Precedence getPrecedence() const override {return UNARY;}
};

class BitwiseInfixOperatorNode : public BinaryOperatorNode {
protected:
    virtual intpar_t compute(intpar_t a, intpar_t b) const = 0;
    virtual ExprValue evaluate(Context *context) const override;
};

class BitwiseAndNode : public BitwiseInfixOperatorNode {
protected:
    virtual intpar_t compute(intpar_t a, intpar_t b) const override { return a & b; }
public:
    virtual ExprNode *dup() const override {return new BitwiseAndNode;}
    virtual std::string getName() const override {return "&";}
    virtual Precedence getPrecedence() const override {return BITWISE_AND;}
};

class BitwiseOrNode : public BitwiseInfixOperatorNode {
protected:
    virtual intpar_t compute(intpar_t a, intpar_t b) const override { return a | b; }
public:
    virtual ExprNode *dup() const override {return new BitwiseOrNode;}
    virtual std::string getName() const override {return "|";}
    virtual Precedence getPrecedence() const override {return BITWISE_OR;}
};

class BitwiseXorNode : public BitwiseInfixOperatorNode {
protected:
    virtual intpar_t compute(intpar_t a, intpar_t b) const override { return a ^ b; }
public:
    virtual ExprNode *dup() const override {return new BitwiseXorNode;}
    virtual std::string getName() const override {return "#";}
    virtual Precedence getPrecedence() const override {return BITWISE_XOR;}
};

class LShiftNode : public BitwiseInfixOperatorNode {
protected:
    virtual intpar_t compute(intpar_t a, intpar_t b) const override { return shift(a,b); }
public:
    virtual ExprNode *dup() const override {return new LShiftNode;}
    virtual std::string getName() const override {return "<<";}
    virtual Precedence getPrecedence() const override {return SHIFT;}
};

class RShiftNode : public BitwiseInfixOperatorNode {
protected:
    virtual intpar_t compute(intpar_t a, intpar_t b) const override { return shift(a,-b); }
public:
    virtual ExprNode *dup() const override {return new RShiftNode;}
    virtual std::string getName() const override {return ">>";}
    virtual Precedence getPrecedence() const override {return SHIFT;}
};

//---

class IntCastNode : public UnaryNode {
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new IntCastNode;}
    virtual std::string getName() const override {return "int";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class DoubleCastNode : public UnaryNode {
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new DoubleCastNode;}
    virtual std::string getName() const override {return "double";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class UnitConversionNode : public UnaryNode {
protected:
    std::string name;
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    UnitConversionNode(const char *name) : name(name) {}
    virtual ExprNode *dup() const override {return new UnitConversionNode(name.c_str());}
    virtual std::string getName() const override {return name;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class MathFunc0Node : public LeafNode {
protected:
    std::string name;
    double (*f)();
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    MathFunc0Node(const char *name, double (*f)()) : name(name), f(f) {}
    virtual ExprNode *dup() const override {return new MathFunc0Node(name.c_str(), f);}
    virtual std::string getName() const override {return name;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class MathFunc1Node : public UnaryNode {
protected:
    std::string name;
    double (*f)(double);
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    MathFunc1Node(const char *name, double (*f)(double)) : name(name), f(f) {}
    virtual ExprNode *dup() const override {return new MathFunc1Node(name.c_str(), f);}
    virtual std::string getName() const override {return name;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class MathFunc2Node : public BinaryNode {
protected:
    std::string name;
    double (*f)(double,double);
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    MathFunc2Node(const char *name, double (*f)(double,double)) : name(name), f(f) {}
    virtual ExprNode *dup() const override {return new MathFunc2Node(name.c_str(), f);}
    virtual std::string getName() const override {return name;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class MathFunc3Node : public TernaryNode {
protected:
    std::string name;
    double (*f)(double,double,double);
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    MathFunc3Node(const char *name, double (*f)(double,double,double)) : name(name), f(f) {}
    virtual ExprNode *dup() const override {return new MathFunc3Node(name.c_str(), f);}
    virtual std::string getName() const override {return name;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class MathFunc4Node : public NaryNode {
protected:
    std::string name;
    double (*f)(double,double,double,double);
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    MathFunc4Node(const char *name, double (*f)(double,double,double,double)) : name(name), f(f) {}
    virtual ExprNode *dup() const override {return new MathFunc4Node(name.c_str(), f);}
    virtual std::string getName() const override {return name;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class FunctionNode : public NaryNode {
protected:
    std::string name;
    ExprValue (*f)(ExprValue*, int);
    mutable ExprValue *values = nullptr; // preallocated buffer
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    FunctionNode(const char *name, ExprValue (*f)(ExprValue*, int)) : name(name), f(f) {}
    ~FunctionNode() {delete[] values;}
    virtual ExprNode *dup() const override {return new FunctionNode(name.c_str(), f);}
    virtual std::string getName() const override {return name;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

} // namespace common
} // namespace common
} // namespace omnetpp


#endif


