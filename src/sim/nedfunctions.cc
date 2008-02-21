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
#include "cfunction.h"
#include "cnedfunction.h"
#include "cexception.h"
#include "opp_ctype.h"

USING_NAMESPACE

typedef cDynamicExpression::Value Value;  // abbreviation for local use

void nedfunctions_dummy() {} //see util.cc

//
// NED math.h functions
//

double min(double a, double b)      {return a<b ? a : b;}
double max(double a, double b)      {return a<b ? b : a;}

Define_Function(min, 2)
Define_Function(max, 2)

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

Define_Function(fabs, 1)
Define_Function(fmod, 2)

Define_Function(hypot, 2)

Define_Function(log, 1)
Define_Function(log10, 1)

//----

//
// NED string manipulation functions.
//

//FIXME check units!
//TODO add: replaceunit(var, "m"); dropunit(var); convertunit(var, "m"); unitof(var)

// length(S) -> L
Value length(cComponent *context, Value args[], int numargs)
{
    return (long)args[0].str.size();
}
Define_NED_Function(length, "S", "L");

// contains(S, S) -> B
Value contains(cComponent *context, Value args[], int numargs)
{
    return args[0].str.find(args[1].str) != std::string::npos;
}
Define_NED_Function(contains, "SS", "B");

// substring(S, L, L) -> S
Value substring(cComponent *context, Value args[], int numargs)
{
    int size = args[0].str.size();
    int index1 = (int)args[1].dbl;
    int index2 = (int)args[2].dbl;

    if (index1 < 0 || index1 > size)
        throw cRuntimeError("substring(): index1 out of range");
    else if (index2 < 0 || index2 > size)
        throw cRuntimeError("substring(): index2 out of range");

    return args[0].str.substr(index1, index2 - index1);
}
Define_NED_Function(substring, "SLL", "S");

// startswith(S, S) -> B
Value startswith(cComponent *context, Value args[], int numargs)
{
    return args[0].str.find(args[1].str) == 0;
}
Define_NED_Function(startswith, "SS", "B");

// endswith(S, S) -> B
Value endswith(cComponent *context, Value args[], int numargs)
{
    return args[0].str.rfind(args[1].str) == args[0].str.size() - args[1].str.size();
}
Define_NED_Function(endswith, "SS", "B");

// startof(S, L) -> S
Value startof(cComponent *context, Value args[], int numargs)
{
    int length = (int)args[1].dbl;

    if (length < 0)
        throw cRuntimeError("startof(): length is negative");

    return args[0].str.substr(0, min(args[0].str.size(), length));
}
Define_NED_Function(startof, "SL", "S");

// endof(S, L) -> S
Value endof(cComponent *context, Value args[], int numargs)
{
    int length = (int)args[1].dbl;

    if (length < 0)
        throw cRuntimeError("endof(): length is negative");

    int size = args[0].str.size();
    return args[0].str.substr(max(0, size - length), size);
}
Define_NED_Function(endof, "SL", "S");

// leftof(S, L) -> S
Value leftof(cComponent *context, Value args[], int numargs)
{
    int size = args[0].str.size();
    int index = (int)args[1].dbl;

    if (index < 0 || index > size)
        throw cRuntimeError("leftof(): index out of range");

    return startof(context, args, numargs);
}
Define_NED_Function(leftof, "SL", "S");

// rightof(S, L) -> S
Value rightof(cComponent *context, Value args[], int numargs)
{
    int size = args[0].str.size();
    int index = (int)args[1].dbl;

    if (index < 0 || index > size)
        throw cRuntimeError("rightof(): index out of range");

    return args[0].str.substr(index, size);
}
Define_NED_Function(rightof, "SL", "S");

