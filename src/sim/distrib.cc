//==========================================================================
// file distrib.cc
//
// Random variate generation
//
// @author Werner Sandmann (ws), Kay Michael Masslow (kmm)
//
// @date 11/26/2002 "doxygenification" (kmm)
// @date 11/20/2002 some final comments (ws)
// @date 10/22/2002 implemented various discrete distributions (kmm)
//
//==========================================================================

#include <float.h>
#include <math.h>
#include "distrib.h"
#include "macros.h"
#include "cexception.h"

//
// Linux - <math.h> supplies a PI, MS does not...
//
#ifndef M_PI
#define M_PI    3.141592654
#endif

#ifndef M_E
#define M_E     2.718281828
#endif

//
// FIXME TBD:
//   add asserts for parameters
//   find a way to verify/test distributions
//


//----------------------------------------------------------------------------
//
//  C O N T I N U O U S
//
//----------------------------------------------------------------------------

double uniform(double a, double b, int rng)
{
    return a + genk_dblrand(rng) * (b-a);
}

double exponential(double p, int rng)
{
    return -p * log(genk_dblrand(rng));
}

double normal(double m, double d, int rng)
{
    return m + d *  sqrt(-2.0*log(genk_dblrand(rng)))*cos(PI*2*genk_dblrand(rng));
}

double truncnormal(double m, double d, int rng)
{
    double res;
    do {
         res = normal(m,d,rng);
    } while(res<0);

    return res;
}

double gamma_d(double alpha, double beta, int rng)
{
    if (alpha<=0 || beta<=0)
        throw new cException("gamma(): alpha and beta params must be positive "
                             "(alpha=%lg, beta=%lg)", alpha, beta);

    // FIXME: temporarily disabled because of compile warning:
    // "not all control paths return a value"
    throw new cException("gamma_d() incomplete!");

    if (fabs(alpha - 1.0) <= DBL_EPSILON)
        return exponential(beta);

    if (0.0 < alpha && alpha < 1.0)
    {
        double b, U1, U2, P, Y;

        unsigned int step = 1;

        b = (M_E + alpha) / M_E;
        for (;;)
        {
            switch (step)
            {
            case 1:
                U1 = normal(0, 1);
                P = b * U1;
                if (P > 1)
                    step = 3;
                else
                    step = 2;
                break;

            case 2:
                Y = pow(P, (1 / alpha));
                U2 = normal(0, 1);
                if (U2 <= exp(-Y))
                    return Y;
                else
                    step = 1;
                break;

            case 3:
                Y = -log((b - P) / alpha);
                U2 = normal(0, 1);
                if (U2 <= pow(Y, alpha - 1.0))
                    return Y;
                else
                    step = 1;
                break;
            }
        }
    }

    if (alpha > 1.0)
    {
        double a = 1.0 / sqrt(2.0 * alpha - 1);
        double b = alpha - log(4.0);
        double q = alpha + 1.0 / a;
        double theta = 4.5;
        double d = 1 + log(theta);

        unsigned int step = 1;
        for (;;)
        {
            double U1, U2, V, Y, Z, W;

            switch (step)
            {
            case 1:
                U1 = genk_dblrand(rng);
                U2 = genk_dblrand(rng);
                step = 2;
                break;

            case 2:
                V = a * log(U1 / (1.0 - U1));
                Y = alpha * exp(V);
                Z = U1 * U1 * U2;
                W = b + q * V - Y;
                step = 3;
                break;

            case 3:
                if (W + d - theta * Z >= .0)
                    return Y;
                else
                    step = 4;
                break;

            case 4:
                if (W >= log(Z))
                    return Y;
                else
                    step = 1;
                break;
            }
        }
    }
}


double beta(double alpha1, double alpha2, int rng)
{
    if (alpha1<=0 || alpha2<=0)
        throw new cException("beta(): alpha1 and alpha2 parameters must be positive "
                             "(alpha1=%lg, alpha2=%lg)", alpha1, alpha2);

    double Y1 = gamma_d(alpha1, 1.0, rng);
    double Y2 = gamma_d(alpha2, 2.0, rng);

    return Y1 / (Y1 + Y2);
}


double erlang_k(unsigned int k, double m, int rng)
{
    double U = 1.0;
    for (unsigned int i = 0; i < k; i++)
        U *= (1.0 - genk_dblrand(rng));

    return -(m / (double) k) * log(U);
}


double chi_square(unsigned int k, int rng)
{
    if (!(k % 2))
        return erlang_k(k >> 1, k, rng);
    else
        return gamma_d((double) k / 2.0, 2.0, rng);
}


double student_t(unsigned int i, int rng)
{
    double Z = normal(0, 1);
    double W = sqrt(chi_square(i, rng) / (double) i);
    return Z / W;
}


double cauchy(double a, double b, int rng)
{
    if (b<=0)
        throw new cException("cauchy(): parameters must be b>0 (a=%lg, b=%lg, c=%lg)", a, b);

    return a + b * tan(M_PI * genk_dblrand(rng));
}


double triang(double a, double b, double c, int rng)
{
    if (b<a || c<b || a==c)
        throw new cException("triang(): parameters must be a<=b<=c, a<c (a=%lg, b=%lg, c=%lg)", a, b, c);

    double U, beta, T;

    U = genk_dblrand(rng);
    beta = (b - a) / (c - a);

    if (U < beta)
        T = sqrt(beta * U);
    else
        T = 1.0 - sqrt((1.0 - beta) * (1.0 - U));

    return a + (c - a) * T;
}


// lognormal() is inline


