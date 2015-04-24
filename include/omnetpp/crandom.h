//==========================================================================
//   CRANDOM.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CRANDOM_H
#define __OMNETPP_CRANDOM_H

#include "cownedobject.h"
#include "crng.h"
#include "distrib.h"

NAMESPACE_BEGIN

class cConfiguration;


/**
 * Abstract interface for random variate generator classes.
 *
 * @ingroup RandomNumbers
 */
class SIM_API cRandom : public cOwnedObject
{
  protected:
    cRNG *rng;

  public:
    cRandom(cRNG *rng=NULL) : cOwnedObject(NULL), rng(rng) {}
    cRandom(const char *name, cRNG *rng=NULL) : cOwnedObject(name), rng(rng) {}
    virtual ~cRandom() {}

    //TODO: dup, copy ctor, copy(), parsimPack/unpack and the usual stuff ---> also for subclasses!!!
    //TODO Register_Class()
    //TODO add to sim_std.msg

    /**
     * Sets the random number generator to use for random variate generation.
     */
    virtual void setRNG(cRNG *rng) {this->rng = rng;}

    /**
     * Returns the RNG used for random variate generation.
     */
    cRNG *getRNG() const {return rng;}

    /**
     * Returns a random variate from the distribution represented by this object.
     */
    virtual double draw() const = 0;
};

class SIM_API cUniform : public cRandom
{
   protected:
     double a;
     double b;
   public:
     cUniform(cRNG *rng, double a, double b) : cRandom(rng), a(a), b(b) {}
     virtual double draw() const {return OPP::uniform(rng, a, b);}
};

class SIM_API cExponential : public cRandom
{
   protected:
     double mean;
   public:
     cExponential(cRNG *rng, double mean) : cRandom(rng), mean(mean) {}
     virtual double draw() const {return OPP::exponential(rng, mean);}
};

class SIM_API cNormal : public cRandom
{
   protected:
     double mean;
     double stddev;
   public:
     cNormal(cRNG *rng, double mean, double stddev) : cRandom(rng), mean(mean), stddev(stddev) {}
     virtual double draw() const {return OPP::normal(rng, mean, stddev);}
};

class SIM_API cTruncNormal : public cRandom
{
   protected:
     double mean;
     double stddev;
   public:
     cTruncNormal(cRNG *rng, double mean, double stddev) : cRandom(rng), mean(mean), stddev(stddev) {}
     virtual double draw() const {return OPP::truncnormal(rng, mean, stddev);}
};

class SIM_API cGamma : public cRandom
{
   protected:
     double alpha;
     double theta;
   public:
     cGamma(cRNG *rng, double alpha, double theta) : cRandom(rng), alpha(alpha), theta(theta) {}
     virtual double draw() const {return OPP::gamma_d(rng, alpha, theta);}
};

class SIM_API cBeta : public cRandom
{
   protected:
     double alpha1;
     double alpha2;
   public:
     cBeta(cRNG *rng, double alpha1, double alpha2) : cRandom(rng), alpha1(alpha1), alpha2(alpha2) {}
     virtual double draw() const {return OPP::beta(rng, alpha1, alpha2);}
};

class SIM_API cErlang : public cRandom
{
   protected:
     unsigned int k;
     double mean;
   public:
     cErlang(cRNG *rng, unsigned int k, double mean) : cRandom(rng), k(k), mean(mean) {}
     virtual double draw() const {return OPP::erlang_k(rng, k, mean);}
};

class SIM_API cChiSquare : public cRandom
{
   protected:
     unsigned int k;
   public:
     cChiSquare(cRNG *rng, unsigned int k) : cRandom(rng), k(k) {}
     virtual double draw() const {return OPP::chi_square(rng, k);}
};

class SIM_API cStudentT : public cRandom
{
   protected:
     unsigned int i;
   public:
     cStudentT(cRNG *rng, unsigned int k) : cRandom(rng), i(k) {}
     virtual double draw() const {return OPP::student_t(rng, i);}
};

class SIM_API cCauchy : public cRandom
{
   protected:
     double a;
     double b;
   public:
     cCauchy(cRNG *rng, double a, double b) : cRandom(rng), a(a), b(b) {}
     virtual double draw() const {return OPP::cauchy(rng, a, b);}
};

class SIM_API cTriang : public cRandom
{
   protected:
     double a;
     double b;
     double c;
   public:
     cTriang(cRNG *rng, double a, double b, double c) : cRandom(rng), a(a), b(b), c(c) {}
     virtual double draw() const {return OPP::triang(rng, a, b, c);}
};

class SIM_API cWeibull : public cRandom
{
   protected:
     double a;
     double b;
   public:
     cWeibull(cRNG *rng, double a, double b) : cRandom(rng), a(a), b(b) {}
     virtual double draw() const {return OPP::weibull(rng, a, b);}
};

class SIM_API cParetoShifted : public cRandom
{
   protected:
     double a;
     double b;
     double c;
   public:
     cParetoShifted(cRNG *rng, double a, double b, double c) : cRandom(rng), a(a), b(b), c(c) {}
     virtual double draw() const {return OPP::pareto_shifted(rng, a, b, c);}
};

// discrete:

class SIM_API cIntUniform : public cRandom
{
   protected:
     int a;
     int b;
   public:
     cIntUniform(cRNG *rng, int a, int b) : cRandom(rng), a(a), b(b) {}
     virtual double draw() const {return (double) OPP::intuniform(rng, a, b);}
};

class SIM_API cBinomial : public cRandom
{
   protected:
     int n;
     double p;
   public:
     cBinomial(cRNG *rng, int n, double p) : cRandom(rng), n(n), p(p) {}
     virtual double draw() const {return (double) OPP::binomial(rng, n, p);}
};

class SIM_API cGeometric : public cRandom
{
   protected:
     double p;
   public:
     cGeometric(cRNG *rng, double p) : cRandom(rng), p(p) {}
     virtual double draw() const {return (double) OPP::geometric(rng, p);}
};

class SIM_API cNegBinomial : public cRandom
{
   protected:
     int n;
     double p;
   public:
     cNegBinomial(cRNG *rng, int n, double p) : cRandom(rng), n(n), p(p) {}
     virtual double draw() const {return (double) OPP::negbinomial(rng, n, p);}
};

class SIM_API cPoisson : public cRandom
{
   protected:
     double p;
   public:
     cPoisson(cRNG *rng, double lambda) : cRandom(rng), p(lambda) {}
     virtual double draw() const {return (double) OPP::poisson(rng, p);}
};

NAMESPACE_END

#endif

