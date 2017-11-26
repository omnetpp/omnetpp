//==========================================================================
//  STATISTICPARSING.CC - part of
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

#include "omnetpp/cmodule.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cstatisticbuilder.h"  // for PROPKEY_STATISTIC_CHECKSIGNALS
#include "sim/resultfilters.h"  // WarmupFilter, ExpressionFilter
#include "sim/resultrecorders.h"  // ExpressionRecorder
#include "statisticparsing.h"

namespace omnetpp {

using namespace omnetpp::common;

void SignalSource::subscribe(cResultListener *listener) const
{
    if (filter)
        filter->addDelegate(listener);
    else if (component && signalID != SIMSIGNAL_NULL)
        component->subscribe(signalID, listener);
    else
        throw opp_runtime_error("subscribe() called on blank SignalSource");
}

//---

class SignalSourceReference : public Expression::Variable
{
  private:
    SignalSource signalSource;

  public:
    SignalSourceReference(const SignalSource& src) : signalSource(src) {}
    virtual Functor *dup() const override { return new SignalSourceReference(signalSource); }
    virtual const char *getName() const override { return "<signalsource>"; }
    virtual char getReturnType() const override { return Expression::Value::DBL; }
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) override { throw opp_runtime_error("unsupported"); }
    const SignalSource& getSignalSource() { return signalSource; }
};

class FilterOrRecorderReference : public Expression::Function
{
  private:
    std::string name;
    int argcount;

  public:
    FilterOrRecorderReference(const char *s, int argc) { name = s; argcount = argc; }
    virtual Functor *dup() const override { return new FilterOrRecorderReference(name.c_str(), argcount); }
    virtual const char *getName() const override { return name.c_str(); }
    virtual const char *getArgTypes() const override { const char *ddd = "DDDDDDDDDD"; Assert(argcount < 10); return ddd+strlen(ddd)-argcount; }
    virtual int getNumArgs() const override { return argcount; }
    virtual char getReturnType() const override { return Expression::Value::DBL; }
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) override { throw opp_runtime_error("unsupported"); }
};

class SourceExpressionResolver : public Expression::Resolver
{
  protected:
    cComponent *context;
    bool needWarmupPeriodFilter;
    TristateBool checkSignalDecl;

  public:
    SourceExpressionResolver(cComponent *context, bool needWarmupPeriodFilter, TristateBool checkSignalDecl)
    {
        this->context = context;
        this->needWarmupPeriodFilter = needWarmupPeriodFilter;
        this->checkSignalDecl = checkSignalDecl;
    }

    virtual Expression::Functor *resolveVariable(const char *varname) override
    {
        // interpret varname as signal name
        cComponent *signalSourceComponent;
        simsignal_t signalID;
        parseSignalPath(varname, signalSourceComponent, signalID);
        if (!needWarmupPeriodFilter)
            return new SignalSourceReference(SignalSource(signalSourceComponent, signalID));
        else {
            WarmupPeriodFilter *warmupFilter = new WarmupPeriodFilter();
            signalSourceComponent->subscribe(signalID, warmupFilter);
            return new SignalSourceReference(SignalSource(warmupFilter));
        }
    }

    void parseSignalPath(const char *signalPath, cComponent *& component, simsignal_t& signalID)
    {
        // parse signalPath as <modulePath>.<signalName>
        const char *signalName;
        if (strchr(signalPath, '.') == nullptr) {
            component = context;
            signalName = signalPath;
        }
        else if (cModule *contextModule = dynamic_cast<cModule*>(context)) {
            const char *lastDot = strrchr(signalPath, '.');
            std::string modulePath = std::string(".") + std::string(signalPath, lastDot - signalPath);
            component = contextModule->getModuleByPath(modulePath.c_str());
            if (!component)
                throw opp_runtime_error("No module '%s' under '%s'", modulePath.c_str()+1, context->getFullPath().c_str());
            signalName = lastDot + 1;
        }
        else
            throw opp_runtime_error("Signal names qualified with module path may only be used in a @statistic declared on a module");

        StatisticSourceParser::checkSignalDeclaration(component, signalName, checkSignalDecl);

        signalID = cComponent::registerSignal(signalName);
    }