double weibull(double a, double b, int rng)
{
    if (a<=0 || b<=0)
        throw new cException("weibull(): a,b parameters must be positive (a=%lg, b=%lg)", a, b);

    return a * pow(-log(1.0 - genk_dblrand(rng)), 1.0 / b);
}


double pareto_shifted(double a, double b, double c, int rng)
{
    // FIXME: check arg values
    double u_pow = pow(1.0 - genk_dblrand(rng), 1.0 / a);
    return (b - c * u_pow) / u_pow;
}

//----------------------------------------------------------------------------
//
//  D I S C R E T E
//
//----------------------------------------------------------------------------

// helper function, needed for hypergeometric distribution
static double _factorial(int n)
{
    if (n<0)
        throw new cException("internal error: _factorial() called with n=%d",n);

    double fact = 1.0;
    while (n>1)
        fact *= n--;
    return fact;
}


int intuniform(int a, int b, int rng)
{
    return a + genk_intrand(rng) % (b-a+1);
}


// bernoulli() is inline


int binomial(int n, double p, int rng)
{
    int X = 0;
    // sum up n bernoulli trials
    for (int i = 0; i < n; i++)
    {
        double U = genk_dblrand(rng);
        if (p > U)
            X++;
    }
    return X;
}


int geometric(double p, int rng)
{
    double a = 1.0 / (log(1.0 - p));
    return (int)floor(a * log(genk_dblrand(rng)));
}


int negbinomial(int n, double p, int rng)
{
    int X = 0;
    for (int i = 0; i < n; i++)
    {
        X += geometric(p, rng);
    }
    return X;
}


int hypergeometric(int a, int b, int n, int rng)
{
    if (a<0 || b<0 || n>a+b)
        throw new cException("hypergeometric(): params must be a>=0, b>=0, n=<a+b "
                             "(a=%d, b=%d, n=%d)", a,b,n);

    double U = genk_dblrand(rng);
    double alpha = _factorial(b) / (double) _factorial(a + b - n);
    double beta = _factorial(b - n) / (double) _factorial(a + b);
    double A = alpha / beta;
    double B = A;
    int X = 0;

    while (U > A)
    {
        X++;
        B *= double(a - X) * double(n - X) / ((X + 1.0) * (b - n + X + 1.0));
        A += B;
    }

    return X;
}


int poisson(double lambda, int rng)
{
    int X;
    if (lambda > 30.0)
    {
        double a = M_PI * sqrt(lambda / 3.0);
        double b = a / lambda;
        double c = 0.767 - 3.36 / lambda;
        double d = log(c) - log(b) - lambda;
        double U, V, Y;

        do
        {
            do
            {
                U = genk_dblrand(rng);
                Y = (a - log((1.0 - U) / U)) / b;
            }
            while (Y <= -0.5);

            X = (int)floor(Y + 0.5);
            V = genk_dblrand(rng);
        }
        while (a - b * Y + log(V / 1.0 + pow(exp(a - b * Y), 2.0)) > d + X * log(lambda) - log(X));
    }
    else
    {
        double a = exp(-lambda);
        double p = 1.0;
        X = -1;

        while (p > a)
        {
            p *= genk_dblrand(rng);
            X++;
        }
    }
    return X;
}

//---------------------------------------------------
//
//  Registration for NED
//
//---------------------------------------------------

// continuous

static double _dbl_erlang_k(double k, double m)
{
    return erlang_k( (unsigned int) k, m);
}

static double _dbl_chi_square( double k )
{
    return chi_square( (unsigned int) k);
}

static double _dbl_student_t( double i )
{
    return student_t( (unsigned int) i);
}

Define_Function(uniform, 2);
Define_Function(exponential, 1);
Define_Function(normal, 2);
Define_Function(truncnormal, 2);
Define_Function(gamma_d, 2);
Define_Function(beta, 2);
Define_Function2(erlang_k, _dbl_erlang_k, 2);
Define_Function2(chi_square, _dbl_chi_square, 1);
Define_Function2(student_t, _dbl_student_t, 1);
Define_Function(cauchy, 2);
Define_Function(triang, 3);
Define_Function(lognormal, 2);
Define_Function(weibull, 2);
Define_Function(pareto_shifted, 3);

// discrete

static double _dbl_intuniform(double a, double b)
{
    return (double) intuniform((int)a, (int)b);
}

static double _dbl_bernoulli(double p)
{
    return (double) bernoulli(p);
}

static double _dbl_binomial(double n, double p)
{
    return (double) binomial( (int)n, p);
}

static double _dbl_geometric(double p)
{
    return (double) geometric(p);
}

static double _dbl_negbinomial( double n, double p)
{
    return (double) negbinomial( (int)n, p);
}

static double _dbl_hypergeometric( double a, double b, double n)
{
    return (double) hypergeometric( (int)a, (int)b, (int)n);
}

static double _dbl_poisson(double lambda)
{
    return (double) poisson(lambda);
}

Define_Function2(intuniform, _dbl_intuniform, 2);
Define_Function2(bernoulli, _dbl_bernoulli, 1);
Define_Function2(binomial, _dbl_binomial, 2);
Define_Function2(geometric, _dbl_geometric, 1);
Define_Function2(negbinomial, _dbl_negbinomial, 2);
Define_Function2(hypergeometric, _dbl_hypergeometric, 2);
Define_Function2(poisson, _dbl_poisson, 1);


// compatibility genk_ versions:

double genk_uniform(double rng, double a, double b)
{
    return uniform(a, b, (int)rng);
}

double genk_intuniform(double rng, double a, double b)
{
    return uniform((int)a, (int)b, (int)rng);
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


