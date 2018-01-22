//==========================================================================
//  CHISTOGRAM2.CC - part of
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

#include <omnetpp/chistogram.h>
#include <algorithm>
#include "omnetpp/chistogramstrategy.h"
#include "omnetpp/distrib.h"

namespace omnetpp {

void cHistogram::copy(const cHistogram& other)
{
    delete strategy;
    strategy = (cIHistogramStrategy *)other.strategy->dup();
    strategy->hist = this; // not calling init(), that would fail if not empty

    binEdges = other.binEdges;
    binValues = other.binValues;

    numUnderflows = other.numUnderflows;
    numOverflows = other.numOverflows;

    underflowSumWeights = other.underflowSumWeights;
    overflowSumWeights = other.overflowSumWeights;
}

void cHistogram::dump() const
{
    std::cout << underflowSumWeights;

    for (size_t i = 0; i < binValues.size(); ++i)
        std::cout << " |" << binEdges[i] << binValues[i];

    std::cout << " |" << binEdges.back() << " " << overflowSumWeights << std::endl;
}


cHistogram::cHistogram(const char *name, bool weighted)
    : cHistogram(name, new cGenericHistogramStrategy, weighted)
{
}

cHistogram::cHistogram(const char *name, cIHistogramStrategy *strategy, bool weighted)
    : cDensityEstBase(name, weighted)
{
    setStrategy(strategy);
}

cHistogram& cHistogram::operator=(const cHistogram& other)
{
    cDensityEstBase::operator=(other);
    copy(other);
    return *this;
}

cHistogram::~cHistogram()
{
    delete strategy;
}

void cHistogram::parsimPack(cCommBuffer *buffer) const
{
    // the setupStrategy pointer makes this difficult/infeasible/cumbersome
    throw cRuntimeError(this, "parsimPack() not implemented");
}

void cHistogram::parsimUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(this, "parsimUnpack() not implemented");
}

void cHistogram::collect(double value)
{
    cDensityEstBase::collect(value);

    if (strategy != nullptr)
        strategy->collect(value);
    else
        collectIntoHistogram(value); // error
}

void cHistogram::collectWeighted(double value, double weight)
{
    cDensityEstBase::collectWeighted(value, weight);

    if (strategy != nullptr)
        strategy->collectWeighted(value, weight);
    else
        collectIntoHistogram(value, weight);
}

void cHistogram::clear()
{
    cDensityEstBase::clear();

    binEdges.clear();
    binValues.clear();

    numUnderflows = 0;
    numOverflows = 0;

    underflowSumWeights = 0;
    overflowSumWeights = 0;
}

double cHistogram::draw() const
{
    // warn/error if there are overflows/underflows? (by sumweights, not by number)

    double binValueSum = getSumWeights();

    double rand = uniform(getRNG(), 0, binValueSum);

    if (rand < getUnderflowSumWeights())
        return uniform(getRNG(), getMin(), getBinEdge(0));

    rand -= getUnderflowSumWeights();

    // selecting a bin, each with a probability proportional to its value
    for (int i = 0; i < getNumBins(); ++i) {
        double binValue = getBinValue(i);
        if (rand < binValue)
            // we can't do better than uniform within a single bin
            return uniform(getRNG(), getBinEdge(i), getBinEdge(i + 1));
        else
            // we're not yet at the selected bin yet
            rand -= binValue;
    }

    return uniform(getRNG(), getBinEdge(getNumBins()), getMax());
}

void cHistogram::saveToFile(FILE *f) const
{
    ASSERT((binEdges.empty() && binValues.empty()) || (binEdges.size() == binValues.size() + 1));

    cDensityEstBase::saveToFile(f);

    fprintf(f, "%" PRId64 "\t #= num_underflows\n", numUnderflows);
    fprintf(f, "%" PRId64 "\t #= num_overflows\n", numOverflows);

    fprintf(f, "%g\t #= underflow\n", underflowSumWeights);
    fprintf(f, "%g\t #= overflow\n", overflowSumWeights);

    fprintf(f, "%d\t #= num_bins\n", getNumBins());

    if (getNumBins() > 0) {
        fprintf(f, "#= bin_edges\n");
        for (double e : binEdges)
            fprintf(f, " %g\n", e);

        fprintf(f, "#= bin_values\n");
        for (double v : binValues)
            fprintf(f, " %g\n", v);
    }
}

