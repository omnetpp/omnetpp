//==========================================================================
// file distrib.h
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


#ifndef __DISTRIB_H_
#define __DISTRIB_H_

#include "defs.h"
#include "random.h"


/**
 * @name Continuous distributions
 *
 * Literature:
 *
 * -# LawKelton: A.M. Law and W.D. Kelton, Simulation Modeling and Analysis,
 *     3rd ed., McGraw Hill, 2000.
 *
 * -# Banks: J. Banks: Handbook of Simulation, Wiley, 1998.
 *
 * @ingroup RandomNumbers
 */
//@{

/**
 * Returns a random variate with uniform distribution in the range [a,b).
 *
 * @param a, b the interval, a<b
 * @param rng the underlying random number generator
 */
SIM_API double uniform(double a, double b, int rng=0);

/**
 * Returns a random variate from the exponential distribution with the
 * given mean (that is, with parameter lambda=1/mean).
 *
 * @param mean mean value
 * @param rng the underlying random number generator
 */
SIM_API double exponential(double mean, int rng=0);

/**
 * Returns a random variate from the normal distribution with the given mean
 * and variance.
 *
 * @param mean mean of the normal distribution
 * @param variance variance of the normal distribution
 * @param rng the underlying random number generator
 */
SIM_API double normal(double mean, double variance, int rng=0);

/**
 * Normal distribution truncated to nonnegative values.
 * It is implemented with a loop that discards negative values until
 * a nonnegative one comes. This means that the execution time is not bounded:
 * a large negative mean with much smaller variance will likely result
 * in a large number of iterations.
 *
 * The mean and variance given as parameters refer are parameters
 * to the normal distribution <i>before</i> truncation. The actual
 * random variate returned will have a different mean and variance.
 *
 * @param mean mean of the normal distribution
 * @param variance variance of the normal distribution
 * @param rng the underlying random number generator
 */
SIM_API double truncnormal(double mean, double variance, int rng=0);

/**
 * Returns a random variate from the gamma distribution with parameters
 * alpha, beta > 0.
 *
 * Generation method depends on the value of alpha:
 *
 *   - alpha=1: gamma_d(1,beta)=exponential(beta)
 *   - 0<alpha<1: Acceptance-Rejection due to Ahrends/Dieter
 *     (see LawKelton page 463, Algo 1.-3.)
 *   - alpha>1: Acceptance-Rejection due to Cheng
 *     (see LawKelton page 464, Algo 1.-4.)
 *
 * @remark the name gamma_d is chosen to avoid ambiguity with
 * a function of the same name
 *
 * @param alpha >0
 * @param beta  >0
 * @param rng the underlying random number generator
 */
SIM_API double gamma_d(double alpha, double beta, int rng=0);

/**
 * Returns a random variate from the beta distribution with parameters
 * alpha1, alpha2.
 *
 * Generation is using relationship to Gamma distribution.
 *
 * @param alpha1
 * @param alpha2 >0
 * @param rng the underlying random number generator
 */
SIM_API double beta(double alpha1, double alpha2, int rng=0);

/**
 * Returns a random variate from the Erlang distribution with k phases
 * and parameter m.
 *
 * Generation is similar to exponential distribution, using
 * the fact that exponential distributions sum up to Erlang.
 *
 * @param k phases
 * @param m >0
 * @param rng the underlying random number generator
 */
SIM_API double erlang_k(unsigned int k, double m, int rng=0);

/**
 * Returns a random variate from the chi-square distribution
 * with k degrees of freedom.  The chi-square distribution arises
 * in statistics. If Yi are k independent random variates from the normal
 * distribution with unit variance, then the sum-of-squares (sum(Yi^2))
 * has a chi-square distribution with k degrees of freedom.
 *
 * Generation is using relationship to Gamma distribution.
 *
 * @param k degrees of freedom
 * @param rng the underlying random number generator
 */
SIM_API double chi_square(unsigned int k, int rng=0);

/**
 * Returns a random variate from the Student-t distribution with
 * i degrees of freedom. If Y1 has a normal distribution and Y2 has a chi-square
 * distribution with k degrees of freedom then X = Y1 / sqrt(Y2/k)
 * has a Student-t distribution with k degrees of freedom.
 *
 * Generation is using relationship to Gamma and Chi-Square.
 *
 * @param i degrees of freedom
 * @param rng the underlying random number generator
 */
SIM_API double student_t(unsigned int i, int rng=0);

/**
 * Returns a random variate from the Cauchy distribution with parameters a,b
 * where b>0.
 *
 * Generation uses inverse transform.
 *
 * @param a
 * @param b  b>0
 * @param rng the underlying random number generator
 */
SIM_API double cauchy(double a, double b, int rng=0);

/**
 * Returns a random variate from the triangular distribution with parameters
 * a < b < c.
 *
 * Generation uses inverse transform.
 *
 * @param a, b, c   a < b < c
 * @param rng the underlying random number generator
 */
SIM_API double triang(double a, double b, double c, int rng=0);

/**
 * Returns a random variate from the lognormal distribution with mean m
 * and variance s>0.
 *
 * Generation is using relationship to normal distribution.
 *
 * @param m  mean
 * @param s  variance, s>0
 * @param rng the underlying random number generator
 */
