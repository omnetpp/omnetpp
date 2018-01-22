//==========================================================================
//  CHISTOGRAMSTRATEGY.CC - part of
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

#include "omnetpp/chistogramstrategy.h"
#include "omnetpp/clog.h"
#include <iomanip>

namespace omnetpp {

// TODO: print histogram name in error messages

void cIHistogramStrategy::init(cHistogram *hist)
{
    if (hist->getCount() > 0)
        throw cRuntimeError(this, "Cannot initialize the histogram strategy with a non-empty histogram");
    this->hist = hist;
}

//----

void cFixedRangeHistogramStrategy::setupBins()
{
    // validate parameters
    if (mode == MODE_AUTO)
        throw cRuntimeError(this, "Mode cannot be MODE_AUTO");
    if (binSize <= 0)
        throw cRuntimeError(this, "Requested bin size must be positive");
    if (!std::isfinite(lo) || !std::isfinite(hi) || lo>=hi)
        throw cRuntimeError(this, "Invalid range (bounds must be finite and lo < hi)");
    if (mode == MODE_INTEGERS) {
        if (binSize != std::floor(binSize) || lo != std::floor(lo) || hi != std::floor(hi))
            throw cRuntimeError(this, "Bin size and range bounds must be integers");
        double numBins = (hi-lo) / binSize;
        if (numBins != std::floor(numBins))
            throw cRuntimeError(this, "Cannot make equal-sized bins (bin size does not divide histogram range)");
    }

    // set up bins
    hist->createUniformBins(lo, hi, binSize);
}

//----

void cFixedRangeHistogramStrategy::collect(double value)
{
    if (hist->getCount() == 1)
        setupBins();
    ASSERT(hist->getNumCells() > 0);
    hist->collectIntoHistogram(value);
}

void cFixedRangeHistogramStrategy::collectWeighted(double value, double weight)
{
    if (hist->getCount() == 1)
        setupBins();
    ASSERT(hist->getNumCells() > 0);
    hist->collectIntoHistogram(value, weight);
}

//----

void cPrecollectionBasedHistogramStrategy::moveValuesIntoHistogram()
{
    if (!hist->isWeighted())
        for (double value : values)
            hist->collectIntoHistogram(value);
    else
        for (size_t i = 0; i < values.size(); i++)
            hist->collectIntoHistogram(values[i], weights[i]);

    values.clear();
    weights.clear();
}

bool cPrecollectionBasedHistogramStrategy::binsCreated() const
{
    return hist && hist->getNumCells() > 0 && values.empty();
}

void cPrecollectionBasedHistogramStrategy::createBins()
{
    moveValuesIntoHistogram();
}

//----

void cGenericHistogramStrategy::collect(double value)
{
    if (hist->getNumCells() > 0) {
        hist->extendBinsTo(value, binSize);
        hist->collectIntoHistogram(value);
        if (hist->getNumCells() >= 2 * desiredNumBins) {
            if (hist->getNumCells() % 2 == 1)
                hist->extendBinsTo(hist->getBinEdges().back(), binSize);
            hist->mergeBins(2);
            binSize *= 2.0;
        }
    }
    else {
        values.push_back(value);
        if (values.size() == numToPrecollect) {
            setupBins();
            ASSERT(hist->getNumCells() > 0);
            moveValuesIntoHistogram();
        }
    }
}

void cGenericHistogramStrategy::collectWeighted(double value, double weight)
{
    if (hist->getNumCells() > 0) {
        hist->extendBinsTo(value, binSize);
        hist->collectIntoHistogram(value, weight);
        if (hist->getNumCells() >= 2 * desiredNumBins) {
            if (hist->getNumCells() % 2 == 1)
                            hist->extendBinsTo(hist->getBinEdges().back(), binSize);
            hist->mergeBins(2);
            binSize *= 2.0;
        }
    }
    else {
        values.push_back(value);
        weights.push_back(weight);
        if (values.size() == numToPrecollect) {
            setupBins();
            ASSERT(hist->getNumCells() > 0);
            moveValuesIntoHistogram();
        }
    }
}

static double roundToPowerOfTen(double x)
{
    return x == 0 ? 0
          : x > 0 ? std::pow(10.0, std::round(std::log10(x)))
          :        -std::pow(10.0, std::round(std::log10(-x)));
}

static double roundToOneTwoFive(double x)
{
    std::vector<double> factors = {1.0, 2.0, 5.0};
    std::vector<double> roundeds;

    for (double f : factors)
        roundeds.push_back(roundToPowerOfTen(x / f) * f);

    double result = x;
    double minError = INFINITY;

    for (double r : roundeds) {
        double error = std::abs(r - x);
        if (error < minError) {
            minError = error;
            result = r;
        }
    }

    return result;
}

void cGenericHistogramStrategy::setupBins()
{
    // determine mode (integers or reals) from precollected observations
    HistogramMode mode = this->mode;
    bool empty = hist->getCount() == 1;

    if (mode == MODE_AUTO) {
        bool allIntegers = true;
        bool allZeroes = true;
        for (size_t i = 0; i < values.size(); i++) {
            if (allIntegers && values[i] != std::floor(values[i]))
                allIntegers = false;
            if (allZeroes && values[i] != 0)
                allZeroes = false;
        }

        mode = (!empty && allIntegers && !allZeroes) ? MODE_INTEGERS : MODE_REALS;
    }

    // compute histogram
    double minValue = hist->getMin();
    double maxValue = hist->getMax();

    double binSize = -1;

    double c, r;
    if (empty)
        c = r = 0;
    else {
        c = (minValue + maxValue) / 2;
        r = (maxValue - minValue) * rangeExtensionFactor;
    }
    if (r == 0)
        r = 1.0;  // warning?

    double rangeMin = c - r / 2;
    double rangeMax = c + r / 2;

    binSize = computeBinSize(rangeMin, rangeMax);

    if (mode == MODE_INTEGERS)
        binSize = ceil(binSize);

    rangeMin = rangeMin - std::fmod(rangeMin, binSize);
    rangeMax = rangeMax - std::fmod(rangeMax, binSize);

    this->binSize = binSize;

    hist->createUniformBins(rangeMin, rangeMax, binSize);
}

double cGenericHistogramStrategy::computeBinSize(double& rangeMin, double& rangeMax)
{
    int numBins = this->desiredNumBins;
    if (numBins == -1)
        numBins = 30;  // to allow merging every 2, 3, 5, 6 adjacent bins during post-processing
    double bs = (rangeMax - rangeMin) / numBins;

    bs = roundToOneTwoFive(bs);

    return bs;
}

//----

void cAutoRangeHistogramStrategy::collect(double value)
{
    if (inPrecollection) {
        values.push_back(value);
        if (values.size() >= numToPrecollect) {
            setupBins();
            ASSERT(hist->getNumCells() > 0);
            moveValuesIntoHistogram();
            inPrecollection = false;
        }
    }
    else {
        hist->collectIntoHistogram(value);
    }
}

void cAutoRangeHistogramStrategy::collectWeighted(double value, double weight)
{
    if (inPrecollection) {
        values.push_back(value);
        weights.push_back(weight);
        if (values.size() >= numToPrecollect) {
            setupBins();
            ASSERT(hist->getNumCells() > 0);
            moveValuesIntoHistogram();
            inPrecollection = false;
        }
    }
    else {
        hist->collectIntoHistogram(value, weight);
    }
}

void cAutoRangeHistogramStrategy::setupBins()
{
    // determine mode (integers or reals) from precollected observations
    HistogramMode mode = this->mode;
    bool empty = hist->getCount() == 1;
    if (mode == MODE_AUTO) {
        bool allIntegers = true;
        bool allZeroes = true;
        for (size_t i = 0; i < values.size(); i++) {
            if (allIntegers && values[i] != std::floor(values[i]))
                allIntegers = false;
            if (allZeroes && values[i] != 0)
                allZeroes = false;
        }

        mode = (!empty && allIntegers && !allZeroes) ? MODE_INTEGERS : MODE_REALS;
    }

    // compute histogram
    double rangeMin = lo, rangeMax = hi;
    bool hasLo = !std::isnan(lo);
    bool hasHi = !std::isnan(hi);
    double minValue = hist->getMin();
    double maxValue = hist->getMax();
    if (!hasLo && !hasHi) {
        double c, r;
        if (empty)
            c = r = 0;
        else {
            c = (minValue + maxValue) / 2;
            r = (maxValue - minValue) * rangeExtensionFactor;
        }
        if (r == 0)
            r = 1.0;  // warning?
        rangeMin = c - r / 2;
        rangeMax = c + r / 2;
    }
    else if (hasHi && !hasLo) {
        if (hi <= minValue)
            rangeMin = hi - 1.0;  // warning?
        else
            rangeMin = rangeMax - (rangeMax - minValue) * rangeExtensionFactor;
    }
    else if (hasLo && !hasHi) {
        if (lo >= maxValue)
            rangeMax = lo + 1.0;  // warning?
        else
            rangeMax = lo + (maxValue - lo) * rangeExtensionFactor;
    }
    else {
        rangeMin = lo;
        rangeMax = hi;
    }

    double binSize = (mode == MODE_REALS) ?
            computeDoubleBinSize(rangeMin, rangeMax) :
            computeIntegerBinSize(rangeMin, rangeMax);

    if (binSizeRounding) {
        binSize = roundToOneTwoFive(binSize);
        if (binSize == 0)
            binSize = 1;
        rangeMin = rangeMin - std::fmod(rangeMin, binSize);
        rangeMax = rangeMax - std::fmod(rangeMax, binSize);
    }

    hist->createUniformBins(rangeMin, rangeMax, binSize);
}

double cAutoRangeHistogramStrategy::computeDoubleBinSize(double& rangeMin, double& rangeMax)
{
    int numBins = this->numBins;
    if (numBins == -1)
        numBins = 30;  // to allow merging every 2, 3, 5, 6 adjacent bins during post-processing
    return (rangeMax - rangeMin) / numBins;
}

#define COMPLAINT    "Cannot set up bins to satisfy constraints"

double cAutoRangeHistogramStrategy::computeIntegerBinSize(double& rangeMin, double& rangeMax)
{
    // set up the missing ones of: rangeMin, rangeMax, numBins, binSize;
    // throw error if not everything can be set up consistently

    // binsize is double but we want to calculate with integers here
    long binSize = -1;  //TODO (long)this->binSize;

    // convert range limits to integers
    rangeMin = floor(rangeMin);
    rangeMax = ceil(rangeMax);

    bool hasLo = !std::isnan(lo);
    bool hasHi = !std::isnan(hi);

    if (hasLo && hasHi) {
        long range = (long)(rangeMax - rangeMin);  //TODO double?

        if (numBins > 0 && binSize > 0) {
            if (numBins * binSize != range)
                throw cRuntimeError(this, COMPLAINT ": numBins*binSize != rangeMax-rangeMin");
        }
        else if (binSize > 0) {
            if (range % binSize != 0)
                throw cRuntimeError(this, COMPLAINT ": specified range is not a multiple of binSize");
            numBins = range / binSize;
        }
        else if (numBins > 0) {
            if (range % numBins != 0)
                throw cRuntimeError(this, COMPLAINT ": specified range is not a multiple of numBins");
            binSize = range / numBins;
        }
        else {
            int minCellsize = (int)ceil(range / 200.0);
            int maxCellsize = (int)ceil(range / 10.0);
            for (binSize = minCellsize; binSize <= maxCellsize; binSize++)
                if (range % binSize == 0)
                    break;

            if (binSize > maxCellsize)
                throw cRuntimeError(this, COMPLAINT ": Specified range is too large, and cannot divide it to 10..200 equal-sized bins");
            numBins = range / binSize;
        }
    }
    else {
        // non-fixed range
        if (numBins > 0 && binSize > 0) {
            // both given; numBins*binSize will determine the range
        }
        else if (numBins > 0) {
            // numBins given ==> choose binSize
            binSize = (long)ceil((rangeMax - rangeMin) / numBins);
        }
        else if (binSize > 0) {
            // binSize given ==> choose numBins
            numBins = (int)ceil((rangeMax - rangeMin) / binSize);
        }
        else {
            // neither given, choose both
            double range = rangeMax - rangeMin;
            binSize = (long)ceil(range / 200.0);  // for range<=200, binSize==1
            numBins = (int)ceil(range / binSize);
        }

        // NOTE: HERE WE APPARENTLY OVERWRITE rangeMin/rangeMax, so result of previous computation only applies to DOUBLES!!!

        // adjust range to be binSize*numBins
        double newRange = binSize * numBins;
        double rangeDiff = newRange - (rangeMax - rangeMin);

        if (!hasLo && !hasHi) {
            rangeMin -= floor(rangeDiff / 2);
            rangeMax = rangeMin + newRange;
        }
        else if (hasHi) {
            rangeMin = rangeMax - newRange;
        }
        else if (hasLo) {
            rangeMax = rangeMin + newRange;
        }
    }
    return binSize;
}

#undef COMPLAINT

}  // namespace omnetpp


