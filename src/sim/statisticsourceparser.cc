//==========================================================================
//  STATISTICSOURCEPARSER.CC - part of
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

#include "omnetpp/cmodule.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cstatisticbuilder.h"  // for PROPKEY_STATISTIC_CHECKSIGNALS
#include "omnetpp/resultfilters.h"  // WarmupFilter
#include "expressionfilter.h"
#include "statisticsourceparser.h"
#include "common/exprnodes.h"
#include "common/exprutil.h"
#include "common/stringutil.h"
#include "common/stlutil.h"

namespace omnetpp {

using namespace omnetpp::common;
using namespace omnetpp::common::expression;

typedef Expression::AstNode AstNode;
typedef ExpressionFilter::WrappedSignalSource WrappedSignalSource;

class StatisticSourceAstTranslator : public Expression::AstTranslator
{
  private:
    cComponent *component;
    cProperty *statisticProperty;
    const char *statisticName;
    TristateBool checkSignalDecl;
    bool needWarmupFilter;

    std::vector<SignalSource> subscribedSignals;
    bool multiplyUsedSignalsExist = false;

  protected:
    bool isIdent(AstNode *astNode) {return astNode->type == AstNode::IDENT|| astNode->type == AstNode::IDENT_W_INDEX; }
    bool isMember(AstNode *astNode) {return astNode->type == AstNode::MEMBER || astNode->type == AstNode::MEMBER_W_INDEX; }
    bool hasIndex(AstNode *astNode) {return astNode->type == AstNode::IDENT_W_INDEX || astNode->type == AstNode::MEMBER_W_INDEX; }
    std::string toIndexedName(AstNode *identOrMember);

    virtual void subscribeExpressionFilterToSources(ExprNode *exprNode, ExpressionFilter *expressionFilter);
    virtual WrappedSignalSource *subscribeToSignal(cComponent *component, const char *signalName);
    virtual std::string collectDottedPath(AstNode *astNode);
    virtual cModule *resolveSubscriptionModule(const char *modulePath);

  public:
    StatisticSourceAstTranslator(cComponent *component, cProperty *statisticProperty, const char *statisticName, TristateBool checkSignalDecl, bool needWarmupFilter) :
        component(component), statisticProperty(statisticProperty), statisticName(statisticName), checkSignalDecl(checkSignalDecl), needWarmupFilter(needWarmupFilter) {}
    virtual ExprNode *translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren);
    virtual SignalSource makeSignalSource(ExprNode *subtree);
    bool hasMultiplyUsedSignals() {return multiplyUsedSignalsExist;}
};

void StatisticSourceAstTranslator::subscribeExpressionFilterToSources(ExprNode *exprNode, ExpressionFilter *expressionFilter)
{
    // find WrappedSignalSource instances in the expr tree, and subscribe the filter to the places they refer to
    const std::vector<ExprNode*>& children = exprNode->getChildren();
    std::vector<ExprNode*> newChildren; // fill lazily
    int numChildren = exprNode->getChildren().size();
    for (int i = 0; i < numChildren; i++) {
        ExprNode *child = children[i];
        if (auto castChild = dynamic_cast<WrappedSignalSource*>(child)) {
            SignalSource signalSource = castChild->get();
            delete castChild;

            if (!signalSource.isSubscribed(expressionFilter))
                signalSource.subscribe(expressionFilter);

            auto *inputNode = new ExpressionFilter::FilterInputNode;
            expressionFilter->addInput(signalSource, inputNode);

            if (newChildren.empty())
                newChildren = children; // initialize on first use
            newChildren[i] = inputNode;
        }
        else {
            subscribeExpressionFilterToSources(child, expressionFilter);
        }
    }
    if (!newChildren.empty())
        exprNode->setChildren(newChildren);
}

SignalSource StatisticSourceAstTranslator::makeSignalSource(ExprNode *subtree)
{
    SignalSource signalSource;
    if (auto wrappedSignalSource = dynamic_cast<WrappedSignalSource*>(subtree)) {
        signalSource = wrappedSignalSource->get();
        delete wrappedSignalSource;
    }
    else {
        // expression node, wrap into an ExpressionFilter
        ExpressionFilter *expressionFilter = new ExpressionFilter;
        expressionFilter->getExpression().setExpressionTree(subtree);
        cResultFilter::Context ctx {component, statisticProperty};
        expressionFilter->init(&ctx);
        subscribeExpressionFilterToSources(subtree, expressionFilter);
        if (expressionFilter->getNumInputs() == 0)
            throw cRuntimeError("Expression has no signal input");
        signalSource = SignalSource(expressionFilter);
    }
    return signalSource;
}

std::string StatisticSourceAstTranslator::toIndexedName(AstNode *identOrMember)
{
    if (!hasIndex(identOrMember))
        return identOrMember->name;
    else {
        AstNode *indexChild = identOrMember->children[isIdent(identOrMember) ? 0 : 1];
        if (indexChild->type != AstNode::CONSTANT || indexChild->constant.getType() != ExprValue::INT)
            return ""; // error
        return identOrMember->name + "[" + indexChild->constant.str() + "]";
    }
}

std::string StatisticSourceAstTranslator::collectDottedPath(AstNode *astNode)
{
    std::string result;
    while (isMember(astNode)) {
        ASSERT(astNode->children.size() == (hasIndex(astNode) ? 2 : 1));
        std::string segment = toIndexedName(astNode);
        if (segment.empty())
            return ""; // error
        result = opp_join(".", segment, result);
        astNode = astNode->children[0];
    }
    if (isIdent(astNode)) {
        ASSERT(astNode->children.size() == (hasIndex(astNode) ? 1 : 0));
        std::string segment = toIndexedName(astNode);
        if (segment.empty())
            return ""; // error
        result = opp_join(".", segment, result);
        return result;
    }
    return ""; // error
}

