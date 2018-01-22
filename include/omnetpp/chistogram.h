//==========================================================================
//  CHISTOGRAM.H - part of
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

#ifndef __OMNETPP_CHISTOGRAM_H
#define __OMNETPP_CHISTOGRAM_H

#include "cdensityestbase.h"

namespace omnetpp {

class cIHistogramStrategy;
class cAutoRangeHistogramStrategy;

/**
 * @brief A
 *
 * TODO explan strategy
 *
 * @ingroup Statistics
 */
class SIM_API cHistogram : public cDensityEstBase
{
  protected:
    cIHistogramStrategy *strategy = nullptr; // owned

    std::vector<double> binEdges;
    std::vector<double> binValues; // one less than bin edges

    int64_t numUnderflows = 0, numOverflows = 0;
    double underflowSumWeights = 0, overflowSumWeights = 0; // weighted sum

  public:
    // INTERNAL, only for cIHistogramSetupStrategy implementations!
    // Directly collects the value into the existing bins, without delegating to the setupStrategy.
    virtual void collectIntoHistogram(double value, double weight=1);
    void dump() const; // for debugging

  private:
    void copy(const cHistogram& other);
    cAutoRangeHistogramStrategy *getOrCreateAutoRangeStrategy() const;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor. Installs GenericStrategy. To create a histogram without a strategy object,
     * use the other constructor and pass nullptr for the 'strategy' parameter.
     */
    explicit cHistogram(const char *name=nullptr, bool weighted=false);

    /**
     * Constructor.
     */
    explicit cHistogram(const char *name, cIHistogramStrategy *strategy, bool weighted=false);

    /**
     * Copy constructor.
     */
    cHistogram(const cHistogram& other): cDensityEstBase(other) {copy(other);}

    /**
     * Assignment operator. The name member is not copied;
     * see cNamedObject's operator=() for more details.
     */
    cHistogram& operator=(const cHistogram& other);

    /**
     * Destructor.
     */
    virtual ~cHistogram();
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cHistogram *dup() const override {return new cHistogram(*this);}

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
    //@}

    /** @name Redefined member functions from cStatistic and its subclasses. */
    //@{

    /**
     * Collects one observation.
     */
    virtual void collect(double value) override;
    using cDensityEstBase::collect;

    /**
     * Collects one observation with a given weight. The weight must not be
     * negative. (Zero-weight observations are allowed, but will not affect
     * mean and stddev, nor the bin values.)
     */
    virtual void collectWeighted(double value, double weight) override;
    using cDensityEstBase::collectWeighted;

    /**
     * Clears the results collected so far.
     */
    virtual void clear() override;