// replace(S, S, S) -> S
Value replace(cComponent *context, Value args[], int numargs)
{
    std::string str = args[0].str;
    std::string &search = args[1].str;
    std::string &replacement = args[2].str;

    int searchSize = search.size();
    int replacementSize = replacement.size();
    int index = 0;

    while ((index = str.find(search, index)) != std::string::npos) {
        str.replace(index, searchSize, replacement);
        index += replacementSize - searchSize + 1;
    }

    return str;
}
Define_NED_Function(replace, "SSS", "S");

// indexof(S, S) -> L
Value indexof(cComponent *context, Value args[], int numargs)
{
    return (long)args[0].str.find(args[1].str);
}
Define_NED_Function(indexof, "SS", "L");

// toupper(S) -> S
Value toupper(cComponent *context, Value args[], int numargs)
{
    std::string tmp = args[0].str;
    int length = tmp.length();
    for (int i=0; i<length; i++)
        tmp[i] = opp_toupper(tmp[i]);
    return tmp;
}
Define_NED_Function(toupper, "S", "S");

// tolower(S) -> S
Value tolower(cComponent *context, Value args[], int numargs)
{
    std::string tmp = args[0].str;
    int length = tmp.length();
    for (int i=0; i<length; i++)
        tmp[i] = opp_tolower(tmp[i]);
    return tmp;
}
Define_NED_Function(tolower, "S", "S");

// toint(*) -> L
Value toint(cComponent *context, Value args[], int numargs)
{
    switch (args[0].type) {
        case cDynamicExpression::Value::BOOL:
            return args[0].bl ? 1L : 0L;
        case cDynamicExpression::Value::DBL:
            return (long)floor(args[0].dbl);
        case cDynamicExpression::Value::STR:
            return atol(args[0].str.c_str());
        case cDynamicExpression::Value::XML:
            throw cRuntimeError("toint(): cannot convert xml to int");
        default:
            throw cRuntimeError("internal error: bad Value type");
    }
}
Define_NED_Function(toint, "*", "L");

// todouble(*) -> D
Value todouble(cComponent *context, Value args[], int numargs)
{
    switch (args[0].type) {
        case cDynamicExpression::Value::BOOL:
            return args[0].bl ? 1.0 : 0.0;
        case cDynamicExpression::Value::DBL:
            return args[0].dbl;
        case cDynamicExpression::Value::STR:
            return atof(args[0].str.c_str());
        case cDynamicExpression::Value::XML:
            throw cRuntimeError("todouble(): cannot convert xml to double");
        default:
            throw cRuntimeError("internal error: bad Value type");
    }
}
Define_NED_Function(todouble, "*", "D");

// tostring(*) -> S
Value tostring(cComponent *context, Value args[], int numargs)
{
    return args[0].toString();
}
Define_NED_Function(tostring, "*", "S");



//
// Random variate generation.
//

// continuous

static double _wrap_uniform(double a, double b)
{
    return uniform(a, b);
}

static double _wrap_exponential(double p)
{
    return exponential(p);
}

static double _wrap_normal(double m, double d)
{
    return normal(m, d);
}

static double _wrap_truncnormal(double m, double d)
{
    return truncnormal(m, d);
}

static double _wrap_gamma_d(double alpha, double theta)
{
    return gamma_d(alpha, theta);
}

static double _wrap_beta(double alpha1, double alpha2)
{
    return beta(alpha1, alpha2);
}

static double _wrap_erlang_k(double k, double m)
{
    return erlang_k( (unsigned int) k, m);
}

static double _wrap_chi_square(double k)
{
    return chi_square( (unsigned int) k);
}

static double _wrap_student_t(double i)
{
    return student_t( (unsigned int) i);
}

static double _wrap_cauchy(double a, double b)
{
    return cauchy(a, b);
}

static double _wrap_triang(double a, double b, double c)
{
    return triang(a, b, c);
}

static double _wrap_lognormal(double m, double d)
{
    return lognormal(m, d);
}

static double _wrap_weibull(double a, double b)
{
    return weibull(a, b);
}

