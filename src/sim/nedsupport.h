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
#include "omnetpp/any_ptr.h"
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

enum IdentSyntax { UNKNOWN, QUALIFIER, OPTQUALIFIER_NAME1, OPTQUALIFIER_INDEXEDNAME1, OPTQUALIFIER_NAME1_DOT_NAME2, OPTQUALIFIER_INDEXEDNAME1_DOT_NAME2 };

enum IdentQualifier { NONE, THIS, PARENT };

inline std::string qualifierToPrefix(IdentQualifier q) {return q==THIS ? "this." : q==PARENT ? "parent." : "";}

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
  public:
    NedFunctionNode(cNedFunction *f) : nedFunction(f) {}
    NedFunctionNode *dup() const override {return new NedFunctionNode(nedFunction);}
    virtual Precedence getPrecedence() const override {return ELEM;}
    virtual std::string getName() const override;
    virtual std::string str() const override {return getName() + "()";}
};

class Index : public LeafNode
{
  protected:
    IdentQualifier qualifier;
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    Index(IdentQualifier qualifier) : qualifier(qualifier) {}
    Index *dup() const override {return new Index(qualifier);}
    virtual std::string getName() const override {return qualifierToPrefix(qualifier) + "index";}
    virtual std::string str() const override {return asPrinted(true);}
};

class Exists : public NaryNode // zero or one child
{
  protected:
    IdentSyntax syntax;
    IdentQualifier qualifier;
    std::string name1, name2;

  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
    int getIndex(Context *context) const;
  public:
    Exists(IdentSyntax syntax, IdentQualifier qualifier, std::string name1, std::string name2) :
        syntax(syntax), qualifier(qualifier), name1(name1), name2(name2) {}
    Exists *dup() const override {return new Exists(syntax, qualifier, name1, name2);}
    virtual Precedence getPrecedence() const override {return ELEM;}
    virtual std::string getName() const override {return asPrinted(true);} //FIXME
    virtual std::string str() const override {return asPrinted(true);}
};

class Typename : public LeafNode
{
  protected:
    IdentQualifier qualifier;
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    Typename(IdentQualifier qualifier) : qualifier(qualifier) {}
    Typename *dup() const override {return new Typename(qualifier);}
    virtual std::string getName() const override {return qualifierToPrefix(qualifier) + "typename";}
    virtual std::string str() const override {return asPrinted(true);}
};

/**
 * @brief Parameter reference, ident and this.ident forms
 */
class Parameter : public LeafNode
{
  protected:
    IdentQualifier qualifier;
    std::string paramName;
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual std::string makeErrorMessage(std::exception& e) const override {return e.what();} // don't prepend parameter name, as msg already contains it
  public:
    Parameter(IdentQualifier qualifier, const char *paramName) : qualifier(qualifier), paramName(paramName) {}
    Parameter *dup() const override {return new Parameter(qualifier, paramName.c_str());}
    virtual std::string getName() const override {return qualifierToPrefix(qualifier) + paramName;}
    virtual std::string str() const override {return asPrinted(true);}
};

/**
 * @brief Parameter reference, ident.ident form
 */
class SubmoduleParameter : public LeafNode
{
  protected:
    IdentQualifier qualifier;
    std::string submoduleName;
    std::string paramName;
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    SubmoduleParameter(IdentQualifier qualifier, const char *submoduleName, const char *paramName) : qualifier(qualifier), submoduleName(submoduleName), paramName(paramName) {}
    SubmoduleParameter *dup() const override {return new SubmoduleParameter(qualifier, submoduleName.c_str(), paramName.c_str());}
    virtual std::string getName() const override {return qualifierToPrefix(qualifier) + submoduleName + "." + paramName;}
    virtual std::string str() const override {return asPrinted(true);}
};

/**
 * @brief Parameter reference, ident[expr].ident form
 */
