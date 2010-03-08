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

//FIXME TODO: in envirbase, dump the filter/recorder chain attached to the components' signals!!!

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
  public:
    FilterOrRecorderReference(const char *s, int argc) {name = s; argcount = argc;}
    virtual Functor *dup() const {return new FilterOrRecorderReference(name.c_str(), argcount);}
    virtual const char *getName() const {return name.c_str();}
    virtual const char *getArgTypes() const {const char *ddd="DDDDDDDDDD"; Assert(argcount<10); return ddd+strlen(ddd)-argcount;}
    virtual int getNumArgs() const {return argcount;}
    virtual char getReturnType() const {return Expression::Value::DBL;}
    virtual Expression::Value evaluate(Expression::Value args[], int numargs) {throw opp_runtime_error("unsupported");}
};


//XXX rename
class XResolver : public Expression::Resolver
{
  protected:
    cComponent *component;
  public:
    XResolver(cComponent *component) {this->component = component;}

    virtual Expression::Functor *resolveVariable(const char *varname)
    {
        // interpret varname as signal name
        simsignal_t signalID = cComponent::registerSignal(varname);
        return new SignalSourceReference(SignalSource(component, signalID));
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
    XResolver resolver(component);
    expr.parse(sourceSpec, &resolver);
    printf("EXPR: %s --> %s\n", sourceSpec, expr.str().c_str());

    int exprLen = expr.getExpressionLength();
    const Expression::Elem *elems = expr.getExpression();

    for (int i=0; i<exprLen; i++)
        printf("    %d: %s\n", i, elems[i].str().c_str());

    std::vector<Expression::Elem> stack;
    SignalSource prev(NULL);

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
           SignalSource signalSource = createFilter(filterRef, stack, len, component, needWarmupFilter);
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
        SignalSource signalSource = createFilter(NULL, stack, len, component, needWarmupFilter);
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

SignalSource StatisticSourceParser::createFilter(FilterOrRecorderReference *filterRef, const std::vector<Expression::Elem>& stack, int len, cComponent *component, bool needWarmupFilter)
{
    Assert(len >= 1);
    int stackSize = stack.size();
    printf(" to be replaced:\n");
    for (int i=0; i<len; i++)
        printf("   %s\n", stack[stackSize-len+i].str().c_str());

    // count SignalSourceReferences (nested filter) - there must be exactly one;
    // i.e. the expression may refer to exactly one signal only
    int n = 0;
    SignalSourceReference *signalSourceReference = NULL;
    for (int i=stackSize-len; i<stackSize; i++)
    {
        const Expression::Elem& e = stack[i];
        if (e.getType()==Expression::Elem::FUNCTOR)
            if (dynamic_cast<SignalSourceReference*>(e.getFunctor()))
                {n++; signalSourceReference = (SignalSourceReference*)e.getFunctor();}
    }
    printf(" refs: %d\n", n);

    if (n != 1)
    {
        if (n==0)
            throw cRuntimeError("expression inside %s() does not refer to any signal", filterRef->getName());
        else
            throw cRuntimeError("expression inside %s() may only refer to one signal", filterRef->getName());
    }

    //FIXME TODO add WarmupFilter!!! maybe already in XResolver??
    //if (needWarmupFilter) {
    //   WarmupPeriodFilter *warmupFilter = new WarmupPeriodFilter();
    //   component->subscribe(signalID, warmupFilter);
    //   ... SignalSource(warmupFilter);
    //}

    // Note: filterRef==NULL is also valid input, need to be prepared for it!
    ResultFilter *filter = filterRef ? new MeanFilter() : NULL; //FIXME lookup from filterRef->getName()!!!

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

void StatisticRecorderParser::parse(const SignalSource& source, const char *mode, bool scalarsEnabled, bool vectorsEnabled, cComponent *component, const char *statisticName, const char *where)
{
    Expression expr;
    XResolver resolver(component);  //FIXME need a different resolver, because plain names should be interpreted as ResultFilters or ResultRecorders
    expr.parse(mode, &resolver);
    printf("EXPR: %s --> %s\n", mode, expr.str().c_str());
    int len = expr.getExpressionLength();
    const Expression::Elem *e = expr.getExpression();

    //FIXME this does not do anything -- we need something like with StatisticSourceParser
    throw opp_runtime_error("result recorder expressions not implemented yet");
}


