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

#include <algorithm>
#include <omnetpp/chistogram.h>
#include "omnetpp/chistogramstrategy.h"
#include "omnetpp/distrib.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

cHistogram::cHistogram(const char *name, bool weighted)
    : cHistogram(name, new cDefaultHistogramStrategy, weighted)
{
}

cHistogram::cHistogram(const char *name, cIHistogramStrategy *strategy, bool weighted)
    : cDensityEstBase(name, weighted)
{
    setStrategy(strategy);
}

cHistogram::cHistogram(const char *name, int numBins) : cHistogram(name)
{
    setNumBins(numBins);
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

void cHistogram::copy(const cHistogram& other)
{
    delete strategy;
    strategy = (cIHistogramStrategy *)other.strategy->dup();
    strategy->hist = this; // not calling setHistogram(), that would fail if not empty

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

void cHistogram::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cDensityEstBase::parsimPack(buffer);

    buffer->pack(binEdges.size());
    for (double binEdge : binEdges)
        buffer->pack(binEdge);

    buffer->pack(binValues.size());
    for (double binValue : binValues)
        buffer->pack(binValue);

    buffer->pack(numUnderflows);
    buffer->pack(numOverflows);
    buffer->pack(underflowSumWeights);
    buffer->pack(overflowSumWeights);

    if (buffer->packFlag(strategy != nullptr))
        buffer->packObject(strategy);
#endif
}

void cHistogram::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cDensityEstBase::parsimUnpack(buffer);

    size_t n;
    buffer->unpack(n);
    binEdges.resize(n);
    for (size_t i = 0; i < n; n++)
        buffer->unpack(binEdges[i]);

    buffer->unpack(n);
    binValues.resize(n);
    for (size_t i = 0; i < n; n++)
        buffer->unpack(binValues[i]);

    buffer->unpack(numUnderflows);
    buffer->unpack(numOverflows);
    buffer->unpack(underflowSumWeights);
    buffer->unpack(overflowSumWeights);

    if (buffer->checkFlag())
        setStrategy((cIHistogramStrategy *)buffer->unpackObject());
#endif
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

    if (strategy != nullptr)
        strategy->clear();

    binEdges.clear();
    binValues.clear();

    numUnderflows = 0;
    numOverflows = 0;

    underflowSumWeights = 0;
    overflowSumWeights = 0;
}