class IndexedSubmoduleParameter : public UnaryNode
{
  protected:
    IdentQualifier qualifier;
    std::string submoduleName;
    std::string paramName;
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    IndexedSubmoduleParameter(IdentQualifier qualifier, const char *submoduleName, const char *paramName) : qualifier(qualifier), submoduleName(submoduleName), paramName(paramName) {}
    IndexedSubmoduleParameter *dup() const override {return new IndexedSubmoduleParameter(qualifier, submoduleName.c_str(), paramName.c_str());}
    virtual std::string getName() const override {return qualifierToPrefix(qualifier) + submoduleName + "[*]." + paramName;}
    virtual std::string str() const override {return asPrinted(true);}
    virtual Precedence getPrecedence() const override {return ELEM;}
};

/**
 * @brief sizeof operator: sizeof(this), sizeof(parent), sizeof(ident), sizeof(this.ident), sizeof(parent.ident) forms
 */
class Sizeof : public LeafNode
{
  protected:
    IdentQualifier qualifier;
    std::string optName1; // may be empty
    std::string optName2;  // may be empty
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
    virtual intval_t gateOrSubmoduleSize(cModule *module, const char *name) const;
  public:
    Sizeof(IdentQualifier qualifier, const char *name1=nullptr, const char *name2=nullptr) : qualifier(qualifier), optName1(opp_nulltoempty(name1)), optName2(opp_nulltoempty(name2)) {}
    Sizeof *dup() const override {return new Sizeof(qualifier, optName1.c_str(), optName2.c_str());}
    virtual std::string getName() const override {return "sizeof(" + qualifierToPrefix(qualifier) + opp_join(".", optName1.c_str(), optName2.c_str()) + ")";}
    virtual std::string str() const override {return asPrinted(true);}
};

/**
 * @brief sizeof operator: sizeof(ident[expr].ident) form
 */
class SizeofIndexedSubmoduleGate : public UnaryNode
{
  protected:
    IdentQualifier qualifier;
    std::string submoduleName;
    std::string gateName;
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    SizeofIndexedSubmoduleGate(IdentQualifier qualifier, const char *submodule, const char *gate) : qualifier(qualifier), submoduleName(submodule), gateName(gate) {}
    SizeofIndexedSubmoduleGate *dup() const override {return new SizeofIndexedSubmoduleGate(qualifier, submoduleName.c_str(), gateName.c_str());}
    virtual std::string getName() const override {return "sizeof(" + qualifierToPrefix(qualifier) + submoduleName + "[*]." + gateName + ")";}
    virtual std::string str() const override {return asPrinted(true);}
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
    virtual std::string str() const override {return asPrinted(true);}
};

/**
 * @brief The expr() function
 */
class DynExpr : public LeafNode
{
  protected:
    std::string exprText;
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    virtual void print(std::ostream& out, int spaciousness) const override;
  public:
    DynExpr(const char *exprText) : exprText(exprText) {}
    DynExpr *dup() const override {return new DynExpr(exprText.c_str());}
    virtual std::string getName() const override {return "expr(...)";}
    virtual std::string str() const override {return "expr(" + exprText + ")";}
};

class NedObjectNode : public ObjectNode
{
  protected:
    virtual ExprValue evaluate(Context *context) const override;
    // helpers for evaluate():
    virtual void setField(cClassDescriptor *desc, any_ptr object, const char *fieldName, const cValue& value) const;
    virtual void setFieldElement(cClassDescriptor *desc, any_ptr object, const char *fieldName, int fieldIndex, int arrayIndex, const cValue& value) const;
    virtual void fillObject(cClassDescriptor *desc, any_ptr object, const cValueMap *map) const;
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
    ExprNode *translateTypename(AstNode *astNode, AstTranslator *translatorForChildren);
    ExprNode *translateParameter(AstNode *astNode, AstTranslator *translatorForChildren);
    ExprNode *translateExpr(AstNode *astNode, AstTranslator *translatorForChildren);
    IdentSyntax matchSyntax(AstNode *astNode, IdentQualifier& qualifier, std::string& name1, AstNode *& index, std::string& name2);
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
    virtual ExprNode *createMethodNode(const char *functionName, int argCount) override;
    virtual ExprNode *createObjectNode(const char *typeName, const std::vector<std::string>& fieldNames) override;
    virtual ExprNode *createArrayNode(int argCount) override;
};

}  // namespace nedsupport
}  // namespace omnetpp


#endif


