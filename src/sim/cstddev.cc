//=========================================================================
//  CSTDDEV.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//        Based on the MISS simulator's result collection
//
//   Member functions of
//    cStdDev: basic statistics (mean, stddev, min, max, etc)
//    cWeightedStdDev: weighted version
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>

#include "common/stringutil.h"
#include "common/commonutil.h"  // NaN
#include "omnetpp/cstddev.h"
#include "omnetpp/distrib.h"
#include "omnetpp/globals.h"
#include "omnetpp/cdetect.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using namespace OPP::common;

NAMESPACE_BEGIN

using std::ostream;

Register_Class(cStdDev);
Register_Class(cWeightedStdDev);

cStdDev::cStdDev(const char *s) : cStatistic(s)
{
    numValues = 0L;
    sumValues = sumSquaredValues = 0;
    minValue = maxValue = 0;
}

std::string cStdDev::info() const
{
    std::stringstream out;
    out << "n=" << getCount()
        << " mean=" << getMean()
        << " stddev=" << getStddev()
        << " min=" << getMin()
        << " max=" << getMax();
    return out.str();
}

void cStdDev::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cStatistic::parsimPack(buffer);
    buffer->pack(numValues);
    buffer->pack(minValue);
    buffer->pack(maxValue);
    buffer->pack(sumValues);
    buffer->pack(sumSquaredValues);
#endif
}

void cStdDev::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cStatistic::parsimUnpack(buffer);
    buffer->unpack(numValues);
    buffer->unpack(minValue);
    buffer->unpack(maxValue);
    buffer->unpack(sumValues);
    buffer->unpack(sumSquaredValues);
#endif
}

void cStdDev::copy(const cStdDev& res)
{
    numValues = res.numValues;
    minValue = res.minValue;
    maxValue = res.maxValue;
    sumValues = res.sumValues;
    sumSquaredValues = res.sumSquaredValues;
}

cStdDev& cStdDev::operator=(const cStdDev& res)
{
    if (this == &res)
        return *this;
    cStatistic::operator=(res);
    copy(res);
    return *this;
}

void cStdDev::collect(double value)
{
    if (++numValues <= 0) {
        // num_vals overflow: issue warning and stop collecting
        getEnvir()->printfmsg("WARNING: (%s)%s: collect(): observation count overflow", getClassName(), getFullPath().c_str());
        numValues--;  // restore
        return;
    }

    sumValues += value;
    sumSquaredValues += value * value;

    if (numValues > 1) {
        if (value < minValue)
            minValue = value;
        else if (value > maxValue)
            maxValue = value;
    }
    else {
        minValue = maxValue = value;
    }

    cTransientDetection *td = getTransientDetectionObject();
    if (td)
        td->collect(value);
    cAccuracyDetection *ra = getAccuracyDetectionObject();
    if (ra)
        ra->collect(value);
}

void cStdDev::doMerge(const cStatistic *other)
{
    long orig_num_vals = numValues;
    numValues += other->getCount();
    if (numValues < 0)
        throw cRuntimeError(this, "merge(): observation count overflow");

    if (other->getCount() > 0 && (orig_num_vals == 0 || minValue > other->getMin()))
        minValue = other->getMin();
    if (other->getCount() > 0 && (orig_num_vals == 0 || maxValue < other->getMax()))
        maxValue = other->getMax();

    sumValues += other->getSum();
    sumSquaredValues += other->getSqrSum();
}

void cStdDev::merge(const cStatistic *other)
{
    if (other->isWeighted())
        throw cRuntimeError(this, "Cannot merge weighted statistics (%s)%s into unweighted statistics",
                other->getClassName(), other->getFullPath().c_str());
    doMerge(other);
}

double cStdDev::getMin() const
{
    return numValues == 0 ? NaN : minValue;
}

double cStdDev::getMax() const
{
    return numValues == 0 ? NaN : maxValue;
}

double cStdDev::getMean() const
{
    return numValues == 0 ? NaN : sumValues / numValues;
}

double cStdDev::getVariance() const
{
    if (numValues <= 1)
        return NaN;
    else {
        // note: no check for division by zero, we prefer to return Inf or NaN
        double devsqr = (sumSquaredValues - sumValues * sumValues / numValues) / (numValues - 1);
        return devsqr < 0 ? 0 : devsqr;
    }
}

double cStdDev::getStddev() const
{
    return sqrt(getVariance());
}

std::string cStdDev::detailedInfo() const
{
    std::stringstream os;
    os << "  Number of values = " << numValues << "\n";
    if (numValues == 1)
        os << "  Value          = " << minValue << "\n";
    else if (numValues > 0) {
        os << "  Mean value     = " << getMean() << "\n";
        os << "  Standard dev.  = " << getStddev() << "\n";
        os << "  Minimal value  = " << getMin() << "\n";
        os << "  Maximal value  = " << getMax() << "\n";
    }
    return os.str();
}

void cStdDev::clearResult()
{
    numValues = 0;
    sumValues = sumSquaredValues = minValue = maxValue = 0;
}

double cStdDev::draw() const
{
    switch (numValues) {
        case 0:  return 0.0;
        case 1:  return minValue;
        default: return normal(getRNG(), getMean(), getStddev());
    }
}