void cHistogram::loadFromFile(FILE *f)
{
    if (strategy != nullptr)
        throw cRuntimeError(this, "Cannot load into a histogram which already has a strategy");

    clear();

    cDensityEstBase::loadFromFile(f);

    freadvarsf(f, "%" SCNd64 "\t #= num_underflows", &numUnderflows);
    freadvarsf(f, "%" SCNd64 "\t #= num_overflows", &numOverflows);

    freadvarsf(f, "%lg\t #= underflow", &underflowSumWeights);
    freadvarsf(f, "%lg\t #= overflow", &overflowSumWeights);

    int numBins;
    freadvarsf(f, "%d\t #= num_bins", &numBins);

    if (numBins > 0) {
        binEdges.resize(numBins + 1);
        binValues.resize(numBins);

        freadvarsf(f, "#= bin_edges");
        for (int i = 0; i <= numBins; ++i)
            freadvarsf(f, " %lg", binEdges.data() + i);

        freadvarsf(f, "#= bin_values");
        for (int i = 0; i < numBins; ++i)
            freadvarsf(f, " %lg", binValues.data() + i);
    }
}

void cHistogram::merge(const cStatistic *other)
{
    //TODO implement!!!
    throw cRuntimeError(this, "cHistogram does not support merging");
}

void cHistogram::setStrategy(cIHistogramStrategy *strategy)
{
    if (getCount() != 0)
        throw cRuntimeError(this, "Cannot set histogram strategy after collection has begun");

    delete this->strategy;
    this->strategy = strategy;
    if (strategy != nullptr)
        strategy->init(this);
}

void cHistogram::setBinEdges(const std::vector<double>& edges)
{
    if (binEdges.size() != 0)
        throw cRuntimeError(this, "setBinEdges(): Histogram bins have already been set up");
    if (edges.size() < 2)
        throw cRuntimeError(this, "setBinEdges(): At least two bin edges expected");

    for (size_t i = 0; i < edges.size() - 1; ++i)
        if (edges[i] >= edges[i + 1])
            throw cRuntimeError(this, "setBinEdges(): Bin edges must be strictly increasing");

    binEdges = edges;
    binValues.resize(binEdges.size() - 1, 0);
}

void cHistogram::createUniformBins(double lo, double hi, double step)
{
    if (lo >= hi)
        throw cRuntimeError(this, "createUniformBins(): Upper bound must be higher than lower bound");
    if (step <= 0)
        throw cRuntimeError(this, "createUniformBins(): Step size must be positive");

    int numBins = std::ceil((hi - lo) * (1 - 1e-12) / step); // the * (1 - 1e-12) is to counteract precision error accumulation

    std::vector<double> binEdges;
    binEdges.reserve(numBins);

    // single multiplication instead of repeated addition to try to avoid error accumulation
    for (int i = 0; i <= numBins; ++i) // less-or-equal because we need one more edge than bin
        binEdges.push_back(lo + i * step);

    setBinEdges(binEdges);

    //dump();
}

void cHistogram::prependBins(const std::vector<double>& edges)
{
    if (binEdges.size() == 0)
        throw cRuntimeError(this, "prependBins() cannot be called if no bins exist yet");

    ASSERT(binEdges.size() == binValues.size() + 1);
    ASSERT(numUnderflows == 0);

    for (size_t i = 0; i < edges.size() - 1; ++i)
        ASSERT(edges[i] < edges[i + 1]);
    ASSERT(edges.back() < binEdges.front());

    binEdges.insert(binEdges.begin(), edges.begin(), edges.end());
    binValues.insert(binValues.begin(), edges.size(), 0.0);
}

