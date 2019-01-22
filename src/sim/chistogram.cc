//==========================================================================
//  CHISTOGRAM.CC - part of
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
#include "omnetpp/regmacros.h"
#include "omnetpp/onstartup.h"
#include "omnetpp/globals.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

Register_Class(cHistogram);

cHistogram::cHistogram(const char *name, bool weighted)
    : cHistogram(name, new cDefaultHistogramStrategy(), weighted)
{
}

cHistogram::cHistogram(const char *name, int numBinsHint, bool weighted)
    : cHistogram(name, new cDefaultHistogramStrategy(numBinsHint), weighted)
{
}

cHistogram::cHistogram(const char *name, cIHistogramStrategy *strategy, bool weighted)
    : cAbstractHistogram(name, weighted)
{
    setStrategy(strategy);
}

cHistogram& cHistogram::operator=(const cHistogram& other)
{
    cAbstractHistogram::operator=(other);
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
    finiteUnderflowSumWeights = other.finiteUnderflowSumWeights;
    finiteOverflowSumWeights = other.finiteOverflowSumWeights;
    negInfSumWeights = other.negInfSumWeights;
    posInfSumWeights = other.posInfSumWeights;
}

void cHistogram::dump() const
{
    std::cout << finiteUnderflowSumWeights;
    for (size_t i = 0; i < binValues.size(); ++i)
        std::cout << " |" << binEdges[i] << binValues[i];
    std::cout << " |" << binEdges.back() << " " << finiteOverflowSumWeights << std::endl;
}

void cHistogram::assertSanity()
{
#ifndef NDEBUG
    if (binEdges.empty()) {
        ASSERT(finiteOverflowSumWeights==0 && finiteUnderflowSumWeights==0);
        ASSERT(binValues.empty());
    }
    else {
        ASSERT(binValues.size() >= 1);
        ASSERT(binEdges.size() == binValues.size()+1);
        double prevEdge = NAN;
        for (double edge : binEdges) {
            ASSERT(std::isnan(prevEdge) || prevEdge < edge);
            prevEdge = edge;
        }
        double sumBinWeights = finiteUnderflowSumWeights + finiteOverflowSumWeights;
        for (double value : binValues)
            sumBinWeights += value;
        ASSERT((sumBinWeights==0 && getSumWeights()==0) || fabs(sumBinWeights/getSumWeights()-1) < 1e-10);
    }
#endif
}

void cHistogram::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cAbstractHistogram::parsimPack(buffer);

    buffer->pack(binEdges.size());
    for (double binEdge : binEdges)
        buffer->pack(binEdge);

    buffer->pack(binValues.size());
    for (double binValue : binValues)
        buffer->pack(binValue);

    buffer->pack(finiteUnderflowSumWeights);
    buffer->pack(finiteOverflowSumWeights);
    buffer->pack(negInfSumWeights);
    buffer->pack(posInfSumWeights);

    if (buffer->packFlag(strategy != nullptr))
        buffer->packObject(strategy);
#endif
}

void cHistogram::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cAbstractHistogram::parsimUnpack(buffer);

    size_t n;
    buffer->unpack(n);
    binEdges.resize(n);
    for (size_t i = 0; i < n; n++)
        buffer->unpack(binEdges[i]);

    buffer->unpack(n);
    binValues.resize(n);
    for (size_t i = 0; i < n; n++)
        buffer->unpack(binValues[i]);

    buffer->unpack(finiteUnderflowSumWeights);
    buffer->unpack(finiteOverflowSumWeights);
    buffer->unpack(negInfSumWeights);
    buffer->unpack(posInfSumWeights);

    if (buffer->checkFlag())
        setStrategy((cIHistogramStrategy *)buffer->unpackObject());
#endif
}

void cHistogram::collect(double value)
{
    cAbstractHistogram::collect(value);

    if (std::isinf(value)) {
        if (value < 0)
            negInfSumWeights += 1;
        else
            posInfSumWeights += 1;
    }
    else {
        if (strategy != nullptr)
            strategy->collect(value);
        else
            collectIntoHistogram(value); // error
    }
}

void cHistogram::collectWeighted(double value, double weight)
{
    cAbstractHistogram::collectWeighted(value, weight);

    if (std::isinf(value)) {
        if (value < 0)
            negInfSumWeights += weight;
        else
            posInfSumWeights += weight;
    }
    else {
        if (strategy != nullptr)
            strategy->collectWeighted(value, weight);
        else
            collectIntoHistogram(value, weight);
    }
}

int64_t cHistogram::getNumUnderflows() const
{
    if (isWeighted())
        throw cRuntimeError(this, "Underflow count is unavailable for weighted statistics");
    return (int64_t)(finiteUnderflowSumWeights + negInfSumWeights);
}

int64_t cHistogram::getNumOverflows() const
{
    if (isWeighted())
        throw cRuntimeError(this, "Overflow count is unavailable for weighted statistics");
    return (int64_t)(finiteOverflowSumWeights + posInfSumWeights);
}

