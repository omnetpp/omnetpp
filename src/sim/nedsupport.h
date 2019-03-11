//==========================================================================
//   NEDSUPPORT.H  - part of
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

#ifndef __OMNETPP_NEDSUPPORT_H
#define __OMNETPP_NEDSUPPORT_H

#include "omnetpp/simkerneldefs.h"
#include "common/expression.h"
#include "common/exprnodes.h"

using namespace omnetpp::common;
using namespace omnetpp::common::expression;

namespace omnetpp {

class cNedFunction;

namespace nedsupport {

using omnetpp::common::expression::Context;

cNedValue makeNedValue(const ExprValue& value);
ExprValue makeExprValue(const cNedValue& value);
ExprValue makeExprValue(const cPar& par);

class NedExpressionContext : public cExpression::Context
{
  public:
    enum ExpressionRole { SUBMODULE_CONDITION, SUBMODULE_ARRAY_CONDITION, OTHER };
    explicit NedExpressionContext(cComponent *component, ExpressionRole role, const char *computedTypename) :
            cExpression::Context(component), role(role), computedTypename(computedTypename) {}
    ExpressionRole role;
    const char *computedTypename;
};

class NedFunctionNode : public NaryNode
{
  private:
    cNedFunction *nedFunction;
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual std::string makeErrorMessage(std::exception& e) const override;
  public:
    NedFunctionNode(cNedFunction *f) : nedFunction(f) {}
    NedFunctionNode *dup() const override {return new NedFunctionNode(nedFunction);}
    virtual Precedence getPrecedence() const override {return ELEM;}
    virtual std::string getName() const override;
};

class ModuleIndex : public LeafNode
{
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    ModuleIndex() {}
    ModuleIndex *dup() const override {return new ModuleIndex();}
    virtual std::string getName() const override {return "index";}
};

class Exists : public LeafNode
{
    std::string name;
    bool inSubcomponentScope;
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    Exists(const char *ident, bool inSubcomponentScope) : name(ident), inSubcomponentScope(inSubcomponentScope) {}
    Exists *dup() const override {return new Exists(name.c_str(), inSubcomponentScope);}
    virtual std::string getName() const override {return "exists";}
};

class Typename : public LeafNode
{
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    Typename() {}
    Typename *dup() const override {return new Typename();}
    virtual std::string getName() const override {return "typename";}
};

/**
 * @brief Parameter reference, ident and this.ident forms
 */
class ParameterRef : public LeafNode
{
  protected:
    std::string paramName;
    bool inSubcomponentScope;  // if true, operate on context module's parent
    bool ofThis; // if true, "this.ident" form
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    ParameterRef(const char *paramName, bool inSubcomponentScope, bool ofThis) :
        paramName(paramName), inSubcomponentScope(inSubcomponentScope), ofThis(ofThis) {}
    ParameterRef *dup() const override {return new ParameterRef(paramName.c_str(), inSubcomponentScope, ofThis);}
    virtual std::string getName() const override {return std::string(ofThis ? "this." : "") + paramName;}
};

/**
 * @brief Parameter reference, ident.ident form
 */
class SubmoduleParameterRef : public LeafNode
{
  protected:
    std::string submoduleName;
    std::string paramName;
    bool inSubcomponentScope;  // if true, operate on context module's parent
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    SubmoduleParameterRef(const char *submoduleName, const char *paramName, bool inSubcomponentScope) :
        submoduleName(submoduleName), paramName(paramName), inSubcomponentScope(inSubcomponentScope) {}
    SubmoduleParameterRef *dup() const override {return new SubmoduleParameterRef(submoduleName.c_str(), paramName.c_str(), inSubcomponentScope);}
    virtual std::string getName() const override {return submoduleName+"."+paramName;}
};

/**
 * @brief Parameter reference, ident[expr].ident form
 */
class IndexedSubmoduleParameterRef : public UnaryNode
{
  protected:
    std::string submoduleName;
    std::string paramName;
    bool inSubcomponentScope;  // if true, operate on context module's parent
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    IndexedSubmoduleParameterRef(const char *submoduleName, const char *paramName, bool inSubcomponentScope);
    IndexedSubmoduleParameterRef *dup() const override {return new IndexedSubmoduleParameterRef(submoduleName.c_str(), paramName.c_str(), inSubcomponentScope);}
    virtual std::string getName() const override {return submoduleName+"[]."+paramName;}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

/**
 * @brief i,j in NED "for" loops
 */
class LoopVar : public LeafNode
{
  private:
    // the loopvar stack (vars of nested loops are pushed on the stack by cNedNetworkBuilder)
    static const char *varNames[32];
    static long vars[32];
    static int varCount;
  public:
    static long& pushVar(const char *varName);
    static void popVar();
    static void clear();
    static const char **getVarNames() {return varNames;}
    static int getNumVars() {return varCount;}
    static bool contains(const char *name);