WrappedSignalSource *StatisticSourceAstTranslator::subscribeToSignal(cComponent *component, const char *signalName)
{
    StatisticSourceParser::checkSignalDeclaration(component, signalName, checkSignalDecl);

    SignalSource signalSource(component, signalName);
    if (contains(subscribedSignals, signalSource))
        multiplyUsedSignalsExist = true;
    else
        subscribedSignals.push_back(signalSource);

    if (!needWarmupFilter)
        return new WrappedSignalSource(SignalSource(component, signalName));
    else {
        WarmupPeriodFilter *warmupFilter = new WarmupPeriodFilter();
        component->subscribe(signalName, warmupFilter);
        return new WrappedSignalSource(SignalSource(warmupFilter));
    }
}

cModule *StatisticSourceAstTranslator::resolveSubscriptionModule(const char *modulePath)
{
    cModule *module = dynamic_cast<cModule*>(component);
    if (!module)
        throw cRuntimeError("Signal names qualified with module path may only be used in a @statistic declared on a module");
    std::string relativePath = std::string(".") + modulePath;
    cModule *sourceModule = module->findModuleByPath(relativePath.c_str());
    if (!sourceModule)
        throw cRuntimeError("No module '%s' under '%s'", modulePath, component->getFullPath().c_str());
    return sourceModule;
}

ExprNode *StatisticSourceAstTranslator::translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren)
{
    if (isIdent(astNode)) {
        // must be a signal name
        const char *signalName = astNode->name.c_str();
        return subscribeToSignal(component, signalName);
    }
    else if (isMember(astNode)) {
        // likely submodulepath.signalname
        ASSERT(astNode->children.size() == 1);
        std::string modulePath = collectDottedPath(astNode->children[0]);
        if (!modulePath.empty()) { // conforms to expected pattern of module paths
            cModule *sourceModule = resolveSubscriptionModule(modulePath.c_str());
            const char *signalName = astNode->name.c_str();
            return subscribeToSignal(sourceModule, signalName);
        }
    }
    else if (isFunction(astNode, "merge")) {
        // merge several inputs into one
        MergeFilter *filter = new MergeFilter();
        for (AstNode *astChild : astNode->children) {
            ExprNode *subtree = translateChild(astChild, translatorForChildren);
            SignalSource signalSource = makeSignalSource(subtree);
            signalSource.subscribe(filter);
        }
        return new WrappedSignalSource(SignalSource(filter));
    }
    else if (astNode->type == AstNode::FUNCTION && astNode->children.size() == 1 && cResultFilterType::find(astNode->name.c_str()) != nullptr) {
        // looks like a result filter
        const char *filterName = astNode->name.c_str();
        cResultFilter *filter = cResultFilterType::get(filterName)->create();
        cResultFilter::Context ctx { component, statisticProperty };
        filter->init(&ctx);

        AstNode *astChild = astNode->children[0];
        ExprNode *subtree = translateChild(astChild, translatorForChildren);
        SignalSource signalSource = makeSignalSource(subtree);
        signalSource.subscribe(filter);

        return new WrappedSignalSource(SignalSource(filter));
    }
    (void)statisticName; // eliminate "unused variable" warning
    return nullptr;
}

//---

SignalSource StatisticSourceParser::parse(cComponent *component, cProperty *statisticProperty, const char *statisticName, const char *sourceSpec, TristateBool checkSignalDecl, bool needWarmupFilter)
{
    if (opp_isblank(sourceSpec)) {
        const char *signalName = statisticName;
        checkSignalDeclaration(component, signalName, checkSignalDecl);
        if (!needWarmupFilter)
            return SignalSource(component, signalName);
        else {
            WarmupPeriodFilter *warmupFilter = new WarmupPeriodFilter();
            component->subscribe(signalName, warmupFilter);
            return SignalSource(warmupFilter);
        }
    }

    StatisticSourceAstTranslator statisticTranslator(component, statisticProperty, statisticName, checkSignalDecl, needWarmupFilter);
    Expression::MultiAstTranslator translator({ &statisticTranslator, Expression::getDefaultAstTranslator() });

    ExprNode *exprTree = Expression().parseAndTranslate(sourceSpec, &translator);

    SignalSource signalSource = statisticTranslator.makeSignalSource(exprTree);

    if (statisticTranslator.hasMultiplyUsedSignals())
        throw cRuntimeError("Cannot use a signal more than once in a statistics source expression, due to multipath propagation causing glitches in the output");

    return signalSource;
}

void StatisticSourceParser::checkSignalDeclaration(cComponent *component, const char *signalName, TristateBool checkSignalDecl)
{
    // If it's a simple module or a channel, we expect the signal to be declared with @signal.
    // This behavior can be overridden with the checkSignal=true/false setting in the @statistic property.
    // We can't always check for the presence of the signal declaration, because a signal also may be emitted by submodules.

    if (checkSignalDecl==cStatisticBuilder::TRISTATE_TRUE || (checkSignalDecl==cStatisticBuilder::TRISTATE_DEFAULT && (component->isChannel() || (component->isModule() && ((cModule*)component)->getModuleType()->isSimple())))) {
        omnetpp::cProperty *signalDeclaration = component->getComponentType()->getSignalDeclaration(signalName);
        if (!signalDeclaration)
            throw opp_runtime_error("Signal '%s' is not declared on type '%s' (you can turn off this check "
                    "by adding %s=false to the @statistic property in the NED file)",
                    signalName, component->getComponentType()->getFullName(), PROPKEY_STATISTIC_CHECKSIGNALS);
    }

}

}  // namespace omnetpp

