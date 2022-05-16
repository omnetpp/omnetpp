//==========================================================================
//  EXPRESSION.CC  - part of
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
#include <limits>
#include <stack>
#include <memory>
#include <typeinfo>
#include <cinttypes>
#include "commonutil.h"
#include "stlutil.h"
#include "expression.h"
#include "exprnodes.h"
#include "unitconversion.h"

using namespace std;
using namespace omnetpp::common::expression;

namespace omnetpp {
namespace common {

typedef Expression::AstNode AstNode;

std::string Expression::AstNode::str() const
{
    // note: the result of str() may appear in user-facing messages, e.g. exceptions
    std::stringstream out;
    out << typeName(type);
    if (type == CONSTANT)
        out << " " << constant.str();
    else if (type != UNDEF && !name.empty())
        out << " '" << name << "'";  // OP, IDENT, etc all have names
    return out.str();
}

const char *Expression::AstNode::typeName(Type type)
{
    switch (type) {
        case AstNode::CONSTANT: return "constant";
        case AstNode::OP: return "operator";
        case AstNode::FUNCTION: return "function";
        case AstNode::METHOD: return "method";
        case AstNode::IDENT: return "identifier";
        case AstNode::IDENT_W_INDEX: return "indexed identifier";
        case AstNode::MEMBER: return "member";
        case AstNode::MEMBER_W_INDEX: return "indexed member";
        case AstNode::OBJECT: return "object";
        case AstNode::ARRAY: return "array";
        case AstNode::KEYVALUE: return "key-value";
        case AstNode::UNDEF: return "undefined";
        default: throw opp_runtime_error("Unrecognized enum value %d", type);
    }
}

static std::string unparseList(std::vector<AstNode*> children, int startIndex=0)
{
    std::stringstream out;
    for (int i = startIndex; i < children.size(); i++) {
        if (i != startIndex) out << ",";
        out << children[i]->unparse();
    }
    return out.str();
}

std::string Expression::AstNode::unparse() const
{
    // Note: This code is oblivious of operator precedence, it solely relies on the "parenthesized" flag
    // in each AST node to insert parens where needed. If those flags are not set correctly (i.e. not set
    // where needed), the unparsed expression may have a different meaning from the original!
    std::string result;
    switch (type) {
        case AstNode::CONSTANT: result = constant.str(); break;
        case AstNode::OP:
            if (children.size()==1 && name == "_!") result = children.at(0)->unparse() + "!";
            else if (children.size()==1) result = name + children.at(0)->unparse();
            else if (children.size()==2) result = children.at(0)->unparse() + name + children.at(1)->unparse();
            else if (children.size()==3 && name == "?:") result = children.at(0)->unparse() + " ? " + children.at(1)->unparse() + " : " + children.at(2)->unparse(); // note: spaces around ":" are necessary, because "foo:bar" parses as a NAME
            else throw opp_runtime_error("Error unparsing operator %s", name.c_str());
            break;
        case AstNode::FUNCTION: result = name + "(" + unparseList(children) + ")"; break;
        case AstNode::METHOD: result = children.at(0)->unparse() + "." + name + "(" + unparseList(children,1) + ")"; break;
        case AstNode::IDENT: result = name; break;
        case AstNode::IDENT_W_INDEX: result = name + "[" + children.at(0)->unparse() + "]"; break;
        case AstNode::MEMBER: result = children.at(0)->unparse() + "." + name; break;
        case AstNode::MEMBER_W_INDEX: result = children.at(0)->unparse() + "." + name + "[" + children.at(1)->unparse() + "]"; break;
        case AstNode::OBJECT: result = name + "{" + unparseList(children) + "}"; break;
        case AstNode::ARRAY: result = "[" + unparseList(children) + "]"; break;
        case AstNode::KEYVALUE: result = name + ": " + children.at(0)->unparse(); break;
        case AstNode::UNDEF: result = "undefined"; break;
        default: throw opp_runtime_error("Unrecognized enum value %d", type);
    }
    return parenthesized ? "(" + result + ")" : result;
}

void Expression::copy(const Expression& other)
{
    delete tree;
    tree = other.tree->dupTree();
}

Expression& Expression::operator=(const Expression& other)
{
    if (this == &other)
        return *this;
    copy(other);
    return *this;
}

std::string Expression::str(int spaciousness) const
{
    std::stringstream out;
    tree->print(out, spaciousness);
    return out.str();
}

Expression& Expression::parse(const char *expr, AstTranslator *translator)
{
    setExpressionTree(parseAndTranslate(expr, translator));
    return *this;
}

ExprNode *Expression::parseAndTranslate(const char *expr, AstTranslator *translator) const
{
    if (!translator)
        translator = &defaultTranslator;
    std::unique_ptr<AstNode> astTree(parseToAst(expr));
    ExprNode *exprTree = translateToExpressionTree(astTree.get(), translator);
    exprTree = performConstantFolding(exprTree);
    return exprTree;
}

void Expression::setExpressionTree(ExprNode* exprTree)
{
    Assert(exprTree);
    if (tree)
        delete tree;
    tree = exprTree;
}

void Expression::dumpAst(AstNode *node, std::ostream& out, int indentLevel) const
{
    out << std::string(4*indentLevel, ' ' ) << node->str() << std::endl;
    for (AstNode *child : node->children)
        dumpAst(child, out, indentLevel+1);
}

void Expression::dumpExprTree(ExprNode *node, std::ostream& out, int indentLevel) const
{
    out << std::string(4*indentLevel, ' ' ) << node->getName() << " ("<< opp_typename(typeid(*node)) << ")\n";
    for (ExprNode *child : node->getChildren())
        dumpExprTree(child, out, indentLevel+1);
}

void Expression::dumpTree(std::ostream& out) const
{
    out << "'" << str() << "' --> {\n";
    dumpExprTree(tree, out, 1);
    out << "}\n";
}

bool Expression::isAConstant() const
{
    return tree != nullptr && dynamic_cast<ConstantNode*>(tree) != nullptr;
}

ExprNode *Expression::performConstantFolding(ExprNode *tree) const
{
    std::vector<ExprNode*> foldableSubtrees;
    findFoldableSubtrees(tree, foldableSubtrees);
    return foldSubtrees(tree, foldableSubtrees);
}

ExprNode *Expression::foldSubtrees(ExprNode *tree, const std::vector<ExprNode*>& foldableSubtrees) const
{
    if (dynamic_cast<LeafNode*>(tree))
        return tree; // nothing to see here

    if (contains(foldableSubtrees, tree)) {
        try {
            // fold node if we can
            ExprValue constant = tree->tryEvaluate(nullptr);
            delete tree;
            return new ConstantNode(constant);
        }
        catch (std::exception& e) {
            // ignore, go on folding child nodes
        }
    }

    // replace child nodes with their folded versions
    bool changed = false;
    std::vector<ExprNode*> children = tree->getChildren();
    for (ExprNode *&childPtrRef : children) {
        ExprNode *foldedChild = foldSubtrees(childPtrRef, foldableSubtrees);
        if (childPtrRef != foldedChild) {
            childPtrRef = foldedChild; // replace in vector
            changed = true;
        }
    }
    if (changed)
        tree->setChildren(children);
    return tree;

}

bool Expression::findFoldableSubtrees(ExprNode *tree, std::vector<ExprNode*>& result) const
{
    bool childrenFoldable = true;
    for (ExprNode *child : tree->getChildren())
        if (findFoldableSubtrees(child, result) == false)
            childrenFoldable = false;
    if (childrenFoldable && isFoldableNode(tree)) {
        result.push_back(tree);
        return true;
    }
    return false;
}

bool Expression::isFoldableNode(ExprNode *node) const
{
    return
            dynamic_cast<ConstantNode*>(node) != nullptr ||
            dynamic_cast<UnaryOperatorNode*>(node) != nullptr ||
            dynamic_cast<BinaryOperatorNode*>(node) != nullptr ||
            dynamic_cast<TernaryOperatorNode*>(node) != nullptr ||
            dynamic_cast<DoubleCastNode*>(node) != nullptr ||
            dynamic_cast<IntCastNode*>(node) != nullptr ||
            (ExprNodeFactory::supportsStdMathFunction(node->getName().c_str()) &&
                    (dynamic_cast<MathFunc1Node*>(node) || dynamic_cast<MathFunc2Node*>(node) || dynamic_cast<MathFunc3Node*>(node))
            );
}

void Expression::errorCouldNotTranslate(AstNode *astNode)
{
    if (astNode->type == AstNode::FUNCTION)
        throw opp_runtime_error("A '%s' function that accepts %d argument(s) was not found", astNode->name.c_str(), (int)astNode->children.size());
    else if (astNode->type == AstNode::METHOD)
        throw opp_runtime_error("A '%s' method accepting %d argument(s) was not found", astNode->name.c_str(), (int)astNode->children.size() - 1);
    else
        throw opp_runtime_error("Unknown %s '%s'", AstNode::typeName(astNode->type), astNode->name.c_str());
}

ExprNode *Expression::translateToExpressionTree(AstNode *ast, AstTranslator *translator) const
{
    ExprNode *tree = translator->translateToExpressionTree(ast, translator);
    if (!tree)
        errorCouldNotTranslate(ast);
    return tree;
}

ExprValue Expression::evaluate(Context *context) const
{
    if (!tree)
        throw opp_runtime_error("Cannot evaluate empty expression");
    Context tmp;
    if (!context)
        context = &tmp;
    context->expression = this;
    return tree->tryEvaluate(context);
}

ExprValue::Type Expression::evaluateForType(Context* context) const
{
    ExprValue v = evaluate(context);  // note: if result is a dynamically allocated object (i.e. with type=POINTER), it is leaked here!
    return v.getType();
}

bool Expression::boolValue(Context *context) const
{
    ExprValue v = evaluate(context);
    return v.boolValue();
}

intval_t Expression::intValue(Context *context, const char *expectedUnit) const
{
    ExprValue v = evaluate(context);
    return expectedUnit == nullptr ? v.intValue() : (intval_t)v.doubleValueInUnit(expectedUnit);
}

double Expression::doubleValue(Context *context, const char *expectedUnit) const
{
    ExprValue v = evaluate(context);
    return expectedUnit == nullptr ? v.doubleValue() : v.doubleValueInUnit(expectedUnit);
}

std::string Expression::stringValue(Context *context) const
{
    ExprValue v = evaluate(context);
    return v.stringValue();
}

typedef Expression::AstTranslator AstTranslator;

ExprNode *Expression::AstTranslator::translateChild(AstNode *astChild, AstTranslator *translatorForChildren)
{
    ExprNode *child = translatorForChildren->translateToExpressionTree(astChild, translatorForChildren);
    if (!child)
        errorCouldNotTranslate(astChild);
    return child;
}

inline AstNode *skipKeyValue(AstNode *astNode)
{
    // in the OBJECT syntax, values are nested under KEYVALUE nodes that need to be left out
    return astNode->type == AstNode::KEYVALUE ? astNode->children[0] : astNode;
}

void Expression::AstTranslator::translateChildren(AstNode *astNode, ExprNode *node, AstTranslator *translatorForChildren)
{
    std::vector<ExprNode*> children;
    try {
        children.reserve(astNode->children.size());
        for (AstNode *astChild : astNode->children)
            children.push_back(translateChild(skipKeyValue(astChild), translatorForChildren));
        node->setChildren(children);
    }
    catch (std::exception& e) {
        delete node;
        for (ExprNode *child : children)
            delete child;
        throw;
    }
}

ExprNode *Expression::MultiAstTranslator::translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren)
{
    for (AstTranslator *translator : translators)
        if (ExprNode *node = translator->translateToExpressionTree(astNode, translatorForChildren))
            return node;
    return nullptr;
}

ExprNode *Expression::BasicAstTranslator::translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren)
{
    switch (astNode->type) {
    case AstNode::CONSTANT:
        return new ConstantNode(astNode->constant);
    case AstNode::OP: {
        const char *name = astNode->name.c_str();
        int argCount = astNode->children.size();
        ExprNode *node = createOperatorNode(name, argCount);
        if (node)
            translateChildren(astNode, node, translatorForChildren);
        return node;
    }
    case AstNode::FUNCTION: {
        const char *name = astNode->name.c_str();
        int argCount = astNode->children.size();
        ExprNode *node = createFunctionNode(name, argCount);
        if (node)
            translateChildren(astNode, node, translatorForChildren);
        return node;
    }
    case AstNode::METHOD: {
        const char *name = astNode->name.c_str();
        Assert(!astNode->children.empty());
        int argCount = astNode->children.size()-1;
        ExprNode *node = createMethodNode(name, argCount);
        if (node)
            translateChildren(astNode, node, translatorForChildren);
        return node;
    }
    case AstNode::IDENT:
    case AstNode::IDENT_W_INDEX: {
        const char *name = astNode->name.c_str();
        bool withIndex = astNode->type == AstNode::IDENT_W_INDEX;
        Assert(astNode->children.size() == (withIndex ? 1 : 0));
        ExprNode *node = createIdentNode(name, withIndex);
        if (node)
            translateChildren(astNode, node, translatorForChildren);
        return node;
    }
    case AstNode::MEMBER:
    case AstNode::MEMBER_W_INDEX: {
        const char *name = astNode->name.c_str();
        bool withIndex = astNode->type == AstNode::MEMBER_W_INDEX;
        Assert(astNode->children.size() == (withIndex ? 2 : 1));
        ExprNode *node = createMemberNode(name, withIndex);
        if (node)
            translateChildren(astNode, node, translatorForChildren);
        return node;
    }
    case AstNode::OBJECT: {
        const char *name = astNode->name.c_str();
        std::vector<std::string> keys;
        for (AstNode *child : astNode->children) {
            if (child->type != AstNode::KEYVALUE)
                throw opp_runtime_error("Expression AST: unexpected node type under OBJECT, must be KEYVALUE");
            keys.push_back(child->name);
        }
        ExprNode *node = createObjectNode(name, keys);
        if (node)
            translateChildren(astNode, node, translatorForChildren);
        return node;
    }
    case AstNode::ARRAY: {
        int argCount = astNode->children.size();
        ExprNode *node = createArrayNode(argCount);
        if (node)
            translateChildren(astNode, node, translatorForChildren);
        return node;
    }
    case AstNode::KEYVALUE: // translated as part of OBJECT
    default: throw opp_runtime_error("Unknown element type");
    }
}