inline double lognormal(double m, double s, int rng=0)
{
    return exp(normal(m, s, rng));
}

/**
 * Returns a random variate from the Weibull distribution with parameters
 * a, b > 0.
 *
 * Generation uses inverse transform.
 *
 * @param a, b  > 0
 * @param rng the underlying random number generator
 */
SIM_API double weibull(double a, double b, int rng=0);

/**
 * Returns a random variate from the shifted generalized Pareto distribution.
 *
 * Generation uses inverse transform.
 *
 * @param a,b  the usual parameters for generalized Pareto
 * @param c    shift parameter for left-shift
 * @param rng the underlying random number generator
 */
SIM_API double pareto_shifted(double a, double b, double c, int rng=0);

//@}

/**
 * @name Discrete distributions
 *
 * Literature:
 *
 * -# LawKelton: A.M. Law and W.D. Kelton, Simulation Modeling and Analysis,
 *     3rd ed., McGraw Hill, 2000.
 *
 * -# Banks: J. Banks: Handbook of Simulation, Wiley, 1998.
 *
 * @ingroup RandomNumbers
 */
//@{

/**
 * Returns a random integer with uniform distribution in the range [a,b],
 * inclusive. (Note that the function can also return b.)
 *
 * @param a, b  the interval, a<b
 * @param rng the underlying random number generator
 */
SIM_API int intuniform(int a, int b, int rng=0);

/**
 * Returns the result of a Bernoulli trial with probability p,
 * that is, 1 with probability p and 0 with probability (1-p).
 *
 * Generation is using elementary look-up.
 *
 * @param p  0=<p<=1
 * @param rng the underlying random number generator
 */
inline int bernoulli(double p, int rng=0)
{
    double U = genk_dblrand(rng);
    return (p > U) ? 1 : 0;
}

/**
 * Returns a random integer from the Binomial distribution with
 * parameters n and p, that is, the number of successes in n independent
 * trials with probability p.
 *
 * Generation is using the relationship to Bernoulli distribution (runtime
 * is proportional to n).
 *
 * @param n n>=0
 * @param p 0<=p<=1
 * @param rng the underlying random number generator
 */
SIM_API int binomial(int n, double p, int rng=0);

/**
 * Returns a random integer from the geometric distribution with parameter p,
 * that is, the number of independent trials with probability p until the
 * first success.
 *
 * Generation uses inverse transform.
 *
 * @param p  0<p<=1
 * @param rng the underlying random number generator
 */
SIM_API int geometric(double p, int rng=0);

/**
 * Returns a random integer from the negative binomial distribution with
 * parameters n and p, that is, the number of failures occurring before
 * n successes in independent trials with probability p of success.
 *
 * Generation is using the relationship to geometric distribution (runtime is
 * proportional to n).
 *
 * @param n  n>=0
 * @param p  0<p<1
 * @param rng the underlying random number generator
 */
SIM_API int negbinomial(int n, double p, int rng=0);

/**
 * Returns a random integer from the hypergeometric distribution with
 * parameters a,b and n.
 *
 * Generation according to inverse transform due to Fishman
 * (see Banks, page 165)
 *
 * @param a, b  a,b>0
 * @param n     0<=n<=a+b
 * @param rng the underlying random number generator
 */
SIM_API int hypergeometric(int a, int b, int n, int rng=0);

/**
 * Returns a random integer from the Poisson distribution with parameter lambda.
 *
 * Generation method depends on value of lambda:
 *
 *   - 0<lambda<=30: count number of events
 *   - lambda>30: Acceptance-Rejection due to Atkinson
 *     (see Banks, page 166)
 *
 * @param lambda  > 0
 * @param rng the underlying random number generator
 */
SIM_API int poisson(double lambda, int rng=0);

//@}

/**
 * @name Compatibility
 *
 * @ingroup RandomNumbers
 */
//@{
/**
 * Same as uniform(), only uses random generator
 * gen_nr instead of generator 0.
 */
SIM_API double genk_uniform(double gen_nr, double a, double b);

/**
 * Same as intuniform(), only uses random generator
 * gen_nr instead of generator 0.
 */
SIM_API double genk_intuniform(double gen_nr, double a, double b);

/**
 * Same as exponential(), only uses random generator
 * gen_nr instead of generator 0.
 */
SIM_API double genk_exponential(double gen_nr, double p);

/**
 * Same as normal(), only uses random generator
 * gen_nr instead of generator 0.
 */
SIM_API double genk_normal(double gen_nr, double mean, double variance);

/**
 * Same as truncnormal(), only uses random generator
 * gen_nr instead of generator 0.
 */
SIM_API double genk_truncnormal(double gen_nr, double mean, double variance);
//@}

//==========================================================================
//=== Implementation of utility functions:

inline double dblrand()
{
   return (double)intrand() / (double)((unsigned long)INTRAND_MAX+1UL);
}

inline double genk_dblrand(int gen_nr)
{
   return (double)genk_intrand(gen_nr) / (double)((unsigned long)INTRAND_MAX+1UL);
}

#endif


