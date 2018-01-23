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
#include "omnetpp/chistogramstrategy.h"

namespace omnetpp {

//TODO
//Register_Class(cFixedRangeHistogramStrategy);
//Register_Class(cDefaultHistogramStrategy);
//Register_Class(cAutoRangeHistogramStrategy);

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
    binSize = other.binSize;
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
    //TODO assert all params are not NAN

    // validate parameters
    if (mode == cHistogram::MODE_AUTO)
        throw cRuntimeError(hist, "%s: Mode cannot be cHistogram::MODE_AUTO", getClassName());
    if (binSize <= 0)
        throw cRuntimeError(hist, "%s: Requested bin size must be positive", getClassName());
    if (!std::isfinite(lo) || !std::isfinite(hi) || lo >= hi)
        throw cRuntimeError(hist, "%s: Invalid range (bounds must be finite and lo < hi)", getClassName());
    if (mode == cHistogram::MODE_INTEGERS) {
        if (binSize != std::floor(binSize) || lo != std::floor(lo) || hi != std::floor(hi))
            throw cRuntimeError(hist, "%s: Bin size and range bounds must be integers", getClassName());
        double numBins = (hi-lo) / binSize;
        if (numBins != std::floor(numBins))
            throw cRuntimeError(hist, "%s: Cannot make equal-sized bins (bin size does not divide histogram range)", getClassName());
    }

    // set up bins
    hist->createUniformBins(lo, hi, binSize);
}

void cFixedRangeHistogramStrategy::collect(double value)
{
    if (!hist->binsAlreadySetUp())
        setUpBins();
    ASSERT(hist->getNumBins() > 0);
    if (autoExtend)
        hist->extendBinsTo(value, binSize); //TODO extra arg: maxNumBins as protection
    hist->collectIntoHistogram(value);
}

void cFixedRangeHistogramStrategy::collectWeighted(double value, double weight)
{
    if (!hist->binsAlreadySetUp())
        setUpBins();
    ASSERT(hist->getNumBins() > 0);
    if (autoExtend)
        hist->extendBinsTo(value, binSize);
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
    desiredNumBins = other.desiredNumBins;
    mode = other.mode;
}

cDefaultHistogramStrategy& cDefaultHistogramStrategy::operator=(const cDefaultHistogramStrategy& other)
{
    cPrecollectionBasedHistogramStrategy::operator=(other);
    copy(other);
    return *this;
}

void cDefaultHistogramStrategy::collect(double value)
{
    if (inPrecollection) {
        precollect(value);
        if (values.size() == numToPrecollect) {
            createBins();
            moveValuesIntoHistogram();
            inPrecollection = false;
        }
    }
    else {
        hist->extendBinsTo(value, binSize);
        hist->collectIntoHistogram(value);
        if (hist->getNumBins() >= 2 * desiredNumBins) {
            if (hist->getNumBins() % 2 == 1)
                hist->extendBinsTo(hist->getBinEdges().back(), binSize);
            hist->mergeBins(2);
            binSize *= 2.0;
        }
    }
}

