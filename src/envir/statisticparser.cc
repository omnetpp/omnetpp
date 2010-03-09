//==========================================================================
//  STATISTICPARSER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "statisticparser.h"
#include "resultfilters.h"


class SignalSourceReference : public Expression::Variable
{
  private:
    SignalSource signalSource;
  public:
    SignalSourceReference(const SignalSource& src) : signalSource(src) {}
    virtual Functor *dup() const {return new SignalSourceReference(signalSource);}
    virtual const char *getName() const {return "<signalsource>";}
    virtual char getReturnType() const {return Expression::Value::DBL;}
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) {throw opp_runtime_error("unsupported");}
    const SignalSource& getSignalSource() {return signalSource;}
};

class FilterOrRecorderReference : public Expression::Function
{
  private:
    std::string name;
    int argcount;
    bool isfilter; // true by default; see markAsRecorders()
  public:
    FilterOrRecorderReference(const char *s, int argc, bool filter=true) {name = s; argcount = argc; isfilter = filter;}
    virtual Functor *dup() const {return new FilterOrRecorderReference(name.c_str(), argcount);}
    virtual const char *getName() const {return name.c_str();}
    bool isFilter() const {return isfilter;}
    void setIsFilter(bool b) {isfilter = b;}
    virtual const char *getArgTypes() const {const char *ddd="DDDDDDDDDD"; Assert(argcount<10); return ddd+strlen(ddd)-argcount;}
    virtual int getNumArgs() const {return argcount;}
    virtual char getReturnType() const {return Expression::Value::DBL;}
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) {throw opp_runtime_error("unsupported");}
};


class SourceExpressionResolver : public Expression::Resolver
{
  protected:
    cComponent *component;
    bool needWarmupPeriodFilter;

  public:
    SourceExpressionResolver(cComponent *comp, bool needWarmupFilter)
    {
        component = comp;
        needWarmupPeriodFilter = needWarmupFilter;
    }

    virtual Expression::Functor *resolveVariable(const char *varname)
    {
        // interpret varname as signal name
        simsignal_t signalID = cComponent::registerSignal(varname);
        if (!needWarmupPeriodFilter)
            return new SignalSourceReference(SignalSource(component, signalID));
        else
        {
            WarmupPeriodFilter *warmupFilter = new WarmupPeriodFilter();
            component->subscribe(signalID, warmupFilter);
            return new SignalSourceReference(SignalSource(warmupFilter));
        }
    }

