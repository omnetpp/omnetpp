//==========================================================================
//  CRANDOM.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "omnetpp/crandom.h"
#include "omnetpp/globals.h"
#include "omnetpp/crng.h"
#include "omnetpp/distrib.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/ccomponent.h"

NAMESPACE_BEGIN

Register_Class(cUniform);
Register_Class(cExponential);
Register_Class(cNormal);
Register_Class(cTruncNormal);
Register_Class(cGamma);
Register_Class(cBeta);
Register_Class(cErlang);
Register_Class(cChiSquare);
Register_Class(cStudentT);
Register_Class(cCauchy);
Register_Class(cTriang);
Register_Class(cWeibull);
Register_Class(cParetoShifted);
Register_Class(cIntUniform);
Register_Class(cBernoulli);
Register_Class(cBinomial);
Register_Class(cGeometric);
Register_Class(cNegBinomial);
Register_Class(cPoisson);


cRandom::cRandom(cRNG *rng) : cOwnedObject(nullptr), rng(rng)
{
    if (!rng)
        rng = cSimulation::getActiveSimulation()->getContext()->getRNG(0);
}

cRandom::cRandom(const char *name, cRNG *rng) : cOwnedObject(name), rng(rng)
{
    if (!rng)
        rng = cSimulation::getActiveSimulation()->getContext()->getRNG(0);
}

//----

void cUniform::copy(const cUniform& other)
{
    a = other.a;
    b = other.b;
}

