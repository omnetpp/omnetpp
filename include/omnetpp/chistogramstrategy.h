//==========================================================================
//  CHISTOGRAMSTRATEGY.H - part of
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

#ifndef __OMNETPP_CHISTOGRAMSTRATEGY_H
#define __OMNETPP_CHISTOGRAMSTRATEGY_H

#include <omnetpp/chistogram.h>

namespace omnetpp {

/**
 * @brief Interface for histogram strategy classes. Histogram strategies
 * encapsulate the task of setting up and managing the bins in a cHistogram.
 *
 * @ingroup Statistics
 */
class SIM_API cIHistogramStrategy : public cObject
{
    friend class cHistogram;
  public:
    typedef enum cHistogram::Mode Mode;

  protected:
    cHistogram *hist = nullptr; // backreference to "owner"

  public:
    /** @name Constructors, copying. */
    //@{
    cIHistogramStrategy() {}
    cIHistogramStrategy(const cIHistogramStrategy& other) {}
    cIHistogramStrategy& operator=(const cIHistogramStrategy& other) {hist=nullptr; return *this;}
    //@}

    /** @name Getting/setting the associated histogram object */
    //@{
    /**
     * Set the cHistogram instance this strategy is associated with.
     * This method is called from cHistogram::setStrategy().
     */
    void setHistogram(cHistogram *hist);

    /**
     * Returns the cHistogram instance this strategy is associated with.
     */
    cHistogram *getHistogram() const {return hist;}
    //@}

    /** @name Methods called from cHistogram */
    //@{
    /**
     * Called from cHistogram's collect() method. Implementations are expected to
     * call back cHistogram's collectIntoHistogram(), or if histogram bins are
     * not yet set up, store it for later.
     */
    virtual void collect(double value) = 0;

    /**
     * Called from cHistogram's collectWeighted() method. Implementations are
     * expected to call back cHistogram's collectIntoHistogram(), or if histogram
     * bins are not yet set up, store it for later.
     */
    virtual void collectWeighted(double value, double weight) = 0;

    /**
     * cHistogram's setUpBins() method delegates here. Implementations are expected
     * to create bins in the associated histogram by calling its setBinEdges()
     * or createUniformBins() method, and move possibly existing precollected
     * observations into the bins using collectIntoHistogram().
     */
    virtual void setUpBins() = 0;

    /**
     * Called from cHistogram's clear() method, to give the strategy object an
     * opportunity to clear its state.
     */
    virtual void clear() = 0;
    //@}
};

/**
 * @brief Histogram strategy that sets up uniform bins over a predetermined
 * interval. The number of bins and the histogram mode (integers or reals)
 * also need to be configured. This strategy does not use precollection,
 * as all input for setting up the bins must be explicitly provided by
 * the user.
 *
 * Bins are set up when the first value is collected, and never change afterwards.
 * If you need a histogram strategy that supports dynamically extending the
 * histogram with new bins, consider using cAutoRangeHistogramStrategy.
 *
 * @ingroup Statistics
 */
class SIM_API cFixedRangeHistogramStrategy : public cIHistogramStrategy
{
  protected:
    double lo = NAN;
    double hi = NAN;
    int numBins = -1;
    Mode mode = cHistogram::MODE_REALS; // may not be AUTO

  private:
    void copy(const cFixedRangeHistogramStrategy& other);

  public:
    /** @name Constructors, copying. */
    //@{
    cFixedRangeHistogramStrategy() {} // must be configured using setter methods
    cFixedRangeHistogramStrategy(double lo, double hi, int numBins, Mode mode=cHistogram::MODE_REALS) :
        lo(lo), hi(hi), numBins(numBins), mode(mode) {}
    cFixedRangeHistogramStrategy(const cFixedRangeHistogramStrategy& other): cIHistogramStrategy(other) {copy(other);}
    cFixedRangeHistogramStrategy& operator=(const cFixedRangeHistogramStrategy& other);
    virtual cFixedRangeHistogramStrategy *dup() const override {return new cFixedRangeHistogramStrategy(*this);}
    //@}

    /** @name Configuring. */
    //@{
    void setRange(double lo, double hi) {this->lo = lo; this->hi = hi;}
    double getUpperLimit() const {return hi;}
    void setUpperLimit(double hi) {this->hi = hi;}
    double getLowerLimit() const {return lo;}
    void setLowerLimit(double lo) {this->lo = lo;}
    Mode getMode() const {return mode;}
    void setMode(Mode mode) {this->mode = mode;}
    double getBinSize() const {return numBins;}
    void setBinSize(double binSize) {this->numBins = binSize;}
    //@}

