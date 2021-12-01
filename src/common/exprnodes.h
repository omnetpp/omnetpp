//==========================================================================
//  EXPRNODES.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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
#include <functional>
#include "exprnode.h"
#include "patternmatcher.h" // MatchConstPattern

namespace omnetpp {
namespace common {

class Expression;

namespace expression {

class COMMON_API ValueNode : public LeafNode {
public:
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API UnaryOperatorNode : public UnaryNode {
public:
    virtual std::string str() const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
};

class COMMON_API BinaryOperatorNode : public BinaryNode {
public:
    virtual std::string str() const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
};

class COMMON_API TernaryOperatorNode : public TernaryNode {
    virtual std::string str() const override;
};

class COMMON_API ExprNodeFactory {
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

class COMMON_API ConstantNode : public ValueNode {
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

class COMMON_API VariableNode : public ValueNode {
protected:
    std::string name;
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
    virtual ExprValue getValue(Context *context) const = 0;
public:
    VariableNode(const char *name) : name(name) {}
    virtual std::string getName() const override {return name;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API ObjectNode : public NaryNode // as in JSON object, a.k.a. dictionary
{
  protected:
    std::string typeName;
    std::vector<std::string> fieldNames; // to be assigned from the child expressions
  protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    ObjectNode(const char *typeName, const std::vector<std::string>& fieldNames) : typeName(typeName), fieldNames(fieldNames) {}
    virtual std::string getName() const override {return typeName;}
    virtual std::string str() const override {return typeName + "{...}";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API ArrayNode : public NaryNode
{
  protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    ArrayNode() {}
    virtual std::string getName() const override {return "array";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

//---

class COMMON_API LambdaVariableNode : public VariableNode {
protected:
    std::function<ExprValue(Context*)> provider;
protected:
    virtual ExprValue getValue(Context *context) const override {return provider(context);}
public:
    LambdaVariableNode(const char *name, std::function<ExprValue(Context*)> provider) : VariableNode(name), provider(provider) {}
    virtual ExprNode *dup() const override {return new LambdaVariableNode(name.c_str(),provider);}
};

class COMMON_API IndexedVariableNode : public UnaryNode {
protected:
    std::string name;
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
    virtual ExprValue getValue(Context *context, intval_t index) const = 0;
public:
    IndexedVariableNode(const char *name) : name(name) {}
    virtual std::string getName() const override {return name;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API LambdaIndexedVariableNode : public IndexedVariableNode {
protected:
    std::function<ExprValue(Context*,intval_t)> provider;
protected:
    virtual ExprValue getValue(Context *context, intval_t index) const override {return provider(context,index);}
public:
    LambdaIndexedVariableNode(const char *name, std::function<ExprValue(Context*,intval_t)> provider) : IndexedVariableNode(name), provider(provider) {}
    virtual ExprNode *dup() const override {return new LambdaIndexedVariableNode(name.c_str(),provider);}
};

class COMMON_API MemberNode : public UnaryNode {
protected:
    std::string name;
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
    virtual ExprValue getValue(Context *context, const ExprValue& object) const = 0;
public:
    MemberNode(const char *name) : name(name) {}
    virtual std::string getName() const override {return name;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API LambdaMemberNode : public MemberNode {
protected:
    std::function<ExprValue(Context*,const ExprValue&)> provider;
protected:
    virtual ExprValue getValue(Context *context, const ExprValue& object) const override {return provider(context,object);}
public:
    LambdaMemberNode(const char *name, std::function<ExprValue(Context*,const ExprValue&)> provider) : MemberNode(name), provider(provider) {}
    virtual ExprNode *dup() const override {return new LambdaMemberNode(name.c_str(),provider);}
};

class COMMON_API IndexedMemberNode : public BinaryNode
{
protected:
    std::string name;
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
    virtual ExprValue getValue(Context *context, const ExprValue& object, intval_t index) const = 0;
public:
    IndexedMemberNode(const char *name) : name(name) {}
    virtual std::string getName() const override {return name;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API LambdaIndexedMemberNode : public IndexedMemberNode {
protected:
    std::function<ExprValue(Context*,const ExprValue&,intval_t)> provider;
protected:
    virtual ExprValue getValue(Context *context, const ExprValue& object, intval_t index) const override {return provider(context,object,index);}
public:
    LambdaIndexedMemberNode(const char *name, std::function<ExprValue(Context*,const ExprValue&,intval_t)> provider) : IndexedMemberNode(name), provider(provider) {}
    virtual ExprNode *dup() const override {return new LambdaIndexedMemberNode(name.c_str(),provider);}
};

class COMMON_API NegateNode : public UnaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new NegateNode;}
    virtual std::string getName() const override {return "-";}
    virtual Precedence getPrecedence() const override {return UNARY;}
};


class COMMON_API AddNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new AddNode;}
    virtual std::string getName() const override {return "+";}
    virtual Precedence getPrecedence() const override {return ADDSUB;}
};

class COMMON_API SubNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new SubNode;}
    virtual std::string getName() const override {return "-";}
    virtual Precedence getPrecedence() const override {return ADDSUB;}
};

class COMMON_API MulNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new MulNode;}
    virtual std::string getName() const override {return "*";}
    virtual Precedence getPrecedence() const override {return MULDIV;}
};

class COMMON_API DivNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new DivNode;}
    virtual std::string getName() const override {return "/";}
    virtual Precedence getPrecedence() const override {return MULDIV;}
};

class COMMON_API ModNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new ModNode;}
    virtual std::string getName() const override {return "%";}
    virtual Precedence getPrecedence() const override {return MULDIV;}
};

class COMMON_API PowNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new PowNode;}
    virtual std::string getName() const override {return "^";}
    virtual Precedence getPrecedence() const override {return POW;}
};

class COMMON_API CompareNode : public BinaryOperatorNode {
protected:
    virtual double compare(ExprValue& left, ExprValue& right) const;
    virtual ExprValue compute(double diff) const = 0;
    virtual ExprValue evaluate(Context *context) const override;
};

class COMMON_API ThreeWayComparisonNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return std::isnan(diff) ? diff : double((0<diff) - (diff<0)); }
public:
    virtual ExprNode *dup() const override {return new ThreeWayComparisonNode;}
    virtual std::string getName() const override {return "<=>";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_EQ;}
};

class COMMON_API EqualNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff == 0; }
public:
    virtual ExprNode *dup() const override {return new EqualNode;}
    virtual std::string getName() const override {return "==";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_EQ;}
};