cUniform& cUniform::operator =(const cUniform& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cUniform::info() const
{
    std::stringstream out;
    out << "a=" << a << ", b=" << b;
    return out.str();
}

double cUniform::draw() const
{
    return OPP::uniform(rng, a, b);
}

//----

void cExponential::copy(const cExponential& other)
{
    mean = other.mean;
}

const cExponential& cExponential::operator =(const cExponential& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cExponential::info() const
{
    std::stringstream out;
    out << "mean=" << mean;
    return out.str();
}

double cExponential::draw() const
{
    return OPP::exponential(rng, mean);
}

//----

void cNormal::copy(const cNormal& other)
{
    mean = other.mean;
    stddev = other.stddev;
}

cNormal& cNormal::operator =(const cNormal& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cNormal::info() const
{
    std::stringstream out;
    out << "mean=" << mean << ", stddev=" << stddev;
    return out.str();
}

double cNormal::draw() const
{
    return OPP::normal(rng, mean, stddev);
}

//----

void cTruncNormal::copy(const cTruncNormal& other)
{
    mean = other.mean;
    stddev = other.stddev;
}

cTruncNormal& cTruncNormal::operator =(const cTruncNormal& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cTruncNormal::info() const
{
    std::stringstream out;
    out << "mean=" << mean << ", stddev=" << stddev;
    return out.str();
}

double cTruncNormal::draw() const
{
    return OPP::truncnormal(rng, mean, stddev);
}

//----

void cGamma::copy(const cGamma& other)
{
    alpha = other.alpha;
    theta = other.theta;
}

cGamma& cGamma::operator =(const cGamma& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cGamma::info() const
{
    std::stringstream out;
    out << "alpha=" << alpha << ", theta=" << theta;
    return out.str();
}

double cGamma::draw() const
{
    return OPP::gamma_d(rng, alpha, theta);
}

//----

void cBeta::copy(const cBeta& other)
{
    alpha1 = other.alpha1;
    alpha2 = other.alpha2;
}

cBeta& cBeta::operator =(const cBeta& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cBeta::info() const
{
    std::stringstream out;
    out << "alpha1=" << alpha1 << ", alpha2=" << alpha2;
    return out.str();
}

double cBeta::draw() const
{
    return OPP::beta(rng, alpha1, alpha2);
}

//----

void cErlang::copy(const cErlang& other)
{
    k = other.k;
    mean = other.mean;
}

cErlang& cErlang::operator =(const cErlang& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cErlang::info() const
{
    std::stringstream out;
    out << "k=" << k << ", mean=" << mean;
    return out.str();
}

double cErlang::draw() const
{
    return OPP::erlang_k(rng, k, mean);
}

//----

void cChiSquare::copy(const cChiSquare& other)
{
    k = other.k;
}

cChiSquare& cChiSquare::operator =(const cChiSquare& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cChiSquare::info() const
{
    std::stringstream out;
    out << "k=" << k;
    return out.str();
}

double cChiSquare::draw() const
{
    return OPP::chi_square(rng, k);
}

//----

void cStudentT::copy(const cStudentT& other)
{
    i = other.i;
}

cStudentT& cStudentT::operator =(const cStudentT& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cStudentT::info() const
{
    std::stringstream out;
    out << "i=" << i;
    return out.str();
}

double cStudentT::draw() const
{
    return OPP::student_t(rng, i);
}

//----

void cCauchy::copy(const cCauchy& other)
{
    a = other.a;
    b = other.b;
}

cCauchy& cCauchy::operator =(const cCauchy& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cCauchy::info() const
{
    std::stringstream out;
    out << "a=" << a << ", b=" << b;
    return out.str();
}

double cCauchy::draw() const
{
    return OPP::cauchy(rng, a, b);
}

//----

void cTriang::copy(const cTriang& other)
{
    a = other.a;
    b = other.b;
    c = other.c;
}

cTriang& cTriang::operator =(const cTriang& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cTriang::info() const
{
    std::stringstream out;
    out << "a=" << a << ", b=" << b << ", c=" << c;
    return out.str();
}

double cTriang::draw() const
{
    return OPP::triang(rng, a, b, c);
}

//----

void cWeibull::copy(const cWeibull& other)
{
    a = other.a;
    b = other.b;
}

cWeibull& cWeibull::operator =(const cWeibull& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cWeibull::info() const
{
    std::stringstream out;
    out << "a=" << a << ", b=" << b;
    return out.str();
}

double cWeibull::draw() const
{
    return OPP::weibull(rng, a, b);
}

//----

void cParetoShifted::copy(const cParetoShifted& other)
{
    a = other.a;
    b = other.b;
    c = other.c;
}

cParetoShifted& cParetoShifted::operator =(const cParetoShifted& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cParetoShifted::info() const
{
    std::stringstream out;
    out << "a=" << a << ", b=" << b << ", c=" << c;
    return out.str();
}

double cParetoShifted::draw() const
{
    return OPP::pareto_shifted(rng, a, b, c);
}

//----

void cIntUniform::copy(const cIntUniform& other)
{
    a = other.a;
    b = other.b;
}

cIntUniform& cIntUniform::operator =(const cIntUniform& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cIntUniform::info() const
{
    std::stringstream out;
    out << "a=" << a << ", b=" << b;
    return out.str();
}

double cIntUniform::draw() const
{
    return (double) OPP::intuniform(rng, a, b);
}

//----

void cBernoulli::copy(const cBernoulli& other)
{
    p = other.p;
}

cBernoulli& cBernoulli::operator =(const cBernoulli& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cBernoulli::info() const
{
    std::stringstream out;
    out << "p=" << p;
    return out.str();
}

double cBernoulli::draw() const
{
    return (double) OPP::bernoulli(rng, p);
}

//----

void cBinomial::copy(const cBinomial& other)
{
    n = other.n;
    p = other.p;
}

cBinomial& cBinomial::operator =(const cBinomial& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cBinomial::info() const
{
    std::stringstream out;
    out << "n=" << n << ", p=" << p;
    return out.str();
}

double cBinomial::draw() const
{
    return (double) OPP::binomial(rng, n, p);
}

//----

void cGeometric::copy(const cGeometric& other)
{
    p = other.p;
}

cGeometric& cGeometric::operator =(const cGeometric& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cGeometric::info() const
{
    std::stringstream out;
    out << "p=" << p;
    return out.str();
}

double cGeometric::draw() const
{
    return (double) OPP::geometric(rng, p);
}

//----

void cNegBinomial::copy(const cNegBinomial& other)
{
    n = other.n;
    p = other.p;
}

cNegBinomial& cNegBinomial::operator =(const cNegBinomial& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cNegBinomial::info() const
{
    std::stringstream out;
    out << "n=" << n << ", p=" << p;
    return out.str();
}

double cNegBinomial::draw() const
{
    return (double) OPP::negbinomial(rng, n, p);
}

//----

void cPoisson::copy(const cPoisson& other)
{
    lambda = other.lambda;
}

cPoisson& cPoisson::operator =(const cPoisson& other)
{
    if (this == &other)
        return *this;

    cRandom::operator =(other);
    copy(other);
    return *this;
}

std::string cPoisson::info() const
{
    std::stringstream out;
    out << "lambda=" << lambda;
    return out.str();
}

double cPoisson::draw() const
{
    return (double) OPP::poisson(rng, lambda);
}


NAMESPACE_END

