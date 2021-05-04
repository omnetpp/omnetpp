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

#include <algorithm>
#include <limits>
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

    double binSize = (hi-lo) / numBins;

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
    numToCollate = other.numToCollate;
    lastRange = other.lastRange;
    rangeUnchangedCounter = other.rangeUnchangedCounter;
    rangeUnchangedThreshold = other.rangeUnchangedThreshold;
    finiteMinValue = other.finiteMinValue;
    finiteMaxValue = other.finiteMaxValue;
    values = other.values;
    weights = other.weights;
}

cPrecollectionBasedHistogramStrategy& cPrecollectionBasedHistogramStrategy::operator=(const cPrecollectionBasedHistogramStrategy& other)
{
    cIHistogramStrategy::operator=(other);
    copy(other);
    return *this;
}

bool cPrecollectionBasedHistogramStrategy::precollect(double value, double weight)
{
    // precollect value
    ASSERT(inPrecollection);
    if (value == std::floor(value)) {
        size_t searchLimit = std::min((size_t)numToCollate, values.size());
        size_t i;
        for (i = 0; i < searchLimit; i++)
            if (values[i] == value)
                break;
        if (i < searchLimit)
            weights[i] += weight;
        else {
            values.push_back(value);
            weights.push_back(weight);
        }
    }
    else {
        values.push_back(value);
        weights.push_back(weight);
    }

    // decide when to stop precollection
    double range = finiteMaxValue - finiteMinValue;
    if (lastRange == range)
        rangeUnchangedCounter++;
    else {
        lastRange = range;
        rangeUnchangedCounter = 0;
    }
    bool over = rangeUnchangedCounter >= rangeUnchangedThreshold || values.size() > numToPrecollect;
    return over;
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
    inPrecollection = false;
}

void cPrecollectionBasedHistogramStrategy::clear()
{
    inPrecollection = true;
    lastRange = NAN;
    rangeUnchangedCounter = 0;
    finiteMinValue = finiteMaxValue = NAN;
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
    binMerging = other.binMerging;
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
    if (std::isfinite(value)) {
        if (std::isnan(finiteMinValue) || value < finiteMinValue)
            finiteMinValue = value;
        if (std::isnan(finiteMaxValue) || value > finiteMaxValue)
            finiteMaxValue = value;
    }

    if (inPrecollection) {
        if (precollect(value, weight))
            setUpBins();
    }
    else {
        if (autoExtend)
            extendBinsTo(value);
        hist->collectIntoHistogram(value, weight);
    }

    ASSERT(hist->getUnderflowSumWeights() - hist->getNegInfSumWeights() == 0);
    ASSERT(hist->getOverflowSumWeights() - hist->getPosInfSumWeights() == 0);
}

static double roundToPowerOfTen(double x)
{
/*  The function implements the following code using loops because
    the pow() and log10() functions behave slightly differently on
    MinGW returning different results for the same input on Linux vs. Windows.

    return x == 0 ? 0
          : x > 0 ? std::pow(10.0, std::round(std::log10(x)))
          :        -std::pow(10.0, std::round(std::log10(-x)));
*/
    if (x == 0.0)
        return 0.0;

    bool negative = false;
    if (x < 0.0) {
        negative = true;
        x = -x;
    }

    // between the lower and upper bounds the returned result should be 1.0
    const double round_boundary_upper = 3.162277660168379522787063251598738133907;   // pow(10, 0.5)
    const double round_boundary_lower = 0.3162277660168379522787063251598738133907;  // pow(10, 0.5) / 10
    double result = 1.0;
    // count how many times x should be divided/multiplied by 10 to get it between the lower and upper bounds
    if (x > round_boundary_upper) {
        while (x > round_boundary_upper) {
            x /= 10.0;
            result *= 10.0;
        }
    } else {
        while (x <= round_boundary_lower) {
            x *= 10.0;
            result /= 10.0;
        }
    }

    return negative ? -result : result;
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
    double minValue = finiteMinValue;
    double maxValue = finiteMaxValue;

    double c = (minValue + maxValue) / 2;
    double r = (maxValue - minValue) * rangeExtensionFactor;
    if (r == 0)
        r = 1.0;  // warning?

    // these might still be the same value because of double imprecision
    double rangeMin = c - r / 2;
    double rangeMax = c + r / 2;

    if (rangeMin < 0 && minValue >= 0 && maxValue > 0)
        rangeMin = 0; // do not go into negative unless warranted by the collected data

    // just to avoid dividing by zero...
    if (rangeMax-rangeMin < std::numeric_limits<decltype(rangeMax)>::denorm_min() * targetNumBins)
        binSize = 1;
    else {
        binSize = (rangeMax - rangeMin) / targetNumBins;
        binSize = roundToOneTwoFive(binSize);
    }
    if (mode == cHistogram::MODE_INTEGERS)
        binSize = ceil(binSize);

    rangeMin = binSize * std::floor(rangeMin / binSize); // snap
    rangeMax = binSize * std::ceil(rangeMax / binSize);
    if (rangeMin == rangeMax) { // this can still happen if the single collected value is around 1e17
        rangeMax = nextafter(rangeMin, INFINITY);
        binSize = rangeMax - rangeMin; // this should help avoid further problems (unless we are right where the precision drops even further)
    }
    hist->createUniformBins(rangeMin, rangeMax, binSize);

    // auto-extending now is needed, otherwise collectIntoHistogram() will
    // create underflows/overflows and we'll run into problems later when
    // new observations will try to further extend the histogram
    // TODO: do we really need the next two lines? (unless rangeExtensionFactor < 1.0?)
    extendBinsTo(finiteMinValue);
    extendBinsTo(finiteMaxValue);

    ASSERT(hist->getBinEdges().front() <= finiteMinValue);
    ASSERT(hist->getBinEdges().back() > finiteMaxValue);
    ASSERT(hist->getNumBins() > 0);
}

