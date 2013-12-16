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

#include <float.h>
#include <math.h>
#include "distrib.h"
#include "globals.h"
#include "cnedmathfunction.h"
#include "cexception.h"

//
// Linux - <math.h> supplies a PI, MS does not...
//
#ifndef M_PI
#define M_PI    3.1415926535897932384626433832795
#endif

#ifndef M_E
#define M_E     2.7182818284590452353602874713527
#endif

NAMESPACE_BEGIN


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
    return -p * log(1.0 - genk_dblrand(rng));
}

double unit_normal(int rng)
{
    double U = 1.0 - genk_dblrand(rng);
    double V = 1.0 - genk_dblrand(rng);
    return sqrt(-2.0*log(U)) * cos(PI*2*V);
}

double normal(double m, double d, int rng)
{
    double U = 1.0 - genk_dblrand(rng);
    double V = 1.0 - genk_dblrand(rng);
    return m + d * sqrt(-2.0*log(U)) * cos(PI*2*V);
}

double truncnormal(double m, double d, int rng)
{
    double res;
    do {
         res = normal(m,d,rng);
    } while(res<0);

    return res;
}

/*
 * internal, for alpha<1. THIS IMPLEMENTATION SEEMS TO BE BOGUS, we use
 * gamma_MarsagliaTransf() instead.
 */
/*
static double gamma_AhrensDieter74(double alpha, int rng)
{
    ASSERT(alpha<1);

    double b = (M_E + alpha) / M_E;

    double Y;
    for (;;)
    {
        double U1, U2, P;

        // step 1
        U1 = normal(0, 1, rng);
        P = b * U1;
        if (P > 1)
        {
            // step 3
            Y = -log((b - P) / alpha);
            U2 = normal(0, 1, rng);
            if (U2 <= pow(Y, alpha - 1.0))
                break; // accept Y
        }
        else
        {
            // step 2
            Y = pow(P, (1 / alpha));
            U2 = normal(0, 1, rng);
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
static double gamma_ChengFeast79(double alpha, int rng)
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
        U1 = genk_dblrand(rng);
        U2 = genk_dblrand(rng);

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
static double gamma_Marsaglia2000(double a, int rng)
{
    ASSERT(a>1);

    double d,c,x,v,u;
    d = a - 1.0/3.0;
    c = 1.0/sqrt(9.0*d);
    for(;;)
    {
        do {x = unit_normal(rng); v = 1.0 + c*x;} while (v<=0);
        v = v*v*v; u = genk_dblrand(rng);
        if (u < 1.0 - 0.0331*(x*x)*(x*x))
            return d*v;
        if (log(u)<0.5*x*x + d*(1.0-v+log(v)))
            return d*v;
    }
}

/*
 * internal, for alpha<1.
 *
 * We can derive the alpha<1 case from the alpha>1 case. See "Note" at the
 * end of Section 6 of the Marsaglia2000 paper (see above).
 */
static double gamma_MarsagliaTransf(double alpha, int rng)
{
    ASSERT(alpha<1);

    //return gamma_ChengFeast79(1+alpha,rng) * pow(genk_dblrand(rng), 1/alpha);
    return gamma_Marsaglia2000(1+alpha,rng) * pow(genk_dblrand(rng), 1/alpha); // faster
}

double gamma_d(double alpha, double theta, int rng)
{
    if (alpha<=0 || theta<=0)
        throw cRuntimeError("gamma(): alpha and theta params must be positive "
                            "(alpha=%g, theta=%g)", alpha, theta);

    if (fabs(alpha - 1.0) <= DBL_EPSILON)
    {
        return exponential(theta, rng);
    }
    else if (alpha < 1.0)
    {
        //return theta * gamma_AhrensDieter74(alpha, rng); // implementation is bogus, see above
        return theta * gamma_MarsagliaTransf(alpha, rng);
    }
    else // if (alpha > 1.0)
    {
        //return theta * gamma_ChengFeast79(alpha, rng);
        return theta * gamma_Marsaglia2000(alpha, rng);  // faster
    }
}


double beta(double alpha1, double alpha2, int rng)
{
    if (alpha1<=0 || alpha2<=0)
        throw cRuntimeError("beta(): alpha1 and alpha2 parameters must be positive "
                            "(alpha1=%g, alpha2=%g)", alpha1, alpha2);

    double Y1 = gamma_d(alpha1, 1.0, rng);
    double Y2 = gamma_d(alpha2, 1.0, rng);

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
    double Z = normal(0, 1, rng);
    double W = sqrt(chi_square(i, rng) / (double) i);
    return Z / W;
}


double cauchy(double a, double b, int rng)
{
    if (b<=0)
        throw cRuntimeError("cauchy(): parameters must be b>0 (a=%g, b=%g)", a, b);

    return a + b * tan(M_PI * genk_dblrand(rng));
}


double triang(double a, double b, double c, int rng)
{
    if (b<a || c<b || a==c)
        throw cRuntimeError("triang(): parameters must be a<=b<=c, a<c (a=%g, b=%g, c=%g)", a, b, c);

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
        throw cRuntimeError("weibull(): a,b parameters must be positive (a=%g, b=%g)", a, b);

    return a * pow(-log(1.0 - genk_dblrand(rng)), 1.0 / b);
}


double pareto_shifted(double a, double b, double c, int rng)
{
    if (a==0)
        throw cRuntimeError("pareto_shifted(): parameter a cannot be zero)");

    double u_pow = pow(1.0 - genk_dblrand(rng), 1.0 / a);
    return b / u_pow - c;
}


//----------------------------------------------------------------------------
//
//  D I S C R E T E
//
//----------------------------------------------------------------------------

/*
// helper function, needed for hypergeometric distribution
static double _factorial(int n)
{
    if (n<0)
        throw cRuntimeError("internal error: _factorial() called with n=%d",n);

    double fact = 1.0;
    while (n>1)
        fact *= n--;
    return fact;
}
*/


int intuniform(int a, int b, int rng)
{
    return a + genk_intrand(rng, b-a+1);
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
    if (p<0 || p>=1)
        throw cRuntimeError("geometric(): parameter p=%g out of range [0,1)", p);

    double a = 1.0 / (log(1.0 - p));
    return (int)floor(a * log(1.0 - genk_dblrand(rng)));
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


/*
 * TBD: hypergeometric() doesn't work yet
 *
int hypergeometric(int a, int b, int n, int rng)
{
    if (a<0 || b<0 || n>a+b)
        throw cRuntimeError("hypergeometric(): params must be a>=0, b>=0, n=<a+b "
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
*/

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
        while (a - b * Y + log(V / 1.0 + pow(exp(a - b * Y), 2.0)) > d + X * log(lambda) - log(double(X)));
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

NAMESPACE_END
