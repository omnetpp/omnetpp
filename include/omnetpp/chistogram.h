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

#include <climits>
#include "cabstracthistogram.h"

namespace omnetpp {

class cIHistogramStrategy;
class cAutoRangeHistogramStrategy;

/**
 * @brief Generic histogram class, capable of representing both unweighted and
 * weighted distributions. Histogram data are stored as n+1 bin edges and
 * n bin values, both being double-precision floating point values. Upper and
 * lower outliers (as well as positive and negative infinities) are kept as counts
 * (for unweighted statistics) or as sum of weights (for weighted statistics).
 *
 * cHistogram is able to generate random numbers from the stored distribution,
 * and also supports save/load of the histogram data in a file.
 *
 * Bins can be set up directly using methods such as setBinEdges() or
 * createUniformBins(), but it is often more practical to automate it by letting
 * a <i>histogram strategy</i> do it. Histogram strategies are objects that
 * encapsulate the job of setting up and managing histogram bins. For example,
 * histogram strategies may employ a precollection phase to get an estimate of
 * the distribution for laying out the bins. Strategies are also capable of
 * extending the histogram range later by adding new bins as needed, or merging
 * adjacent bins to reduce their count. Strategies that create non-uniform
 * (e.g. equi-probable) bins are also possible to implement. Histogram
 * strategies subclass from cIHistogramStrategy.
 *
 * The default constructor of cHistogram installs a default histogram strategy
 * which was designed to provide a good quality histogram for arbitrary
 * distributions, without manual configuration. It employs precollection
 * and also auto-extends the histogram at runtime, merging neighbouring groups
 * of bins if there would be too many of them after extension.
 *
 * Custom behavior can be achieved by setting up and installing an appropriate
 * strategy class, such as cFixedRangeHistogramStrategy or cAutoRangeHistogramStrategy.
 * There are also convenience methods such as setRange() and setNumBins()
 * that internally use cAutoRangeHistogramStrategy.
 *
 * Examples:
 *
 * Automatic mode:
 *
 * \code
 * cHistogram histogram("histogram");
 * \endcode
 *
 * Setting up a 50-bin histogram on the range [0,100):
 *
 * \code
 * cAutoRangeHistogramStrategy *strategy = new cAutoRangeHistogramStrategy();
 * strategy->setRange(0, 100);
 * strategy->setNumBins(50);
 * strategy->setMode(cHistogram::MODE_INTEGERS);
 * cHistogram histogram("histogram", strategy);
 * \endcode
 *
 * The same effect using convenience methods:
 *
 * \code
 * cHistogram histogram("histogram");
 * histogram.setRange(0, 100);
 * histogram.setNumBins(50);
 * histogram.setMode(cHistogram::MODE_INTEGERS);
 * \endcode
 *
 * @ingroup Statistics
 */
class SIM_API cHistogram : public cAbstractHistogram
{
  public:
    /**
     * Histogram mode. In INTEGERS mode, bin edges are whole numbers; in REALS mode
     * they can be real numbers.
     */
    enum Mode {MODE_AUTO, MODE_INTEGERS, MODE_REALS, MODE_DOUBLES /*deprecated*/ = MODE_REALS};

  protected:
    cIHistogramStrategy *strategy = nullptr; // owned

    std::vector<double> binEdges;
    std::vector<double> binValues; // one less than bin edges
    double finiteUnderflowSumWeights = 0, finiteOverflowSumWeights = 0;
    double posInfSumWeights = 0, negInfSumWeights = 0;

  public:
    // INTERNAL, only for cIHistogramSetupStrategy implementations.
    // Directly collects the value into the existing bins, without delegating to the strategy object
    virtual void collectIntoHistogram(double value, double weight=1);
    void dump() const; // for debugging
    void assertSanity();

  private:
    void copy(const cHistogram& other);
    cAutoRangeHistogramStrategy *getOrCreateAutoRangeStrategy() const;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * This constructor installs a cDefaultHistogramStrategy on the histogram.
     * To create a histogram without a strategy object, use the constructor
     * that takes a 'strategy' parameter and pass nullptr for it.
     */
    explicit cHistogram(const char *name=nullptr, bool weighted=false);

    /**
     * This convenience constructor installs a cDefaultHistogramStrategy
     * with the desired number of bins on the histogram.
     */
    explicit cHistogram(const char *name, int numBinsHint, bool weighted=false);

    /**
     * Constructor that allows one to install a histogram strategy on the
     * histogram. It is also legal to pass nullptr as strategy.
     */
    explicit cHistogram(const char *name, cIHistogramStrategy *strategy, bool weighted=false);

    /**
     * Copy constructor.
     */
    cHistogram(const cHistogram& other): cAbstractHistogram(other) {copy(other);}

    /**
     * Assignment operator. The name member is not copied;
     * see cNamedObject::operator=() for details.
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
     * Collects one observation. Delegates to the strategy object if there is
     * one installed.
     */
    virtual void collect(double value) override;
    using cAbstractHistogram::collect;

    /**
     * Collects one observation with a given weight. The weight must not be
     * negative. (Zero-weight observations are allowed, but will not affect
     * mean and stddev, nor the bin values.) This methods delegates to the strategy
     * object if there is one installed.
     */
    virtual void collectWeighted(double value, double weight) override;
    using cAbstractHistogram::collectWeighted;