class COMMON_API NotEqualNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff != 0; }
public:
    virtual ExprNode *dup() const override {return new NotEqualNode;}
    virtual std::string getName() const override {return "!=";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_EQ;}
};

class COMMON_API GreaterThanNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff > 0; }
public:
    virtual ExprNode *dup() const override {return new GreaterThanNode;}
    virtual std::string getName() const override {return ">";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_LT;}
};

class COMMON_API LessThanNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff < 0; }
public:
    virtual ExprNode *dup() const override {return new LessThanNode;}
    virtual std::string getName() const override {return "<";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_LT;}
};

class COMMON_API LessOrEqualNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff <= 0; }
public:
    virtual ExprNode *dup() const override {return new LessOrEqualNode;}
    virtual std::string getName() const override {return "<=";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_LT;}
};

class COMMON_API GreaterOrEqualNode : public CompareNode {
protected:
    virtual ExprValue compute(double diff) const override { return diff >= 0; }
public:
    virtual ExprNode *dup() const override {return new GreaterOrEqualNode;}
    virtual std::string getName() const override {return ">=";}
    virtual Precedence getPrecedence() const override {return RELATIONAL_LT;}
};

class COMMON_API MatchNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new MatchNode;}
    virtual std::string getName() const override {return "=~";}
    virtual Precedence getPrecedence() const override {return MATCH;}
};

class COMMON_API MatchConstPatternNode : public UnaryNode {
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

class COMMON_API InlineIfNode : public TernaryOperatorNode {
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new InlineIfNode;}
    virtual std::string getName() const override {return "?:";}
    virtual Precedence getPrecedence() const override {return IIF;}
};

class COMMON_API NotNode : public UnaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new NotNode;}
    virtual std::string getName() const override {return "!";}
    virtual Precedence getPrecedence() const override {return UNARY;}
};

class COMMON_API LogicalInfixOperatorNode : public BinaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual bool shortcut(bool left) const = 0;
    virtual bool compute(bool a, bool b) const = 0;
};

class COMMON_API AndNode : public LogicalInfixOperatorNode {
protected:
    virtual bool shortcut(bool a) const override {return a == false;}
    virtual bool compute(bool a, bool b) const override {return a && b; }
public:
    virtual ExprNode *dup() const override {return new AndNode;}
    virtual std::string getName() const override {return "&&";}
    virtual Precedence getPrecedence() const override {return LOGICAL_AND;}
};

class COMMON_API OrNode : public LogicalInfixOperatorNode {
protected:
    virtual bool shortcut(bool a) const override {return a == true;}
    virtual bool compute(bool a, bool b) const override {return a || b; }
public:
    virtual ExprNode *dup() const override {return new OrNode;}
    virtual std::string getName() const override {return "||";}
    virtual Precedence getPrecedence() const override {return LOGICAL_OR;}
};

