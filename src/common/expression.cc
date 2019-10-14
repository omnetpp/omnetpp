//==========================================================================
//  EXPRESSION.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
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
    std::stringstream out;
    out << typeName(type);
    if (type == CONSTANT)
        out << " " << constant.str();
    else if (type != UNDEF)
        out << " " << name;  // OP, IDENT, etc all have names
    return out.str();
}

const char *Expression::AstNode::typeName(Type type)
{
    switch (type) {
        case AstNode::CONSTANT: return "constant";
        case AstNode::OP: return "operator";
        case AstNode::FUNCTION: return "function";
        case AstNode::METHOD: return "method";
        case AstNode::IDENT:
        case AstNode::IDENT_W_INDEX: return "identifier";
        case AstNode::MEMBER:
        case AstNode::MEMBER_W_INDEX: return "member";
        case AstNode::UNDEF: return "undef";
	default: throw opp_runtime_error("Unrecognized enum value %d", type);
    }
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

void Expression::parse(const char *expr, AstTranslator *translator)
{
    setExpressionTree(parseAndTranslate(expr, translator));
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
    out << std::string(4*indentLevel, ' ' ) << node->getName() << " ("<< typeid(*node).name() << ")\n";
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
    return tree->tryEvaluate(context);
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

void Expression::AstTranslator::translateChildren(AstNode *astNode, ExprNode *node, AstTranslator *translatorForChildren)
{
    std::vector<ExprNode*> children;
    try {
        children.reserve(astNode->children.size());
        for (AstNode *astChild : astNode->children)
            children.push_back(translateChild(astChild, translatorForChildren));
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

Expression::MultiAstTranslator Expression::defaultTranslator({
    &operatorAstTranslator,
    &stdMathAstTranslator,
    &unitConversionAstTranslator
});

}  // namespace common
}  // namespace omnetpp
