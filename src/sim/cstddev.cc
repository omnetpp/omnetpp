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
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using namespace omnetpp::common;

namespace omnetpp {

using std::ostream;

Register_Class(cStdDev);

cStdDev::cStdDev(const char *s, bool weighted) : cStatistic(s), weighted(weighted)
{
    clear();
}

std::string cStdDev::str() const
{
    std::stringstream out;
    out << "count=" << getCount();
    if (isWeighted())
        out << " sumweights=" << getSumWeights();
    out << " mean=" << getMean();
    out << " stddev=" << getStddev();
    out << " min=" << getMin();
    out << " max=" << getMax();
    return out.str();
}

void cStdDev::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cStatistic::parsimPack(buffer);
    buffer->pack(weighted);
    buffer->pack(minValue);
    buffer->pack(maxValue);
    buffer->pack(numValues);
    buffer->pack(sumWeights);
    buffer->pack(sumWeightedValues);
    buffer->pack(sumSquaredWeights);
    buffer->pack(sumWeightedSquaredValues);
#endif
}

void cStdDev::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cStatistic::parsimUnpack(buffer);
    buffer->unpack(weighted);
    buffer->unpack(minValue);
    buffer->unpack(maxValue);
    buffer->unpack(numValues);
    buffer->unpack(sumWeights);
    buffer->unpack(sumWeightedValues);
    buffer->unpack(sumSquaredWeights);
    buffer->unpack(sumWeightedSquaredValues);
#endif
}

void cStdDev::copy(const cStdDev& res)
{
    weighted = res.weighted;
    minValue = res.minValue;
    maxValue = res.maxValue;
    numValues = res.numValues;
    sumWeights = res.sumWeights;
    sumWeightedValues = res.sumWeightedValues;
    sumSquaredWeights = res.sumSquaredWeights;
    sumWeightedSquaredValues = res.sumWeightedSquaredValues;
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
    if (weighted)
        throw cRuntimeError(this, "Use collectWeighted(value, weight) to add observations to a weighted statistics");
    if (std::isnan(value))
        throw cRuntimeError(this, "collect(): NaN values are not allowed");
    numValues++;
    sumWeights += 1;
    if (minValue > value)
        minValue = value;
    if (maxValue < value)
        maxValue = value;
    sumWeightedValues += value;
    sumSquaredWeights += 1;
    sumWeightedSquaredValues += value * value;
}

void cStdDev::collectWeighted(double value, double weight)
{
    if (!weighted)
        throw cRuntimeError(this, "Use collect(value) to add observations to an unweighted statistics");
    if (!std::isfinite(weight) || weight < 0)
        throw cRuntimeError(this, "collectWeighted(): weight must be nonnegative and finite (%g)", weight); // zero is OK
    if (std::isnan(value))
        throw cRuntimeError(this, "collect(): NaN values are not allowed");

    numValues++;
    if (minValue > value)
        minValue = value;
    if (maxValue < value)
        maxValue = value;
    sumWeights += weight;
    sumWeightedValues += weight * value;
    sumSquaredWeights += weight * weight;
    sumWeightedSquaredValues += weight * value * value;
}

void cStdDev::merge(const cStatistic *other)
{
    if (!weighted && other->isWeighted())
        throw cRuntimeError(this, "Cannot merge weighted statistics (%s)%s into unweighted statistics",
                other->getClassName(), other->getFullPath().c_str());

    int64_t origCount = numValues;
    numValues += other->getCount();
    if (numValues < 0)
        throw cRuntimeError(this, "merge(): Observation count overflow");

    if (other->getCount() > 0 && (origCount == 0 || minValue > other->getMin()))
        minValue = other->getMin();
    if (other->getCount() > 0 && (origCount == 0 || maxValue < other->getMax()))
        maxValue = other->getMax();

    sumWeights += other->getSumWeights();
    sumWeightedValues += other->getWeightedSum();
    sumSquaredWeights += other->getSqrSumWeights();
    sumWeightedSquaredValues += other->getWeightedSqrSum();
}

void cStdDev::assertUnweighted() const
{
    if (weighted)
        throw cRuntimeError(this, "sum and sqrSum are not available for weighted statistics");
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
    return sumWeightedValues / sumWeights;
}

double cStdDev::getVariance() const
{
    // note: no checks for division by zero, we prefer to return Inf or NaN
    if (numValues <= 1)
        return NaN;
    else if (minValue == maxValue)
        return 0;
    else if (!weighted) {
        double var = (sumWeightedSquaredValues - sumWeightedValues*sumWeightedValues/sumWeights) / (sumWeights-1);
        return var < 0 ? 0 : var;
    }
    else {
        double var = (sumWeights * sumWeightedSquaredValues - sumWeightedValues * sumWeightedValues) / (sumWeights * sumWeights - sumSquaredWeights);
        return var < 0 ? 0 : var;
    }
}

double cStdDev::getStddev() const
{
    return sqrt(getVariance());
}

void cStdDev::clear()
{
    numValues = 0;
    sumWeights = 0.0;
    minValue = POSITIVE_INFINITY;
    maxValue = NEGATIVE_INFINITY;
    sumWeightedValues = 0.0;
    sumSquaredWeights = 0.0;
    sumWeightedSquaredValues = 0.0;
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
    fprintf(f, "%" PRId64 "\t #= num_vals\n", numValues);
    fprintf(f, "%lg %lg\t #= min, max\n", minValue, maxValue);
    fprintf(f, "%d\t #= weighted\n", weighted);
    if (!weighted) {
        fprintf(f, "%lg\t #= sum\n", sumWeightedValues);
        fprintf(f, "%lg\t #= sum_squared_vals\n", sumWeightedSquaredValues);
    }
    else {
        fprintf(f, "%lg\t #= sum_weights", sumWeights);
        fprintf(f, "%lg\t #= sum_weighted_vals", sumWeightedValues);
        fprintf(f, "%lg\t #= sum_squared_weights", sumSquaredWeights);
        fprintf(f, "%lg\t #= sum_weighted_squared_vals", sumWeightedSquaredValues);
    }
}

void cStdDev::loadFromFile(FILE *f)
{
    int tmp;
    freadvarsf(f, "%" PRId64 "\t #= num_vals", &numValues);
    freadvarsf(f, "%lg %lg\t #= min, max", &minValue, &maxValue);
    freadvarsf(f, "%d\t #= weighted", &tmp); weighted = tmp;
    if (!weighted) {
        freadvarsf(f, "%lg\t #= sum", &sumWeightedValues);
        freadvarsf(f, "%lg\t #= sum_squared_vals", &sumWeightedSquaredValues);
        sumWeights = sumSquaredWeights = numValues;
    }
    else {
        freadvarsf(f, "%lg\t #= sum_weights", &sumWeights);
        freadvarsf(f, "%lg\t #= sum_weighted_vals", &sumWeightedValues);
        freadvarsf(f, "%lg\t #= sum_squared_weights", &sumSquaredWeights);
        freadvarsf(f, "%lg\t #= sum_weighted_squared_vals", &sumWeightedSquaredValues);
    }
}

}  // namespace omnetpp