class COMMON_API XorNode : public LogicalInfixOperatorNode {
protected:
    virtual bool shortcut(bool a) const override {return false;}
    virtual bool compute(bool a, bool b) const override {return a != b; }
public:
    virtual ExprNode *dup() const override {return new XorNode;}
    virtual std::string getName() const override {return "##";}
    virtual Precedence getPrecedence() const override {return LOGICAL_XOR;}
};

class COMMON_API BitwiseNotNode : public UnaryOperatorNode {
protected:
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new BitwiseNotNode;}
    virtual std::string getName() const override {return "~";}
    virtual Precedence getPrecedence() const override {return UNARY;}
};

class COMMON_API BitwiseInfixOperatorNode : public BinaryOperatorNode {
protected:
    virtual intval_t compute(intval_t a, intval_t b) const = 0;
    virtual ExprValue evaluate(Context *context) const override;
};

class COMMON_API BitwiseAndNode : public BitwiseInfixOperatorNode {
protected:
    virtual intval_t compute(intval_t a, intval_t b) const override { return a & b; }
public:
    virtual ExprNode *dup() const override {return new BitwiseAndNode;}
    virtual std::string getName() const override {return "&";}
    virtual Precedence getPrecedence() const override {return BITWISE_AND;}
};

class COMMON_API BitwiseOrNode : public BitwiseInfixOperatorNode {
protected:
    virtual intval_t compute(intval_t a, intval_t b) const override { return a | b; }
public:
    virtual ExprNode *dup() const override {return new BitwiseOrNode;}
    virtual std::string getName() const override {return "|";}
    virtual Precedence getPrecedence() const override {return BITWISE_OR;}
};

class COMMON_API BitwiseXorNode : public BitwiseInfixOperatorNode {
protected:
    virtual intval_t compute(intval_t a, intval_t b) const override { return a ^ b; }
public:
    virtual ExprNode *dup() const override {return new BitwiseXorNode;}
    virtual std::string getName() const override {return "#";}
    virtual Precedence getPrecedence() const override {return BITWISE_XOR;}
};

class COMMON_API LShiftNode : public BitwiseInfixOperatorNode {
protected:
    virtual intval_t compute(intval_t a, intval_t b) const override { return shift(a,b); }
public:
    virtual ExprNode *dup() const override {return new LShiftNode;}
    virtual std::string getName() const override {return "<<";}
    virtual Precedence getPrecedence() const override {return SHIFT;}
};

class COMMON_API RShiftNode : public BitwiseInfixOperatorNode {
protected:
    virtual intval_t compute(intval_t a, intval_t b) const override { return shift(a,-b); }
public:
    virtual ExprNode *dup() const override {return new RShiftNode;}
    virtual std::string getName() const override {return ">>";}
    virtual Precedence getPrecedence() const override {return SHIFT;}
};

//---