void cDefaultHistogramStrategy::extendBinsTo(double value)
{
    double firstEdge = hist->getBinEdges().front();
    double lastEdge = hist->getBinEdges().back();
    bool isUnderflow = (value < firstEdge);
    bool isOverflow = (value >= lastEdge);
    if (!isUnderflow && !isOverflow)
        return; // nothing to do
    if (isUnderflow && hist->getUnderflowSumWeights() > 0)
        return; // cannot extend
    if (isOverflow && hist->getOverflowSumWeights() > 0)
        return; // cannot extend

    if (binMerging) {  //TODO remove the possibility to turn off binMerging!
        double range = lastEdge - firstEdge;
        double newRange = std::max(value, lastEdge) - std::min(value, firstEdge);
        double newApproxBinSize = newRange / targetNumBins;
        if (newApproxBinSize >= range)
            mergeAllBinsIntoOne(newApproxBinSize);
        hist->extendBinsTo(value, binSize);
        if (hist->getNumBins() > (targetNumBins*3)/2)
            reduceNumBinsTo(targetNumBins);
    }
    else
        hist->extendBinsTo(value, binSize, maxNumBins);

    ASSERT(hist->getBinEdges().front() <= value);
    ASSERT(hist->getBinEdges().back() > value);
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
        for (int i = 0; i < numBinsToAdd; i++) {
            double newEdge = lastEdge + (i+1)*binSize;
            // The condition below makes sure we won't add any consequential edges that
            // are exactly equal, despite our best efforts to avoid this, due to the
            // limited precision of the `double` type in some cases (big values, small binSize).
            if (newEdge > lastEdge && (newEdges.empty() || newEdge > newEdges.back()))
                newEdges.push_back(newEdge);
        }
        hist->appendBins(newEdges);
    }

    hist->mergeBins(groupSize);
    binSize *= groupSize;
}

void cDefaultHistogramStrategy::mergeAllBinsIntoOne(double newApproxBinSize)
{
    // merge the whole existing histogram into a single bin of the given size (approximately)
    double firstEdge = hist->getBinEdges().front();
    double lastEdge = hist->getBinEdges().back();
    double range = lastEdge - firstEdge;

    // determine new bin size (round, and >=range)
    double newBinSize = roundToOneTwoFive(newApproxBinSize);
    if (newBinSize < range)
        newBinSize = newBinSize * std::ceil(range / newBinSize);

    // compute new histogram range
    double newFirstEdge = firstEdge;
    double newLastEdge = lastEdge;
    if (hist->getUnderflowSumWeights() == 0 && hist->getOverflowSumWeights() == 0) {  // TODO this is always TRUE in our case! as binMerging cannot be turned off
        newFirstEdge = newBinSize * std::floor(newFirstEdge / newBinSize); // snap
        newLastEdge = newBinSize * std::ceil(newLastEdge / newBinSize); // snap
        newBinSize = newLastEdge - newFirstEdge;
    }
    else if (hist->getUnderflowSumWeights() == 0)
        newFirstEdge = newLastEdge - newBinSize;
    else if (hist->getNumOverflows() == 0)
        newLastEdge = newFirstEdge + newBinSize;
    else
        ; // cannot extend range

    hist->mergeBins(hist->getNumBins());
    if (newFirstEdge != firstEdge)
        hist->prependBins(std::vector<double> { newFirstEdge });
    if (newLastEdge != lastEdge)
        hist->appendBins(std::vector<double> { newLastEdge });
    hist->mergeBins(hist->getNumBins());
    binSize = newBinSize;
}

