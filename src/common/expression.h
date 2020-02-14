//==========================================================================
//  EXPRESSION.H  - part of
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

#ifndef __OMNETPP_COMMON_EXPRESSION_H
#define __OMNETPP_COMMON_EXPRESSION_H

#include "commondefs.h"
#include "exprvalue.h"
#include "exprnode.h"
#include "stringpool.h"

namespace omnetpp {
namespace common {

/**
 * @brief Generic expression-evaluator class.
 */
class COMMON_API Expression
{
public:
    typedef omnetpp::common::expression::ExprValue ExprValue;
    typedef omnetpp::common::expression::ExprNode ExprNode;
    typedef omnetpp::common::expression::Context Context;

    /**
     * Node type for the expression AST, an intermediate representation which
     * is the result of parsing and the input to the ExprNode tree generation.
     */
    struct COMMON_API AstNode
    {
        enum Type {UNDEF, CONSTANT, OP, IDENT, IDENT_W_INDEX, FUNCTION, MEMBER, MEMBER_W_INDEX, METHOD, OBJECT, KEYVALUE, ARRAY};
        Type type;
        ExprValue constant;
        std::string name; // of operator, identifier or function
        std::vector<AstNode*> children;

        AstNode() : type(UNDEF) {}
        AstNode(const ExprValue& c) : type(CONSTANT), constant(c) {}
        AstNode(Type type, const char *name) : type(type), name(name) {}
        ~AstNode() {for (AstNode *child : children) delete child;}
        void append(AstNode *node) {children.push_back(node);}
        void prepend(AstNode *node) {children.insert(children.begin(), node);}
        std::string str() const;
        static const char *typeName(Type type);
    };

    /**
     * Translates AST to expression evaluation tree. translateToExpressionTree()
     * should translate the given AST into expression tree if it is capable of doing so,
     * otherwise it should return nullptr to give other translators a chance.
     */
    class COMMON_API AstTranslator {
    protected:
        virtual ExprNode *translateChild(AstNode *astChild, AstTranslator *translatorForChildren); // utility
        virtual void translateChildren(AstNode *astNode, ExprNode *node, AstTranslator *translatorForChildren); // utility
    public:
        virtual ~AstTranslator() {}
        virtual ExprNode *translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren) = 0; // return nullptr of not applicable
    };

    /**
     * Try multiple translators until one succeeds. If none does, return nullptr.
     */
    class COMMON_API MultiAstTranslator : public AstTranslator {
    protected:
        std::vector<AstTranslator*> translators;
    public:
        MultiAstTranslator() {}
        MultiAstTranslator(const std::vector<AstTranslator*>& translators) : translators(translators) {}
        virtual void setTranslators(const std::vector<AstTranslator*>& list) {translators=list;}
        virtual void addTranslator(AstTranslator *translator) {translators.push_back(translator);}
        virtual const std::vector<AstTranslator*>& getTranslators() {return translators;}
        virtual ExprNode *translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren);
    };