    virtual Expression::Functor *resolveFunction(const char *funcname, int argcount) override
    {
        if (MathFunction::supports(funcname))
            return new MathFunction(funcname);
        else if (false)
            ;  // TODO: recognize and handle custom functions (i.e. NedFunctions!)
        else
            return new FilterOrRecorderReference(funcname, argcount);
    }
};

//---

static int countDepth(const std::vector<Expression::Elem>& v, int root)
{
    Assert(root >= 0);
    int argc = v[root].getNumArgs();
    int depth = 1;
    for (int i = 0; i < argc; i++)
        depth += countDepth(v, root-depth);
    return depth;
}

SignalSource StatisticSourceParser::parse(cComponent *component, cProperty *statisticProperty, const char *statisticName, const char *sourceSpec, TristateBool checkSignalDecl, bool needWarmupFilter)
{
    // parse expression
    Expression expr;
    SourceExpressionResolver resolver(component, needWarmupFilter, checkSignalDecl);
    expr.parse(sourceSpec, &resolver);

    int exprLen = expr.getExpressionLength();
    const Expression::Elem *elems = expr.getExpression();

    // printf("Source spec expression %s was parsed as:\n", sourceSpec);
    // for (int i=0; i<exprLen; i++)
    //    printf("  [%d] %s\n", i, elems[i].str().c_str());

    std::vector<Expression::Elem> stack;

    // NOTE: forbid using signals more than once in the source expression, because that would result weird glitches in the output.
    // e.g. count(x) - count(x) (whenever x changes it can reach the recorder through two different paths)
    for (int i = 0; i < exprLen; i++) {
        const Expression::Elem& e = elems[i];
        for (int j = 0; j < i; j++) {
            const Expression::Elem& f = elems[j];
            if (e.getType() == Expression::Elem::FUNCTOR && f.getType() == Expression::Elem::FUNCTOR &&
                dynamic_cast<SignalSourceReference *>(e.getFunctor()) && dynamic_cast<SignalSourceReference *>(f.getFunctor()))
            {
                const SignalSource& u = ((SignalSourceReference *)e.getFunctor())->getSignalSource();
                const SignalSource& v = ((SignalSourceReference *)f.getFunctor())->getSignalSource();
                if (u.getComponent() && v.getComponent() && u.getComponent() == v.getComponent() && u.getSignalID() == v.getSignalID())
                    throw opp_runtime_error("Cannot use a signal more than once in a statistics source expression");  // something wrong
            }
        }
    }

    for (int i = 0; i < exprLen; i++) {
        const Expression::Elem& e = elems[i];

        // push it onto the stack
        stack.push_back(e);

        // if TOS is a filter: create ExpressionFilter from top n items, install it
        // as listener, create and chain the cResultFilter after it, and replace
        // expression on the stack (top n items) with a SourceReference.
        if (e.getType() == Expression::Elem::FUNCTOR && dynamic_cast<FilterOrRecorderReference *>(e.getFunctor())) {
            // determine how many elements this expression covers on the stack
            int len = countDepth(stack, stack.size()-1);
            // use top 'len' elements to create an ExpressionFilter,
            // install it, and replace top 'len' elements with a SignalSourceReference
            // on the stack.
            // if top 'len' elements contain more than one signalsource/filterorrecorder elements --> throw error (not supported for now)
            FilterOrRecorderReference *filterRef = (FilterOrRecorderReference *)e.getFunctor();
            SignalSource signalSource = createFilter(filterRef, stack, len, component, statisticProperty);
            stack.erase(stack.end()-len, stack.end());
            stack.push_back(Expression::Elem());
            stack.back() = new SignalSourceReference(signalSource);
        }
    }

    // there may be an outer expression, like: source=2*mean(eed); wrap it into an ExpressionFilter
    if (stack.size() > 1) {
        // determine how many elements this covers on the stack
        int len = countDepth(stack, stack.size()-1);
        // use top 'len' elements to create an ExpressionFilter,
        // install it, and replace top 'len' elements with a SignalSourceReference
        // on the stack.
        // if top 'len' elements contain more than one signalsource/filterorrecorder elements --> throw error (not supported for now)
        SignalSource signalSource = createFilter(nullptr, stack, len, component, statisticProperty);
        stack.erase(stack.end()-len, stack.end());
        stack.push_back(Expression::Elem());
        stack.back() = new SignalSourceReference(signalSource);
    }

    // the whole expression must have evaluated to a single SignalSourceReference
    // (at least now, when we don't support computing a scalar from multiple signals)
    if (stack.size() != 1)
        throw opp_runtime_error("Malformed expression");  // something wrong
    if (stack[0].getType() != Expression::Elem::FUNCTOR || !dynamic_cast<SignalSourceReference *>(stack[0].getFunctor()))
        throw opp_runtime_error("A constant statistics source is meaningless");  // something wrong
    SignalSourceReference *signalSourceReference = (SignalSourceReference *)stack[0].getFunctor();
    return signalSourceReference->getSignalSource();
}