    virtual Expression::Functor *resolveFunction(const char *funcname, int argcount)
    {
        if (MathFunction::supports(funcname))
            return new MathFunction(funcname);
        else if (false)
            ; // TODO: recognize and handle custom functions (i.e. NEDFunctions!)
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
    for (int i=0; i<argc; i++)
        depth += countDepth(v, root-depth);
    return depth;
}

SignalSource StatisticSourceParser::parse(cComponent *component, const char *statisticName, const char *sourceSpec, bool needWarmupFilter)
{
    // parse expression
    Expression expr;
    SourceExpressionResolver resolver(component, needWarmupFilter);
    expr.parse(sourceSpec, &resolver);

    int exprLen = expr.getExpressionLength();
    const Expression::Elem *elems = expr.getExpression();

    //printf("Source spec expression %s was parsed as:\n", sourceSpec);
    //for (int i=0; i<exprLen; i++)
    //    printf("  [%d] %s\n", i, elems[i].str().c_str());

    std::vector<Expression::Elem> stack;

    for (int i = 0; i < exprLen; i++)
    {
       const Expression::Elem& e = elems[i];
       if (e.getType()!=Expression::Elem::FUNCTOR || !dynamic_cast<FilterOrRecorderReference*>(e.getFunctor()))
       {
           // some normal expression ingredient
           stack.push_back(e);
       }
       else
       {
           // filter or recorder reference
           // determine how many elements this covers on the stack
           int len = countDepth(stack, stack.size()-1);
           // use top 'len' elements to create an ExpressionFilter,
           // install it, and replace top 'len' elements with a SignalSourceReference
           // on the stack.
           // if top 'len' elements contain more than one signalsource/filterorrecorder elements --> throw error (not supported for now)
           FilterOrRecorderReference *filterRef = (FilterOrRecorderReference *) e.getFunctor();
           SignalSource signalSource = createFilter(filterRef, stack, len);
           stack.erase(stack.end()-len, stack.end());
           stack.push_back(Expression::Elem());
           stack.back() = new SignalSourceReference(signalSource);
       }
    }

    // there may be an outer expression, like: source=2*mean(eed); wrap it into an ExpressionFilter
    if (stack.size() > 1)
    {
        // determine how many elements this covers on the stack
        int len = countDepth(stack, stack.size()-1);
        // use top 'len' elements to create an ExpressionFilter,
        // install it, and replace top 'len' elements with a SignalSourceReference
        // on the stack.
        // if top 'len' elements contain more than one signalsource/filterorrecorder elements --> throw error (not supported for now)
        SignalSource signalSource = createFilter(NULL, stack, len);
        stack.erase(stack.end()-len, stack.end());
        stack.push_back(Expression::Elem());
        stack.back() = new SignalSourceReference(signalSource);
    }

    // the whole expression must have evaluated to a single SignalSourceReference
    // (at least now, when we don't support computing a scalar from multiple signals)
    if (stack.size() != 1)
        throw opp_runtime_error("malformed expression"); // something wrong
    if (stack[0].getType()!=Expression::Elem::FUNCTOR || !dynamic_cast<SignalSourceReference*>(stack[0].getFunctor()))
        throw opp_runtime_error("malformed expression"); // something wrong
    SignalSourceReference *signalSourceReference = (SignalSourceReference*) stack[0].getFunctor();
    return signalSourceReference->getSignalSource();
}

SignalSource StatisticSourceParser::createFilter(FilterOrRecorderReference *filterRef, const std::vector<Expression::Elem>& stack, int len)
{
    Assert(len >= 1);
    int stackSize = stack.size();

    // count SignalSourceReferences (nested filter) - there must be exactly one;
    // i.e. the expression may refer to exactly one signal only
    int numSignalRefs = 0;
    SignalSourceReference *signalSourceReference = NULL;
    for (int i = stackSize-len; i < stackSize; i++)
    {
        const Expression::Elem& e = stack[i];
        if (e.getType()==Expression::Elem::FUNCTOR)
            if (dynamic_cast<SignalSourceReference*>(e.getFunctor()))
                {numSignalRefs++; signalSourceReference = (SignalSourceReference*)e.getFunctor();}
    }

    if (numSignalRefs != 1)
    {
        if (numSignalRefs == 0)
            throw cRuntimeError("expression inside %s() does not refer to any signal", filterRef->getName());
        else
            throw cRuntimeError("expression inside %s() may only refer to one signal", filterRef->getName());
    }

    // Note: filterRef==NULL is also valid input, need to be prepared for it!
    ResultFilter *filter = NULL;
    if (filterRef)
    {
        const char *filterName = filterRef->getName();
        filter = ResultFilterDescriptor::get(filterName)->create();
    }

    SignalSource result(NULL);

    if (len == 1)
    {
        // a plain signal reference or chained filter -- no ExpressionFilter needed
        const SignalSource& signalSource = signalSourceReference->getSignalSource();
        if (!filter)
            result = signalSource;
        else {
            signalSource.subscribe(filter);
            result = SignalSource(filter);
        }
    }
    else // (len > 1)
    {
        // some expression -- add an ExpressionFilter, and the new filter on top
        // replace Expr with SignalSourceReference
        ExpressionFilter *exprFilter = new ExpressionFilter();

        Expression::Elem *v = new Expression::Elem[len];
        for (int i=0; i<len; i++)
        {
            v[i] = stack[stackSize-len+i];
            if (v[i].getType()==Expression::Elem::FUNCTOR && dynamic_cast<SignalSourceReference*>(v[i].getFunctor()))
                v[i] = exprFilter->makeValueVariable();
        }

        exprFilter->getExpression().setExpression(v,len);

        // subscribe
        const SignalSource& signalSource = signalSourceReference->getSignalSource();
        signalSource.subscribe(exprFilter);
        if (!filter)
            result = SignalSource(exprFilter);
        else {
            exprFilter->addDelegate(filter);
            result = SignalSource(filter);
        }
    }
    return result;
}

//---

class RecorderExpressionResolver : public Expression::Resolver
{
  protected:
    SignalSource signalSource;