void cStdDev::saveToFile(FILE *f) const
{
    fprintf(f, "\n#\n# (%s) %s\n#\n", getClassName(), getFullPath().c_str());
    fprintf(f, "%ld\t #= num_vals\n", numValues);
    fprintf(f, "%g %g\t #= min, max\n", minValue, maxValue);
    fprintf(f, "%g\t #= sum\n", sumValues);
    fprintf(f, "%g\t #= square sum\n", sumSquaredValues);
}

void cStdDev::loadFromFile(FILE *f)
{
    freadvarsf(f, "");
    freadvarsf(f, "");
    freadvarsf(f, "");
    freadvarsf(f, "");
    freadvarsf(f, "%ld\t #= num_vals", &numValues);
    freadvarsf(f, "%g %g\t #= min, max", &minValue, &maxValue);
    freadvarsf(f, "%g\t #= sum", &sumValues);
    freadvarsf(f, "%g\t #= square sum", &sumSquaredValues);
}

//----

std::string cWeightedStdDev::info() const
{
    std::stringstream out;
    out << "n=" << getCount()
        << " mean=" << getMean()
        << " stddev=" << getStddev()
        << " min=" << getMin()
        << " max=" << getMax();
    return out.str();
}

void cWeightedStdDev::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cStdDev::parsimPack(buffer);
    buffer->pack(sumWeights);
    buffer->pack(sumWeightedValues);
    buffer->pack(sumSquaredWeights);
    buffer->pack(sumWeightedSquaredValues);
#endif
}

void cWeightedStdDev::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cStdDev::parsimUnpack(buffer);
    buffer->unpack(sumWeights);
    buffer->unpack(sumWeightedValues);
    buffer->unpack(sumSquaredWeights);
    buffer->unpack(sumWeightedSquaredValues);
#endif
}

void cWeightedStdDev::copy(const cWeightedStdDev& res)
{
    sumWeights = res.sumWeights;
    sumWeightedValues = res.sumWeightedValues;
    sumSquaredWeights = res.sumSquaredWeights;
    sumWeightedSquaredValues = res.sumWeightedSquaredValues;
}

cWeightedStdDev& cWeightedStdDev::operator=(const cWeightedStdDev& res)
{
    if (this == &res)
        return *this;
    cStdDev::operator=(res);
    copy(res);
    return *this;
}

void cWeightedStdDev::collect2(double value, double weight)
{
    if (weight < 0)
        throw cRuntimeError(this, "collect2(): negative weight %g", weight);

    if (++numValues <= 0) {
        // num_vals overflow: issue warning and stop collecting
        getEnvir()->printfmsg("\a\nWARNING: (%s)%s: collect2(): observation count overflow!\n\n", getClassName(), getFullPath().c_str());
        numValues--;  // restore
        return;
    }

    sumValues += value;
    sumSquaredValues += value * value;

    if (numValues > 1) {
        if (value < minValue)
            minValue = value;
        else if (value > maxValue)
            maxValue = value;
    }
    else {
        minValue = maxValue = value;
    }

    sumWeights += weight;
    sumWeightedValues += weight * value;
    sumSquaredWeights += weight * weight;
    sumWeightedSquaredValues += weight * value * value;

    cTransientDetection *td = getTransientDetectionObject();
    if (td)
        td->collect(value);
    cAccuracyDetection *ra = getAccuracyDetectionObject();
    if (ra)
        ra->collect(value);
}

double cWeightedStdDev::getMean() const
{
    return sumWeights == 0 ? NaN : sumWeightedValues / sumWeights;
}

void cWeightedStdDev::merge(const cStatistic *other)
{
    cStdDev::doMerge(other);
    sumWeights += other->getWeights();
    sumWeightedValues += other->getWeightedSum();
    sumSquaredWeights += other->getSqrSumWeights();
    sumWeightedSquaredValues += other->getWeightedSqrSum();
}

void cWeightedStdDev::clearResult()
{
    cStdDev::clearResult();
    sumWeights = 0.0;
    sumWeightedValues = 0.0;
    sumSquaredWeights = 0.0;
    sumWeightedSquaredValues = 0.0;
}

double cWeightedStdDev::getVariance() const
{
    if (numValues <= 1)
        return NaN;
    else {
        // note: no check for division by zero, we prefer to return Inf or NaN
        double denominator = sumWeights * sumWeights - sumSquaredWeights;
        double var = (sumWeights * sumWeightedSquaredValues - sumWeightedValues * sumWeightedValues) / denominator;
        return var < 0 ? 0 : var;
    }
}

void cWeightedStdDev::saveToFile(FILE *f) const
{
    cStdDev::saveToFile(f);
    fprintf(f, "%g\t #= sum_weights\n", sumWeights);
    fprintf(f, "%g\t #= sum_weighted_vals\n", sumWeightedValues);
    fprintf(f, "%g\t #= sum_squared_weights\n", sumSquaredWeights);
    fprintf(f, "%g\t #= sum_weights_squared_vals\n", sumWeightedSquaredValues);
}

void cWeightedStdDev::loadFromFile(FILE *f)
{
    cStdDev::loadFromFile(f);
    freadvarsf(f, "%g\t #= sum_weights", &sumWeights);
    freadvarsf(f, "%g\t #= sum_weighted_vals", &sumWeightedValues);
    freadvarsf(f, "%g\t #= sum_squared_weights", &sumSquaredWeights);
    freadvarsf(f, "%g\t #= sum_weights_squared_vals", &sumWeightedSquaredValues);
}

NAMESPACE_END