    /** @name Redefined cIHistogramStrategy methods */
    //@{
    virtual void collect(double value) override;
    virtual void collectWeighted(double value, double weight) override;
    virtual void setUpBins() override;
    virtual void clear() override {}
    //@}
};

/**
 * @brief Base class for histogram strategies that employ a precollection phase
 * in order to gather input for setting up the bins. This class provides
 * storage for the precollected values, and also a built-in algorithm for
 * deciding when to stop precollection.
 *
 * @ingroup Statistics
 */
class SIM_API cPrecollectionBasedHistogramStrategy : public cIHistogramStrategy
{
  protected:
    bool inPrecollection = true;
    size_t numToPrecollect = 1000;
    size_t numToCollate = 10;
    double lastRange = NAN;
    int rangeUnchangedCounter = 0;
    int rangeUnchangedThreshold = 50;
    double finiteMinValue = NAN, finiteMaxValue = NAN; // have to keep track separate from cStdDev, ignoring infs
    std::vector<double> values;
    std::vector<double> weights; // same size as values[]

  protected:
    virtual void moveValuesIntoHistogram();
    virtual bool precollect(double value, double weight=1.0); // true: precollection over
    virtual void createBins() = 0;

  private:
    void copy(const cPrecollectionBasedHistogramStrategy& other);

  public:
    /** @name Constructors, copying. */
    //@{
    cPrecollectionBasedHistogramStrategy() {}
    cPrecollectionBasedHistogramStrategy(const cPrecollectionBasedHistogramStrategy& other): cIHistogramStrategy(other) {copy(other);}
    cPrecollectionBasedHistogramStrategy& operator=(const cPrecollectionBasedHistogramStrategy& other);
    //@}

    /** @name Configuring. */
    //@{
    int getNumToPrecollect() const {return numToPrecollect;}
    void setNumToPrecollect(int numToPrecollect) {this->numToPrecollect = numToPrecollect;}
    int getNumToCollate() const {return numToCollate;}
    void setNumToCollate(int numToCollate) {this->numToCollate = numToCollate;}
    int getRangeUnchangedThreshold() const {return rangeUnchangedThreshold;}
    void setRangeUnchangedThreshold(int threshold) {this->rangeUnchangedThreshold = threshold;}
    //@}

    /** @name Redefined cIHistogramStrategy methods */
    //@{
    virtual void setUpBins() override;
    virtual void clear() override;
    //@}
};

/**
 * @brief A strategy class used by the default setup of cHistogram. It is
 * meant to provide a good quality uniform-bin histogram without requiring
 * manual configuration.
 *
 * This strategy uses precollection to gather input information about the
 * distribution before setting up the bins. Precollection is used to determine
 * the initial histogram range and the histogram mode (integers vs. reals).
 * In integers mode, bin edges will be whole numbers.
 *
 * To keep up with distributions that change over time, this histogram strategy
 * can auto-extend the histogram range by adding new bins as needed. It also
 * performs bin merging when necessary, to keep the number of bins reasonably low.
 *
 * Collected infinities are counted as underflows/overflows, and they do not
 * affect the histogram layout in any way.
 *
 * @ingroup Statistics
 */
class SIM_API cDefaultHistogramStrategy : public cPrecollectionBasedHistogramStrategy
{
  private:
    static const int DEFAULT_NUM_BINS = 60; // a number with many divisors
    double rangeExtensionFactor = 1.5;
    double binSize = NAN;
    int numBinsHint = -1;
    int targetNumBins = DEFAULT_NUM_BINS;
    Mode mode = cHistogram::MODE_AUTO;
    bool autoExtend = true;
    bool binMerging = true;
    int maxNumBins = 1000;

  protected:
    virtual void createBins() override;
    virtual void extendBinsTo(double value);
    virtual void reduceNumBinsTo(int numBins);
    virtual void mergeAllBinsIntoOne(double newApproxBinSize);

  private:
    void copy(const cDefaultHistogramStrategy& other);

  public:
    /** @name Constructors, copying. */
    //@{
    explicit cDefaultHistogramStrategy(int numBinsHint=-1, Mode mode=cHistogram::MODE_AUTO) : numBinsHint(numBinsHint), mode(mode) {}
    cDefaultHistogramStrategy(const cDefaultHistogramStrategy& other): cPrecollectionBasedHistogramStrategy(other) {copy(other);}
    cDefaultHistogramStrategy& operator=(const cDefaultHistogramStrategy& other);
    virtual cDefaultHistogramStrategy *dup() const override {return new cDefaultHistogramStrategy(*this);}
    //@}

    /** @name Configuring. */
    //@{
    int getNumBinsHint() const {return numBinsHint;}
    void setNumBinsHint(int numBins) {this->numBinsHint = numBins;}
    //@}