static double _wrap_pareto_shifted(double a, double b, double c)
{
    return pareto_shifted(a, b, c);
}

// discrete

static double _wrap_intuniform(double a, double b)
{
    return (double) intuniform((int)a, (int)b);
}

static double _wrap_bernoulli(double p)
{
    return (double) bernoulli(p);
}

static double _wrap_binomial(double n, double p)
{
    return (double) binomial( (int)n, p);
}

static double _wrap_geometric(double p)
{
    return (double) geometric(p);
}

static double _wrap_negbinomial(double n, double p)
{
    return (double) negbinomial( (int)n, p);
}

/* hypergeometric doesn't work yet
static double _wrap_hypergeometric(double a, double b, double n)
{
    return (double) hypergeometric( (int)a, (int)b, (int)n);
}
*/

static double _wrap_poisson(double lambda)
{
    return (double) poisson(lambda);
}


Define_Function2(uniform, _wrap_uniform, 2);
Define_Function2(exponential, _wrap_exponential, 1);
Define_Function2(normal, _wrap_normal, 2);
Define_Function2(truncnormal, _wrap_truncnormal, 2);
Define_Function2(gamma_d, _wrap_gamma_d, 2);
Define_Function2(beta, _wrap_beta, 2);
Define_Function2(erlang_k, _wrap_erlang_k, 2);
Define_Function2(chi_square, _wrap_chi_square, 1);
Define_Function2(student_t, _wrap_student_t, 1);
Define_Function2(cauchy, _wrap_cauchy, 2);
Define_Function2(triang, _wrap_triang, 3);
Define_Function2(lognormal, _wrap_lognormal, 2);
Define_Function2(weibull, _wrap_weibull, 2);
Define_Function2(pareto_shifted, _wrap_pareto_shifted, 3);

Define_Function2(intuniform, _wrap_intuniform, 2);
Define_Function2(bernoulli, _wrap_bernoulli, 1);
Define_Function2(binomial, _wrap_binomial, 2);
Define_Function2(geometric, _wrap_geometric, 1);
Define_Function2(negbinomial, _wrap_negbinomial, 2);
/* hypergeometric doesn't work yet
Define_Function2(hypergeometric, _wrap_hypergeometric, 3);
*/
Define_Function2(poisson, _wrap_poisson, 1);


// continuous, rng versions

static double _wrap_uniform_with_rng(double a, double b, double rng)
{
    return uniform(a, b, (int)rng);
}

static double _wrap_exponential_with_rng(double p, double rng)
{
    return exponential(p, (int)rng);
}

static double _wrap_normal_with_rng(double m, double d, double rng)
{
    return normal(m, d, (int)rng);
}

static double _wrap_truncnormal_with_rng(double m, double d, double rng)
{
    return truncnormal(m, d, (int)rng);
}

static double _wrap_gamma_d_with_rng(double alpha, double theta, double rng)
{
    return gamma_d(alpha, theta, (int)rng);
}

static double _wrap_beta_with_rng(double alpha1, double alpha2, double rng)
{
    return beta(alpha1, alpha2, (int)rng);
}

static double _wrap_erlang_k_with_rng(double k, double m, double rng)
{
    return erlang_k( (unsigned int) k, m, (int)rng);
}

static double _wrap_chi_square_with_rng(double k, double rng)
{
    return chi_square( (unsigned int) k, (int)rng);
}

static double _wrap_student_t_with_rng(double i, double rng)
{
    return student_t( (unsigned int) i, (int)rng);
}

static double _wrap_cauchy_with_rng(double a, double b, double rng)
{
    return cauchy(a, b, (int)rng);
}

static double _wrap_triang_with_rng(double a, double b, double c, double rng)
{
    return triang(a, b, c, (int)rng);
}

static double _wrap_lognormal_with_rng(double m, double d, double rng)
{
    return lognormal(m, d, (int)rng);
}