    /**
     * Returns a random number from the distribution represented by the histogram.
     */
    virtual double draw() const override;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *f) const override;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *f) override;

    /**
     * Merge another statistics object into this one.
     */
    virtual void merge(const cStatistic *other) override;
    //@}

    /** @name Configuring and querying the histogram. */
    //@{

    /**
     * Installs a new histogram strategy, replacing the current one, taking ownership
     * of 'setupStrategy'. Can only be called while the histogram is empty.
     */
    void setStrategy(cIHistogramStrategy *strategy);

    /**
     * Returns a pointer to the currently used histogram strategy.
     */
    cIHistogramStrategy *getStrategy() const {return strategy;}

    /**
     * Returns true if histogram is already available. See setUpBins().
     */
    virtual bool binsAlreadySetUp() const override;

    /**
     * Sets up histogram bins using the installed strategy (see cIHistogramStrategy).
     * The histogram strategy class may use precollection to gather information
     * for laying out the bins.
     */
    virtual void setUpBins() override;

    /**
     * Configures a histogram with bins defined by 'edges'.
     * Can only be called once, and only when there are no bins defined.
     * 'edges' must contain at least two values, and it must be strictly increasing.
     */
    virtual void setBinEdges(const std::vector<double>& edges);

    /**
     * Sets the histogram up to have bins covering the range from 'lo' to 'hi',
     * each bin being 'step' wide. Can only be called on a histogram without bins.
     * 'lo' will always be added as an edge, all bins will be 'step' wide, and
     * the last bin edge will be at or above 'hi'.
     */
    virtual void createUniformBins(double lo, double hi, double step);

    /**
     * Extends the histogram to the left with some bins, as delimited by 'edges'.
     * This can only be used if there is at least one bin already, and there are
     * no underflows. 'edges' must not be empty, it must be strictly increasing,
     * and its last value must be less than the first already existing bin edge.
     */
    virtual void prependBins(const std::vector<double>& edges);

    /**
     * Extends the histogram to the right with some bins, as delimited by 'edges'.
     * This can only be used if there is at least one bin already, and there are
     * no overflows. 'edges' must not be empty, it must be strictly increasing,
     * and its first value must be greater than the last already existing bin edge.
     */
    virtual void appendBins(const std::vector<double>& edges);

    /**
     * Makes sure that 'value' will falls in the range covered by the bins, by
     * potentially extending the histogram with some bins of width 'step'.
     * If 'value' is already in the range of the existing bins, the function
     * does nothing. This can only be used if there is at least one bin already,
     * and there are no over- or underflows. 'step' must be positive.
     */
    virtual void extendBinsTo(double value, double step);

    /**
     * TODO not half! TODO also work if #bins in not a multiple; assumes uniform bins
     * Cuts the number of bins in half, by merging each consecutive pair of bins into one.
     * Can only be called if there are at least two bins, and the number of bins is even.
     *
     * If there are an odd number of bins, you can use extendBinsTo(getBinEdges().back(), binSize)
     * to append empty bins before calling this function.
     */
    virtual void mergeBins(size_t groupSize);

    /**
     * Returns the bin edges of the histogram. There is always one more edge than bin,
     * except when the histogram has not been set up yet, in which case there is zero of both.
     */
    const std::vector<double>& getBinEdges() const {return binEdges;} // one more than values

    /**
     * Returns the bin values of the histogram. There is always one less bin than edge,
     * except when the histogram has not been set up yet, in which case there is zero of both.
     */
    const std::vector<double>& getBinValues() const {return binValues;}

    /**
     * Returns the number of bins in the histogram.
     */
    int getNumBins() const override {return binValues.size();}

    /**
     * Returns the k'th bin edge of the histogram. The k'th bin is delimited by the edge k and k+1.
     */
    double getBinEdge(int k) const override {return binEdges.at(k);}

    /**
     * Returns the value of the k'th bin of the histogram.
     */
    double getBinValue(int k) const override {return binValues.at(k);}

    /**
     * Returns the weighted sum of the underflown values.
     */
    double getUnderflowSumWeights() const override {return underflowSumWeights;}

    /**
     * Returns the weighted sum of the overflown values.
     */
    double getOverflowSumWeights() const override {return overflowSumWeights;}

    /**
     * Returns the number of underflown values, without regard to their weights.
     */
    virtual int64_t getNumUnderflows() const override { return numUnderflows; }

    /**
     * Returns the number of overflown values, without regard to their weights.
     */
    virtual int64_t getNumOverflows() const override { return numOverflows; }
    //@}

    /** @name Legacy API. */
    //@{
    enum _OPPDEPRECATED HistogramMode {MODE_AUTO, MODE_INTEGERS, MODE_DOUBLES};
    cHistogram(const char *name, int numCells);
    _OPPDEPRECATED virtual void setMode(HistogramMode mode);
    _OPPDEPRECATED virtual void setRange(double lower, double upper);
    _OPPDEPRECATED virtual void setRangeAuto(int numPrecollect=100, double rangeExtensionFactor=2.0);
    _OPPDEPRECATED virtual void setRangeAutoLower(double upper, int numPrecollect=100, double rangeExtensionFactor=2.0);
    _OPPDEPRECATED virtual void setRangeAutoUpper(double lower, int numPrecollect=100, double rangeExtensionFactor=2.0);
    _OPPDEPRECATED virtual void setNumPrecollectedValues(int numPrecollect);
    _OPPDEPRECATED virtual int getNumPrecollectedValues() const;
    _OPPDEPRECATED virtual void setRangeExtensionFactor(double rangeExtensionFactor);
    _OPPDEPRECATED virtual double getRangeExtensionFactor() const;
    _OPPDEPRECATED virtual void setNumCells(int numCells);
    _OPPDEPRECATED virtual void setCellSize(double d);
    _OPPDEPRECATED virtual double getCellSize() const;
    //@}
};

}  // namespace omnetpp

#endif