int64_t cHistogram::getNumNegInfs() const
{
    if (isWeighted())
        throw cRuntimeError(this, "Negative infinity count is unavailable for weighted statistics");
    return (int64_t)negInfSumWeights;
}

int64_t cHistogram::getNumPosInfs() const
{
    if (isWeighted())
        throw cRuntimeError(this, "Positive infinity count is unavailable for weighted statistics");
    return (int64_t)posInfSumWeights;
}

void cHistogram::clear()
{
    cAbstractHistogram::clear();

    if (strategy != nullptr)
        strategy->clear();

    binEdges.clear();
    binValues.clear();
    finiteUnderflowSumWeights = 0;
    finiteOverflowSumWeights = 0;
    negInfSumWeights = 0;
    posInfSumWeights = 0;
}

void cHistogram::saveToFile(FILE *f) const
{
    ASSERT((binEdges.empty() && binValues.empty()) || (binEdges.size() == binValues.size() + 1));

    cAbstractHistogram::saveToFile(f);

    fprintf(f, "%g\t #= underflow\n", finiteUnderflowSumWeights);
    fprintf(f, "%g\t #= overflow\n", finiteOverflowSumWeights);
    fprintf(f, "%g\t #= neg_inf\n", negInfSumWeights);
    fprintf(f, "%g\t #= pos_inf\n", posInfSumWeights);

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

    cAbstractHistogram::loadFromFile(f);

    freadvarsf(f, "%lg\t #= underflow", &finiteUnderflowSumWeights);
    freadvarsf(f, "%lg\t #= overflow", &finiteOverflowSumWeights);
    freadvarsf(f, "%lg\t #= neg_inf", &negInfSumWeights);
    freadvarsf(f, "%lg\t #= pos_inf", &posInfSumWeights);

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
    const cAbstractHistogram *other = dynamic_cast<const cAbstractHistogram *>(stat);
    if (other == nullptr)
        throw cRuntimeError(this, "merge(): Cannot merge non-cAbstractHistogram statistics (%s)%s into a histogram",  stat->getClassName(), stat->getFullPath().c_str());
    if (!other->binsAlreadySetUp())
        throw cRuntimeError(this, "merge(): Object (%s)%s does not have histogram bins set up yet", other->getClassName(), other->getFullPath().c_str());
    if (!binsAlreadySetUp())
        throw cRuntimeError(this, "merge(): No histogram bins set up yet (try setUpBins())");

    // merge the base class
    cAbstractHistogram::merge(other);

    // Note: The current check is too strict: if other's bins can be obtained by splitting
    // our bins (i.e. our bin edges are a subset of other's bin edges), that would also be OK.

    // check bin edges
    if (getNumBins() != other->getNumBins())
        throw cRuntimeError(this, "Cannot merge (%s)%s: Different number of histogram bins (%d vs. %d)",
                other->getClassName(), other->getFullPath().c_str(), getNumBins(), other->getNumBins());
    int n = getNumBins();
    for (int i = 0; i <= n; i++)
        if (getBinEdge(i) != other->getBinEdge(i))
            throw cRuntimeError(this, "Cannot merge (%s)%s: Histogram bins are not aligned", other->getClassName(), other->getFullPath().c_str());

    // merge underflow/overflow "bins"
    finiteUnderflowSumWeights += (other->getUnderflowSumWeights() - other->getNegInfSumWeights());
    finiteOverflowSumWeights += (other->getOverflowSumWeights() - other->getPosInfSumWeights());

    negInfSumWeights += other->getNegInfSumWeights();
    posInfSumWeights += other->getPosInfSumWeights();

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
    binEdges.reserve(numBins + 1);

    // single multiplication instead of repeated addition to try to avoid error accumulation
    for (int i = 0; i <= numBins; ++i) { // less-or-equal because we need one more edge than bin
        double edge = lo + i * step;

        if (std::abs(edge) < step * 1e-10) // work around double imprecision when we are very close to zero
            edge = 0;

        // When `lo` is huge, but `step` is tiny, we might actually get the exact same value for `edge`
        // for a couple of iterations, and that would break the invariant of the bin edges being
        // strictly increasing, causing assertion failures and infinite loops, so skip the repeated values.
        if (binEdges.empty() || binEdges.back() != edge)
            binEdges.push_back(edge);
    }

    setBinEdges(binEdges);
}

