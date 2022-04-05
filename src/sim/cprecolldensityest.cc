//=========================================================================
//  CPRECOLLDENSITYEST.CC - part of
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
#include <cinttypes>  // PRId64
#include <sstream>
#include <algorithm>
#include "omnetpp/globals.h"
#include "omnetpp/cprecolldensityest.h"
#include "omnetpp/cexception.h"
#include "omnetpp/distrib.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using namespace std;

namespace omnetpp {

cPrecollectionBasedDensityEst::cPrecollectionBasedDensityEst(const char *name, bool weighted) : cAbstractHistogram(name, weighted)
{
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
    cAbstractHistogram::parsimPack(buffer);

    buffer->pack(rangeMin);
    buffer->pack(rangeMax);
    buffer->pack(numPrecollected);
    buffer->pack(numFiniteUnderflows);
    buffer->pack(numFiniteOverflows);
    buffer->pack(numNegInfs);
    buffer->pack(numPosInfs);
    buffer->pack(finiteUnderflowSumWeights);
    buffer->pack(finiteOverflowSumWeights);
    buffer->pack(negInfSumWeights);
    buffer->pack(posInfSumWeights);
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
    cAbstractHistogram::parsimUnpack(buffer);

    buffer->unpack(rangeMin);
    buffer->unpack(rangeMax);
    buffer->unpack(numPrecollected);
    buffer->unpack(numFiniteUnderflows);
    buffer->unpack(numFiniteOverflows);
    buffer->unpack(numNegInfs);
    buffer->unpack(numPosInfs);
    buffer->unpack(finiteUnderflowSumWeights);
    buffer->unpack(finiteOverflowSumWeights);
    buffer->unpack(negInfSumWeights);
    buffer->unpack(posInfSumWeights);
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
    numFiniteUnderflows = res.numFiniteUnderflows;
    numFiniteOverflows = res.numFiniteOverflows;
    numNegInfs = res.numNegInfs;
    numPosInfs = res.numPosInfs;
    finiteUnderflowSumWeights = res.finiteUnderflowSumWeights;
    finiteOverflowSumWeights = res.finiteOverflowSumWeights;
    negInfSumWeights = res.negInfSumWeights;
    posInfSumWeights = res.posInfSumWeights;

    rangeExtFactor = res.rangeExtFactor;
    rangeMode = res.rangeMode;

    transformed = res.transformed;

    delete[] precollectedValues;
    precollectedValues = nullptr;
    if (res.precollectedValues) {
        precollectedValues = new double[numPrecollected];
        std::copy_n(res.precollectedValues, numPrecollected, precollectedValues);
    }

    delete[] precollectedWeights;
    precollectedWeights = nullptr;
    if (res.precollectedWeights) {
        precollectedWeights = new double[numPrecollected];
        std::copy_n(res.precollectedWeights, numPrecollected, precollectedWeights);
    }
}

cPrecollectionBasedDensityEst& cPrecollectionBasedDensityEst::operator=(const cPrecollectionBasedDensityEst& res)
{
    cAbstractHistogram::operator=(res);
    copy(res);
    return *this;
}

void cPrecollectionBasedDensityEst::merge(const cStatistic *other)
{
    if (dynamic_cast<const cPrecollectionBasedDensityEst *>(other) == nullptr)
        throw cRuntimeError(this, "Cannot merge non-cPrecollectionBasedDensityEst statistics (%s)%s into a histogram type",
                other->getClassName(), other->getFullPath().c_str());

    const cPrecollectionBasedDensityEst *otherd = (const cPrecollectionBasedDensityEst *)other;

    if (!otherd->binsAlreadySetUp()) {
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
        cAbstractHistogram::merge(otherd);

        // force this object to set up the bins as well
        if (!binsAlreadySetUp())
            setUpBins();

        // make sure that bins are aligned
        if (getNumBins() != otherd->getNumBins())
            throw cRuntimeError(this, "Cannot merge (%s)%s: Different number of histogram bins (%d vs %d)",
                    otherd->getClassName(), otherd->getFullPath().c_str(), getNumBins(), otherd->getNumBins());
        int n = getNumBins();
        for (int i = 0; i <= n; i++)
            if (getBinEdge(i) != otherd->getBinEdge(i))
                throw cRuntimeError(this, "Cannot merge (%s)%s: Histogram bins are not aligned",
                        otherd->getClassName(), otherd->getFullPath().c_str());

        // merge underflow/overflow bins
        numFiniteUnderflows += otherd->numFiniteUnderflows;
        numFiniteOverflows += otherd->numFiniteOverflows;
        finiteUnderflowSumWeights += otherd->finiteUnderflowSumWeights;
        finiteOverflowSumWeights += otherd->finiteOverflowSumWeights;
        numNegInfs += otherd->numNegInfs;
        numPosInfs += otherd->numPosInfs;
        negInfSumWeights += otherd->negInfSumWeights;
        posInfSumWeights += otherd->posInfSumWeights;

        // then merge bin values
        doMergeBinValues(otherd);
    }
}

void cPrecollectionBasedDensityEst::clear()
{
    cAbstractHistogram::clear();

    transformed = false;
    rangeMode = RANGE_AUTO;
    numPrecollected = 100;
    rangeExtFactor = 2.0;
    rangeMin = rangeMax = 0;
    numFiniteUnderflows = numFiniteOverflows = 0;
    numNegInfs = numPosInfs = 0;
    finiteUnderflowSumWeights = finiteOverflowSumWeights = 0;
    negInfSumWeights = posInfSumWeights = 0;
    finiteMinValue = finiteMaxValue = NAN;

    delete[] precollectedValues;
    precollectedValues = new double[numPrecollected];  // to match RANGE_AUTO
    delete[] precollectedWeights;
    precollectedWeights = weighted ? new double[numPrecollected] : nullptr;  // to match RANGE_AUTO
}

void cPrecollectionBasedDensityEst::setRange(double lower, double upper)
{
    if (numValues > 0 || binsAlreadySetUp())
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
    if (numValues > 0 || binsAlreadySetUp())
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
    if (numValues > 0 || binsAlreadySetUp())
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
    if (numValues > 0 || binsAlreadySetUp())
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
    if (numValues > 0 || binsAlreadySetUp())
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

inline double fixupDelta(double delta, double base) {
    ASSERT(delta > 0);
    return std::max(fabs(base) * 1e-14, delta);
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
            if (numValues == 0 || !std::isfinite(finiteMinValue) || !std::isfinite(finiteMaxValue))
                c = r = 0;
            else {
                c = (finiteMinValue + finiteMaxValue) / 2;
                r = (finiteMaxValue - finiteMinValue) * rangeExtFactor;
            }
            if (r == 0)
                r = 1.0;  // warning?
            r = fixupDelta(r, c);
            rangeMin = c - r / 2;
            rangeMax = c + r / 2;
            break;

        case RANGE_AUTOLOWER:
            if (rangeMax <= finiteMinValue || !std::isfinite(finiteMinValue) || !std::isfinite(finiteMaxValue))
                rangeMin = rangeMax - 1.0;  // warning?
            else
                rangeMin = rangeMax - fixupDelta((rangeMax - finiteMinValue) * rangeExtFactor, rangeMax);
            break;

        case RANGE_AUTOUPPER:
            if (rangeMin >= finiteMaxValue || !std::isfinite(finiteMinValue) || !std::isfinite(finiteMaxValue))
                rangeMax = rangeMin + 1.0;  // warning?
            else
                rangeMax = rangeMin + fixupDelta((finiteMaxValue - rangeMin) * rangeExtFactor, rangeMin);
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
    if (!binsAlreadySetUp() && rangeMode == RANGE_FIXED)
        setUpBins();

    cAbstractHistogram::collect(value);

    if (!std::isfinite(finiteMinValue) || value < finiteMinValue)
        finiteMinValue = value;
    if (!std::isfinite(finiteMaxValue) || value > finiteMaxValue)
        finiteMaxValue = value;

    if (!binsAlreadySetUp()) {
        ASSERT(precollectedValues);
        precollectedValues[numValues-1] = value;

        if (numValues == numPrecollected)
            setUpBins();
    }
    else {
        collectIntoHistogram(value);  // must maintain underflow/overflow bins
    }
}

void cPrecollectionBasedDensityEst::collectWeighted(double value, double weight)
{
    if (!binsAlreadySetUp() && rangeMode == RANGE_FIXED)
        setUpBins();

    cAbstractHistogram::collectWeighted(value, weight);

    if (!std::isfinite(finiteMinValue) || value < finiteMinValue)
        finiteMinValue = value;
    if (!std::isfinite(finiteMaxValue) || value > finiteMaxValue)
        finiteMaxValue = value;

    if (!binsAlreadySetUp()) {
        ASSERT(precollectedValues);
        ASSERT(precollectedWeights);
        precollectedValues[numValues-1] = value;
        precollectedWeights[numValues-1] = weight;

        if (numValues == numPrecollected)
            setUpBins();
    }
    else {
        collectWeightedIntoHistogram(value, weight);  // must maintain underflow/overflow bins
    }
}

void cPrecollectionBasedDensityEst::saveToFile(FILE *f) const
{
    cAbstractHistogram::saveToFile(f);

    fprintf(f, "%d\t #= transformed\n", transformed);
    fprintf(f, "%d\t #= range_mode\n", rangeMode);
    fprintf(f, "%lg\t #= range_ext_factor\n", rangeExtFactor);
    fprintf(f, "%lg %g\t #= range\n", rangeMin, rangeMax);
    fprintf(f, "%" PRId64 " %" PRId64 "\t #= cell_under, cell_over\n", numFiniteUnderflows, numFiniteOverflows);
    fprintf(f, "%" PRId64 " %" PRId64 "\t #= num_neg_infs, num_pos_infs\n", numNegInfs, numPosInfs);
    fprintf(f, "%lg %lg\t #= sum_weights_underflow, sum_weights_overflow\n", finiteUnderflowSumWeights, finiteOverflowSumWeights);
    fprintf(f, "%lg %lg\t #= sum_weights_neg_inf, sum_weights_pos_inf\n", negInfSumWeights, posInfSumWeights);
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
    cAbstractHistogram::loadFromFile(f);

    int tmp;
    freadvarsf(f, "%d\t #= transformed", &tmp); transformed = tmp;
    freadvarsf(f, "%d\t #= range_mode", &tmp); rangeMode = (RangeMode)tmp;
    freadvarsf(f, "%lg\t #= range_ext_factor", &rangeExtFactor);
    freadvarsf(f, "%lg %lg\t #= range", &rangeMin, &rangeMax);
    freadvarsf(f, "%" PRId64 " %" PRId64 "\t #= cell_under, cell_over", &numFiniteUnderflows, &numFiniteOverflows);
    freadvarsf(f, "%" PRId64 " %" PRId64 "\t #= num_neg_infs, num_pos_infs", &numNegInfs, &numPosInfs);
    freadvarsf(f, "%lg %lg\t #= sum_weights_underflow, sum_weights_overflow", &finiteUnderflowSumWeights, &finiteOverflowSumWeights);
    freadvarsf(f, "%lg %lg\t #= sum_weights_neg_inf, sum_weights_pos_inf", &negInfSumWeights, &posInfSumWeights);
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