SignalSource StatisticSourceParser::createFilter(FilterOrRecorderReference *filterRef, const std::vector<Expression::Elem>& stack, int len, cComponent *component, cProperty *statisticProperty)
{
    Assert(len >= 1);
    int stackSize = stack.size();
    int argLen = filterRef ? len-1 : len;  // often we need to ignore the last element on the stack, which is the filter name itself

    // count SignalSourceReferences (nested filter) - there must be exactly one;
    // i.e. the expression may refer to exactly one signal only
    int numSignalRefs = 0;
    SignalSourceReference *signalSourceReference = nullptr;
    for (int i = stackSize-len; i < stackSize; i++) {
        const Expression::Elem& e = stack[i];
        if (e.getType() == Expression::Elem::FUNCTOR)
            if (SignalSourceReference *ref = dynamic_cast<SignalSourceReference *>(e.getFunctor())) {
                numSignalRefs++;
                signalSourceReference = ref;
            }
    }

    // Note: filterRef==nullptr is also valid input, need to be prepared for it!
    cResultFilter *filter = nullptr;
    if (filterRef) {
        const char *filterName = filterRef->getName();
        filter = cResultFilterType::get(filterName)->create();
        filter->init(component, statisticProperty);
    }

    SignalSource result(nullptr);

    if (argLen == 1) {
        // a plain signal reference or chained filter -- no ExpressionFilter needed
        const SignalSource& signalSource = signalSourceReference->getSignalSource();
        if (!filter)
            result = signalSource;
        else {
            signalSource.subscribe(filter);
            result = SignalSource(filter);
        }
    }
    else {  // (argLen > 1) an ExpressionFilter is needed
            // some expression -- add an ExpressionFilter, and the new filter on top
            // replace Expr with SignalSourceReference
        ExpressionFilter *exprFilter;
        if (numSignalRefs == 0)
            throw opp_runtime_error("A constant statistics source expression is meaningless");
        else if (numSignalRefs == 1)
            exprFilter = new UnaryExpressionFilter();
        else if (numSignalRefs > 1)
            exprFilter = new NaryExpressionFilter(numSignalRefs);

        int index = 0;
        Expression::Elem *v = new Expression::Elem[argLen];
        for (int i = 0; i < argLen; i++) {
            v[i] = stack[stackSize-len+i];
            if (v[i].getType() == Expression::Elem::FUNCTOR && dynamic_cast<SignalSourceReference *>(v[i].getFunctor())) {
                signalSourceReference = (SignalSourceReference *)v[i].getFunctor();
                const SignalSource& signalSource = signalSourceReference->getSignalSource();
                cResultFilter *signalFilter = signalSource.getFilter();
                if (signalFilter) {
                    signalSource.subscribe(exprFilter);
                    v[i] = exprFilter->makeValueVariable(index, signalFilter);
                }
                else if (numSignalRefs == 1) {
                    signalSource.subscribe(exprFilter);
                    v[i] = exprFilter->makeValueVariable(index, nullptr);
                }
                else {
                    IdentityFilter *identityFilter = new IdentityFilter();
                    signalSource.subscribe(identityFilter);
                    identityFilter->addDelegate(exprFilter);
                    v[i] = exprFilter->makeValueVariable(index, identityFilter);
                }
                index++;
            }
        }
        exprFilter->getExpression().setExpression(v, argLen);
        if (exprFilter)
            exprFilter->init(component, statisticProperty);

        if (!filter)
            result = SignalSource(exprFilter);
        else {
            exprFilter->addDelegate(filter);
            result = SignalSource(filter);
        }
    }
    return result;
}

