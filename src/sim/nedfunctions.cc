//=========================================================================
//  NEDFUNCTIONS.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <string.h>
#include "distrib.h"
#include "cmathfunction.h"
#include "cnedfunction.h"
#include "cexception.h"
#include "unitconversion.h"
#include "stringutil.h"
#include "opp_ctype.h"

USING_NAMESPACE

//TODO add "list handling" functions (like Tcl lists): lindex "SL->S", "SL->D" etc
//FIXME cDynamicExpression to add function name to exceptions thrown from functions

void nedfunctions_dummy() {} //see util.cc

typedef cDynamicExpression::Value Value;  // abbreviation for local use

#define DEF(NAME, SIGNATURE, BODY) \
    static Value NAME##_f(cComponent *context, Value argv[], int argc) {BODY} \
    Define_NED_Function2(NAME, NAME##_f, SIGNATURE);


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


DEF(fabs, "Q->Q", {
    argv[0].dbl = fabs(argv[0].dbl);  // preserve unit
    return argv[0];
})

DEF(fmod, "QQ->Q", {
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = fmod(argv[0].dbl, argv1converted);
    return argv[0];
})

DEF(min, "QQ->Q", {
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    return argv[0].dbl < argv1converted ? argv[0] : argv[1];
})

DEF(max, "QQ->Q", {
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    return argv[0].dbl < argv1converted ? argv[1] : argv[0];
})


//
// Unit handling
//

static cStringPool stringPool;

DEF(dropunit, "Q->D", {
    argv[0].dblunit = NULL;
    return argv[0];
})

DEF(replaceunit, "QS->Q", {
    argv[0].dblunit = stringPool.get(argv[1].str.c_str());
    return argv[0];
})

DEF(convertunit, "QS->Q", {
    const char *newUnit = stringPool.get(argv[1].str.c_str());
    argv[0].dbl = UnitConversion::convertUnit(argv[0].dbl, argv[0].dblunit, newUnit);
    argv[0].dblunit = newUnit;
    return argv[0];
})

DEF(unitof, "Q->S", {
    return argv[0].dblunit;
})


//
// String manipulation functions.
//

DEF(length, "S->L", {
    return (long)argv[0].str.size();
})

DEF(contains, "SS->B", {
    return argv[0].str.find(argv[1].str) != std::string::npos;
})

DEF(substring, "SL/L->S", {  // Note: substring(str,index[,length]), i.e. length is optional
    int size = argv[0].str.size();
    int index = (int)argv[1].dbl;
    int length = argc==3 ? (int)argv[2].dbl : size-index;

    if (index < 0 || index > size)
        throw cRuntimeError("substring(): index out of range");
    if (length < 0)
        throw cRuntimeError("substring(): length is negative");
    return argv[0].str.substr(index, length);
})

DEF(startswith, "SS->B", {
    return argv[0].str.find(argv[1].str) == 0;
})

DEF(endswith, "SS->B", {
    return argv[0].str.rfind(argv[1].str) == argv[0].str.size() - argv[1].str.size();
})

DEF(tail, "SL->S", {
    int length = (int)argv[1].dbl;
    if (length < 0)
        throw cRuntimeError("tail(): length is negative");
    int size = (int) argv[0].str.size();
    return argv[0].str.substr(std::max(0, size - length), size);
})

DEF(replace, "SSS->S", {
    std::string str = argv[0].str;
    std::string& search = argv[1].str;
    std::string& replacement = argv[2].str;

    unsigned int searchSize = search.size();
    unsigned int replacementSize = replacement.size();
    unsigned int  index = 0;
    while ((index = str.find(search, index)) != std::string::npos) {
        str.replace(index, searchSize, replacement);
        index += replacementSize - searchSize + 1;
    }
    return str;
})

DEF(indexof, "SS->L", {
    return (long)argv[0].str.find(argv[1].str);
})

DEF(toupper, "S->S", {
    std::string tmp = argv[0].str;
    int length = tmp.length();
    for (int i=0; i<length; i++)
        tmp[i] = opp_toupper(tmp[i]);
    return tmp;
})

DEF(tolower, "S->S", {
    std::string tmp = argv[0].str;
    int length = tmp.length();
    for (int i=0; i<length; i++)
        tmp[i] = opp_tolower(tmp[i]);
    return tmp;
})

DEF(int, "*->L", {
    switch (argv[0].type) {
        case Value::BOOL:
            return argv[0].bl ? 1L : 0L;
        case Value::DBL:
            return (long)floor(argv[0].dbl);
        case Value::STR:
            return (long)floor(opp_atof(argv[0].str.c_str()));  //XXX catch & wrap exception?
        case Value::XML:
            throw cRuntimeError("int(): cannot convert xml to int");
        default:
            throw cRuntimeError("internal error: bad Value type");
    }
})

DEF(double, "*->D", {
    switch (argv[0].type) {
        case Value::BOOL:
            return argv[0].bl ? 1.0 : 0.0;
        case Value::DBL:
            return argv[0].dbl;
        case Value::STR:
            return opp_atof(argv[0].str.c_str());  //XXX catch & wrap exception?
        case Value::XML:
            throw cRuntimeError("double(): cannot convert xml to double");
        default:
            throw cRuntimeError("internal error: bad Value type");
    }
})

DEF(string, "*->S", {
    return argv[0].toString();
})

//
// Random variate generation.
//

// continuous
DEF(uniform, "QQ/L->Q", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = uniform(argv[0].dbl, argv1converted, rng);
    return argv[0];
})