void cDefaultHistogramStrategy::collectWeighted(double value, double weight)
{
    if (inPrecollection) {
        precollect(value, weight);
        if (values.size() == numToPrecollect) {
            createBins();
            moveValuesIntoHistogram();
        }
    }
    else {
        hist->extendBinsTo(value, binSize);
        hist->collectIntoHistogram(value, weight);
        if (hist->getNumBins() >= 2 * desiredNumBins) {
            if (hist->getNumBins() % 2 == 1)
                hist->extendBinsTo(hist->getBinEdges().back(), binSize);
            hist->mergeBins(2);
            binSize *= 2.0;
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

void cDefaultHistogramStrategy::createBins()
{
    bool empty = values.empty();

    // determine mode (integers or reals) from precollected observations
    HistogramMode mode = this->mode;

    if (mode == cHistogram::MODE_AUTO) {
        bool allIntegers = true;
        bool allZeroes = true;
        for (size_t i = 0; i < values.size(); i++) {
            if (allIntegers && values[i] != std::floor(values[i]))
                allIntegers = false;
            if (allZeroes && values[i] != 0)
                allZeroes = false;
        }

        mode = (!empty && allIntegers && !allZeroes) ? cHistogram::MODE_INTEGERS : cHistogram::MODE_REALS;
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

    if (mode == cHistogram::MODE_INTEGERS)
        binSize = ceil(binSize);

    rangeMin = rangeMin - std::fmod(rangeMin, binSize);
    rangeMax = rangeMax - std::fmod(rangeMax, binSize);

    this->binSize = binSize;

    hist->createUniformBins(rangeMin, rangeMax, binSize);

    ASSERT(hist->getNumBins() >  0);
}

double cDefaultHistogramStrategy::computeBinSize(double& rangeMin, double& rangeMax)
{
    int numBins = this->desiredNumBins;
    if (numBins == -1)
        numBins = 30;  // to allow merging every 2, 3, 5, 6 adjacent bins during post-processing
    double bs = (rangeMax - rangeMin) / numBins;

    bs = roundToOneTwoFive(bs);

    return bs;
}

//----

void cAutoRangeHistogramStrategy::copy(const cAutoRangeHistogramStrategy& other)
{
    lo = other.lo;
    hi = other.hi;
    rangeExtensionFactor = other.rangeExtensionFactor;
    desiredNumBins = other.desiredNumBins;
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
            hist->extendBinsTo(value, binSize);
        hist->collectIntoHistogram(value, weight);
        if (binMerging && desiredNumBins > 0 && hist->getNumBins() >= 2*desiredNumBins)
            mergeBins();
    }
}

void cAutoRangeHistogramStrategy::createBins()
{
    bool empty = hist->getCount() <= 1;

    // determine mode (integers or reals) from precollected observations
    HistogramMode mode = this->mode;
    if (mode == cHistogram::MODE_AUTO) {
        bool allIntegers = true;
        bool allZeroes = true;
        for (size_t i = 0; i < values.size(); i++) {
            if (allIntegers && values[i] != std::floor(values[i]))
                allIntegers = false;
            if (allZeroes && values[i] != 0)
                allZeroes = false;
        }

        mode = (!empty && allIntegers && !allZeroes) ? cHistogram::MODE_INTEGERS : cHistogram::MODE_REALS;
    }

    // compute range from the range of the observations

    //TODO instead: take extended range, then replace its lower/upper endpoint with lo/hi, whichever is given; then fix up result so that lower<upper
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
            r = 1.0;
        rangeMin = c - r / 2;
        rangeMax = c + r / 2;
    }
    else if (hasHi && !hasLo) {
        if (hi <= minValue)
            rangeMin = hi - 1.0;
        else
            rangeMin = rangeMax - (rangeMax - minValue) * rangeExtensionFactor;
    }
    else if (hasLo && !hasHi) {
        if (lo >= maxValue)
            rangeMax = lo + 1.0;
        else
            rangeMax = lo + (maxValue - lo) * rangeExtensionFactor;
    }
    else {
        if (lo >= hi)
            throw cRuntimeError(hist, "%s: Invalid range specified, must be lo < hi", getClassName());
        rangeMin = lo;
        rangeMax = hi;
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
        int approxNumBins = desiredNumBins == -1 ? DEFAULT_NUM_BINS : desiredNumBins;
        double approxBinSize = (rangeMax - rangeMin) / approxNumBins;
        binSize = (mode == cHistogram::MODE_INTEGERS) ? ceil(approxBinSize) : approxBinSize;

        if (binSizeRounding) {
            binSize = roundToOneTwoFive(binSize);
            if (binSize == 0)
                binSize = 1;
            rangeMin = rangeMin - std::fmod(rangeMin, binSize); // snap
            rangeMax = rangeMax - std::fmod(rangeMax, binSize);
            if (rangeMin >= rangeMax)
                rangeMax = rangeMin + 1;
        }
    }
    ASSERT(binSize > 0);

    // adjust range to be a multiple of binSize, especially for the MODE_INTEGERS case
    int numBins = desiredNumBins > 0 ? desiredNumBins : (int)ceil((rangeMax - rangeMin) / binSize);
    ASSERT(numBins > 0);

    double newRange = binSize * numBins;
    if (!hasLo && !hasHi) {
        double rangeDiff = newRange - (rangeMax - rangeMin);
        rangeMin -= (mode == cHistogram::MODE_INTEGERS) ? floor(rangeDiff / 2) : rangeDiff / 2;
        rangeMax = rangeMin + newRange;
    }
    else if (hasHi) {
        rangeMin = rangeMax - newRange;
    }
    else if (hasLo) {
        rangeMax = rangeMin + newRange;
    }
    else {
        //TODO check consistency, esp. in the integer case?
    }

    hist->createUniformBins(rangeMin, rangeMax, binSize);

    ASSERT(hist->getNumBins() > 0);
}

void cAutoRangeHistogramStrategy::mergeBins() //TODO employ this in Fixed and Default strategies too?
{
    int numBins = hist->getNumBins();
    int groupSize = (numBins + desiredNumBins-1) / desiredNumBins;

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
}

void cAutoRangeHistogramStrategy::clear()
{
    cPrecollectionBasedHistogramStrategy::clear();
    binSize = NAN;
}

}  // namespace omnetpp