void cHistogram::prependBins(const std::vector<double>& edges)
{
    if (binEdges.size() == 0)
        throw cRuntimeError(this, "prependBins() cannot be called if no bins exist yet");
    if (finiteUnderflowSumWeights != 0)
        throw cRuntimeError(this, "prependBins() cannot be called if some observations have already been counted as underflows");
    for (int i = 0; i < (int)edges.size() - 1; ++i)
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
    if (finiteOverflowSumWeights != 0)
        throw cRuntimeError(this, "appendBins() cannot be called if some observations have already been counted as overflows");
    for (int i = 0; i < (int)edges.size() - 1; ++i)
        if (edges[i] >= edges[i + 1])
            throw cRuntimeError(this, "appendBins(): new edges must be in strictly increasing order");
    if (!edges.empty() && binEdges.back() >= edges.front())
        throw cRuntimeError(this, "appendBins(): new edges overlap with existing histogram range");

    ASSERT(binEdges.size() == binValues.size() + 1); // histogram is sane
    binEdges.insert(binEdges.end(), edges.begin(), edges.end());
    binValues.insert(binValues.end(), edges.size(), 0.0);
}

void cHistogram::extendBinsTo(double value, double step, int maxNumBins)
{
    if (binEdges.size() == 0)
        throw cRuntimeError(this, "extendBinsTo() cannot be called if no bins exist yet");
    if (step <= 0)
        throw cRuntimeError(this, "extendBinsTo(): step must be positive");

    // if there are under or overflows, we don't know how to divide the under/overflows between the new bin(s), and the new under/overflow
    if (value < binEdges.front() && finiteUnderflowSumWeights != 0)
        throw cRuntimeError(this, "extendBinsTo(): cannot extend the histogram in the downward direction, because some observations have already been counted as underflows (numUnderflows > 0)");
    if (value >= binEdges.back() && finiteOverflowSumWeights != 0)
        throw cRuntimeError(this, "extendBinsTo(): cannot extend the histogram in the upward direction, because some observations have already been counted as overflows (numOverflows > 0)");

    ASSERT(binEdges.size() == binValues.size() + 1);
    ASSERT(binEdges.size() >= 2);

    // bins are inclusive on the left, and exclusive on the right

    double originalLeftEdge = binEdges.front();

    for (int i = 0; value < binEdges.front() && (int)binValues.size() < maxNumBins; ++i) {
        double newEdge = originalLeftEdge - step * i;

        // When `binEdges.front()` is huge, but `step` is tiny, `newEdge` might actually be exactly equal to
        // `binEdges.front()`, and that would break the invariant of the bin edges being strictly increasing,
        // causing assertion failures and infinite loops, so better skip the repeating edges.
        if (newEdge != binEdges.front()) {
            binEdges.insert(binEdges.begin(), newEdge);
            binValues.insert(binValues.begin(), 0);
        }
    }

    double originalRightEdge = binEdges.back();

    for (int i = 0; value >= binEdges.back() && (int)binValues.size() < maxNumBins; ++i) {
        double newEdge = originalRightEdge + step * i;

        // When `binEdges.back()` is huge, but `step` is tiny, `newEdge` might actually be exactly equal to
        // `binEdges.back()`, and that would break the invariant of the bin edges being strictly increasing,
        // causing assertion failures and infinite loops, so better skip the repeating edges.
        if (newEdge != binEdges.back()) {
            binEdges.push_back(newEdge);
            binValues.push_back(0);
        }
    }
}

void cHistogram::mergeBins(int groupSize)
{
    if (groupSize <= 0)
        throw cRuntimeError(this, "mergeBins(): groupSize must be positive");
    if (groupSize == 1)
        return; // nothing to do

    ASSERT(binEdges.size() == binValues.size() + 1);  // histogram is sane

    int numBins = (int)binValues.size();
    int newNumBins = (numBins + groupSize -1) / groupSize;  // round up

    for (int i = 0; i < newNumBins; ++i) {
        int count = std::min(groupSize, numBins - groupSize*i);
        double binValue = 0;
        for (int j = 0; j < count; ++j)
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
    if (index == -1)
        finiteUnderflowSumWeights += weight;
    else if (index == (int)binValues.size())
        finiteOverflowSumWeights += weight;
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
    else if (const auto *defaultStrat = dynamic_cast<const cDefaultHistogramStrategy *>(strategy)) {
        // silently replace the default strategy
        int numBins = defaultStrat->getNumBinsHint();
        auto strat = new cAutoRangeHistogramStrategy();
        if (numBins != -1)
            strat->setNumBinsHint(numBins);
        mutableThis->setStrategy(strat);
        return strat;
    }
    else
        throw cRuntimeError(this, "Cannot use convenience histogram setup methods when a different strategy has already been installed");
}

void cHistogram::setMode(Mode mode)
{
    getOrCreateAutoRangeStrategy()->setMode(mode);
}

void cHistogram::setRange(double lower, double upper)
{
    getOrCreateAutoRangeStrategy()->setRangeHint(lower, upper);
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

void cHistogram::setNumBinsHint(int numBins)
{
    getOrCreateAutoRangeStrategy()->setNumBinsHint(numBins);
}

void cHistogram::setBinSizeHint(double d)
{
    getOrCreateAutoRangeStrategy()->setBinSizeHint(d);
}

}  // namespace omnetpp
