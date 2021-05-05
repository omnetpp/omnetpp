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
#include "omnetpp/cnedmathfunction.h"
#include "common/expression.h"
#include "common/exprnodes.h"

using namespace omnetpp::common;
using namespace omnetpp::common::expression;

namespace omnetpp {

class cNedFunction;
class cValueMap;

namespace nedsupport {

using omnetpp::common::expression::Context;

cValue makeNedValue(const ExprValue& value);
ExprValue makeExprValue(const cValue& value);
ExprValue makeExprValue(const cPar& par);
cValue *makeNedValues(cValue *&buffer, const ExprValue argv[], int argc);

class NedExpressionContext : public cExpression::Context
{
  public:
    enum ExpressionRole { SUBMODULE_CONDITION, SUBMODULE_ARRAY_CONDITION, OTHER };
    explicit NedExpressionContext(cComponent *component, const char *baseDirectory, ExpressionRole role, const char *computedTypename) :
            cExpression::Context(component, baseDirectory), role(role), computedTypename(computedTypename) {}
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
    virtual std::string str() const override {return getName() + "()";}
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
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    Exists(const char *ident) : name(ident) {}
    Exists *dup() const override {return new Exists(name.c_str());}
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
    bool ofThis; // if true, "this.ident" form
    bool ofParent; // if true, "parent.ident" form
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    ParameterRef(const char *paramName, bool ofThis, bool ofParent) : paramName(paramName), ofThis(ofThis), ofParent(ofParent) {}
    ParameterRef *dup() const override {return new ParameterRef(paramName.c_str(), ofThis, ofParent);}
    virtual std::string getName() const override {return std::string(ofThis ? "this." : ofParent ? "parent." : "") + paramName;}
};

/**
 * @brief Parameter reference, ident.ident form
 */
class SubmoduleParameterRef : public LeafNode
{
  protected:
    std::string submoduleName;
    std::string paramName;
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    SubmoduleParameterRef(const char *submoduleName, const char *paramName) : submoduleName(submoduleName), paramName(paramName) {}
    SubmoduleParameterRef *dup() const override {return new SubmoduleParameterRef(submoduleName.c_str(), paramName.c_str());}
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
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    IndexedSubmoduleParameterRef(const char *submoduleName, const char *paramName);
    IndexedSubmoduleParameterRef *dup() const override {return new IndexedSubmoduleParameterRef(submoduleName.c_str(), paramName.c_str());}
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
 * @brief sizeof operator: sizeof(ident), sizeof(this), sizeof(parent),
 * sizeof(this.ident), sizeof(parent.ident) forms.
 */
class SizeofGateOrSubmodule : public LeafNode
{
  protected:
    std::string name1;
    std::string name2;  // may be empty
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
    // helpers:
    virtual cModule *parentModule(cExpression::Context *context) const;
    virtual cModule *contextAsModule(cExpression::Context *context) const;
    virtual cModule *submodule(cModule *module, const char *name) const;
    virtual intval_t gateOrSubmoduleSize(cModule *module, const char *name) const;
  public:
    SizeofGateOrSubmodule(const char *name1, const char *name2) : name1(name1), name2(opp_nulltoempty(name2)) {}
    SizeofGateOrSubmodule *dup() const override {return new SizeofGateOrSubmodule(name1.c_str(), name2.c_str());}
    virtual std::string getName() const override {return std::string("sizeof(") + (name2.empty() ? name1 : (name1 + "." + name2)) + ")";}
};

/**
 * @brief sizeof operator: sizeof(ident[expr].ident) form
 */
class SizeofIndexedSubmoduleGate : public UnaryNode
{
  protected:
    std::string submoduleName;
    std::string gateName;
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    SizeofIndexedSubmoduleGate(const char *submodule, const char *gate) :
        submoduleName(submodule), gateName(gate) {}
    SizeofIndexedSubmoduleGate *dup() const override {return new SizeofIndexedSubmoduleGate(submoduleName.c_str(), gateName.c_str());}
    virtual std::string getName() const override {return "sizeof("+submoduleName+"[]."+gateName+")";}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

class NedObjectNode : public ObjectNode
{
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    // helpers for evaluate():
    virtual void setField(cClassDescriptor *desc, void *object, const char *fieldName, const cValue& value) const;
    virtual void setFieldElement(cClassDescriptor *desc, void *object, const char *fieldName, int fieldIndex, int arrayIndex, const cValue& value) const;
    virtual void fillObject(cClassDescriptor *desc, void *object, const cValueMap *map) const;
  public:
    NedObjectNode(const char *typeName, const std::vector<std::string>& fieldNames) : ObjectNode(typeName, fieldNames) {}
    NedObjectNode *dup() const override {return new NedObjectNode(typeName.c_str(), fieldNames);}
};

class NedArrayNode : public ArrayNode
{
  protected:
    virtual ExprValue evaluate(Context *context) const override;
  public:
    NedArrayNode() {}
    NedArrayNode *dup() const override {return new NedArrayNode();}
};

class NedOperatorTranslator : public Expression::AstTranslator
{
  protected:
    typedef Expression::AstNode AstNode;
    ExprNode *translateSizeof(AstNode *astNode, AstTranslator *translatorForChildren);
    ExprNode *translateExists(AstNode *astNode, AstTranslator *translatorForChildren);
    ExprNode *translateIndex(AstNode *astNode, AstTranslator *translatorForChildren);
    ExprNode *translateIdent(AstNode *astNode, AstTranslator *translatorForChildren);
    ExprNode *translateMember(AstNode *astNode, AstTranslator *translatorForChildren);

  public:
    NedOperatorTranslator() {}
    virtual ExprNode *translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren) override;
};

class NedFunctionTranslator : public Expression::BasicAstTranslator
{
  protected:
    virtual ExprNode *createNedFunctionNode(cNedFunction *nedFunction, int argCount);
    virtual ExprNode *createMathFunctionNode(cNedMathFunction *nedFunction, int argCount);
  public:
    virtual ExprNode *createFunctionNode(const char *functionName, int argCount) override;
    virtual ExprNode *createObjectNode(const char *typeName, const std::vector<std::string>& fieldNames) override;
    virtual ExprNode *createArrayNode(int argCount) override;
};

}  // namespace nedsupport
}  // namespace omnetpp


#endif


