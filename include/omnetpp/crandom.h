//==========================================================================
//   CRANDOM.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CRANDOM_H
#define __OMNETPP_CRANDOM_H

#include <sstream>
#include "cownedobject.h"

namespace omnetpp {

class cRNG;

/**
 * @brief Abstract interface for random variate generator classes.
 *
 * @ingroup RandomNumbers
 */
class SIM_API cRandom : public cOwnedObject
{
    protected:
        cRNG *rng;

    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cRandom(cRNG *rng);
        cRandom(const char *name=nullptr, cRNG *rng=nullptr);
        virtual ~cRandom() {}
        //@}

        /** @name Parameters. */
        //@{
        /**
         * Sets the random number generator to use for random variate generation.
         */
        virtual void setRNG(cRNG *rng) {this->rng = rng;}

        /**
         * Returns the RNG used for random variate generation.
         */
        cRNG *getRNG() const {return rng;}
        //@}

        /** @name Random number generation */
        //@{
        /**
         * Returns a random variate from the distribution represented by this object.
         */
        virtual double draw() const = 0;
        //@}
};

/**
 * @brief Generates random numbers from the uniform distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>uniform(cRNG *rng, double a, double b)</code>
 * function.
 *
 * @ingroup RandomNumbersContCont
 */
class SIM_API cUniform : public cRandom
{
    protected:
        double a;
        double b;
    private:
        void copy(const cUniform& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cUniform(cRNG *rng, double a, double b) : cRandom(rng), a(a), b(b) {}
        cUniform(const char *name=nullptr, cRNG *rng=nullptr, double a=0, double b=1) : cRandom(name, rng), a(a), b(b) {}
        cUniform(const cUniform& other) : cRandom(other) {copy(other);}
        virtual cUniform *dup() const override {return new cUniform(*this);}
        cUniform& operator =(const cUniform& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setA(double a) {this->a = a;}
        double getA() const {return a;}
        void setB(double b) {this->b = b;}
        double getB() const {return b;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the exponential distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>exponential(cRNG *rng, double mean)</code> function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cExponential : public cRandom
{
    protected:
        double mean;
    private:
        void copy(const cExponential& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cExponential(cRNG *rng, double mean) : cRandom(rng), mean(mean) {}
        cExponential(const char *name=nullptr, cRNG *rng=nullptr, double mean=1) : cRandom(rng), mean(mean) {}
        cExponential(const cExponential& other) : cRandom(other) {copy(other);}
        virtual cExponential *dup() const override {return new cExponential(*this);}
        const cExponential& operator =(const cExponential& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setMean(double mean) {this->mean = mean;}
        double getMean() const {return mean;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the normal distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>normal(cRNG *rng, double mean, double stddev)</code>
 * function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cNormal : public cRandom
{
    protected:
        double mean;
        double stddev;
    private:
        void copy(const cNormal& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cNormal(cRNG *rng, double mean, double stddev) : cRandom(rng), mean(mean), stddev(stddev) {}
        cNormal(const char *name=nullptr, cRNG *rng=nullptr, double mean=0, double stddev=1) : cRandom(rng), mean(mean), stddev(stddev) {}
        cNormal(const cNormal& other) : cRandom(other) {copy(other);}
        virtual cNormal *dup() const override {return new cNormal(*this);}
        cNormal& operator =(const cNormal& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setMean(double mean) {this->mean = mean;}
        double getMean() const {return mean;}
        void setStddev(double stddev) {this->stddev = stddev;}
        double getStddev() const {return stddev;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the truncated normal distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>truncnormal(cRNG *rng, double mean, double stddev)</code>
 * function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cTruncNormal : public cRandom
{
    protected:
        double mean;
        double stddev;
    private:
        void copy(const cTruncNormal& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cTruncNormal(cRNG *rng, double mean, double stddev) : cRandom(rng), mean(mean), stddev(stddev) {}
        cTruncNormal(const char *name=nullptr, cRNG *rng=nullptr, double mean=0, double stddev=1) : cRandom(rng), mean(mean), stddev(stddev) {}
        cTruncNormal(const cTruncNormal& other) : cRandom(other) {copy(other);}
        virtual cTruncNormal *dup() const override {return new cTruncNormal(*this);}
        cTruncNormal& operator =(const cTruncNormal& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setMean(double mean) {this->mean = mean;}
        double getMean() const {return mean;}
        void setStddev(double stddev) {this->stddev = stddev;}
        double getStddev() const {return stddev;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the gamma distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>gamma_d(cRNG *rng, double alpha, double theta)</code>
 * function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cGamma : public cRandom
{
    protected:
        double alpha;
        double theta;
    private:
        void copy(const cGamma& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cGamma(cRNG *rng, double alpha, double theta) : cRandom(rng), alpha(alpha), theta(theta) {}
        cGamma(const char *name=nullptr, cRNG *rng=nullptr, double alpha=1, double theta=1) : cRandom(rng), alpha(alpha), theta(theta) {}
        cGamma(const cGamma& other) : cRandom(other) {copy(other);}
        virtual cGamma *dup() const override {return new cGamma(*this);}
        cGamma& operator =(const cGamma& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setAlpha(double alpha) {this->alpha = alpha;}
        double getAlpha() const {return alpha;}
        void setTheta(double theta) {this->theta = theta;}
        double getTheta() const {return theta;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the beta distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>beta(cRNG *rng, double alpha1, double alpha2)</code>
 * function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cBeta : public cRandom
{
    protected:
        double alpha1;
        double alpha2;
    private:
        void copy(const cBeta& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cBeta(cRNG *rng, double alpha1, double alpha2) : cRandom(rng), alpha1(alpha1), alpha2(alpha2) {}
        cBeta(const char *name=nullptr, cRNG *rng=nullptr, double alpha1=1, double alpha2=1) : cRandom(rng), alpha1(alpha1), alpha2(alpha2) {}
        cBeta(const cBeta& other) : cRandom(other) {copy(other);}
        virtual cBeta *dup() const override {return new cBeta(*this);}
        cBeta& operator =(const cBeta& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setAlpha1(double alpha1) {this->alpha1 = alpha1;}
        double getAlpha1() const {return alpha1;}
        void setAlpha2(double alpha2) {this->alpha2 = alpha2;}
        double getAlpha2() const {return alpha2;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the Erlang distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>erlang_k(cRNG *rng, unsigned int k, double mean)</code>
 * function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cErlang : public cRandom
{
    protected:
        unsigned int k;
        double mean;
    private:
        void copy(const cErlang& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cErlang(cRNG *rng, unsigned int k, double mean) : cRandom(rng), k(k), mean(mean) {}
        cErlang(const char *name=nullptr, cRNG *rng=nullptr, unsigned int k=1, double mean=1) : cRandom(rng), k(k), mean(mean) {}
        cErlang(const cErlang& other) : cRandom(other) {copy(other);}
        virtual cErlang *dup() const override {return new cErlang(*this);}
        cErlang& operator =(const cErlang& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setK(unsigned int k) {this->k = k;}
        unsigned int getK() const {return k;}
        void setMean(double mean) {this->mean = mean;}
        double getMean() const {return mean;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the chi-square distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>chi_square(cRNG *rng, unsigned int k)</code>
 * function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cChiSquare : public cRandom
{
    protected:
        unsigned int k;
    private:
        void copy(const cChiSquare& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cChiSquare(cRNG *rng, unsigned int k) : cRandom(rng), k(k) {}
        cChiSquare(const char *name=nullptr, cRNG *rng=nullptr, unsigned int k=1) : cRandom(rng), k(k) {}
        cChiSquare(const cChiSquare& other) : cRandom(other) {copy(other);}
        virtual cChiSquare *dup() const override {return new cChiSquare(*this);}
        cChiSquare& operator =(const cChiSquare& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setK(unsigned int k) {this->k = k;}
        unsigned int getK() const {return k;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from Student's T distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>student_t(cRNG *rng, unsigned int i)</code>
 * function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cStudentT : public cRandom
{
    protected:
        unsigned int i;
    private:
        void copy(const cStudentT& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cStudentT(cRNG *rng, unsigned int i) : cRandom(rng), i(i) {}
        cStudentT(const char *name=nullptr, cRNG *rng=nullptr, unsigned int i=1) : cRandom(rng), i(i) {}
        cStudentT(const cStudentT& other) : cRandom(other) {copy(other);}
        virtual cStudentT *dup() const override {return new cStudentT(*this);}
        cStudentT& operator =(const cStudentT& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setI(unsigned int i) {this->i = i;}
        unsigned int getI() const {return i;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the Cauchy distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>cauchy(cRNG *rng, double a, double b)</code>
 * function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cCauchy : public cRandom
{
    protected:
        double a;
        double b;
    private:
        void copy(const cCauchy& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cCauchy(cRNG *rng, double a, double b) : cRandom(rng), a(a), b(b) {}
        cCauchy(const char *name=nullptr, cRNG *rng=nullptr, double a=0, double b=1) : cRandom(rng), a(a), b(b) {}
        cCauchy(const cCauchy& other) : cRandom(other) {copy(other);}
        virtual cCauchy *dup() const override {return new cCauchy(*this);}
        cCauchy& operator =(const cCauchy& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setA(double a) {this->a = a;}
        double getA() const {return a;}
        void setB(double b) {this->b = b;}
        double getB() const {return b;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the triangular distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>triang(cRNG *rng, double a, double b, double c)</code>
 * function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cTriang : public cRandom
{
    protected:
        double a;
        double b;
        double c;
    private:
        void copy(const cTriang& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cTriang(cRNG *rng, double a, double b, double c) : cRandom(rng), a(a), b(b), c(c) {}
        cTriang(const char *name=nullptr, cRNG *rng=nullptr, double a=-1, double b=0, double c=1) : cRandom(rng), a(a), b(b), c(c) {}
        cTriang(const cTriang& other) : cRandom(other) {copy(other);}
        virtual cTriang *dup() const override {return new cTriang(*this);}
        cTriang& operator =(const cTriang& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setA(double a) {this->a = a;}
        double getA() const {return a;}
        void setB(double b) {this->b = b;}
        double getB() const {return b;}
        void setC(double c) {this->c = c;}
        double getC() const {return c;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the Weibull distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>weibull(cRNG *rng, double a, double b)</code>
 * function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cWeibull : public cRandom
{
    protected:
        double a;
        double b;
    private:
        void copy(const cWeibull& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cWeibull(cRNG *rng, double a, double b) : cRandom(rng), a(a), b(b) {}
        cWeibull(const char *name=nullptr, cRNG *rng=nullptr, double a=1, double b=1) : cRandom(rng), a(a), b(b) {}
        cWeibull(const cWeibull& other) : cRandom(other) {copy(other);}
        virtual cWeibull *dup() const override {return new cWeibull(*this);}
        cWeibull& operator =(const cWeibull& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setA(double a) {this->a = a;}
        double getA() const {return a;}
        void setB(double b) {this->b = b;}
        double getB() const {return b;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the shifted Pareto distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>pareto_shifted(cRNG *rng, double a, double b, double c)</code>
 * function.
 *
 * @ingroup RandomNumbersCont
 */
class SIM_API cParetoShifted : public cRandom
{
    protected:
        double a;
        double b;
        double c;
    private:
        void copy(const cParetoShifted& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cParetoShifted(cRNG *rng, double a, double b, double c) : cRandom(rng), a(a), b(b), c(c) {}
        cParetoShifted(const char *name=nullptr, cRNG *rng=nullptr, double a=1, double b=1, double c=0) : cRandom(rng), a(a), b(b), c(c) {}
        cParetoShifted(const cParetoShifted& other) : cRandom(other) {copy(other);}
        virtual cParetoShifted *dup() const override {return new cParetoShifted(*this);}
        cParetoShifted& operator =(const cParetoShifted& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setA(double a) {this->a = a;}
        double getA() const {return a;}
        void setB(double b) {this->b = b;}
        double getB() const {return b;}
        void setC(double c) {this->c = c;}
        double getC() const {return c;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

// discrete:

/**
 * @brief Generates random numbers from the discrete uniform distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>intuniform(cRNG *rng, int a, int b)</code>
 * function.
 *
 * @ingroup RandomNumbersDiscr
 */
class SIM_API cIntUniform : public cRandom
{
    protected:
        int a;
        int b;
    private:
        void copy(const cIntUniform& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cIntUniform(cRNG *rng, int a, int b) : cRandom(rng), a(a), b(b) {}
        cIntUniform(const char *name=nullptr, cRNG *rng=nullptr, int a=0, int b=1) : cRandom(rng), a(a), b(b) {}
        cIntUniform(const cIntUniform& other) : cRandom(other) {copy(other);}
        virtual cIntUniform *dup() const override {return new cIntUniform(*this);}
        cIntUniform& operator =(const cIntUniform& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setA(int a) {this->a = a;}
        int getA() const {return a;}
        void setB(int b) {this->b = b;}
        int getB() const {return b;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the discrete uniform distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>intuniformexcl(cRNG *rng, int a, int b)</code>
 * function.
 *
 * @ingroup RandomNumbersDiscr
 */
class SIM_API cIntUniformExcl : public cRandom
{
    protected:
        int a;
        int b;
    private:
        void copy(const cIntUniformExcl& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cIntUniformExcl(cRNG *rng, int a, int b) : cRandom(rng), a(a), b(b) {}
        cIntUniformExcl(const char *name=nullptr, cRNG *rng=nullptr, int a=0, int b=1) : cRandom(rng), a(a), b(b) {}
        cIntUniformExcl(const cIntUniformExcl& other) : cRandom(other) {copy(other);}
        virtual cIntUniformExcl *dup() const override {return new cIntUniformExcl(*this);}
        cIntUniformExcl& operator =(const cIntUniformExcl& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setA(int a) {this->a = a;}
        int getA() const {return a;}
        void setB(int b) {this->b = b;}
        int getB() const {return b;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers that are the results of Bernoulli trials.
 *
 * This class is an object-oriented wrapper around the
 * <code>bernoulli(cRNG *rng, double p)</code>
 * function.
 *
 * @ingroup RandomNumbersDiscr
 */
class SIM_API cBernoulli : public cRandom
{
    protected:
        double p;
    private:
        void copy(const cBernoulli& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cBernoulli(cRNG *rng, double p) : cRandom(rng), p(p) {}
        cBernoulli(const char *name=nullptr, cRNG *rng=nullptr, double p=0.5) : cRandom(rng), p(p) {}
        cBernoulli(const cBernoulli& other) : cRandom(other) {copy(other);}
        virtual cBernoulli *dup() const override {return new cBernoulli(*this);}
        cBernoulli& operator =(const cBernoulli& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setP(double p) {this->p = p;}
        double getP() const {return p;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the binomial distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>binomial(cRNG *rng, int n, double p)</code>
 * function.
 *
 * @ingroup RandomNumbersDiscr
 */
class SIM_API cBinomial : public cRandom
{
    protected:
        int n;
        double p;
    private:
        void copy(const cBinomial& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cBinomial(cRNG *rng, int n, double p) : cRandom(rng), n(n), p(p) {}
        cBinomial(const char *name=nullptr, cRNG *rng=nullptr, int n=1, double p=0.5) : cRandom(rng), n(n), p(p) {}
        cBinomial(const cBinomial& other) : cRandom(other) {copy(other);}
        virtual cBinomial *dup() const override {return new cBinomial(*this);}
        cBinomial& operator =(const cBinomial& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setN(int n) {this->n = n;}
        int getN() const {return n;}
        void setP(double p) {this->p = p;}
        double getP() const {return p;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the geometric distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>geometric(cRNG *rng, double p)</code>
 * function.
 *
 * @ingroup RandomNumbersDiscr
 */
class SIM_API cGeometric : public cRandom
{
    protected:
        double p;
    private:
        void copy(const cGeometric& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cGeometric(cRNG *rng, double p) : cRandom(rng), p(p) {}
        cGeometric(const char *name=nullptr, cRNG *rng=nullptr, double p=0.5) : cRandom(rng), p(p) {}
        cGeometric(const cGeometric& other) : cRandom(other) {copy(other);}
        virtual cGeometric *dup() const override {return new cGeometric(*this);}
        cGeometric& operator =(const cGeometric& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setP(double p) {this->p = p;}
        double getP() const {return p;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the negative binomial distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>negbinomial(cRNG *rng, int n, double p)</code>
 * function.
 *
 * @ingroup RandomNumbersDiscr
 */
class SIM_API cNegBinomial : public cRandom
{
    protected:
        int n;
        double p;
    private:
        void copy(const cNegBinomial& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cNegBinomial(cRNG *rng, int n, double p) : cRandom(rng), n(n), p(p) {}
        cNegBinomial(const char *name=nullptr, cRNG *rng=nullptr, int n=0, double p=0.5) : cRandom(rng), n(n), p(p) {}
        cNegBinomial(const cNegBinomial& other) : cRandom(other) {copy(other);}
        virtual cNegBinomial *dup() const override {return new cNegBinomial(*this);}
        cNegBinomial& operator =(const cNegBinomial& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setN(int n) {this->n = n;}
        int getN() const {return n;}
        void setP(double p) {this->p = p;}
        double getP() const {return p;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

/**
 * @brief Generates random numbers from the Poisson distribution.
 *
 * This class is an object-oriented wrapper around the
 * <code>poisson(cRNG *rng, double lambda)</code>
 * function.
 *
 * @ingroup RandomNumbersDiscr
 */
class SIM_API cPoisson : public cRandom
{
    protected:
        double lambda;
    private:
        void copy(const cPoisson& other);
    public:
        /** @name Constructors, destructor, etc. */
        //@{
        cPoisson(cRNG *rng, double lambda) : cRandom(rng), lambda(lambda) {}
        cPoisson(const char *name=nullptr, cRNG *rng=nullptr, double lambda=1) : cRandom(rng), lambda(lambda) {}
        cPoisson(const cPoisson& other) : cRandom(other) {copy(other);}
        virtual cPoisson *dup() const override {return new cPoisson(*this);}
        cPoisson& operator =(const cPoisson& other);
        virtual std::string str() const override;
        //@}

        /** @name Parameters. */
        //@{
        void setLambda(double lambda) {this->lambda = lambda;}
        double getLambda() const {return lambda;}
        //@}

        /** @name Random number generation. */
        //@{
        virtual double draw() const override;
        //@}
};

}  // namespace omnetpp

#endif

