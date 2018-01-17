//=========================================================================
//  CDENSITYESTBASE.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Authors: Andras Varga
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
#include <sstream>
#include <algorithm>
#include "omnetpp/globals.h"
#include "omnetpp/cdensityestbase.h"
#include "omnetpp/cexception.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using namespace std;

namespace omnetpp {


cDensityEstBase::Bin cDensityEstBase::getBinInfo(int k) const
{
    if (k < 0 || k >= getNumBins())
        return Bin();
    Bin c;
    c.lower = getBinEdge(k);
    c.upper = getBinEdge(k+1);
    c.value = getBinValue(k);
    c.relativeFreq = getBinPDF(k);
    return c;
}

double cDensityEstBase::getPDF(double x) const
{
    if (!isTransformed())
        throw cRuntimeError(this, "");

    if (x < getMin())
        return 0;

    if (x < getBinEdge(0))
        return getUnderflowSumWeights() / (getBinEdge(0) - getMin());

    int numBins = getNumBins();

    // returns 0..1; assumes constant PDF within a bin
    for (int i = 0; i < numBins; ++i) {
        double lowerEdge = getBinEdge(i);
        double upperEdge = getBinEdge(i + 1);

        if (x < upperEdge)
            return getBinValue(i) / (upperEdge-lowerEdge);
    }

    if (x < getMax())
        return getOverflowSumWeights() / (getMax() - getBinEdge(numBins));

    return 0;
}


double cDensityEstBase::getCDF(double x) const
{
    if (x < getMin())
        return 0;

    if (x < getBinEdge(0))
        return getUnderflowSumWeights() * ((x - getMin()) / (getBinEdge(0) - getMin()));

    int numBins = getNumBins();

    // returns 0..1; uses linear approximation between two markers
    for (int i = 0; i < numBins; ++i) {
        double lowerEdge = getBinEdge(i);
        double upperEdge = getBinEdge(i + 1);

        if (x < upperEdge)
            return getBinValue(i) * ((x - lowerEdge) / (upperEdge - lowerEdge));
    }

    if (x < getMax())
        return getOverflowSumWeights() * ((x - getBinEdge(numBins)) / (getMax() - getBinEdge(numBins)));

    return 1;
}


const cDensityEstBase::Bin& cDensityEstBase::internalGetBinInfo(int k) const
{
    // only for use in sim_std.msg (each call overwrites the static buffer!)
    static Bin buf;
    buf = getBinInfo(k);
    return buf;
}

double cDensityEstBase::getBinPDF(int k) const
{
    if (numValues == 0)
        return 0.0;
    double binSize = getBinEdge(k+1) - getBinEdge(k);
    return binSize == 0 ? 0.0 : getBinValue(k) / binSize / getCount();
}


//----

cPrecollectionBasedDensityEst::cPrecollectionBasedDensityEst(const char *name, bool weighted) : cDensityEstBase(name, weighted)
{
    rangeMode = RANGE_AUTO;
    numPrecollected = 100;
    rangeExtFactor = 2.0;
    rangeMin = rangeMax = 0; //TODO NaN?
    numUnderflows = numOverflows = 0;
    underflowSumWeights = overflowSumWeights = 0;
    transformed = false;
    precollectedValues = new double[numPrecollected];  // to match RANGE_AUTO
    precollectedWeights = weighted ? new double[numPrecollected] : nullptr;
}

cPrecollectionBasedDensityEst::~cPrecollectionBasedDensityEst()
{
    delete[] precollectedValues;
    delete[] precollectedWeights;
}

void cPrecollectionBasedDensityEst::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cDensityEstBase::parsimPack(buffer);

    buffer->pack(rangeMin);
    buffer->pack(rangeMax);
    buffer->pack(numPrecollected);
    buffer->pack(numUnderflows);
    buffer->pack(numOverflows);
    buffer->pack(underflowSumWeights);
    buffer->pack(overflowSumWeights);
    buffer->pack(rangeExtFactor);
    buffer->pack(rangeMode);
    buffer->pack(transformed);

    if (buffer->packFlag(precollectedValues != nullptr))
        buffer->pack(precollectedValues, numValues);  // pack the used positions only
    if (buffer->packFlag(precollectedWeights != nullptr))
        buffer->pack(precollectedWeights, numValues);  // pack the used positions only
#endif
}