    /** @name Redefined cIHistogramStrategy methods. */
    //@{
    virtual void collect(double value) override;
    virtual void collectWeighted(double value, double weight) override;
    virtual void clear() override {cPrecollectionBasedHistogramStrategy::clear();}
    //@}
};

/**
 * @brief A generic, very configurable histogram strategy that is meant to provide
 * a good quality histogram for practical distributions, and creates uniform bins.
 * This strategy uses precollection to gather input information about the
 * distribution before setting up the bins.
 *
 * Several parameters and constraints can be specified for setting up the bins:
 * range lower and/or upper endpoint, bin size, number of bins, mode (integers
 * or reals), and whether bin size rounding is to be used. If bin size rounding
 * is turned on, a bin size of the form {1|2|5}*10^n is chosen, i.e. a power of ten,
 * or two or five times a power of ten. When both endpoints of the range are
 * left unspecified, the histogram range is derived by taking the range of the
 * precollected observations and extending it symmetrically by a range extension
 * factor. If one of the endpoints is specified by the user, that value is used
 * instead of the computed one. The number of observations to precollect as well
 * as the range extension factor can be configured. If the histogram mode
 * (integers vs. reals) is left unspecified, it will be determined by examining
 * the precollected values. In integers mode, bin edges will be whole numbers.
 * If the histogram range and mode are all specified by the user, the
 * precollection phase is skipped.
 *
 * If there are inconsistent or conflicting settings, e.g. an explicitly given
 * histogram range is not multiple of the bin size in integers mode, the bin
 * setup algorithm will do "best effort" to set up the histogram instead of
 * stopping with an exception.
 *
 * This histogram strategy can auto-extend the histogram range by adding new
 * bins at either end. One can also set up an upper limit to the number of
 * histogram bins to prevent it from growing indefinitely. Bin merging can
 * also be enabled: it will cause every two (or N) adjacent bins to be
 * merged to reduce the number of bins if their number grows too high.
 *
 * Collected infinities are counted as underflows/overflows, and they do not
 * affect the histogram layout in any way.
 *
 * @ingroup Statistics
 */
class SIM_API cAutoRangeHistogramStrategy : public cPrecollectionBasedHistogramStrategy
{
  private:
    static const int DEFAULT_NUM_BINS = 60; // a number with many divisors
    double lo = NAN;  // range lower limit; use NaN for unspecified
    double hi = NAN;  // range upper limit; use NaN for unspecified
    double rangeExtensionFactor = 1.5;
    int numBinsHint = -1;
    int targetNumBins = DEFAULT_NUM_BINS;
    double requestedBinSize = NAN; // user-given
    double binSize = NAN; // actual (computed)
    Mode mode = cHistogram::MODE_AUTO;
    bool binSizeRounding = true;
    bool autoExtend = true;
    bool binMerging = true;
    int maxNumBins = 1000;

  protected:
    virtual void createBins() override;
    virtual void extendBinsTo(double value);
    virtual void reduceNumBinsTo(int numBins);
    virtual void mergeAllBinsIntoOne(double newApproxBinSize);

  private:
    void copy(const cAutoRangeHistogramStrategy& other);

  public:
    /** @name Constructors, copying. */
    //@{
    explicit cAutoRangeHistogramStrategy(Mode mode=cHistogram::MODE_AUTO) : mode(mode) {}
    explicit cAutoRangeHistogramStrategy(int numBins, Mode mode=cHistogram::MODE_AUTO) : numBinsHint(numBins), mode(mode) {}
    cAutoRangeHistogramStrategy(const cAutoRangeHistogramStrategy& other): cPrecollectionBasedHistogramStrategy(other) {copy(other);}
    cAutoRangeHistogramStrategy& operator=(const cAutoRangeHistogramStrategy& other);
    virtual cAutoRangeHistogramStrategy *dup() const override {return new cAutoRangeHistogramStrategy(*this);}
    //@}

    /** @name Configuring. */
    //@{
    void setRangeHint(double lo, double hi) {this->lo = lo; this->hi = hi;}  ///< Use NAN to leave either value unspecified.
    double getUpperLimitHint() const {return hi;}
    void setUpperLimitHint(double hi) {this->hi = hi;}
    double getLowerLimitHint() const {return lo;}
    void setLowerLimitHint(double lo) {this->lo = lo;}
    double getRangeExtensionFactor() const {return rangeExtensionFactor;}
    void setRangeExtensionFactor(double rangeExtensionFactor) {this->rangeExtensionFactor = rangeExtensionFactor;}
    Mode getMode() const {return mode;}
    void setMode(Mode mode) {this->mode = mode;}
    double getBinSizeHint() const {return requestedBinSize;}
    void setBinSizeHint(double binSize) {this->requestedBinSize = binSize;}
    bool getBinSizeRounding() const {return binSizeRounding;}
    void setBinSizeRounding(bool binSizeRounding) {this->binSizeRounding = binSizeRounding;}
    int getNumBinsHint() const {return numBinsHint;}
    void setNumBinsHint(int numBins) {this->numBinsHint = numBins;}
    void setAutoExtend(bool enable) {this->autoExtend = enable;}
    bool getAutoExtend() const {return autoExtend;}
    void setBinMerging(bool enable) {this->binMerging = enable;}
    bool getBinMerging() const {return binMerging;}
    int getMaxNumBins() const {return maxNumBins;}
    void setMaxNumBins(int numBins) {this->maxNumBins = numBins;}
    //@}

    /** @name Redefined cIHistogramStrategy methods. */
    //@{
    virtual void collect(double value) override;
    virtual void collectWeighted(double value, double weight) override;
    virtual void clear() override;
    //@}
};

}  // namespace omnetpp


#endif



