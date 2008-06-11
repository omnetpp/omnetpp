//=========================================================================
//  NEDFUNCTIONS.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <string.h>
#include "distrib.h"
#include "cmathfunction.h"
#include "cnedfunction.h"
#include "cexception.h"
#include "cstringtokenizer.h"
#include "unitconversion.h"
#include "stringutil.h"
#include "opp_ctype.h"

USING_NAMESPACE

//FIXME cDynamicExpression to add function name to exceptions thrown from functions

void nedfunctions_dummy() {} //see util.cc

typedef cDynamicExpression::Value Value;  // abbreviation for local use

#define DEF(NAME, SIGNATURE, CATEGORY, DESCRIPTION, BODY) \
    static Value NAME##_f(cComponent *context, Value argv[], int argc) {BODY} \
    Define_NED_Function3(NAME, NAME##_f, SIGNATURE, CATEGORY, DESCRIPTION);


//
// NED math.h functions
//

Define_Function(acos, 1)
Define_Function(asin, 1)
Define_Function(atan, 1)
Define_Function(atan2, 2)

Define_Function(sin, 1)
Define_Function(cos, 1)
Define_Function(tan, 1)

Define_Function(ceil, 1)
Define_Function(floor, 1)

Define_Function(exp, 1)
Define_Function(pow, 2)
Define_Function(sqrt, 1)

Define_Function(hypot, 2)

Define_Function(log, 1)
Define_Function(log10, 1)


DEF(fabs, "Q->Q", "math", "", {
    argv[0].dbl = fabs(argv[0].dbl);  // preserve unit
    return argv[0];
})

DEF(fmod, "QQ->Q", "math", "", {
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = fmod(argv[0].dbl, argv1converted);
    return argv[0];
})

DEF(min, "QQ->Q", "math", "", {
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    return argv[0].dbl < argv1converted ? argv[0] : argv[1];
})

DEF(max, "QQ->Q", "math", "", {
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    return argv[0].dbl < argv1converted ? argv[1] : argv[0];
})


//
// Unit handling
//

static cStringPool stringPool;

DEF(dropUnit, "Q->D", "unit", "", {
    argv[0].dblunit = NULL;
    return argv[0];
})

DEF(replaceUnit, "QS->Q", "unit", "", {
    argv[0].dblunit = stringPool.get(argv[1].s.c_str());
    return argv[0];
})

DEF(convertUnit, "QS->Q", "unit", "", {
    const char *newUnit = stringPool.get(argv[1].s.c_str());
    argv[0].dbl = UnitConversion::convertUnit(argv[0].dbl, argv[0].dblunit, newUnit);
    argv[0].dblunit = newUnit;
    return argv[0];
})

DEF(unitOf, "Q->S", "unit", "Returns the unit of the given quantity.", {
    return argv[0].dblunit;
})


//
// String manipulation functions.
//

DEF(length, "S->L", "string", "", {
    return (long)argv[0].s.size();
})

DEF(contains, "SS->B", "string", "", {
    return argv[0].s.find(argv[1].s) != std::string::npos;
})

DEF(substring, "SL/L->S", "string", "", {  // Note: substring(str,index[,length]), i.e. length is optional
    int size = argv[0].s.size();
    int index = (int)argv[1].dbl;
    int length = argc==3 ? (int)argv[2].dbl : size-index;

    if (index < 0 || index > size)
        throw cRuntimeError("substring(): index out of bounds");
    if (length < 0)
        throw cRuntimeError("substring(): length is negative");
    return argv[0].s.substr(index, length);
})

DEF(substringBefore, "SS->S", "string", "", {
    unsigned int pos = argv[0].s.find(argv[1].s);
    return pos==std::string::npos ? "" : argv[0].s.substr(0,pos);
})

DEF(substringAfter, "SS->S", "string", "", {
    unsigned int pos = argv[0].s.find(argv[1].s);
    return pos==std::string::npos ? "" : argv[0].s.substr(pos+argv[1].s.size());
})

DEF(substringBeforeLast, "SS->S", "string", "", {
    unsigned int pos = argv[0].s.rfind(argv[1].s);
    return pos==std::string::npos ? "" : argv[0].s.substr(0,pos);
})

DEF(substringAfterLast, "SS->S", "string", "", {
    unsigned int pos = argv[0].s.rfind(argv[1].s);
    return pos==std::string::npos ? "" : argv[0].s.substr(pos+argv[1].s.size());
})

DEF(startsWith, "SS->B", "string", "", {
    return argv[0].s.find(argv[1].s) == 0;
})

DEF(endsWith, "SS->B", "string", "", {
    return argv[0].s.rfind(argv[1].s) == argv[0].s.size() - argv[1].s.size();
})

DEF(tail, "SL->S", "", "string", {
    int length = (int)argv[1].dbl;
    if (length < 0)
        throw cRuntimeError("tail(): length is negative");
    int size = (int) argv[0].s.size();
    return argv[0].s.substr(std::max(0, size - length), size);
})

DEF(replace, "SSS/L->S", "string", "", {
    std::string str = argv[0].s;
    std::string& search = argv[1].s;
    std::string& replacement = argv[2].s;
    int index = 0;
    if (argc==4) {
        index = (int)argv[3].dbl;
        if (index<0 || index>(int)str.size())
            throw cRuntimeError("replace(): start index out of bounds");
    }

    unsigned int searchSize = search.size();
    unsigned int replacementSize = replacement.size();
    while ((index = str.find(search, index)) != (int)std::string::npos) {
        str.replace(index, searchSize, replacement);
        index += replacementSize - searchSize + 1;
    }
    return str;
})

DEF(replaceFirst, "SSS/L->S", "string", "", {
    std::string str = argv[0].s;
    std::string& search = argv[1].s;
    std::string& replacement = argv[2].s;
    int index = 0;
    if (argc==4) {
        index = (int)argv[3].dbl;
        if (index<0 || index>(int)str.size())
            throw cRuntimeError("replaceFirst(): start index out of bounds");
    }

    unsigned int searchSize = search.size();
    if ((index = str.find(search, index)) != (int)std::string::npos)
        str.replace(index, searchSize, replacement);
    return str;
})

DEF(trim, "S->S", "", "string", {
    return opp_trim(argv[0].s.c_str());
})

DEF(indexOf, "SS->L", "string", "", {
    return (long)argv[0].s.find(argv[1].s);
})

DEF(choose, "LS->S", "string", "", {
    int index = (int)argv[0].dbl;
    if (index < 0)
        throw cRuntimeError("choose(): negative index");
    cStringTokenizer tokenizer(argv[1].s.c_str());
    for (int i=0; i<index && tokenizer.hasMoreTokens(); i++)
        tokenizer.nextToken();
    if (!tokenizer.hasMoreTokens())
        throw cRuntimeError("choose(): index out of bounds: %d", index);
    return tokenizer.nextToken();
})

DEF(toUpper, "S->S", "string", "", {
    std::string tmp = argv[0].s;
    int length = tmp.length();
    for (int i=0; i<length; i++)
        tmp[i] = opp_toupper(tmp[i]);
    return tmp;
})

DEF(toLower, "S->S", "string", "", {
    std::string tmp = argv[0].s;
    int length = tmp.length();
    for (int i=0; i<length; i++)
        tmp[i] = opp_tolower(tmp[i]);
    return tmp;
})

DEF(int, "*->L", "conversion", "", {
    switch (argv[0].type) {
        case Value::BOOL:
            return argv[0].bl ? 1L : 0L;
        case Value::DBL:
            return (long)floor(argv[0].dbl);
        case Value::STR:
            return (long)floor(opp_atof(argv[0].s.c_str()));  //XXX catch & wrap exception?
        case Value::XML:
            throw cRuntimeError("int(): cannot convert xml to int");
        default:
            throw cRuntimeError("internal error: bad Value type");
    }
})

DEF(double, "*->D", "conversion", "", {
    switch (argv[0].type) {
        case Value::BOOL:
            return argv[0].bl ? 1.0 : 0.0;
        case Value::DBL:
            return argv[0].dbl;
        case Value::STR:
            return opp_atof(argv[0].s.c_str());  //XXX catch & wrap exception?
        case Value::XML:
            throw cRuntimeError("double(): cannot convert xml to double");
        default:
            throw cRuntimeError("internal error: bad Value type");
    }
})

DEF(string, "*->S", "conversion", "", {
    return argv[0].str();
})


//
// Reflection
//

DEF(fullPath, "->S", "ned", "", {
    return context->getFullPath();
})

DEF(fullName, "->S", "ned", "", {
    return context->getFullName();
})

DEF(parentIndex, "->L", "ned", "", {
    cModule *mod = context->getParentModule();
    if (!mod)
        throw cRuntimeError("parentIndex(): `%s' has no parent module", context->getFullPath().c_str());
    if (!mod->isVector())
        throw cRuntimeError("parentIndex(): module `%s' is not a vector", mod->getFullPath().c_str());
    return (long)mod->getIndex();
})

DEF(ancestorIndex, "L->L", "ned", "", {
    int levels = (int)argv[0].dbl;
    if (levels<0)
        throw cRuntimeError("ancestorIndex(): negative number of levels");
    if (levels==0 && !context->isModule())
        throw cRuntimeError("ancestorIndex(): numlevels==0 and this is not a module");
    cModule *mod = dynamic_cast<cModule *>(context);
    for (int i=0; mod && i<levels; i++)
        mod = mod->getParentModule();
    if (!mod)
        throw cRuntimeError("ancestorIndex(): argument is larger than current nesting level");
    if (!mod->isVector())
        throw cRuntimeError("ancestorIndex(): module `%s' is not a vector", mod->getFullPath().c_str());
    return (long)mod->getIndex();
})


//
// Random variate generation.
//

// continuous
DEF(uniform, "QQ/L->Q", "random", "", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = uniform(argv[0].dbl, argv1converted, rng);
    return argv[0];
})