void cPrecollectionBasedDensityEst::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cDensityEstBase::parsimUnpack(buffer);

    buffer->unpack(rangeMin);
    buffer->unpack(rangeMax);
    buffer->unpack(numPrecollected);
    buffer->unpack(numUnderflows);
    buffer->unpack(numOverflows);
    buffer->unpack(underflowSumWeights);
    buffer->unpack(overflowSumWeights);
    buffer->unpack(rangeExtFactor);
    int tmp;
    buffer->unpack(tmp);
    rangeMode = (RangeMode)tmp;
    buffer->unpack(transformed);

    delete[] precollectedValues;
    precollectedValues = nullptr;
    if (buffer->checkFlag()) {
        precollectedValues = new double[numPrecollected];
        buffer->unpack(precollectedValues, numValues);
    }
    delete[] precollectedWeights;
    precollectedWeights = nullptr;
    if (buffer->checkFlag()) {
        precollectedWeights = new double[numPrecollected];
        buffer->unpack(precollectedWeights, numValues);
    }
#endif
}

void cPrecollectionBasedDensityEst::copy(const cPrecollectionBasedDensityEst& res)
{
    rangeMin = res.rangeMin;
    rangeMax = res.rangeMax;
    numPrecollected = res.numPrecollected;
    numUnderflows = res.numUnderflows;
    numOverflows = res.numOverflows;
    underflowSumWeights = res.underflowSumWeights;
    overflowSumWeights = res.overflowSumWeights;

    rangeExtFactor = res.rangeExtFactor;
    rangeMode = res.rangeMode;

    transformed = res.transformed;

    delete[] precollectedValues;
    precollectedValues = nullptr;
    if (res.precollectedValues) {
        precollectedValues = new double[numPrecollected];
        memcpy(precollectedValues, res.precollectedValues, numPrecollected * sizeof(double));
    }

    delete[] precollectedWeights;
    precollectedWeights = nullptr;
    if (res.precollectedWeights) {
        precollectedWeights = new double[numPrecollected];
        memcpy(precollectedWeights, res.precollectedWeights, numPrecollected * sizeof(double));
    }

}

cPrecollectionBasedDensityEst& cPrecollectionBasedDensityEst::operator=(const cPrecollectionBasedDensityEst& res)
{
    cDensityEstBase::operator=(res);
    copy(res);
    return *this;
}

void cPrecollectionBasedDensityEst::merge(const cStatistic *other)
{
    if (dynamic_cast<const cPrecollectionBasedDensityEst *>(other) == nullptr)
        throw cRuntimeError(this, "Cannot merge non-histogram (non-cPrecollectionBasedDensityEst) statistics (%s)%s into a histogram type",
                other->getClassName(), other->getFullPath().c_str());

    const cPrecollectionBasedDensityEst *otherd = (const cPrecollectionBasedDensityEst *)other;

    if (!otherd->isTransformed()) {
        // easiest and exact solution: simply recollect the observations
        // the other object has collected
        for (int i = 0; i < otherd->numValues; i++)
            if (!otherd->isWeighted())
                collect(otherd->precollectedValues[i]);
            else
                collectWeighted(otherd->precollectedValues[i], otherd->precollectedWeights[i]);
    }
    else {
        // merge the base class
        cDensityEstBase::merge(otherd);

        // force this object to be transformed as well
        if (!isTransformed())
            transform();

        // make sure that cells are aligned
        if (getNumBins() != otherd->getNumBins())
            throw cRuntimeError(this, "Cannot merge (%s)%s: Different number of histogram cells (%d vs %d)",
                    otherd->getClassName(), otherd->getFullPath().c_str(), getNumBins(), otherd->getNumBins());
        int n = getNumBins();
        for (int i = 0; i <= n; i++)
            if (getBinEdge(i) != otherd->getBinEdge(i))
                throw cRuntimeError(this, "Cannot merge (%s)%s: Histogram cells are not aligned",
                        otherd->getClassName(), otherd->getFullPath().c_str());


        // merge underflow/overflow cells
        numUnderflows += otherd->getNumUnderflows();
        numOverflows += otherd->getNumOverflows();
        underflowSumWeights += otherd->getUnderflowSumWeights();
        overflowSumWeights += otherd->getOverflowSumWeights();

        // then merge bin values
        doMergeBinValues(otherd);
    }
}

void cPrecollectionBasedDensityEst::clearResult()
{
    cDensityEstBase::clearResult();

    transformed = false;
    rangeMode = RANGE_AUTO;
    numPrecollected = 100;
    rangeExtFactor = 2.0;
    rangeMin = rangeMax = 0;
    numUnderflows = numOverflows = 0;
    underflowSumWeights = overflowSumWeights = 0;

    delete[] precollectedValues;
    precollectedValues = new double[numPrecollected];  // to match RANGE_AUTO
    delete[] precollectedWeights;
    precollectedWeights = weighted ? new double[numPrecollected] : nullptr;  // to match RANGE_AUTO
}