  public:
    RecorderExpressionResolver(const SignalSource& source) : signalSource(source) {}

    virtual Expression::Functor *resolveVariable(const char *varname)
    {
        // "$source" to mean the source signal; "timeavg" to mean "timeavg($source)"
        if (strcmp(varname, "$source")==0)
            return new SignalSourceReference(signalSource);
        else
            // argcount=0 means that it refers to the source spec; so "max" and "max()"
            // will translate to the same thing (cf. with resolveFunction())
            return new FilterOrRecorderReference(varname, 0);
    }

    virtual Expression::Functor *resolveFunction(const char *funcname, int argcount)
    {
        if (MathFunction::supports(funcname))
            return new MathFunction(funcname);
        else if (false)
            ; // TODO: recognize and handle custom functions (i.e. NEDFunctions!)
        else
            return new FilterOrRecorderReference(funcname, argcount);
    }
};

// mark the outermost FilterOrRecorderReference(s) as recorders;
// this is basically the same as countDepth(), but marks recorders along the way
static int markRecorders(Expression::Elem *v, int root, bool underRecorder=false)
{
    Assert(root >= 0);
    Expression::Elem& e = v[root];
    if (e.getType()==Expression::Elem::FUNCTOR && dynamic_cast<FilterOrRecorderReference*>(e.getFunctor())) {
        ((FilterOrRecorderReference*)e.getFunctor())->setIsFilter(false);
        underRecorder = true;
    }
    int argc = e.getNumArgs();
    int depth = 1;
    for (int i=0; i<argc; i++)
        depth += markRecorders(v, root-depth, underRecorder);
    return depth;
}


void StatisticRecorderParser::parse(const SignalSource& source, const char *mode, bool scalarsEnabled, bool vectorsEnabled, cComponent *component, const char *statisticName, const char *where)
{
    // parse expression
    Expression expr;
    RecorderExpressionResolver resolver(source);
    expr.parse(mode, &resolver);

    int exprLen = expr.getExpressionLength();
    const Expression::Elem *elems = expr.getExpression();

    markRecorders(const_cast<Expression::Elem *>(elems), exprLen-1);

    //printf("Recorder expression %s was parsed as:\n", sourceSpec);
    //for (int i=0; i<exprLen; i++)
    //    printf("  [%d] %s\n", i, elems[i].str().c_str());

    std::vector<Expression::Elem> stack;

    for (int i = 0; i < exprLen; i++)
    {
       const Expression::Elem& e = elems[i];
       if (e.getType()!=Expression::Elem::FUNCTOR || !dynamic_cast<FilterOrRecorderReference*>(e.getFunctor()))
       {
           // some normal expression ingredient
           stack.push_back(e);
       }
       else
       {
           // filter or recorder reference
           // determine how many elements this covers on the stack
           int len = countDepth(stack, stack.size()-1);
           // use top 'len' elements to create an ExpressionFilter,
           // install it, and replace top 'len' elements with a SignalSourceReference
           // on the stack.
           // if top 'len' elements contain more than one signalsource/filterorrecorder elements --> throw error (not supported for now)
           FilterOrRecorderReference *filterOrRecorderRef = (FilterOrRecorderReference *) e.getFunctor();
           SignalSource signalSource = createFilterOrRecorder(filterOrRecorderRef, stack, len, source);
           stack.erase(stack.end()-len, stack.end());
           stack.push_back(Expression::Elem());
           stack.back() = new SignalSourceReference(signalSource);
       }
    }

    // there may be an outer expression, like: source=2*mean(eed); wrap it into an ExpressionRecorder
    if (stack.size() > 1)
    {
        // determine how many elements this covers on the stack
        int len = countDepth(stack, stack.size()-1);
        // use top 'len' elements to create an ExpressionFilter,
        // install it, and replace top 'len' elements with a SignalSourceReference
        // on the stack.
        // if top 'len' elements contain more than one signalsource/filterorrecorder elements --> throw error (not supported for now)
        SignalSource signalSource = createFilterOrRecorder(NULL, stack, len, source);
        stack.erase(stack.end()-len, stack.end());
        stack.push_back(Expression::Elem());
        stack.back() = new SignalSourceReference(signalSource);
    }

    // the whole expression must have evaluated to a single SignalSourceReference
    // containing a SignalSource(NULL), meaning that the last element is a recorder
    // which does not support further chaining
    if (stack.size() != 1)
        throw opp_runtime_error("malformed expression"); // something wrong
    if (stack[0].getType()!=Expression::Elem::FUNCTOR || !dynamic_cast<SignalSourceReference*>(stack[0].getFunctor()))
        throw opp_runtime_error("malformed expression"); // something wrong
    if (!((SignalSourceReference*)stack[0].getFunctor())->getSignalSource().isNull())
        throw opp_runtime_error("malformed expression"); // something wrong
}

SignalSource StatisticRecorderParser::createFilterOrRecorder(FilterOrRecorderReference *filterOrRecorderRef, const std::vector<Expression::Elem>& stack, int len, const SignalSource& source)
{
    //FIXME we need to create an ExpressionRecorder instead of ExpressionParser if filterOrRecorderRef==NULL
    Assert(len >= 1);
    int stackSize = stack.size();

    SignalSourceReference *signalSourceReference = NULL;
    if (filterOrRecorderRef && filterOrRecorderRef->getNumArgs()==0)  // implicit source arg, like "timeavg" which means "timeavg($source)"
    {
        //FIXME must use "source" as SignalSource
    }
    else
    {
        // count SignalSourceReferences (nested filter) - there must be exactly one;
        // i.e. the expression may refer to exactly one signal only
        int numSignalRefs = 0;
        for (int i = stackSize-len; i < stackSize; i++)
        {
            const Expression::Elem& e = stack[i];
            if (e.getType()==Expression::Elem::FUNCTOR)
                if (dynamic_cast<SignalSourceReference*>(e.getFunctor()))
                    {numSignalRefs++; signalSourceReference = (SignalSourceReference*)e.getFunctor();}
        }

        if (numSignalRefs != 1)
        {
            if (numSignalRefs == 0)
                throw cRuntimeError("expression inside %s() does not refer to any signal", filterOrRecorderRef->getName());
            else
                throw cRuntimeError("expression inside %s() may only refer to one signal", filterOrRecorderRef->getName());
        }
    }

    // Note: filterOrRecorderRef==NULL is also valid input, need to be prepared for it!
    ResultListener *filterOrRecorder = NULL;
    if (filterOrRecorderRef)
    {
        const char *name = filterOrRecorderRef->getName();
        if (filterOrRecorderRef->isFilter())
            filterOrRecorder = ResultFilterDescriptor::get(name)->create();
        else
            filterOrRecorder = ResultRecorderDescriptor::get(name)->create();
    }

    SignalSource result(NULL);

    if (len == 1)
    {
        // a plain signal reference or chained filter -- no ExpressionFilter needed
        const SignalSource& signalSource = signalSourceReference->getSignalSource();
        if (!filterOrRecorder)
            result = signalSource;
        else {
            signalSource.subscribe(filterOrRecorder);
            result = filterOrRecorderRef->isFilter() ? SignalSource((ResultFilter*)filterOrRecorder) : SignalSource(NULL);
        }
    }
    else // (len > 1)
    {
        // some expression -- add an ExpressionFilter, and the new filter on top
        // replace Expr with SignalSourceReference
        ExpressionFilter *exprFilter = new ExpressionFilter();

        Expression::Elem *v = new Expression::Elem[len];
        for (int i=0; i<len; i++)
        {
            v[i] = stack[stackSize-len+i];
            if (v[i].getType()==Expression::Elem::FUNCTOR && dynamic_cast<SignalSourceReference*>(v[i].getFunctor()))
                v[i] = exprFilter->makeValueVariable();
        }

        exprFilter->getExpression().setExpression(v,len);

        // subscribe
        const SignalSource& signalSource = signalSourceReference->getSignalSource();
        signalSource.subscribe(exprFilter);
        if (!filterOrRecorder)
            result = SignalSource(exprFilter);
        else {
            exprFilter->addDelegate(filterOrRecorder);
            result = filterOrRecorderRef->isFilter() ? SignalSource((ResultFilter*)filterOrRecorder) : SignalSource(NULL);
        }
    }
    return result; // if isRecorder==NULL, we return a NULL SignalSource (no chaining possible)
}