DEF(exponential, "Q/L->Q", "random", "", {
    int rng = argc==2 ? (int)argv[1].dbl : 0;
    argv[0].dbl = exponential(argv[0].dbl, rng);
    return argv[0];
})

DEF(normal, "QQ/L->Q", "random", "", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = normal(argv[0].dbl, argv1converted, rng);
    return argv[0];
})

DEF(truncnormal, "QQ/L->Q", "random", "", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = truncnormal(argv[0].dbl, argv1converted, rng);
    return argv[0];
})

DEF(gamma_d, "DQ/L->Q", "random", "", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[1].dbl = gamma_d(argv[0].dbl, argv[1].dbl, rng);
    return argv[1];
})

DEF(beta, "DD/L->D", "", "", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[0].dbl = beta(argv[0].dbl, argv[1].dbl, rng);
    return argv[0];
})

DEF(erlang_k, "LQ/L->Q", "random", "", {
    if (argv[0].dbl < 0.0)
       throw cRuntimeError("erlang_k(): k parameter (number of phases) must be positive "
                           "(k=%g", argv[0].dbl);
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[1].dbl = erlang_k((unsigned int)argv[0].dbl, argv[1].dbl, rng);
    return argv[1];
})

DEF(chi_square, "L/L->D", "random", "", {
    if (argv[0].dbl < 0.0)
       throw cRuntimeError("chi_square(): k parameter (degrees of freedom) must be positive "
                           "(k=%g", argv[0].dbl);
    int rng = argc==2 ? (int)argv[1].dbl : 0;
    argv[0].dbl = chi_square((unsigned int)argv[0].dbl, rng);
    return argv[0];
})