    /**
     * Practical base class for translators. translateToExpressionTree() does the main job, and
     * relies on factory methods. Factory methods should return nullptr if the name cannot be resolved,
     * to give other resolvers a chance. Methods should throw an exception with a human-readable
     * description when the function or variable was recognized, but it is used incorrectly
     * (e.g. with the wrong number or type of parameters).
     */
    class COMMON_API BasicAstTranslator : public AstTranslator {
    public:
        virtual ExprNode *translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren);
    protected:
        // override any that applies:
        virtual ExprNode *createIdentNode(const char *varName, bool withIndex) {return nullptr;}
        virtual ExprNode *createMemberNode(const char *varName, bool withIndex) {return nullptr;}
        virtual ExprNode *createFunctionNode(const char *functionName, int argCount) {return nullptr;}
        virtual ExprNode *createMethodNode(const char *functionName, int argCount) {return nullptr;}
        virtual ExprNode *createOperatorNode(const char *opName, int argCount) {return nullptr;}
        virtual ExprNode *createObjectNode(const char *typeName, const std::vector<std::string>& keys) {return nullptr;}
        virtual ExprNode *createArrayNode(int argCount) {return nullptr;}
    };

    /**
     * Spaciousness values for str().
     */
    enum {
        SPACIOUSNESS_MIN = ExprNode::SPACIOUSNESS_MIN,
        SPACIOUSNESS_DEFAULT = ExprNode::SPACIOUSNESS_DEFAULT,
        SPACIOUSNESS_MAX = ExprNode::SPACIOUSNESS_MAX
    };

  protected:
    ExprNode *tree = nullptr;
    static MultiAstTranslator defaultTranslator;

  protected:
    void copy(const Expression& other);
    virtual bool findFoldableSubtrees(ExprNode *tree, std::vector<ExprNode*>& foldableSubtrees) const;
    virtual ExprNode *foldSubtrees(ExprNode *tree, const std::vector<ExprNode*>& foldableSubtrees) const;
    virtual bool isFoldableNode(ExprNode *node) const;
    static void errorCouldNotTranslate(AstNode *astNode);

  public:
    /**
     * Install global default translators.
     */
    static void installAstTranslator(AstTranslator *translator) {defaultTranslator.addTranslator(translator);}
    static const std::vector<AstTranslator*>& getInstalledAstTranslators() {return defaultTranslator.getTranslators();}
    static MultiAstTranslator *getDefaultAstTranslator() {return &defaultTranslator;}

    /**
     * Constructor.
     */
    Expression() {}
    Expression(const Expression& other) {copy(other);}
    virtual ~Expression() {delete tree;}
    Expression& operator=(const Expression& other);

    /**
     * Returns the string form of the expression, as assembled by the ExprNode expression tree.
     */
    virtual std::string str() const {return tree->str();}
    virtual std::string str(int spaciousness) const {return tree->str(spaciousness);}

    /**
     * Interprets the string as an expression, and stores it. The translator can be used
     * to customize how the expression tree is built from the AST, e.g. how variables
     * and functions are translated.
     *
     * If no translator is given, it uses the global default translator.
     */
    virtual Expression& parse(const char *text, AstTranslator *translator=nullptr);

    /**
     * Returns true if the expression is empty. An empty expression cannot be evaluated.
     */
    bool isEmpty() const {return tree != nullptr;}

    /**
     * Returns true if the expression consists of a constant, or can be transparently
     * replaced by a constant.
     */
    virtual bool isAConstant() const;

    /**
     * Evaluate the expression, and return the results as a ExprValue.
     * Throws an exception if there is an error during evaluation.
     */
    virtual ExprValue evaluate(Context* context = nullptr) const;

    /**
     * Evaluate the expression and convert the result to bool if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual bool boolValue(Context *context=nullptr) const;

    /**
     * Evaluate the expression and convert the result to intval_t if possible;
     * throw an error if conversion from that type is not supported, or
     * the value is out of the range of intval_t.
     */
    virtual intval_t intValue(const char *expectedUnit=nullptr) const {return intValue(nullptr, expectedUnit);}
    virtual intval_t intValue(Context *context, const char *expectedUnit=nullptr) const;

    /**
     * Evaluate the expression and convert the result to double if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual double doubleValue(const char *expectedUnit=nullptr) const {return doubleValue(nullptr, expectedUnit);}
    virtual double doubleValue(Context *context, const char *expectedUnit=nullptr) const;

    /**
     * Evaluate the expression and convert the result to string if possible;
     * throw an error if conversion from that type is not supported.
     */
    virtual std::string stringValue(Context *context=nullptr) const;

    // Advanced use and debugging:

    // direct access to the expression evaluator tree
    virtual void setExpressionTree(ExprNode *exprTree);
    virtual const ExprNode *getExpressionTree() const {return tree;}
    virtual ExprNode *removeExpressionTree() {ExprNode *result = tree; tree = nullptr; return result;}

    // various stages of the expression parsing and translation, as utility functions
    virtual AstNode *parseToAst(const char *text) const;
    virtual ExprNode *translateToExpressionTree(AstNode *astTree, AstTranslator *translator) const;
    virtual ExprNode *performConstantFolding(ExprNode *tree) const;
    virtual ExprNode *parseAndTranslate(const char *text, AstTranslator *translator=nullptr) const;


    // print expression tree and AST as tree
    virtual void dumpTree(std::ostream& out=std::cout) const;
    virtual void dumpAst(AstNode *node, std::ostream& out=std::cout, int indentLevel=0) const;
    virtual void dumpExprTree(ExprNode *node, std::ostream& out=std::cout, int indentLevel=0) const;
};


}  // namespace common
}  // namespace omnetpp


#endif