class OperatorAstTranslator : public Expression::BasicAstTranslator
{
  public:
    virtual ExprNode *createOperatorNode(const char *opName, int argCount) override;
};

class StdMathAstTranslator : public Expression::BasicAstTranslator
{
  public:
    virtual ExprNode *createFunctionNode(const char *functionName, int argCount) override;
};

class UnitConversionAstTranslator : public Expression::BasicAstTranslator
{
  public:
    virtual ExprNode *createFunctionNode(const char *functionName, int argCount) override;
};

class UnresolvedNameAstTranslator : public Expression::BasicAstTranslator
{
  public:
    UnresolvedNameAstTranslator() {}
    virtual ExprNode *createIdentNode(const char *varName, bool withIndex) override {return withIndex ? (ExprNode *)new DynamicallyResolvedIndexedVariableNode(varName) : (ExprNode *)new DynamicallyResolvedVariableNode(varName);}
    virtual ExprNode *createMemberNode(const char *varName, bool withIndex) override {return withIndex ? (ExprNode *)new DynamicallyResolvedIndexedMemberNode(varName) : (ExprNode *)new DynamicallyResolvedMemberNode(varName);}
    virtual ExprNode *createFunctionNode(const char *functionName, int argCount) override {return new DynamicallyResolvedFunctionNode(functionName);}
    virtual ExprNode *createMethodNode(const char *functionName, int argCount) override {return new DynamicallyResolvedMethodNode(functionName);}
};