DEF(student_t, "L/L->D", "random", "", {
    if (argv[0].dbl < 0.0)
       throw cRuntimeError("student_t(): i parameter (degrees of freedom) must be positive "
                           "(i=%g", argv[0].dbl);
    int rng = argc==2 ? (int)argv[1].dbl : 0;
    argv[0].dbl = student_t((unsigned int)argv[0].dbl, rng);
    return argv[0];
})

DEF(cauchy, "QQ/L->Q", "random", "", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = cauchy(argv[0].dbl, argv1converted, rng);
    return argv[0];
})

DEF(triang, "QQQ/L->Q", "random", "", {
    int rng = argc==4 ? (int)argv[3].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    double argv2converted = UnitConversion::convertUnit(argv[2].dbl, argv[2].dblunit, argv[0].dblunit);
    argv[0].dbl = triang(argv[0].dbl, argv1converted, argv2converted, rng);
    return argv[0];
})

DEF(lognormal, "DD/L->D", "random", "", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[0].dbl = lognormal(argv[0].dbl, argv[1].dbl, rng);
    return argv[0];
})

DEF(weibull, "QQ/L->Q", "random", "", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = weibull(argv[0].dbl, argv1converted, rng);
    return argv[0];
})

DEF(pareto_shifted, "DQQ/L->Q", "", "", {
    int rng = argc==4 ? (int)argv[3].dbl : 0;
    double argv2converted = UnitConversion::convertUnit(argv[2].dbl, argv[2].dblunit, argv[1].dblunit);
    argv[1].dbl = pareto_shifted(argv[0].dbl, argv[1].dbl, argv2converted, rng);
    return argv[1];
})

// discrete

DEF(intuniform, "LL/L->L", "random", "", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[0].dbl = intuniform((int)argv[0].dbl, (int)argv[1].dbl, rng);
    return argv[0];
})

DEF(bernoulli, "D/L->L", "random", "", {
    int rng = argc==2 ? (int)argv[1].dbl : 0;
    argv[0].dbl = bernoulli(argv[0].dbl, rng);
    return argv[0];
})

DEF(binomial, "LD/L->L", "random", "", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[0].dbl = binomial((int)argv[0].dbl, argv[1].dbl, rng);
    return argv[0];
})

DEF(geometric, "D/L->L", "random", "", {
    int rng = argc==2 ? (int)argv[1].dbl : 0;
    argv[0].dbl = geometric(argv[0].dbl, rng);
    return argv[0];
})

DEF(negbinomial, "LD/L->L", "random", "", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[0].dbl = negbinomial((int)argv[0].dbl, argv[1].dbl, rng);
    return argv[0];
})

DEF(poisson, "D/L->L", "random", "", {
    int rng = argc==2 ? (int)argv[1].dbl : 0;
    argv[0].dbl = poisson(argv[0].dbl, rng);
    return argv[0];
})

//
// Meaningful error for obsolete genk_ functions
//

static double obsolete_genk_function(...)
{
    throw cRuntimeError("Obsolete function -- use the one without the \"genk_\" prefix, and rng as last argument");
}

Define_Function2(genk_uniform, (MathFunc3Args)obsolete_genk_function, 3);
Define_Function2(genk_intuniform, (MathFunc3Args)obsolete_genk_function, 3);
Define_Function2(genk_exponential, (MathFunc2Args)obsolete_genk_function, 2);
Define_Function2(genk_normal, (MathFunc3Args)obsolete_genk_function, 3);
Define_Function2(genk_truncnormal, (MathFunc3Args)obsolete_genk_function, 3);