void cHistogram::appendBins(const std::vector<double>& edges)
{
    if (binEdges.size() == 0)
        throw cRuntimeError(this, "appendBins() cannot be called if no bins exist yet");

    ASSERT(binEdges.size() == binValues.size() + 1);  //TODO convert these ASSERTs (similar ones in other methods) to exception
    ASSERT(numOverflows == 0);

    ASSERT(binEdges.back() < edges.front());
    for (size_t i = 0; i < edges.size() - 1; ++i)
        ASSERT(edges[i] < edges[i + 1]);

    binEdges.insert(binEdges.end(), edges.begin(), edges.end());
    binValues.insert(binValues.end(), edges.size(), 0.0);
}

void cHistogram::extendBinsTo(double value, double step)
{
    ASSERT(binEdges.size() == binValues.size() + 1);
    ASSERT(binEdges.size() >= 2);
    ASSERT(step > 0);

    // bins are inclusive on the left, and exclusive on the right

    while (value < binEdges.front()) {
        ASSERT(numUnderflows == 0); // how would we divide the underflows between the new bin(s), and the new underflow?

        binEdges.insert(binEdges.begin(), binEdges.front() - step);
        binValues.insert(binValues.begin(), 0);
    }

    while (value >= binEdges.back()) {
        ASSERT(numOverflows == 0); // how would we divide the overflows between the new bin(s), and the new overflow?

        binEdges.push_back(binEdges.back() + step);
        binValues.push_back(0);
    }
}

void cHistogram::mergeBins(size_t groupSize)
{
    if (binValues.size() < groupSize)
        throw cRuntimeError(this, "Cannot merge bins with a group size larger than the number of bins");

    if (binValues.size() % groupSize != 0)
        throw cRuntimeError(this, "Cannot merge bins; the number of bins is not divisible by the group size");

    ASSERT(binEdges.size() == binValues.size() + 1);

    size_t newNumBins = binValues.size() / groupSize;

    for (size_t i = 0; i < newNumBins; ++i) {
        double binValue = 0;
        for (size_t j = 0; j < groupSize; ++j)
            binValue += binValues[groupSize*i + j];
        binValues[i] = binValue;
        binEdges[i] = binEdges[groupSize*i];
    }
    binEdges[newNumBins] = binEdges.back();

    binValues.resize(newNumBins);
    binEdges.resize(newNumBins + 1);
}

bool cHistogram::binsAlreadySetUp() const
{
    return strategy ? strategy->binsAlreadySetUp() : getNumBins() > 0;
}

void cHistogram::setUpBins()
{
    if (strategy)
        strategy->setUpBins();
}

void cHistogram::collectIntoHistogram(double value, double weight)
{
    ASSERT(binEdges.size() >= 2);
    ASSERT(binEdges.size() == binValues.size() + 1);

    auto it = std::upper_bound(binEdges.begin(), binEdges.end(), value);
    int index = it - binEdges.begin() - 1;
    if (index == -1) {
        underflowSumWeights += weight;
        ++numUnderflows;
    }
    else if (index == (int)binValues.size()) {
        overflowSumWeights += weight;
        ++numOverflows;
    }
    else
        binValues[index] += weight;
}

// Deprecated API:

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

cHistogram::cHistogram(const char* name, int numCells) :
        cHistogram(name)
{
    setNumCells(numCells);
}

cAutoRangeHistogramStrategy *cHistogram::getOrCreateAutoRangeStrategy() const
{
    cHistogram *mutableThis = const_cast<cHistogram *>(this);

    if (auto autoStrat = dynamic_cast<cAutoRangeHistogramStrategy *>(strategy)) {
        return autoStrat;
    }
    else if (strategy == nullptr) {
        auto strat = new cAutoRangeHistogramStrategy();
        mutableThis->setStrategy(strat);
        return strat;
    }
    else if (dynamic_cast<cGenericHistogramStrategy *>(strategy) != nullptr) {
        // silently replacing the default strategy if still empty
        auto strat = new cAutoRangeHistogramStrategy();
        mutableThis->setStrategy(strat);
        return strat;
    }
    else
        throw cRuntimeError(this, "Cannot use legacy histogram setup methods when a different strategy has already been installed");
}