void StatisticSourceParser::checkSignalDeclaration(cComponent *component, const char *signalName, TristateBool checkSignalDecl)
{
    // if it's a simple module or a channel, we expect the signal to be declared with @signal.
    // This behavior can be overridden with the checkSignal=true/false setting.
    // We can't always check for the presence of the signal declaration, because
    // a signal also may be emitted by submodules

    if (checkSignalDecl==cStatisticBuilder::TRISTATE_TRUE || (checkSignalDecl==cStatisticBuilder::TRISTATE_DEFAULT && (component->isChannel() || (component->isModule() && ((cModule*)component)->isSimple())))) {
        omnetpp::cProperty *signalDeclaration = component->getComponentType()->getSignalDeclaration(signalName);
        if (!signalDeclaration)
            throw opp_runtime_error("Signal '%s' is not declared on type '%s' (you can turn off or force this check by setting %s=false/true)",
                    signalName, component->getComponentType()->getFullName(), PROPKEY_STATISTIC_CHECKSIGNALS);
    }

}

//---

class RecorderExpressionResolver : public Expression::Resolver
{
  protected:
    SignalSource signalSource;

  public:
    RecorderExpressionResolver(const SignalSource& source) : signalSource(source) {}

    virtual Expression::Functor *resolveVariable(const char *varname) override
    {
        // "$input" to mean the input of the expression; "timeavg" to mean "timeavg($input)"; "$source" is the old name
        if (strcmp(varname, "$input") == 0 || strcmp(varname, "$source") == 0)
            return new SignalSourceReference(signalSource);
        else
            // argcount=0 means that it refers to the source spec; so "max" and "max()"
            // will translate to the same thing (cf. with resolveFunction())
            return new FilterOrRecorderReference(varname, 0);
    }

    virtual Expression::Functor *resolveFunction(const char *funcname, int argcount) override
    {
        if (MathFunction::supports(funcname))
            return new MathFunction(funcname);
        else if (false)
            ;  // TODO: recognize and handle custom functions (i.e. NedFunctions!)
        else
            return new FilterOrRecorderReference(funcname, argcount);
    }
};