    /**
     * Clears the results collected so far.
     */
    virtual void clear() override;

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
     * Returns true if histogram bins are already available. (Bins are not yet
     * available in the precollection phase.) See setUpBins().
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
     * Makes sure that 'value' will fall in the range covered by the bins, by
     * potentially extending the histogram with some bins of width 'step'.
     * Creation of new bins stops when either the total number of bins reaches
     * 'maxNumBins', or 'value' is covered by the histogram. If 'value' is already
     * in the range of the existing bins, the function does nothing.
     * This method may only be called when there are no over- or underflows.
     * 'step' must be positive.
     */
    virtual void extendBinsTo(double value, double step, int maxNumBins=INT_MAX); // TODO: remove default value for maxNumBins

    /**
     * Reduces the number of bins by merging each 'groupSize' consecutive bins into one.
     * The number of bins must be a multiple of 'groupSize'. If that is not the case,
     * extendBinsTo() or similar methods may be used to create extra empty bins
     * before calling this function.
     */
    virtual void mergeBins(int groupSize);

    /**
     * Returns the bin edges of the histogram. There is always one more edge than bin,
     * except when the histogram has not been set up yet, in which case both are zero.
     */
    virtual std::vector<double> getBinEdges() const override {return binEdges;}

    /**
     * Returns the bin values of the histogram. There is always one less bin than edge,
     * except when the histogram has not been set up yet, in which case both are zero.
     */
    virtual std::vector<double> getBinValues() const override {return binValues;}

    /**
     * Returns the number of bins in the histogram.
     */
    virtual int getNumBins() const override {return binValues.size();}

    /**
     * Returns the k'th bin edge of the histogram. The k'th bin is delimited by the edge k and k+1.
     */
    virtual double getBinEdge(int k) const override {return binEdges.at(k);}

    /**
     * Returns the total weight of the observations in the k'th bin of the histogram.
     * (In the unweighted case, every observation is regarded as having the weight 1.0.)
     */
    virtual double getBinValue(int k) const override {return binValues.at(k);}

    /**
     * Returns the total weight of the observations that were under the histogram range.
     */
    virtual double getUnderflowSumWeights() const override {return finiteUnderflowSumWeights + negInfSumWeights;}

    /**
     * Returns the total weight of the observations that were above the histogram range.
     */
    virtual double getOverflowSumWeights() const override {return finiteOverflowSumWeights + posInfSumWeights;}

    /**
     * Returns the number of observations that were under the histogram range.
     * This value is only collected for unweighted statistics, i.e. it is an
     * error to call this method on a weighted histogram.
     */
    virtual int64_t getNumUnderflows() const override;

    /**
     * Returns the number of observations that were under the histogram range.
     * This value is only collected for unweighted statistics, i.e. it is an
     * error to call this method on a weighted histogram.
     */
    virtual int64_t getNumOverflows() const override;

    /**
     * Returns number of observations that were negative infinity, independent of their weights.
     * This value is only collected for unweighted statistics, i.e. it is an
     * error to call this method on a weighted histogram.
     */
    virtual int64_t getNumNegInfs() const override;

    /**
     * Returns number of observations that were positive infinity, independent of their weights.
     * This value is only collected for unweighted statistics, i.e. it is an
     * error to call this method on a weighted histogram.
     */
    virtual int64_t getNumPosInfs() const override;

    /**
     * Returns the total weight of the observations that were negative infinity.
     */
    virtual double getNegInfSumWeights() const override {return negInfSumWeights;}

    /**
     * Returns the total weight of the observations that were above the histogram range.
     */
    virtual double getPosInfSumWeights() const override {return posInfSumWeights;}
    //@}

    /** @name cAutoRangeHistogramStrategy-based convenience API. */
    //@{
    /**
     * Sets the histogram mode: MODE_AUTO, MODE_INTEGERS or MODE_DOUBLES.
     * Cannot be called when the bins have been set up already.
     * This method internally installs (or uses an already installed)
     * cAutoRangeHistogramStrategy on the histogram, and configures it accordingly.
     */
    virtual void setMode(Mode mode);

    /**
     * Sets the histogram range explicitly to [lower, upper). Use NAN to leave
     * either or both values unspecified. This method internally installs
     * (or uses an already installed) cAutoRangeHistogramStrategy on the
     * histogram, and configures it accordingly.
     */
    virtual void setRange(double lower, double upper);

    /**
     * Sets the number of observations to collect before setting up histogram bins.
     * This method internally installs (or uses an already installed)
     * cAutoRangeHistogramStrategy on the histogram, and configures it accordingly.
     */
    virtual void setNumPrecollectedValues(int numPrecollect);

    /**
     * Sets the factor by which the range of the precollected observations
     * is multiplied for determining the range of the histogram bins.
     * This method internally installs (or uses an already installed)
     * cAutoRangeHistogramStrategy on the histogram, and configures it accordingly.
     */
    virtual void setRangeExtensionFactor(double rangeExtensionFactor);

    /**
     * When set to true, observations that fall outside of the histogram range
     * will cause the histogram to be extended with new bins, instead of being
     * collected as outliers.
     * This method internally installs (or uses an already installed)
     * cAutoRangeHistogramStrategy on the histogram, and configures it accordingly.
     */
    virtual void setAutoExtend(bool autoExtend);

    /**
     * Sets the preferred number of bins. Cannot be called when the bins have been
     * set up already. This method internally installs (or uses an already installed)
     * cAutoRangeHistogramStrategy on the histogram, and configures it accordingly.
     */
    virtual void setNumBinsHint(int numCells);

    /**
     * Sets the preferred bin size. Cannot be called when the bins have been
     * set up already. This method internally installs (or uses an already installed)
     * cAutoRangeHistogramStrategy on the histogram, and configures it accordingly.
     */
    virtual void setBinSizeHint(double d);
    //@}
};

}  // namespace omnetpp

#endif