void cHistogram::setMode(HistogramMode mode)
{
    auto stratMode = mode == MODE_INTEGERS ? cIHistogramStrategy::MODE_INTEGERS :
                     mode == MODE_DOUBLES ? cIHistogramStrategy::MODE_REALS :
                     cIHistogramStrategy::MODE_AUTO;

    if (auto autoStrat = dynamic_cast<cAutoRangeHistogramStrategy *>(strategy))
        return autoStrat->setMode(stratMode);
    else if (auto fixedStrat = dynamic_cast<cFixedRangeHistogramStrategy *>(strategy))
        return fixedStrat->setMode(stratMode);
    else if (strategy == nullptr) {
        auto strat = new cAutoRangeHistogramStrategy();
        strat->setMode(stratMode);
        setStrategy(strat);
    } else
        throw cRuntimeError(this, "setMode(): Cannot set histogram mode on installed strategy");
}

void cHistogram::setRange(double lower, double upper)
{
    getOrCreateAutoRangeStrategy()->setLo(lower);
    getOrCreateAutoRangeStrategy()->setHi(upper);
    getOrCreateAutoRangeStrategy()->setBinSizeRounding(false);
}

void cHistogram::setRangeAuto(int numPrecollect, double rangeExtensionFactor)
{
    auto strat = getOrCreateAutoRangeStrategy();
    strat->setNumToPrecollect(numPrecollect);
    strat->setRangeExtensionFactor(rangeExtensionFactor);
    getOrCreateAutoRangeStrategy()->setBinSizeRounding(false);
}

void cHistogram::setRangeAutoLower(double upper, int numPrecollect, double rangeExtensionFactor)
{
    auto strat = getOrCreateAutoRangeStrategy();
    strat->setHi(upper);
    strat->setNumToPrecollect(numPrecollect);
    strat->setRangeExtensionFactor(rangeExtensionFactor);
    getOrCreateAutoRangeStrategy()->setBinSizeRounding(false);
}

void cHistogram::setRangeAutoUpper(double lower, int numPrecollect, double rangeExtensionFactor)
{
    auto strat = getOrCreateAutoRangeStrategy();
    strat->setLo(lower);
    strat->setNumToPrecollect(numPrecollect);
    strat->setRangeExtensionFactor(rangeExtensionFactor);
    getOrCreateAutoRangeStrategy()->setBinSizeRounding(false);
}

void cHistogram::setNumPrecollectedValues(int numPrecollect)
{
    getOrCreateAutoRangeStrategy()->setNumToPrecollect(numPrecollect);
}

int cHistogram::getNumPrecollectedValues() const
{
    return getOrCreateAutoRangeStrategy()->getNumToPrecollect();
}

void cHistogram::setRangeExtensionFactor(double rangeExtensionFactor)
{
    getOrCreateAutoRangeStrategy()->setRangeExtensionFactor(rangeExtensionFactor);
}

double cHistogram::getRangeExtensionFactor() const
{
    return getOrCreateAutoRangeStrategy()->getRangeExtensionFactor();
}

void cHistogram::setNumCells(int numCells)
{
    getOrCreateAutoRangeStrategy()->setNumBins(numCells);
    getOrCreateAutoRangeStrategy()->setBinSizeRounding(false);
}

void cHistogram::setCellSize(double d)
{
    getOrCreateAutoRangeStrategy()->setBinSize(d);
    getOrCreateAutoRangeStrategy()->setBinSizeRounding(false);
}

double cHistogram::getCellSize() const
{
    return getOrCreateAutoRangeStrategy()->getBinSize();
}

#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

}  // namespace omnetpp