void StatisticRecorderParser::parse(const SignalSource& source, const char *recordingMode, cComponent *component, const char *statisticName, cProperty *attrsProperty)
{
    // parse expression
    Expression expr;
    RecorderExpressionResolver resolver(source);
    expr.parse(recordingMode, &resolver);

    int exprLen = expr.getExpressionLength();
    const Expression::Elem *elems = expr.getExpression();

    // printf("Recorder expression %s was parsed as:\n", sourceSpec);
    // for (int i=0; i<exprLen; i++)
    //    printf("  [%d] %s\n", i, elems[i].str().c_str());

    std::vector<Expression::Elem> stack;

    for (int i = 0; i < exprLen; i++) {
        const Expression::Elem& e = elems[i];

        // push it onto the stack
        stack.push_back(e);

        // if TOS is a filter: create ExpressionFilter from top n items, install it
        // as listener, create and chain the cResultFilter after it, and replace
        // expression on the stack (top n items) with a SourceReference.
        if (e.getType() == Expression::Elem::FUNCTOR && dynamic_cast<FilterOrRecorderReference *>(e.getFunctor())) {
            // determine how many elements this expression covers on the stack
            int len = countDepth(stack, stack.size()-1);
            // use top 'len' elements to create an ExpressionFilter,
            // install it, and replace top 'len' elements with a SignalSourceReference
            // on the stack.
            // if top 'len' elements contain more than one signalsource/filterorrecorder elements --> throw error (not supported for now)
            FilterOrRecorderReference *filterOrRecorderRef = (FilterOrRecorderReference *)e.getFunctor();
            SignalSource signalSource = createFilterOrRecorder(filterOrRecorderRef, i == exprLen-1, stack, len, source, component, statisticName, recordingMode, attrsProperty);
            stack.erase(stack.end()-len, stack.end());
            stack.push_back(Expression::Elem());
            stack.back() = new SignalSourceReference(signalSource);
        }
    }

    // there may be an outer expression, like: source=2*mean(eed); wrap it into an ExpressionRecorder
    if (stack.size() > 1) {
        // determine how many elements this covers on the stack
        int len = countDepth(stack, stack.size()-1);
        // use top 'len' elements to create an ExpressionFilter,
        // install it, and replace top 'len' elements with a SignalSourceReference
        // on the stack.
        // if top 'len' elements contain more than one signalsource/filterorrecorder elements --> throw error (not supported for now)
        SignalSource signalSource = createFilterOrRecorder(nullptr, true, stack, len, source, component, statisticName, recordingMode, attrsProperty);
        stack.erase(stack.end()-len, stack.end());
        stack.push_back(Expression::Elem());
        stack.back() = new SignalSourceReference(signalSource);
    }

    // the whole expression must have evaluated to a single SignalSourceReference
    // containing a SignalSource(nullptr), because the outer element must be a recorder
    // that does not support further chaining (see markRecorders())
    if (stack.size() != 1)
        throw opp_runtime_error("Malformed expression");  // something wrong
    if (stack[0].getType() != Expression::Elem::FUNCTOR || !dynamic_cast<SignalSourceReference *>(stack[0].getFunctor()))
        throw opp_runtime_error("Malformed expression");  // something wrong
    if (!((SignalSourceReference *)stack[0].getFunctor())->getSignalSource().isNull())
        throw opp_runtime_error("Malformed expression");  // something wrong
}