  protected:
    std::string varName;
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    LoopVar(const char *varName) {this->varName = varName;}
    LoopVar *dup() const override {return new LoopVar(varName.c_str());}
    virtual std::string getName() const override {return varName;}
};


/**
 * @brief sizeof operator: sizeof(ident), sizeof(this.ident) form
 */
class SizeofGateOrSubmodule : public LeafNode
{
  protected:
    std::string name;
    bool inSubcomponentScope;  // if true, operate on context module's parent
    bool ofThis; // "sizeof(this.ident)"; only when inSubcomponentScope==false
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    SizeofGateOrSubmodule(const char *name, bool inSubcomponentScope, bool ofThis) :
        name(name), inSubcomponentScope(inSubcomponentScope), ofThis(ofThis) {}
    SizeofGateOrSubmodule *dup() const override {return new SizeofGateOrSubmodule(name.c_str(), inSubcomponentScope, ofThis);}
    virtual std::string getName() const override {return std::string("sizeof(")+(ofThis?"this.":"")+name+")";}
};

/**
 * @brief sizeof operator: sizeof(ident.ident) form
 */
class SizeofSubmoduleGate : public LeafNode
{
  protected:
    std::string submoduleName;
    std::string gateName;
    bool inSubcomponentScope;  // if true, operate on context module's parent
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    SizeofSubmoduleGate(const char *submodule, const char *gate, bool inSubcomponentScope) :
        submoduleName(submodule), gateName(gate), inSubcomponentScope(inSubcomponentScope) {}
    SizeofSubmoduleGate *dup() const override {return new SizeofSubmoduleGate(submoduleName.c_str(), gateName.c_str(), inSubcomponentScope);}
    virtual std::string getName() const override {return "sizeof("+submoduleName+"."+gateName+")";}
};

/**
 * @brief sizeof operator: sizeof(ident[expr].ident) form
 */
class SizeofIndexedSubmoduleGate : public UnaryNode
{
  protected:
    std::string submoduleName;
    std::string gateName;
    bool inSubcomponentScope;  // if true, operate on context module's parent
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    SizeofIndexedSubmoduleGate(const char *submodule, const char *gate, bool inSubcomponentScope) :
        submoduleName(submodule), gateName(gate), inSubcomponentScope(inSubcomponentScope) {}
    SizeofIndexedSubmoduleGate *dup() const override {return new SizeofIndexedSubmoduleGate(submoduleName.c_str(), gateName.c_str(), inSubcomponentScope);}
    virtual std::string getName() const override {return "sizeof("+submoduleName+"[]."+gateName+")";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class NedOperatorTranslator : public Expression::AstTranslator
{
  private:
    bool inSubcomponentScope;
    bool inInifile;
  protected:
    typedef Expression::AstNode AstNode;
    ExprNode *translateSizeof(AstNode *astNode, AstTranslator *translatorForChildren);
    ExprNode *translateExists(AstNode *astNode, AstTranslator *translatorForChildren);
    ExprNode *translateIndex(AstNode *astNode, AstTranslator *translatorForChildren);
    ExprNode *translateIdent(AstNode *astNode, AstTranslator *translatorForChildren);
    ExprNode *translateMember(AstNode *astNode, AstTranslator *translatorForChildren);

  public:
    NedOperatorTranslator(bool inSubcomponentScope, bool inInifile) : inSubcomponentScope(inSubcomponentScope), inInifile(inInifile) {}
    virtual ExprNode *translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren) override;
};

class NedFunctionTranslator : public Expression::BasicAstTranslator
{
  protected:
    virtual ExprNode *createNedFunctionNode(cNedFunction *nedFunction, int argCount);
    virtual ExprNode *createMathFunctionNode(cNedMathFunction *nedFunction, int argCount);
  public:
    virtual ExprNode *createFunctionNode(const char *functionName, int argCount) override;
};

}  // namespace nedsupport
}  // namespace omnetpp


#endif