void cPrecollectionBasedDensityEst::setRange(double lower, double upper)
{
    if (numValues > 0 || isTransformed())
        throw cRuntimeError(this, "setRange() can only be called before collecting any values");

    rangeMode = RANGE_FIXED;
    rangeMin = lower;
    rangeMax = upper;

    delete[] precollectedValues;
    precollectedValues = nullptr;  // not needed with RANGE_FIXED

    delete[] precollectedWeights;
    precollectedWeights = nullptr;  // not needed with RANGE_FIXED
}

void cPrecollectionBasedDensityEst::setRangeAuto(int num_fstvals, double range_ext_fct)
{
    if (numValues > 0 || isTransformed())
        throw cRuntimeError(this, "setRange...() can only be called before collecting any values");

    rangeMode = RANGE_AUTO;
    numPrecollected = num_fstvals;
    rangeExtFactor = range_ext_fct;

    //TODO these allocations should take place in collect(), not here!
    delete[] precollectedValues;
    precollectedValues = new double[numPrecollected];

    delete[] precollectedWeights;
    precollectedWeights = nullptr;  // not needed with RANGE_FIXED
}

void cPrecollectionBasedDensityEst::setRangeAutoLower(double upper, int numPrecoll, double rangeExtFact)
{
    if (numValues > 0 || isTransformed())
        throw cRuntimeError(this, "setRange...() can only be called before collecting any values");

    rangeMode = RANGE_AUTOLOWER;
    numPrecollected = numPrecoll;
    rangeMax = upper;
    rangeExtFactor = rangeExtFact;

    delete[] precollectedValues;
    precollectedValues = new double[numPrecollected];

    delete[] precollectedWeights;
    precollectedWeights = weighted ? new double[numPrecollected] : nullptr;
}

void cPrecollectionBasedDensityEst::setRangeAutoUpper(double lower, int numPrecoll, double rangeExtFact)
{
    if (numValues > 0 || isTransformed())
        throw cRuntimeError(this, "setRange...() can only be called before collecting any values");

    rangeMode = RANGE_AUTOUPPER;
    numPrecollected = numPrecoll;
    rangeMin = lower;
    rangeExtFactor = rangeExtFact;

    delete[] precollectedValues;
    precollectedValues = new double[numPrecollected];

    delete[] precollectedWeights;
    precollectedWeights = weighted ? new double[numPrecollected] : nullptr;
}

void cPrecollectionBasedDensityEst::setNumPrecollectedValues(int numPrecoll)
{
    if (numValues > 0 || isTransformed())
        throw cRuntimeError(this, "setNumPrecollectedValues() can only be called before collecting any values");

    numPrecollected = numPrecoll;

    // if already allocated, reallocate with the correct size
    if (precollectedValues) {
        delete[] precollectedValues;
        precollectedValues = new double[numPrecollected];
    }

    if (precollectedWeights) {
        delete[] precollectedWeights;
        precollectedWeights = weighted ? new double[numPrecollected] : nullptr;
    }
}

void cPrecollectionBasedDensityEst::setupRange()
{
    //
    // Set rangeMin and rangeMax.
    // Attempts not to make zero width range (makes it 1.0 wide).
    //
    double c, r;
    switch (rangeMode) {
        case RANGE_AUTO:
            if (numValues == 0)
                c = r = 0;
            else {
                c = (minValue + maxValue) / 2;
                r = (maxValue - minValue) * rangeExtFactor;
            }
            if (r == 0)
                r = 1.0;  // warning?
            rangeMin = c - r / 2;
            rangeMax = c + r / 2;
            break;

        case RANGE_AUTOLOWER:
            if (rangeMax <= minValue)
                rangeMin = rangeMax - 1.0;  // warning?
            else
                rangeMin = rangeMax - (rangeMax - minValue) * rangeExtFactor;
            break;

        case RANGE_AUTOUPPER:
            if (rangeMin >= maxValue)
                rangeMax = rangeMin + 1.0;  // warning?
            else
                rangeMax = rangeMin + (maxValue - rangeMin) * rangeExtFactor;
            break;

        case RANGE_FIXED:
            // no-op: rangeMin, rangeMax already set
            break;

        case RANGE_NOTSET:
            // no-op: rangeMin, rangeMax should remain unset
            break;
    }
}