//----

void cAutoRangeHistogramStrategy::copy(const cAutoRangeHistogramStrategy& other)
{
    lo = other.lo;
    hi = other.hi;
    rangeExtensionFactor = other.rangeExtensionFactor;
    numBinsHint = other.numBinsHint;
    targetNumBins = other.targetNumBins;
    requestedBinSize = other.requestedBinSize;
    binSize = other.binSize;
    mode = other.mode;
    binSizeRounding = other.binSizeRounding;
    autoExtend = other.autoExtend;
    binMerging = other.binMerging;
    maxNumBins = other.maxNumBins;
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
    if (std::isfinite(value)) {
        if (std::isnan(finiteMinValue) || value < finiteMinValue)
            finiteMinValue = value;
        if (std::isnan(finiteMaxValue) || value > finiteMaxValue)
            finiteMaxValue = value;
    }

    if (inPrecollection) {
        bool needPrecollection = (mode == cHistogram::MODE_AUTO) || std::isnan(lo) || std::isnan(hi); // if true, we precollect the first value and immediately set up the bins
        bool precollectionOver = precollect(value, weight);
        if (!needPrecollection || precollectionOver)
            setUpBins();
    }
    else {
        if (autoExtend)
            extendBinsTo(value);
        hist->collectIntoHistogram(value, weight);
    }
}

void cAutoRangeHistogramStrategy::createBins()
{
    if (!std::isnan(requestedBinSize) && requestedBinSize <= 0)
        throw cRuntimeError(hist, "%s: Negative or zero bin size requested", getClassName());
    if (numBinsHint != -1 && numBinsHint <= 0)
        throw cRuntimeError(hist, "%s: Negative or zero bins requested", getClassName());

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
        double minValue = finiteMinValue;
        double maxValue = finiteMaxValue;

        c = (minValue + maxValue) / 2;
        r = (maxValue - minValue) * rangeExtensionFactor;
    }
    double rangeMin = hasLo ? lo : c - r / 2;
    double rangeMax = hasHi ? hi : c + r / 2;

    if (!hasLo && rangeMin < 0 && hist->getCount() > 0 && finiteMinValue >= 0 && finiteMaxValue > 0)
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

    if (std::isnan(rangeMin) && std::isnan(rangeMax)) {
        rangeMin = 0;
        rangeMax = 1;
    }
    if (std::isnan(rangeMin))
        rangeMin = rangeMax - 1;
    if (std::isnan(rangeMax))
        rangeMax = rangeMin + 1;

    // determine bin size
    if (!std::isnan(requestedBinSize)) {
        binSize = requestedBinSize;
    }
    else {
        double approxBinSize;
        if (rangeMax-rangeMin < std::numeric_limits<decltype(rangeMax)>::denorm_min() * targetNumBins)
            approxBinSize = 1;
        else
            approxBinSize = (rangeMax - rangeMin) / targetNumBins;
        binSize = (mode == cHistogram::MODE_INTEGERS) ? ceil(approxBinSize) : approxBinSize;

        if (binSizeRounding) {
            binSize = roundToOneTwoFive(binSize);
            ASSERT(binSize > 0);
            rangeMin = binSize * std::floor(rangeMin / binSize); // snap
            rangeMax = binSize * std::ceil(rangeMax / binSize);
            ASSERT(rangeMin < rangeMax);
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
        if (!hasLo && rangeMin < 0 && hist->getCount() > 0 && finiteMinValue >= 0 && finiteMaxValue > 0)
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
        if (std::isfinite(finiteMinValue))
            extendBinsTo(finiteMinValue);
        if (std::isfinite(finiteMaxValue))
            extendBinsTo(finiteMaxValue);
    }

    ASSERT(hist->getNumBins() > 0);
}

