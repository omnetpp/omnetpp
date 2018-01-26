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

#include "omnetpp/regmacros.h"
#include "omnetpp/onstartup.h"
#include "omnetpp/globals.h"
#include "omnetpp/chistogramstrategy.h"

namespace omnetpp {

Register_Class(cFixedRangeHistogramStrategy);
Register_Class(cDefaultHistogramStrategy);
Register_Class(cAutoRangeHistogramStrategy);

void cIHistogramStrategy::setHistogram(cHistogram *hist)
{
    if (hist->getCount() > 0)
        throw cRuntimeError(hist, "%s: Cannot initialize the histogram strategy with a non-empty histogram", getClassName());
    this->hist = hist;
}

//----

void cFixedRangeHistogramStrategy::copy(const cFixedRangeHistogramStrategy& other)
{
    lo = other.lo;
    hi = other.hi;
    numBins = other.numBins;
    mode = other.mode;
}

cFixedRangeHistogramStrategy& cFixedRangeHistogramStrategy::operator=(const cFixedRangeHistogramStrategy& other)
{
    cIHistogramStrategy::operator=(other);
    copy(other);
    return *this;
}

void cFixedRangeHistogramStrategy::setUpBins()
{
    // validate parameters
    if (mode == cHistogram::MODE_AUTO)
        throw cRuntimeError(hist, "%s: Mode cannot be cHistogram::MODE_AUTO", getClassName());
    if (!std::isfinite(lo) || !std::isfinite(hi) || lo >= hi)
        throw cRuntimeError(hist, "%s: Invalid range (bounds must be finite and lo < hi)", getClassName());
    if (numBins <= 0)
        throw cRuntimeError(hist, "%s: Number of bins must be positive", getClassName());

    binSize = (hi-lo) / numBins;

    if (mode == cHistogram::MODE_INTEGERS) {
        if (lo != std::floor(lo) || hi != std::floor(hi))
            throw cRuntimeError(hist, "%s: Range bounds must be whole numbers as INTEGERS mode was requested", getClassName());
        if (binSize != std::floor(binSize))
            throw cRuntimeError(hist, "%s: Cannot make %d equal-sized bins in INTEGERS mode over the specified interval", getClassName(), numBins);
    }

    // set up bins
    hist->createUniformBins(lo, hi, binSize);
}

void cFixedRangeHistogramStrategy::collect(double value)
{
    if (!hist->binsAlreadySetUp())
        setUpBins();
    ASSERT(hist->getNumBins() > 0);
    hist->collectIntoHistogram(value);
}

void cFixedRangeHistogramStrategy::collectWeighted(double value, double weight)
{
    if (!hist->binsAlreadySetUp())
        setUpBins();
    ASSERT(hist->getNumBins() > 0);
    hist->collectIntoHistogram(value, weight);
}

//----

void cPrecollectionBasedHistogramStrategy::copy(const cPrecollectionBasedHistogramStrategy& other)
{
    inPrecollection = other.inPrecollection;
    numToPrecollect = other.numToPrecollect;
    values = other.values;
    weights = other.weights;
}

cPrecollectionBasedHistogramStrategy& cPrecollectionBasedHistogramStrategy::operator=(const cPrecollectionBasedHistogramStrategy& other)
{
    cIHistogramStrategy::operator=(other);
    copy(other);
    return *this;
}

void cPrecollectionBasedHistogramStrategy::precollect(double value, double weight)
{
    ASSERT(inPrecollection);
    if (!values.empty() && value == values.front()) // prevent degenerate case of the first value being repeated numToPrecollect times
        weights.front() += weight;
    else {
        values.push_back(value);
        weights.push_back(weight);
    }
}

void cPrecollectionBasedHistogramStrategy::moveValuesIntoHistogram()
{
    size_t numValues = values.size();
    for (size_t i = 0; i < numValues; i++)
        hist->collectIntoHistogram(values[i], weights[i]);
    values.clear();
    weights.clear();
}

void cPrecollectionBasedHistogramStrategy::setUpBins()
{
    createBins();
    moveValuesIntoHistogram();
}

void cPrecollectionBasedHistogramStrategy::clear()
{
    inPrecollection = true;
    values.clear();
    weights.clear();
}

//----

void cDefaultHistogramStrategy::copy(const cDefaultHistogramStrategy& other)
{
    rangeExtensionFactor = other.rangeExtensionFactor;
    binSize = other.binSize;
    numBinsHint = other.numBinsHint;
    targetNumBins = other.targetNumBins;
    mode = other.mode;
    autoExtend = other.autoExtend;
    binMerging =other.binMerging;
    maxNumBins = other.maxNumBins;
}

cDefaultHistogramStrategy& cDefaultHistogramStrategy::operator=(const cDefaultHistogramStrategy& other)
{
    cPrecollectionBasedHistogramStrategy::operator=(other);
    copy(other);
    return *this;
}

void cDefaultHistogramStrategy::collect(double value)
{
    collectWeighted(value, 1);
}

void cDefaultHistogramStrategy::collectWeighted(double value, double weight)
{
    if (inPrecollection) {
        precollect(value, weight);
        if (values.size() == numToPrecollect) {
            createBins();
            moveValuesIntoHistogram();
            inPrecollection = false;
        }
    }
    else {
        if (autoExtend)
            extendBinsTo(value);
        hist->collectIntoHistogram(value, weight);
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

void cDefaultHistogramStrategy::createBins()
{
    targetNumBins = numBinsHint == -1 ? DEFAULT_NUM_BINS : numBinsHint;

    if (values.empty()) {
        this->binSize = 1;
        hist->createUniformBins(0, 1, binSize);
        return;
    }

    // determine mode (integers or reals) from precollected observations
    Mode mode = this->mode;

    if (mode == cHistogram::MODE_AUTO) {
        bool allIntegers = true;
        bool allZeroes = true;
        for (size_t i = 0; i < values.size(); i++) {
            if (allIntegers && values[i] != std::floor(values[i]))
                allIntegers = false;
            if (allZeroes && values[i] != 0)
                allZeroes = false;
        }

        mode = (allIntegers && !allZeroes) ? cHistogram::MODE_INTEGERS : cHistogram::MODE_REALS;
    }

    // compute histogram
    double minValue = hist->getMin();
    double maxValue = hist->getMax();

    double c = (minValue + maxValue) / 2;
    double r = (maxValue - minValue) * rangeExtensionFactor;
    if (r == 0)
        r = 1.0;  // warning?

    double rangeMin = c - r / 2;
    double rangeMax = c + r / 2;

    if (rangeMin < 0 && minValue >= 0 && maxValue > 0)
        rangeMin = 0; // do not go into negative unless warranted by the collected data

    double binSize = computeBinSize(rangeMin, rangeMax);

    if (mode == cHistogram::MODE_INTEGERS)
        binSize = ceil(binSize);

    rangeMin = binSize * std::floor(rangeMin / binSize); // snap
    rangeMax = binSize * std::ceil(rangeMax / binSize);

    this->binSize = binSize;

    hist->createUniformBins(rangeMin, rangeMax, binSize);

    // auto-extending now is needed, otherwise collectIntoHistogram() will
    // create underflows/overflows and we'll run into problems later when
    // new observations will try to further extend the histogram
    extendBinsTo(hist->getMin());
    extendBinsTo(hist->getMax());

    ASSERT(hist->getBinEdges().front() <= hist->getMin());
    ASSERT(hist->getBinEdges().back() > hist->getMax());
    ASSERT(hist->getNumBins() >  0);
}

double cDefaultHistogramStrategy::computeBinSize(double& rangeMin, double& rangeMax)
{
    double binSize = (rangeMax - rangeMin) / targetNumBins;
    binSize = roundToOneTwoFive(binSize);
    return binSize;
}

void cDefaultHistogramStrategy::extendBinsTo(double value)
{
    bool isUnderflow = value < hist->getBinEdges().front();
    bool isOverflow = value >= hist->getBinEdges().back();
    if (!isUnderflow && !isOverflow)
        return; // nothing to do
    if (isUnderflow && hist->getNumUnderflows() > 0)
        return; // cannot extend
    if (isOverflow && hist->getNumOverflows() > 0)
        return; // cannot extend

    if (binMerging) {
        hist->extendBinsTo(value, binSize);
        if (hist->getNumBins() > (targetNumBins*3)/2)
            reduceNumBinsTo(targetNumBins);
    }
    else
        hist->extendBinsTo(value, binSize, maxNumBins);
}

void cDefaultHistogramStrategy::reduceNumBinsTo(int numBinsLimit)
{
    int numBins = hist->getNumBins();
    int groupSize = (numBins + numBinsLimit-1) / numBinsLimit;
    ASSERT(groupSize >= 2);

    // create more bins to get a multiple of groupSize
    if (numBins % groupSize != 0) {
        int numBinsToAdd = groupSize - numBins % groupSize;
        std::vector<double> newEdges;
        double lastEdge = hist->getBinEdge(numBins);
        for (int i = 0; i < numBinsToAdd; i++)
            newEdges.push_back(lastEdge + (i+1)*binSize);
        hist->appendBins(newEdges);
    }

    hist->mergeBins(groupSize);
    binSize *= groupSize;
}

//----

void cAutoRangeHistogramStrategy::copy(const cAutoRangeHistogramStrategy& other)
{
    lo = other.lo;
    hi = other.hi;
    rangeExtensionFactor = other.rangeExtensionFactor;
    numBinsHint = other.numBinsHint;
    targetNumBins = other.targetNumBins;
    binSize = other.binSize;
    mode = other.mode;
    binSizeRounding = other.binSizeRounding;
    autoExtend = other.autoExtend;
    binMerging = other.binMerging;
}

cAutoRangeHistogramStrategy& cAutoRangeHistogramStrategy::operator=(const cAutoRangeHistogramStrategy& other)
{
    cPrecollectionBasedHistogramStrategy::operator=(other);
    copy(other);
    return *this;
}

void cAutoRangeHistogramStrategy::collect(double value)
{
    collectWeighted(value, 1.0);
}

void cAutoRangeHistogramStrategy::collectWeighted(double value, double weight)
{
    if (inPrecollection) {
        precollect(value, weight);
        bool needPrecollection = (mode == cHistogram::MODE_AUTO) || std::isnan(lo) || std::isnan(hi); // if true, we precollect the first value and immediately set up the bins
        if (!needPrecollection || values.size() >= numToPrecollect) {
            createBins();
            ASSERT(hist->getNumBins() > 0);
            moveValuesIntoHistogram();
            inPrecollection = false;
        }
    }
    else {
        if (autoExtend)
            extendBinsTo(value);
        hist->collectIntoHistogram(value, weight);
    }
}

void cAutoRangeHistogramStrategy::createBins()
{
    targetNumBins = numBinsHint == -1 ? DEFAULT_NUM_BINS : numBinsHint;

    // determine mode (integers or reals) from precollected observations
    Mode mode = this->mode;
    if (mode == cHistogram::MODE_AUTO) {
        bool allIntegers = true;
        bool allZeroes = true;
        for (size_t i = 0; i < values.size(); i++) {
            if (allIntegers && values[i] != std::floor(values[i]))
                allIntegers = false;
            if (allZeroes && values[i] != 0)
                allZeroes = false;
        }

        mode = (!values.empty() && allIntegers && !allZeroes) ? cHistogram::MODE_INTEGERS : cHistogram::MODE_REALS;
    }

    // compute range from the range of the observations
    bool hasLo = !std::isnan(lo);
    bool hasHi = !std::isnan(hi);
    if (hasLo && hasHi && lo >= hi)
        throw cRuntimeError(hist, "%s: Invalid range specified, must be lo < hi", getClassName());
    double c, r;
    if (hist->getCount() == 0)
        c = r = 0;
    else {
        double minValue = hist->getMin();
        double maxValue = hist->getMax();
        c = (minValue + maxValue) / 2;
        r = (maxValue - minValue) * rangeExtensionFactor;
    }
    double rangeMin = hasLo ? lo : c - r / 2;
    double rangeMax = hasHi ? hi : c + r / 2;

    if (!hasLo && rangeMin < 0 && hist->getCount() > 0 && hist->getMin() >= 0 && hist->getMax() > 0)
        rangeMin = 0; // do not go into negative unless warranted by the collected data

    if (rangeMin >= rangeMax) {
        if (hasHi)
            rangeMin = rangeMax - 1;
        else
            rangeMax = rangeMin + 1;
    }

    if (mode == cHistogram::MODE_INTEGERS) {
        rangeMin = floor(rangeMin);
        rangeMax = ceil(rangeMax);
    }

    // determine bin size
    if (!std::isnan(requestedBinSize)) {
        binSize = requestedBinSize;
    }
    else {
        double approxBinSize = (rangeMax - rangeMin) / targetNumBins;
        binSize = (mode == cHistogram::MODE_INTEGERS) ? ceil(approxBinSize) : approxBinSize;

        if (binSizeRounding) {
            binSize = roundToOneTwoFive(binSize);
            if (binSize == 0)
                binSize = 1;
            rangeMin = binSize * std::floor(rangeMin / binSize); // snap
            rangeMax = binSize * std::ceil(rangeMax / binSize);
            if (rangeMin >= rangeMax)
                rangeMax = rangeMin + 1;
        }
    }
    ASSERT(binSize > 0);

    // adjust range to be a multiple of binSize, especially for the MODE_INTEGERS case
    int numBins = numBinsHint > 0 ? numBinsHint : (int)ceil((rangeMax - rangeMin) / binSize);
    ASSERT(numBins > 0);

    double newRange = binSize * numBins;
    if (!hasLo && !hasHi) {
        double rangeDiff = newRange - (rangeMax - rangeMin);
        rangeMin -= (mode == cHistogram::MODE_INTEGERS) ? floor(rangeDiff / 2) : rangeDiff / 2;
        if (!hasLo && rangeMin < 0 && hist->getCount() > 0 && hist->getMin() >= 0 && hist->getMax() > 0)
             rangeMin = 0; // do not go into negative unless warranted by the collected data
        rangeMax = rangeMin + newRange;
    }
    else if (!hasLo) {
        rangeMin = rangeMax - newRange;
    }
    else if (!hasHi) {
        rangeMax = rangeMin + newRange;
    }
    else {
        // we have both
    }

    // set up the histogram
    hist->createUniformBins(rangeMin, rangeMax, binSize);

    if (autoExtend) {
        // auto-extending now is needed, otherwise collectIntoHistogram() will
        // create underflows/overflows and we'll run into problems later when
        // new observations will try to further extend the histogram
        extendBinsTo(hist->getMin());
        extendBinsTo(hist->getMax());
    }

    ASSERT(hist->getNumBins() > 0);
}

void cAutoRangeHistogramStrategy::extendBinsTo(double value)
{
    bool isUnderflow = value < hist->getBinEdges().front();
    bool isOverflow = value >= hist->getBinEdges().back();
    if (!isUnderflow && !isOverflow)
        return; // nothing to do
    if (isUnderflow && (hist->getNumUnderflows() > 0 || !std::isnan(lo)))
        return; // cannot extend
    if (isOverflow &&  (hist->getNumOverflows() > 0 || !std::isnan(hi)))
        return; // cannot extend

    if (binMerging) {
        hist->extendBinsTo(value, binSize);
        if (hist->getNumBins() > (targetNumBins*3)/2)
            reduceNumBinsTo(targetNumBins);
    }
    else
        hist->extendBinsTo(value, binSize, maxNumBins);
}

void cAutoRangeHistogramStrategy::reduceNumBinsTo(int numBinsLimit)
{
    int numBins = hist->getNumBins();
    int groupSize = (numBins + numBinsLimit-1) / numBinsLimit;
    ASSERT(groupSize >= 2);

    // create more bins to get a multiple of groupSize
    if (numBins % groupSize != 0) {
        int numBinsToAdd = groupSize - numBins % groupSize;
        std::vector<double> newEdges;
        double lastEdge = hist->getBinEdge(numBins);
        for (int i = 0; i < numBinsToAdd; i++)
            newEdges.push_back(lastEdge + (i+1)*binSize);
        hist->appendBins(newEdges);
    }

    hist->mergeBins(groupSize);
    binSize *= groupSize;
}

void cAutoRangeHistogramStrategy::clear()
{
    cPrecollectionBasedHistogramStrategy::clear();
    binSize = NAN;
}

}  // namespace omnetpp