ExprNode *OperatorAstTranslator::createOperatorNode(const char *opName, int argCount)
{
    switch (argCount) {
    case 1: return ExprNodeFactory::createUnaryOperator(opName);
    case 2: return ExprNodeFactory::createBinaryOperator(opName);
    case 3: return ExprNodeFactory::createTernaryOperator(opName);
    default: return nullptr;
    }
}

ExprNode *StdMathAstTranslator::createFunctionNode(const char *functionName, int argCount)
{
    if (ExprNodeFactory::supportsStdMathFunction(functionName)) {
        ExprNode *node = ExprNodeFactory::createStdMathFunction(functionName);
        int expectedArgCount = -1;
        if (dynamic_cast<LeafNode*>(node))
            expectedArgCount = 0;
        else if (dynamic_cast<UnaryNode*>(node))
            expectedArgCount = 1;
        else if (dynamic_cast<BinaryNode*>(node))
            expectedArgCount = 2;
        else if (dynamic_cast<TernaryNode*>(node))
            expectedArgCount = 3;
        if (expectedArgCount != -1 && argCount != expectedArgCount)
            throw opp_runtime_error("Function '%s' invoked with wrong number of arguments (%d instead of %d)", functionName, argCount, expectedArgCount);
        return node;
    }
    return nullptr;
}

ExprNode *UnitConversionAstTranslator::createFunctionNode(const char *functionName, int argCount)
{
    if (argCount == 1) {
        if (UnitConversion::isUnit(functionName))
            return new UnitConversionNode(functionName);
        if (strchr(functionName, '_')) { // some unit names contain hyphen or slash, the corresponding functions use underscore and "_per_"
            std::string unitName = opp_replacesubstring(opp_replacesubstring(functionName, "_per_", "/", true), "_", "-", true);
            if (UnitConversion::isUnit(unitName.c_str()))
                return new UnitConversionNode(unitName.c_str());
        }
    }
    return nullptr;
}

static OperatorAstTranslator operatorAstTranslator;
static StdMathAstTranslator stdMathAstTranslator;
static UnitConversionAstTranslator unitConversionAstTranslator;
static UnresolvedNameAstTranslator unresolvedNameAstTranslator;

Expression::MultiAstTranslator Expression::defaultTranslator({
    &operatorAstTranslator,
    &stdMathAstTranslator,
    &unitConversionAstTranslator,
    &unresolvedNameAstTranslator
});

}  // namespace common
}  // namespace omnetpp