void cAutoRangeHistogramStrategy::extendBinsTo(double value)
{
    double firstEdge = hist->getBinEdges().front();
    double lastEdge = hist->getBinEdges().back();
    bool isUnderflow = value < firstEdge;
    bool isOverflow = value >= lastEdge;

    double finiteUnderflowSumWeights = hist->getUnderflowSumWeights() - hist->getNegInfSumWeights();
    double finiteOverflowSumWeights = hist->getOverflowSumWeights() - hist->getPosInfSumWeights();

    if (!isUnderflow && !isOverflow)
        return; // nothing to do
    if (isUnderflow && (finiteUnderflowSumWeights > 0 || !std::isnan(lo)))
        return; // cannot extend
    if (isOverflow &&  (finiteOverflowSumWeights > 0 || !std::isnan(hi)))
        return; // cannot extend

    if (binMerging) {
        double range = lastEdge - firstEdge;
        double newRange = std::max(value, lastEdge) - std::min(value, firstEdge);
        double newApproxBinSize = newRange / targetNumBins;
        if (newApproxBinSize >= range)
            mergeAllBinsIntoOne(newApproxBinSize);
        hist->extendBinsTo(value, binSize);
        if (hist->getNumBins() > (targetNumBins*3)/2)
            reduceNumBinsTo(targetNumBins);

        // If we can merge bins, we must have reached a range that includes value now. No excuses.
        ASSERT(hist->getBinEdges().front() <= value);
        ASSERT(value < hist->getBinEdges().back());
    }
    else {
        hist->extendBinsTo(value, binSize, maxNumBins);

        // However, even when we are not allowed to merge bins, in case we couldn't make the range big enough
        // to contain the value, we must have tried as hard as we could, and reached the max number of bins.
        if (value < hist->getBinEdges().front() || hist->getBinEdges().back() <= value)
            ASSERT(hist->getNumBins() == maxNumBins);

        // Actually, it might be possible that we didn't stop extending because we hit the maxNumBins
        // limit, instead it just so happened that the number of bins needed was exactly the maximum.
        // We can't tell the difference now, so not using <= here.
        if (hist->getNumBins() < maxNumBins) {
            ASSERT(hist->getBinEdges().front() <= value);
            ASSERT(hist->getBinEdges().back() > value);
        }
    }
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
        for (int i = 0; i < numBinsToAdd; i++) {
            double newEdge = lastEdge + (i+1)*binSize;
            // The condition below makes sure we won't add any consequential edges that
            // are exactly equal, despite our best efforts to avoid this, due to the
            // limited precision of the `double` type in some cases (big values, small binSize).
            if (newEdge > lastEdge && (newEdges.empty() || newEdge > newEdges.back()))
                newEdges.push_back(newEdge);
        }
        hist->appendBins(newEdges);
    }

    hist->mergeBins(groupSize);
    binSize *= groupSize;
}

void cAutoRangeHistogramStrategy::mergeAllBinsIntoOne(double newApproxBinSize)
{
    // merge the whole existing histogram into a single bin of the given size (approximately)
    double firstEdge = hist->getBinEdges().front();
    double lastEdge = hist->getBinEdges().back();
    double range = lastEdge - firstEdge;

    // determine new bin size (round, and >=range)
    double newBinSize = roundToOneTwoFive(newApproxBinSize);
    if (newBinSize < range)
        newBinSize = newBinSize * std::ceil(range / newBinSize);

    // compute new histogram range
    double newFirstEdge = firstEdge;
    double newLastEdge = lastEdge;

    double finiteUnderflowSumWeights = hist->getUnderflowSumWeights() - hist->getNegInfSumWeights();
    double finiteOverflowSumWeights = hist->getOverflowSumWeights() - hist->getPosInfSumWeights();

    //TODO the following code ignores lo and hi!!!
    // note: cHistogram doesn't store the _number of_ under/overflow samples, only
    // their sumweights, so no need to use getNumFiniteUnderflows/getNumFiniteOverflows
    if (finiteUnderflowSumWeights == 0 && finiteOverflowSumWeights == 0) {
        newFirstEdge = newBinSize * std::floor(newFirstEdge / newBinSize); // snap
        newLastEdge = newBinSize * std::ceil(newLastEdge / newBinSize); // snap
        newBinSize = newLastEdge - newFirstEdge;
    }
    else if (finiteUnderflowSumWeights == 0)
        newFirstEdge = newLastEdge - newBinSize;
    else if (finiteOverflowSumWeights == 0)
        newLastEdge = newFirstEdge + newBinSize;
    else
        ; // cannot extend range

    hist->mergeBins(hist->getNumBins());
    if (newFirstEdge < firstEdge)
        hist->prependBins({ newFirstEdge });
    if (newLastEdge > lastEdge)
        hist->appendBins({ newLastEdge });
    hist->mergeBins(hist->getNumBins());
    binSize = newBinSize;
}

void cAutoRangeHistogramStrategy::clear()
{
    cPrecollectionBasedHistogramStrategy::clear();
    lo = NAN;
    hi = NAN;
    binSize = NAN;
}

}  // namespace omnetpp