double cHistogram::draw() const
{
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

void cHistogram::merge(const cStatistic *stat)
{
    const cDensityEstBase *other = dynamic_cast<const cDensityEstBase *>(stat);
    if (other == nullptr)
        throw cRuntimeError(this, "merge(): Cannot merge non-cDensityEstBase statistics (%s)%s into a histogram",  stat->getClassName(), stat->getFullPath().c_str());
    if (!other->binsAlreadySetUp())
        throw cRuntimeError(this, "merge(): Object (%s)%s does not have histogram bins set up yet", other->getClassName(), other->getFullPath().c_str());
    if (!binsAlreadySetUp())
        throw cRuntimeError(this, "merge(): No histogram bins set up yet (try setUpBins())");

    // merge the base class
    cDensityEstBase::merge(other);

    // prepend/append extra bins
    std::vector<double> edgesToPrepend, edgesToAppend;
    for (int i = 0; i < other->getNumBins()+1; i++) {
        double edge = other->getBinEdge(i);
        if (edge < binEdges.front())
            edgesToPrepend.push_back(edge);
        if (edge > binEdges.back())
            edgesToAppend.push_back(edge);
    }
    prependBins(edgesToPrepend);  //TODO this can only be done if numOverFlows/underflows are zero! (or extra cells can be merged into overflows/undefrlows)
    appendBins(edgesToAppend);


    // check bin edges
    //TODO current check is too strict: if other's bins can be obtained by splitting our bins (i.e. our bin edges are a subset of other's bin edges), that's also OK
    if (getNumBins() != other->getNumBins())
        throw cRuntimeError(this, "Cannot merge (%s)%s: Different number of histogram bins (%d vs %d)",
                other->getClassName(), other->getFullPath().c_str(), getNumBins(), other->getNumBins());
    int n = getNumBins();
    for (int i = 0; i <= n; i++)
        if (getBinEdge(i) != other->getBinEdge(i))
            throw cRuntimeError(this, "Cannot merge (%s)%s: Histogram bins are not aligned", other->getClassName(), other->getFullPath().c_str());

    // merge underflow/overflow bins
    numUnderflows += other->getNumUnderflows();
    numOverflows += other->getNumOverflows();
    underflowSumWeights += other->getUnderflowSumWeights();
    overflowSumWeights += other->getOverflowSumWeights();

    // merge bin values
    for (int i = 0; i < getNumBins(); i++)
        binValues[i] += other->getBinValue(i);
}

void cHistogram::setStrategy(cIHistogramStrategy *strategy)
{
    if (getCount() != 0)
        throw cRuntimeError(this, "Cannot set histogram strategy after collection has begun");

    delete this->strategy;
    this->strategy = strategy;
    if (strategy != nullptr)
        strategy->setHistogram(this);
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

    int numBins = std::ceil((hi - lo) * (1 - 1e-10) / step); // the * (1 - 1e-10) is to counteract precision error accumulation

    std::vector<double> binEdges;
    binEdges.reserve(numBins);

    // single multiplication instead of repeated addition to try to avoid error accumulation
    for (int i = 0; i <= numBins; ++i) // less-or-equal because we need one more edge than bin
        binEdges.push_back(lo + i * step);

    setBinEdges(binEdges);
}

void cHistogram::prependBins(const std::vector<double>& edges)
{
    if (binEdges.size() == 0)
        throw cRuntimeError(this, "prependBins() cannot be called if no bins exist yet");
    if (numUnderflows != 0)
        throw cRuntimeError(this, "prependBins() cannot be called if some observations have already been counted as underflows");
    for (size_t i = 0; i < edges.size() - 1; ++i)
        if (edges[i] >= edges[i + 1])
            throw cRuntimeError(this, "prependBins(): new edges must be in strictly increasing order");
    if (!edges.empty() && edges.back() >= binEdges.front())
        throw cRuntimeError(this, "prependBins(): new edges overlap with existing histogram range");

    ASSERT(binEdges.size() == binValues.size() + 1); // histogram is sane
    binEdges.insert(binEdges.begin(), edges.begin(), edges.end());
    binValues.insert(binValues.begin(), edges.size(), 0.0);
}

void cHistogram::appendBins(const std::vector<double>& edges)
{
    if (binEdges.size() == 0)
        throw cRuntimeError(this, "appendBins() cannot be called if no bins exist yet");
    if (numOverflows != 0)
        throw cRuntimeError(this, "appendBins() cannot be called if some observations have already been counted as overflows");
    for (size_t i = 0; i < edges.size() - 1; ++i)
        if (edges[i] >= edges[i + 1])
            throw cRuntimeError(this, "appendBins(): new edges must be in strictly increasing order");
    if (!edges.empty() && binEdges.back() >= edges.front())
        throw cRuntimeError(this, "appendBins(): new edges overlap with existing histogram range");

    ASSERT(binEdges.size() == binValues.size() + 1); // histogram is sane
    binEdges.insert(binEdges.end(), edges.begin(), edges.end());
    binValues.insert(binValues.end(), edges.size(), 0.0);
}

void cHistogram::extendBinsTo(double value, double step)
{
    if (binEdges.size() == 0)
        throw cRuntimeError(this, "extendBinsTo() cannot be called if no bins exist yet");
    if (step <= 0)
        throw cRuntimeError(this, "extendBinsTo(): step must be positive");

    ASSERT(binEdges.size() == binValues.size() + 1);
    ASSERT(binEdges.size() >= 2);

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

void cHistogram::mergeBins(int groupSize)
{
    if (groupSize <= 0)
        throw cRuntimeError(this, "mergeBins(): groupSize must be positive");

    ASSERT(binEdges.size() == binValues.size() + 1);  // histogram is sane

    size_t newNumBins = binValues.size() / groupSize;

    for (size_t i = 0; i < newNumBins; ++i) {
        double binValue = 0;
        for (int j = 0; j < groupSize; ++j) //TODO modulo
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
    return getNumBins() > 0;
}

void cHistogram::setUpBins()
{
    if (!strategy)
        throw cRuntimeError(this, "setUpBins(): No histogram strategy installed");
    if (binsAlreadySetUp())
        throw cRuntimeError(this, "setUpBins(): Bins already set up");
    strategy->setUpBins();
    ASSERT(getNumBins() > 0);
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
    else if (dynamic_cast<cDefaultHistogramStrategy *>(strategy) != nullptr) {
        // silently replacing the default strategy if still empty
        auto strat = new cAutoRangeHistogramStrategy();
        mutableThis->setStrategy(strat);
        return strat;
    }
    else
        throw cRuntimeError(this, "Cannot use convenience histogram setup methods when a different strategy has already been installed");
}

void cHistogram::setMode(HistogramMode mode)
{
    getOrCreateAutoRangeStrategy()->setMode(mode);
}

void cHistogram::setRange(double lower, double upper)
{
    getOrCreateAutoRangeStrategy()->setRangeLo(lower);
    getOrCreateAutoRangeStrategy()->setRangeHi(upper);
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
    strat->setRangeHi(upper);
    strat->setNumToPrecollect(numPrecollect);
    strat->setRangeExtensionFactor(rangeExtensionFactor);
    getOrCreateAutoRangeStrategy()->setBinSizeRounding(false);
}

void cHistogram::setRangeAutoUpper(double lower, int numPrecollect, double rangeExtensionFactor)
{
    auto strat = getOrCreateAutoRangeStrategy();
    strat->setRangeLo(lower);
    strat->setNumToPrecollect(numPrecollect);
    strat->setRangeExtensionFactor(rangeExtensionFactor);
    getOrCreateAutoRangeStrategy()->setBinSizeRounding(false);
}

void cHistogram::setNumPrecollectedValues(int numPrecollect)
{
    getOrCreateAutoRangeStrategy()->setNumToPrecollect(numPrecollect);
}

void cHistogram::setRangeExtensionFactor(double rangeExtensionFactor)
{
    getOrCreateAutoRangeStrategy()->setRangeExtensionFactor(rangeExtensionFactor);
}

void cHistogram::setAutoExtend(bool autoExtend)
{
    getOrCreateAutoRangeStrategy()->setAutoExtend(autoExtend);
}

void cHistogram::setNumBins(int numBins)
{
    getOrCreateAutoRangeStrategy()->setNumBins(numBins);
    getOrCreateAutoRangeStrategy()->setBinSizeRounding(false);
}

void cHistogram::setBinSize(double d)
{
    getOrCreateAutoRangeStrategy()->setBinSize(d);
    getOrCreateAutoRangeStrategy()->setBinSizeRounding(false);
}

}  // namespace omnetpp