static double _wrap_weibull_with_rng(double a, double b, double rng)
{
    return weibull(a, b, (int)rng);
}

static double _wrap_pareto_shifted_with_rng(double a, double b, double c, double rng)
{
    return pareto_shifted(a, b, c, (int)rng);
}

// discrete, rng versions

static double _wrap_intuniform_with_rng(double a, double b, double rng)
{
    return (double) intuniform((int)a, (int)b, (int)rng);
}

static double _wrap_bernoulli_with_rng(double p, double rng)
{
    return (double) bernoulli(p, (int)rng);
}

static double _wrap_binomial_with_rng(double n, double p, double rng)
{
    return (double) binomial( (int)n, p, (int)rng);
}

static double _wrap_geometric_with_rng(double p, double rng)
{
    return (double) geometric(p, (int)rng);
}

static double _wrap_negbinomial_with_rng(double n, double p, double rng)
{
    return (double) negbinomial( (int)n, p, (int)rng);
}

/* hypergeometric doesn't work yet
static double _wrap_hypergeometric_with_rng(double a, double b, double n, double rng)
{
    return (double) hypergeometric( (int)a, (int)b, (int)n, (int)rng);
}
*/

static double _wrap_poisson_with_rng(double lambda, double rng)
{
    return (double) poisson(lambda, (int)rng);
}


Define_Function2(uniform, _wrap_uniform_with_rng, 3);
Define_Function2(exponential, _wrap_exponential_with_rng, 2);
Define_Function2(normal, _wrap_normal_with_rng, 3);
Define_Function2(truncnormal, _wrap_truncnormal_with_rng, 3);
Define_Function2(gamma_d, _wrap_gamma_d_with_rng, 3);
Define_Function2(beta, _wrap_beta_with_rng, 3);
Define_Function2(erlang_k, _wrap_erlang_k_with_rng, 3);
Define_Function2(chi_square, _wrap_chi_square_with_rng, 2);
Define_Function2(student_t, _wrap_student_t_with_rng, 2);
Define_Function2(cauchy, _wrap_cauchy_with_rng, 3);
Define_Function2(triang, _wrap_triang_with_rng, 4);
Define_Function2(lognormal, _wrap_lognormal_with_rng, 3);
Define_Function2(weibull, _wrap_weibull_with_rng, 3);
Define_Function2(pareto_shifted, _wrap_pareto_shifted_with_rng, 4);

Define_Function2(intuniform, _wrap_intuniform_with_rng, 3);
Define_Function2(bernoulli, _wrap_bernoulli_with_rng, 2);
Define_Function2(binomial, _wrap_binomial_with_rng, 3);
Define_Function2(geometric, _wrap_geometric_with_rng, 2);
Define_Function2(negbinomial, _wrap_negbinomial_with_rng, 3);
/* hypergeometric doesn't work yet
Define_Function2(hypergeometric, _wrap_hypergeometric_with_rng, 4);
*/
Define_Function2(poisson, _wrap_poisson_with_rng, 2);


// compatibility genk_ versions:

double genk_uniform(double rng, double a, double b)
{
    return uniform(a, b, (int)rng);
}

double genk_intuniform(double rng, double a, double b)
{
    return intuniform((int)a, (int)b, (int)rng);
}

double genk_exponential(double rng, double p)
{
    return exponential(p, (int)rng);
}

double genk_normal(double rng, double m, double d)
{
    return normal(m, d, (int)rng);
}

double genk_truncnormal(double rng, double m, double d)
{
    return truncnormal(m, d, (int)rng);
}

// register functions for findFunction()
Define_Function(genk_uniform, 3);
Define_Function(genk_intuniform, 3);
Define_Function(genk_exponential, 2);
Define_Function(genk_normal, 3);
Define_Function(genk_truncnormal, 3);

//Define_Function(intrand, 1)
//Define_Function(dblrand, 0)