DEF(exponential, "Q/L->Q", {
    int rng = argc==2 ? (int)argv[1].dbl : 0;
    argv[0].dbl = exponential(argv[0].dbl, rng);
    return argv[0];
})

DEF(normal, "QQ/L->Q", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = normal(argv[0].dbl, argv1converted, rng);
    return argv[0];
})

DEF(truncnormal, "QQ/L->Q", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = truncnormal(argv[0].dbl, argv1converted, rng);
    return argv[0];
})

DEF(gamma_d, "DQ/L->Q", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[1].dbl = gamma_d(argv[0].dbl, argv[1].dbl, rng);
    return argv[1];
})

DEF(beta, "DD/L->D", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[0].dbl = beta(argv[0].dbl, argv[1].dbl, rng);
    return argv[0];
})

DEF(erlang_k, "LQ/L->Q", {
    if (argv[0].dbl < 0.0)
       throw cRuntimeError("erlang_k(): k parameter (number of phases) must be positive "
                           "(k=%g", argv[0].dbl);
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[1].dbl = erlang_k((unsigned int)argv[0].dbl, argv[1].dbl, rng);
    return argv[1];
})

DEF(chi_square, "L/L->D", {
    if (argv[0].dbl < 0.0)
       throw cRuntimeError("chi_square(): k parameter (degrees of freedom) must be positive "
                           "(k=%g", argv[0].dbl);
    int rng = argc==2 ? (int)argv[1].dbl : 0;
    argv[0].dbl = chi_square((unsigned int)argv[0].dbl, rng);
    return argv[0];
})

DEF(student_t, "L/L->D", {
    if (argv[0].dbl < 0.0)
       throw cRuntimeError("student_t(): i parameter (degrees of freedom) must be positive "
                           "(i=%g", argv[0].dbl);
    int rng = argc==2 ? (int)argv[1].dbl : 0;
    argv[0].dbl = student_t((unsigned int)argv[0].dbl, rng);
    return argv[0];
})

DEF(cauchy, "QQ/L->Q", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = cauchy(argv[0].dbl, argv1converted, rng);
    return argv[0];
})

DEF(triang, "QQQ/L->Q", {
    int rng = argc==4 ? (int)argv[3].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    double argv2converted = UnitConversion::convertUnit(argv[2].dbl, argv[2].dblunit, argv[0].dblunit);
    argv[0].dbl = triang(argv[0].dbl, argv1converted, argv2converted, rng);
    return argv[0];
})

DEF(lognormal, "DD/L->D", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[0].dbl = lognormal(argv[0].dbl, argv[1].dbl, rng);
    return argv[0];
})

DEF(weibull, "QQ/L->Q", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    double argv1converted = UnitConversion::convertUnit(argv[1].dbl, argv[1].dblunit, argv[0].dblunit);
    argv[0].dbl = weibull(argv[0].dbl, argv1converted, rng);
    return argv[0];
})

DEF(pareto_shifted, "DQQ/L->Q", {
    int rng = argc==4 ? (int)argv[3].dbl : 0;
    double argv2converted = UnitConversion::convertUnit(argv[2].dbl, argv[2].dblunit, argv[1].dblunit);
    argv[1].dbl = pareto_shifted(argv[0].dbl, argv[1].dbl, argv2converted, rng);
    return argv[1];
})

// discrete

DEF(intuniform, "LL/L->L", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[0].dbl = intuniform((int)argv[0].dbl, (int)argv[1].dbl, rng);
    return argv[0];
})

DEF(bernoulli, "D/L->L", {
    int rng = argc==2 ? (int)argv[1].dbl : 0;
    argv[0].dbl = bernoulli(argv[0].dbl, rng);
    return argv[0];
})

DEF(binomial, "LD/L->L", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[0].dbl = binomial((int)argv[0].dbl, argv[1].dbl, rng);
    return argv[0];
})

DEF(geometric, "D/L->L", {
    int rng = argc==2 ? (int)argv[1].dbl : 0;
    argv[0].dbl = geometric(argv[0].dbl, rng);
    return argv[0];
})

DEF(negbinomial, "LD/L->L", {
    int rng = argc==3 ? (int)argv[2].dbl : 0;
    argv[0].dbl = negbinomial((int)argv[0].dbl, argv[1].dbl, rng);
    return argv[0];
})

DEF(poisson, "D/L->L", {
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