void cPrecollectionBasedDensityEst::collect(double value)
{
    if (!isTransformed() && rangeMode == RANGE_FIXED)
        transform();

    cDensityEstBase::collect(value);

    if (!isTransformed()) {
        ASSERT(precollectedValues);
        precollectedValues[numValues-1] = value;

        if (numValues == numPrecollected)
            transform();
    }
    else {
        collectTransformed(value);  // must maintain underflow/overflow cells
    }
}

void cPrecollectionBasedDensityEst::collectWeighted(double value, double weight)
{
    if (!isTransformed() && rangeMode == RANGE_FIXED)
        transform();

    cDensityEstBase::collectWeighted(value, weight);

    if (!isTransformed()) {
        ASSERT(precollectedValues);
        ASSERT(precollectedWeights);
        precollectedValues[numValues-1] = value;
        precollectedWeights[numValues-1] = weight;

        if (numValues == numPrecollected)
            transform();
    }
    else {
        collectTransformed2(value, weight);  // must maintain underflow/overflow cells
    }
}

void cPrecollectionBasedDensityEst::plotline(ostream& os, const char *pref, double xval, double count, double a)
{
    const int pictureWidth = 54;
    char buf[101];
    sprintf(buf, "   %s%12f %5g :", pref, xval, count);
    char *s = buf + strlen(buf);
    int x = (int)floor(a * count + .5);
    int k = x <= pictureWidth ? x : pictureWidth;
    for (int m = 1; m <= k; m++)
        *s++ = '-';
    strcpy(s, x <= pictureWidth ? "*\n" : ">\n");
    os << buf;
}

void cPrecollectionBasedDensityEst::saveToFile(FILE *f) const
{
    cDensityEstBase::saveToFile(f);

    fprintf(f, "%d\t #= transformed\n", transformed);
    fprintf(f, "%d\t #= range_mode\n", rangeMode);
    fprintf(f, "%lg\t #= range_ext_factor\n", rangeExtFactor);
    fprintf(f, "%lg %g\t #= range\n", rangeMin, rangeMax);
    fprintf(f, "%" PRId64 " %" PRId64 "\t #= cell_under, cell_over\n", numUnderflows, numOverflows);
    fprintf(f, "%lg %lg\t #= sumweights_underflow, sum_weights_overflow\n", underflowSumWeights, overflowSumWeights);
    fprintf(f, "%d\t #= num_firstvals\n", numPrecollected);

    fprintf(f, "%d\t #= firstvals[] exists\n", precollectedValues != nullptr);
    if (precollectedValues) {
        int count = (int) std::min(numValues, (int64_t)numPrecollected);
        for (int i = 0; i < count; i++)
            fprintf(f, " %lg\n", precollectedValues[i]);
    }
    if (precollectedWeights) {
        int count = (int) std::min(numValues, (int64_t)numPrecollected);
        for (int i = 0; i < count; i++)
            fprintf(f, " %lg\n", precollectedWeights[i]);
    }

}

void cPrecollectionBasedDensityEst::loadFromFile(FILE *f)
{
    cDensityEstBase::loadFromFile(f);

    int tmp;
    freadvarsf(f, "%d\t #= transformed", &tmp); transformed = tmp;
    freadvarsf(f, "%d\t #= range_mode", &tmp); rangeMode = (RangeMode)tmp;
    freadvarsf(f, "%lg\t #= range_ext_factor", &rangeExtFactor);
    freadvarsf(f, "%lg %lg\t #= range", &rangeMin, &rangeMax);
    freadvarsf(f, "%" PRId64 " %" PRId64 "\t #= cell_under, cell_over", &numUnderflows, &numOverflows);
    freadvarsf(f, "%lg %lg\t #= sumweights_underflow, sum_weights_overflow", &underflowSumWeights, &overflowSumWeights);
    freadvarsf(f, "%d\t #= num_firstvals", &numPrecollected);

    int hasPrecollectedValues;
    freadvarsf(f, "%d\t #= firstvals[] exists", &hasPrecollectedValues);

    delete[] precollectedValues;
    precollectedValues = nullptr;
    if (hasPrecollectedValues) {
        precollectedValues = new double[numPrecollected];
        for (int i = 0; i < numValues; i++)
            freadvarsf(f, " %lg", &precollectedValues[i]);
    }
    if (hasPrecollectedValues && weighted) {
        precollectedWeights = new double[numPrecollected];
        for (int i = 0; i < numValues; i++)
            freadvarsf(f, " %lg", &precollectedWeights[i]);
    }
}

}  // namespace omnetpp

