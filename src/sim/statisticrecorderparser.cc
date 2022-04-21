//==========================================================================
//  STATISTICRECORDERPARSER.CC - part of
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

#include "omnetpp/resultrecorders.h"  // LastValueRecorder
#include "expressionfilter.h"
#include "statisticrecorderparser.h"
#include "common/exprutil.h"

namespace omnetpp {

using namespace omnetpp::common;
using namespace omnetpp::common::expression;

typedef Expression::AstNode AstNode;
typedef ExpressionFilter::WrappedSignalSource WrappedSignalSource;

class StatisticRecorderAstTranslator : public Expression::AstTranslator
{
  private:
    SignalSource source;
    const char *recordingMode;
    cComponent *component;
    cProperty *attrsProperty;
    const char *statisticName;
    AstNode *astRoot = nullptr;

  protected:
    virtual void subscribeExpressionFilterToSources(ExprNode *exprNode, ExpressionFilter *expressionFilter);

  public:
    StatisticRecorderAstTranslator(const SignalSource& source, const char *recordingMode, cComponent *component, cProperty *statisticProperty, const char *statisticName) :
        source(source), recordingMode(recordingMode), component(component), attrsProperty(statisticProperty), statisticName(statisticName) {}
    virtual ExprNode *translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren);
    virtual SignalSource makeSignalSource(ExprNode *subtree);
};

void StatisticRecorderAstTranslator::subscribeExpressionFilterToSources(ExprNode *exprNode, ExpressionFilter *expressionFilter)
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

SignalSource StatisticRecorderAstTranslator::makeSignalSource(ExprNode *subtree)
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
        cResultFilter::Context ctx {component, attrsProperty};
        expressionFilter->init(&ctx);
        subscribeExpressionFilterToSources(subtree, expressionFilter);
        signalSource = SignalSource(expressionFilter);
    }
    return signalSource;
}

ExprNode *StatisticRecorderAstTranslator::translateToExpressionTree(AstNode *astNode, AstTranslator *translatorForChildren)
{
    if (!astRoot)
        astRoot = astNode;

    const std::string& name = astNode->name;
    cResultRecorderType *recorderFactory = nullptr;
    cResultFilterType *filterFactory = nullptr;
    if (isIdent(astNode) || isFunction(astNode,0) || isFunction(astNode, 1)) {
        recorderFactory = cResultRecorderType::find(name.c_str());
        filterFactory = cResultFilterType::find(name.c_str());
    }

    if (isFunction(astNode, "merge")) {
        throw cRuntimeError("merge() may only appear in the source= part of a @statistic");
    }
    if (isIdent(astNode) || isFunction(astNode, 0)) {
        // "$input" to mean the input of the expression; "timeavg" to mean "timeavg($input)"; "$source" is the old name
        if (isIdent(astNode) && (name == "$input" || name == "$source"))
            return new WrappedSignalSource(source);
        else if (recorderFactory && astNode==astRoot) { // only do recorder if we are the root
            cResultRecorder *recorder = recorderFactory->create();
            cResultRecorder::Context ctx {component, statisticName, recordingMode, attrsProperty};
            recorder->init(&ctx);
            source.subscribe(recorder);
            return new WrappedSignalSource(SignalSource()); // blank
        }
        else if (filterFactory) {
            cResultFilter *filter = filterFactory->create();
            cResultFilter::Context ctx {component, attrsProperty};
            filter->init(&ctx);
            source.subscribe(filter);
            return new WrappedSignalSource(SignalSource(filter));
        }
    }
    else if (isFunction(astNode, 1) && (recorderFactory || filterFactory)) {
        AstNode *astChild = astNode->children[0];
        ExprNode *subtree = translateChild(astChild, translatorForChildren);
        SignalSource prev = makeSignalSource(subtree);
        if (prev.isEmpty())
            throw cRuntimeError("Cannot apply filter/recorder '%s' to a recorder", name.c_str());

        if (recorderFactory && astNode==astRoot) {
            cResultRecorder *recorder = recorderFactory->create();
            cResultRecorder::Context ctx {component, statisticName, recordingMode, attrsProperty};
            recorder->init(&ctx);
            prev.subscribe(recorder);
            return new WrappedSignalSource(SignalSource()); // blank
        }
        else if (filterFactory) {
            cResultFilter *filter = filterFactory->create();
            cResultFilter::Context ctx {component, attrsProperty};
            filter->init(&ctx);
            prev.subscribe(filter);
            return new WrappedSignalSource(SignalSource(filter));
        }
    }
    return nullptr;
}

//---

void StatisticRecorderParser::parse(const SignalSource& source, const char *recordingMode, cComponent *component, const char *statisticName, cProperty *attrsProperty)
{
    StatisticRecorderAstTranslator statisticTranslator(source, recordingMode, component, attrsProperty, statisticName);
    Expression::MultiAstTranslator translator({ &statisticTranslator, Expression::getDefaultAstTranslator() });

    ExprNode *exprTree = Expression().parseAndTranslate(recordingMode, &translator);

    SignalSource output = statisticTranslator.makeSignalSource(exprTree);

    if (!output.isEmpty()) {
        LastValueRecorder *lastValueRecorder = new LastValueRecorder();
        cResultRecorder::Context ctx {component, statisticName, recordingMode, attrsProperty};
        lastValueRecorder->init(&ctx);
        output.subscribe(lastValueRecorder);
    }
}

}  // namespace omnetpp