// XXX now it appears that StatisticSourceParser::createFilter() is a special case of this -- eliminate it?
SignalSource StatisticRecorderParser::createFilterOrRecorder(FilterOrRecorderReference *filterOrRecorderRef, bool makeRecorder, const std::vector<Expression::Elem>& stack, int len, const SignalSource& source, cComponent *component, const char *statisticName, const char *recordingMode, cProperty *attrsProperty)
{
    Assert(len >= 1);
    int stackSize = stack.size();

    int argLen = filterOrRecorderRef ? len-1 : len;  // often we need to ignore the last element on the stack, which is the filter name itself

    // count embedded signal references, unless filter is arg-less (i.e. it has an
    // implicit source arg, like "record=timeavg" which means "record=timeavg($input)")
    SignalSourceReference *signalSourceReference = nullptr;
    if (!filterOrRecorderRef || filterOrRecorderRef->getNumArgs() > 0) {
        // count SignalSourceReferences (nested filter) - there must be exactly one;
        // i.e. the expression may refer to exactly one signal only
        int numSignalRefs = 0;
        for (int i = stackSize-len; i < stackSize; i++) {
            const Expression::Elem& e = stack[i];
            if (e.getType() == Expression::Elem::FUNCTOR)
                if (SignalSourceReference *ref = dynamic_cast<SignalSourceReference *>(e.getFunctor())) {
                    numSignalRefs++;
                    signalSourceReference = ref;
                }
        }

        if (numSignalRefs != 1) {
            // note: filterOrRecorderRef can be nullptr, so cannot use its name in the error msg
            if (numSignalRefs == 0)
                throw cRuntimeError("Expression does not refer to any signal");
            else
                throw cRuntimeError("Expression may only refer to one signal");
        }
    }

    // Note: filterOrRecorderRef==nullptr is also valid input, need to be prepared for it!
    cResultListener *filterOrRecorder = nullptr;
    if (filterOrRecorderRef) {
        const char *name = filterOrRecorderRef->getName();
        if (makeRecorder) {
            cResultRecorder *recorder = cResultRecorderType::get(name)->create();
            recorder->init(component, statisticName, recordingMode, attrsProperty);
            filterOrRecorder = recorder;
        }
        else {
            cResultFilter *filter = cResultFilterType::get(name)->create();
            filter->init(component, attrsProperty);
            filterOrRecorder = filter;
        }
    }

    SignalSource result(nullptr);

    if (argLen <= 1) {
        // a plain signal reference or chained filter -- no ExpressionFilter needed
        const SignalSource& signalSource = signalSourceReference ?
            signalSourceReference->getSignalSource() : source;

        if (!filterOrRecorder)
            result = signalSource;
        else {
            signalSource.subscribe(filterOrRecorder);
            if (!makeRecorder)
                result = SignalSource((cResultFilter *)filterOrRecorder);
        }
    }
    else {  // (argLen > 1)
            // some expression -- add an ExpressionFilter or Recorder, and chain the
            // new filter (if exists) on top of it.
        cResultListener *exprListener;
        if (!filterOrRecorder && makeRecorder) {
            // expression recorder
            ExpressionRecorder *exprRecorder = new ExpressionRecorder();
            exprRecorder->init(component, statisticName, recordingMode, attrsProperty);

            Expression::Elem *v = new Expression::Elem[argLen];
            for (int i = 0; i < argLen; i++) {
                v[i] = stack[stackSize-len+i];
                if (v[i].getType() == Expression::Elem::FUNCTOR && dynamic_cast<SignalSourceReference *>(v[i].getFunctor()))
                    v[i] = exprRecorder->makeValueVariable();
            }
            exprRecorder->getExpression().setExpression(v, argLen);
            exprListener = exprRecorder;
        }
        else {
            // expression filter
            UnaryExpressionFilter *exprFilter = new UnaryExpressionFilter();

            int index = 0;
            Expression::Elem *v = new Expression::Elem[argLen];
            for (int i = 0; i < argLen; i++) {
                v[i] = stack[stackSize-len+i];
                if (v[i].getType() == Expression::Elem::FUNCTOR && dynamic_cast<SignalSourceReference *>(v[i].getFunctor())) {
                    signalSourceReference = (SignalSourceReference *)v[i].getFunctor();
                    v[i] = exprFilter->makeValueVariable(index, signalSourceReference->getSignalSource().getFilter());
                    index++;
                }
            }
            exprFilter->getExpression().setExpression(v, argLen);
            exprListener = exprFilter;
        }

        // subscribe
        const SignalSource& signalSource = signalSourceReference->getSignalSource();
        signalSource.subscribe(exprListener);
        if (!filterOrRecorder) {
            if (UnaryExpressionFilter *unaryExpressionFilter = dynamic_cast<UnaryExpressionFilter *>(exprListener))
                result = SignalSource(unaryExpressionFilter);
        }
        else {
            Assert(dynamic_cast<UnaryExpressionFilter *>(exprListener));
            ((UnaryExpressionFilter *)exprListener)->addDelegate(filterOrRecorder);
            if (!makeRecorder)
                result = SignalSource((cResultFilter *)filterOrRecorder);
        }
    }
    return result;  // if makeRecorder=true, we return a nullptr SignalSource (no chaining possible)
}

}  // namespace omnetpp