class COMMON_API IntCastNode : public UnaryNode {
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new IntCastNode;}
    virtual std::string getName() const override {return "int";}
    virtual std::string str() const override {return getName() + "()";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API DoubleCastNode : public UnaryNode {
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
public:
    virtual ExprNode *dup() const override {return new DoubleCastNode;}
    virtual std::string getName() const override {return "double";}
    virtual std::string str() const override {return getName() + "()";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API UnitConversionNode : public UnaryNode {
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

class COMMON_API MathFunc0Node : public LeafNode {
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
    virtual std::string str() const override {return getName() + "()";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API MathFunc1Node : public UnaryNode {
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
    virtual std::string str() const override {return getName() + "()";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API MathFunc2Node : public BinaryNode {
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
    virtual std::string str() const override {return getName() + "()";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API MathFunc3Node : public TernaryNode {
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
    virtual std::string str() const override {return getName() + "()";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API MathFunc4Node : public NaryNode {
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
    virtual std::string str() const override {return getName() + "()";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API FunctionNode : public NaryNode {
protected:
    std::string name;
    mutable ExprValue *values = nullptr; // preallocated buffer
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
    virtual ExprValue compute(Context *context, ExprValue argv[], int argc) const = 0;
public:
    FunctionNode(const char *name) : name(name) {}
    ~FunctionNode() {delete[] values;}
    virtual std::string getName() const override {return name;}
    virtual std::string str() const override {return getName() + "()";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API LambdaFunctionNode : public FunctionNode {
protected:
    std::function<ExprValue(Context*,ExprValue[],int)> function;
protected:
    virtual ExprValue compute(Context *context, ExprValue argv[], int argc) const override {return function(context,argv,argc);}
public:
    LambdaFunctionNode(const char *name, std::function<ExprValue(Context*,ExprValue[],int)> function) : FunctionNode(name), function(function) {}
    virtual ExprNode *dup() const override {return new LambdaFunctionNode(name.c_str(),function);}
};

class COMMON_API MethodNode : public NaryNode {
protected:
    std::string name;
    mutable ExprValue *values = nullptr; // preallocated buffer
protected:
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual ExprValue evaluate(Context *context) const override;
    virtual ExprValue compute(Context *context, ExprValue& object, ExprValue argv[], int argc) const = 0;
public:
    MethodNode(const char *name) : name(name) {}
    ~MethodNode() {delete[] values;}
    virtual std::string getName() const override {return name;}
    virtual std::string str() const override {return "." + getName() + "()";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class COMMON_API LambdaMethodNode : public MethodNode {
protected:
    std::function<ExprValue(Context*,const ExprValue&,ExprValue[],int)> function;
protected:
    virtual ExprValue compute(Context *context, ExprValue& object, ExprValue argv[], int argc) const override {return function(context,object,argv,argc);}
public:
    LambdaMethodNode(const char *name, std::function<ExprValue(Context*,const ExprValue&,ExprValue[],int)> function) : MethodNode(name), function(function) {}
    virtual ExprNode *dup() const override {return new LambdaMethodNode(name.c_str(),function);}
};

class DynamicallyResolvedVariableNode : public VariableNode {
protected:
    virtual ExprValue getValue(Context *context) const override;
    virtual std::string makeErrorMessage(std::exception& e) const override {return e.what();} // don't prepend variable name, as msg already contains it
public:
    DynamicallyResolvedVariableNode(const char *name) : VariableNode(name) {}
    virtual ExprNode *dup() const override {return new DynamicallyResolvedVariableNode(name.c_str());}
};

class DynamicallyResolvedIndexedVariableNode : public IndexedVariableNode {
protected:
    virtual ExprValue getValue(Context *context, intval_t index) const override;
    virtual std::string makeErrorMessage(std::exception& e) const override {return e.what();} // don't prepend variable name, as msg already contains it
public:
    DynamicallyResolvedIndexedVariableNode(const char *name) : IndexedVariableNode(name) {}
    virtual ExprNode *dup() const override {return new DynamicallyResolvedIndexedVariableNode(name.c_str());}
};

class DynamicallyResolvedMemberNode : public MemberNode {
protected:
    virtual ExprValue getValue(Context *context, const ExprValue& object) const override;
    virtual std::string makeErrorMessage(std::exception& e) const override {return e.what();} // don't prepend variable name, as msg already contains it
public:
    DynamicallyResolvedMemberNode(const char *name) : MemberNode(name) {}
    virtual ExprNode *dup() const override {return new DynamicallyResolvedMemberNode(name.c_str());}
};

class DynamicallyResolvedIndexedMemberNode : public IndexedMemberNode {
protected:
    virtual ExprValue getValue(Context *context, const ExprValue& object, intval_t index) const override;
    virtual std::string makeErrorMessage(std::exception& e) const override {return e.what();} // don't prepend variable name, as msg already contains it
public:
    DynamicallyResolvedIndexedMemberNode(const char *name) : IndexedMemberNode(name) {}
    virtual ExprNode *dup() const override {return new DynamicallyResolvedIndexedMemberNode(name.c_str());}
};

class DynamicallyResolvedFunctionNode : public FunctionNode {
protected:
    virtual ExprValue compute(Context *context, ExprValue argv[], int argc) const override;
    virtual std::string makeErrorMessage(std::exception& e) const override {return e.what();} // don't prepend variable name, as msg already contains it
public:
    DynamicallyResolvedFunctionNode(const char *name) : FunctionNode(name) {}
    virtual ExprNode *dup() const override {return new DynamicallyResolvedFunctionNode(name.c_str());}
};

class DynamicallyResolvedMethodNode : public MethodNode {
protected:
    virtual ExprValue compute(Context *context, ExprValue& object, ExprValue argv[], int argc) const override;
    virtual std::string makeErrorMessage(std::exception& e) const override {return e.what();} // don't prepend variable name, as msg already contains it
public:
    DynamicallyResolvedMethodNode(const char *name) : MethodNode(name) {}
    virtual ExprNode *dup() const override {return new DynamicallyResolvedMethodNode(name.c_str());}
};

}  // namespace expression
}  // namespace common
}  // namespace omnetpp


#endif
