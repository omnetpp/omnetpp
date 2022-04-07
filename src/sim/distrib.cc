//==========================================================================
// DISTRIB.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Random variate generation
//
// @author Werner Sandmann (ws), Kay Michael Masslow (kmm), Kyeong Soo (Joseph) Kim (jk)
//
// @date 11/26/2002 "doxygenification" (kmm)
// @date 11/20/2002 some final comments (ws)
// @date 10/22/2002 implemented various discrete distributions (kmm)
//
//==========================================================================

#include <cfloat>
#include <cmath>
#include "omnetpp/distrib.h"
#include "omnetpp/globals.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/cexception.h"
#include "omnetpp/platdep/platmisc.h" // M_PI

#ifndef M_E
#define M_E     2.7182818284590452353602874713527
#endif

namespace omnetpp {

//----------------------------------------------------------------------------
//
//  C O N T I N U O U S
//
//----------------------------------------------------------------------------

double uniform(cRNG *rng, double a, double b)
{
    if (a > b)
        throw cRuntimeError("uniform(): Wrong parameters a=%g and b=%g: a <= b required", a, b);
    return a + rng->doubleRand() * (b-a);
}

double exponential(cRNG *rng, double p)
{
    return -p *log(1.0 - rng->doubleRand());
}

double unit_normal(cRNG *rng)
{
    double U = 1.0 - rng->doubleRand();
    double V = 1.0 - rng->doubleRand();
    return sqrt(-2.0*log(U)) * cos(M_PI*2*V);
}

double normal(cRNG *rng, double m, double d)
{
    double U = 1.0 - rng->doubleRand();
    double V = 1.0 - rng->doubleRand();
    return m + d * sqrt(-2.0*log(U)) * cos(M_PI*2*V);
}

double truncnormal(cRNG *rng, double m, double d)
{
    double res;
    do {
        res = normal(rng, m, d);
    } while (res < 0);

    return res;
}

/*
 * internal, for alpha<1. THIS IMPLEMENTATION SEEMS TO BE BOGUS, we use
 * gamma_MarsagliaTransf() instead.
 */
/*
static double gamma_AhrensDieter74(cRNG *rng, double alpha)
{
    ASSERT(alpha<1);

    double b = (M_E + alpha) / M_E;

    double Y;
    for (;;)
    {
        double U1, U2, P;

        // step 1
        U1 = unit_normal(rng);
        P = b * U1;
        if (P > 1)
        {
            // step 3
            Y = -log((b - P) / alpha);
            U2 = unit_normal(rng);
            if (U2 <= pow(Y, alpha - 1.0))
                break; // accept Y
        }
        else
        {
            // step 2
            Y = pow(P, (1 / alpha));
            U2 = unit_normal(rng);
            if (U2 <= exp(-Y))
                break;  // accept Y
        }
    }
    return Y;
}
*/

/*
 * internal, for alpha>1. Currently unused, we use gamma_Marsaglia2000() instead.
 */
/*
static double gamma_ChengFeast79(cRNG *rng, double alpha)
{
    ASSERT(alpha>1);

    double a = 1.0 / sqrt(2.0 * alpha - 1);
    double b = alpha - log(4.0);
    double q = alpha + 1.0 / a;
    double theta = 4.5;
    double d = 1 + log(theta);

    double Y;
    for (;;)
    {
        double U1, U2, V, Z, W;

        // step 1
        U1 = rng->doubleRand();
        U2 = rng->doubleRand();

        // step 2
        V = a * log(U1 / (1.0 - U1));
        Y = alpha * exp(V);
        Z = U1 * U1 * U2;
        W = b + q * V - Y;

        // step 3
        if (W + d - theta * Z >= .0)
            break;  // accept Y

        // step 4
        if (W >= log(Z))
            break;  // accept Y
    }
    return Y;
}
*/

/*
 * internal, for alpha>1.
 *
 * From: "A Simple Method for Generating Gamma Variables", George Marsaglia and
 * Wai Wan Tsang, ACM Transactions on Mathematical Software, Vol. 26, No. 3,
 * September 2000. Available online.
 */
static double gamma_Marsaglia2000(cRNG *rng, double a)
{
    ASSERT(a > 1);

    double d, c, x, v, u;
    d = a - 1.0/3.0;
    c = 1.0/sqrt(9.0*d);
    for (;;) {
        do {
            x = unit_normal(rng);
            v = 1.0 + c*x;
        } while (v <= 0);
        v = v*v*v;
        u = rng->doubleRand();
        if (u < 1.0 - 0.0331*(x*x)*(x*x))
            return d*v;
        if (log(u) < 0.5*x*x + d*(1.0-v+log(v)))
            return d*v;
    }
}

/*
 * internal, for alpha<1.
 *
 * We can derive the alpha<1 case from the alpha>1 case. See "Note" at the
 * end of Section 6 of the Marsaglia2000 paper (see above).
 */
static double gamma_MarsagliaTransf(cRNG *rng, double alpha)
{
    ASSERT(alpha < 1);

    // return gamma_ChengFeast79(rng, 1+alpha) * pow(rng->doubleRand(), 1/alpha);
    return gamma_Marsaglia2000(rng, 1+alpha) * pow(rng->doubleRand(), 1/alpha);  // faster
}

double gamma_d(cRNG *rng, double alpha, double theta)
{
    if (alpha <= 0 || theta <= 0)
        throw cRuntimeError("gamma(): Wrong parameters alpha=%g, theta=%g: Both must be positive", alpha, theta);

    if (fabs(alpha - 1.0) <= DBL_EPSILON) {
        return exponential(rng, theta);
    }
    else if (alpha < 1.0) {
        // return theta * gamma_AhrensDieter74(rng, alpha); // implementation is bogus, see above
        return theta * gamma_MarsagliaTransf(rng, alpha);
    }
    else {  // if (alpha > 1.0)
            // return theta * gamma_ChengFeast79(rng, alpha);
        return theta * gamma_Marsaglia2000(rng, alpha);  // faster
    }
}

double beta(cRNG *rng, double alpha1, double alpha2)
{
    if (alpha1 <= 0 || alpha2 <= 0)
        throw cRuntimeError("beta(): Wrong parameters alpha1=%g, alpha2=%g: Both must be positive", alpha1, alpha2);

    double Y1 = gamma_d(rng, alpha1, 1.0);
    double Y2 = gamma_d(rng, alpha2, 1.0);

    return Y1 / (Y1 + Y2);
}

double erlang_k(cRNG *rng, unsigned int k, double m)
{
    double U = 1.0;
    for (unsigned int i = 0; i < k; i++)
        U *= (1.0 - rng->doubleRand());

    return -(m / (double)k) * log(U);
}

double chi_square(cRNG *rng, unsigned int k)
{
    if (!(k % 2))
        return erlang_k(rng, k >> 1, k);
    else
        return gamma_d(rng, (double)k / 2.0, 2.0);
}

double student_t(cRNG *rng, unsigned int i)
{
    double Z = normal(rng, 0, 1);
    double W = sqrt(chi_square(rng, i) / (double)i);
    return Z / W;
}

double cauchy(cRNG *rng, double a, double b)
{
    if (b <= 0)
        throw cRuntimeError("cauchy(): Wrong parameters a=%g, b=%g: b>0 expected", a, b);

    return a + b * tan(M_PI * rng->doubleRand());
}

double triang(cRNG *rng, double a, double b, double c)
{
    if (b < a || c < b || a == c)
        throw cRuntimeError("triang(): Wrong parameters a=%g, b=%g, c=%g: a<=b<=c, a<c expected", a, b, c);

    double U, beta, T;

    U = rng->doubleRand();
    beta = (b - a) / (c - a);

    if (U < beta)
        T = sqrt(beta * U);
    else
        T = 1.0 - sqrt((1.0 - beta) * (1.0 - U));

    return a + (c - a) * T;
}

// lognormal() is inline

double weibull(cRNG *rng, double a, double b)
{
    if (a <= 0 || b <= 0)
        throw cRuntimeError("weibull(): Wrong parameters a=%g, b=%g: Both must be positive", a, b);

    return a * pow(-log(1.0 - rng->doubleRand()), 1.0 / b);
}

double pareto_shifted(cRNG *rng, double a, double b, double c)
{
    if (a == 0)
        throw cRuntimeError("pareto_shifted(): Wrong parameter a=0: Cannot be zero");

    double u_pow = pow(1.0 - rng->doubleRand(), 1.0 / a);
    return b / u_pow - c;
}

//----------------------------------------------------------------------------
//
//  D I S C R E T E
//
//----------------------------------------------------------------------------

/*
// helper function, needed for hypergeometric distribution
static double _factorial(cRNG *rng, int n)
{
    if (n<0)
        throw cRuntimeError("internal error: _factorial() called with n=%d",n);

    double fact = 1.0;
    while (n>1)
        fact *= n--;
    return fact;
}
*/


int intuniform(cRNG *rng, int a, int b)
{
    if (a > b)
        throw cRuntimeError("intuniform(): Wrong parameters a=%d and b=%d: a <= b required", a, b);
    int64_t range = (int64_t)b - (int64_t)a + 1;
    if (range > rng->intRandMax())
        throw cRuntimeError("intuniform(): Wrong parameters a=%d and b=%d: Interval is greater than RNG range", a, b); // could work around but nobody will need it
    return a + rng->intRand((uint32_t)range);
}

int intuniformexcl(cRNG *rng, int a, int b)
{
    if (a >= b)
        throw cRuntimeError("intuniformexcl(): Wrong parameters a=%d and b=%d: a < b required", a, b);
    int64_t range = (int64_t)b - (int64_t)a;
    if (range > rng->intRandMax())
        throw cRuntimeError("intuniformexcl(): Wrong parameters a=%d and b=%d: Interval is greater than RNG range", a, b); // could work around but nobody will need it
    return a + rng->intRand((uint32_t)range);
}

// bernoulli() is inline

int binomial(cRNG *rng, int n, double p)
{
    int X = 0;
    // sum up n bernoulli trials
    for (int i = 0; i < n; i++) {
        double U = rng->doubleRand();
        if (p > U)
            X++;
    }
    return X;
}

int geometric(cRNG *rng, double p)
{
    if (p < 0 || p >= 1)
        throw cRuntimeError("geometric(): Wrong parameter p=%g: Out of range [0,1)", p);

    double a = 1.0 / (log(1.0 - p));
    return (int)floor(a * log(1.0 - rng->doubleRand()));
}

int negbinomial(cRNG *rng, int n, double p)
{
    int X = 0;
    for (int i = 0; i < n; i++) {
        X += geometric(rng, p);
    }
    return X;
}

/*
 * TBD: hypergeometric() doesn't work yet
 *
int hypergeometric(cRNG *rng, int a, int b, int n)
{
    if (a<0 || b<0 || n>a+b)
        throw cRuntimeError("hypergeometric(): Wrong parameters a=%d, b=%d, n=%d: a>=0, b>=0, n=<a+b expected", a,b,n);

    double U = rng->doubleRand();
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
*/

int poisson(cRNG *rng, double lambda)
{
    int X;
    if (lambda > 30.0) {
        double a = M_PI * sqrt(lambda / 3.0);
        double b = a / lambda;
        double c = 0.767 - 3.36 / lambda;
        double d = log(c) - log(b) - lambda;
        double U, V, Y;

        do {
            do {
                U = rng->doubleRand();
                Y = (a - log((1.0 - U) / U)) / b;
            } while (Y <= -0.5);

            X = (int)floor(Y + 0.5);
            V = rng->doubleRand();
        } while (a - b * Y + log(V / 1.0 + pow(exp(a - b * Y), 2.0)) > d + X * log(lambda) - log(double(X)));
    }
    else {
        double a = exp(-lambda);
        double p = 1.0;
        X = -1;

        while (p > a) {
            p *= rng->doubleRand();
            X++;
        }
    }
    return X;
}

}  // namespace omnetpp

