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
 * encapsulate the task of setting up an managing the bins in a cHistogram.
 *
 * @ingroup Statistics
 */
//TODO override getOwner()?
class SIM_API cIHistogramStrategy : public cObject
{
    friend class cHistogram;
  public:
    typedef enum cHistogram::HistogramMode HistogramMode;

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
 * interval. The bin size and histogram mode (integers or reals) can
 * also be configured. This strategy does not use precollection.
 *
 * Auto-extending the histogram with new bins during collection is optional;
 * it is turned off by default.
 *
 * @ingroup Statistics
 */
class SIM_API cFixedRangeHistogramStrategy : public cIHistogramStrategy
{
  protected:
    double lo = NAN;
    double hi = NAN;
    double binSize = NAN;
    HistogramMode mode = cHistogram::MODE_REALS; // may not be AUTO
    bool autoExtend = false;

  private:
    void copy(const cFixedRangeHistogramStrategy& other);

  public:
    /** @name Constructors, copying. */
    //@{
    cFixedRangeHistogramStrategy() {}
    cFixedRangeHistogramStrategy(double lo, double hi, double binSize, HistogramMode mode=cHistogram::MODE_REALS) :
        lo(lo), hi(hi), binSize(binSize), mode(mode) {}
    cFixedRangeHistogramStrategy(const cFixedRangeHistogramStrategy& other): cIHistogramStrategy(other) {copy(other);}
    cFixedRangeHistogramStrategy& operator=(const cFixedRangeHistogramStrategy& other);
    virtual cFixedRangeHistogramStrategy *dup() const override {return new cFixedRangeHistogramStrategy(*this);}
    //@}

    /** @name Configuring. */
    //@{
    double getHi() const {return hi;}
    void setHi(double hi) {this->hi = hi;}
    double getLo() const {return lo;}
    void setLo(double lo) {this->lo = lo;}
    HistogramMode getMode() const {return mode;}
    void setMode(HistogramMode mode) {this->mode = mode;}
    double getBinSize() const {return binSize;}
    void setBinSize(double binSize) {this->binSize = binSize;}
    void setAutoExtend(bool enable) {this->autoExtend = enable;}
    bool getAutoExtend() const {return autoExtend;}
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
 * in order to get information for setting up the bins. This class provides
 * storage for the precollected values.
 *
 * @ingroup Statistics
 */
class SIM_API cPrecollectionBasedHistogramStrategy : public cIHistogramStrategy
{
  protected:
    bool inPrecollection = true;
    size_t numToPrecollect = 100;
    std::vector<double> values;
    std::vector<double> weights; // same size as values[]

  protected:
    virtual void moveValuesIntoHistogram();
    virtual void precollect(double value, double weight=1.0);
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
    //@}

    /** @name Redefined cIHistogramStrategy methods */
    //@{
    virtual void setUpBins() override;
    virtual void clear() override;
    //@}
};

/**
 * @brief The histogram strategy used in the default setup of cHistogram.
 * This strategy should be able to provide a good quality histogram for
 * practical distributions without requiring manual configuration.
 *
 * This strategy uses the following techniques: precollection; bin size rounding
 * (to 1, 2 or 5 times powers of ten); auto-extension by adding bins if needed;
 * merging of adjacent bins to reduce bin count if needed.
 *
 * @ingroup Statistics
 */
class SIM_API cDefaultHistogramStrategy : public cPrecollectionBasedHistogramStrategy
{
  private:
    double rangeExtensionFactor = 1.5;
    double binSize = NAN;
    int desiredNumBins;
    HistogramMode mode = cHistogram::MODE_AUTO;

  protected:
    virtual void createBins() override;
    double computeBinSize(double& rangeMin, double& rangeMax);
    double computeIntegerBinSize(double& rangeMin, double& rangeMax);

  private:
    void copy(const cDefaultHistogramStrategy& other);

  public:
    /** @name Constructors, copying. */
    //@{
    explicit cDefaultHistogramStrategy(int desiredNumBins=30, HistogramMode mode=cHistogram::MODE_AUTO) : desiredNumBins(desiredNumBins), mode(mode) {}  //TODO more bins by default? (60)
    cDefaultHistogramStrategy(const cDefaultHistogramStrategy& other): cPrecollectionBasedHistogramStrategy(other) {copy(other);}
    cDefaultHistogramStrategy& operator=(const cDefaultHistogramStrategy& other);
    virtual cDefaultHistogramStrategy *dup() const override {return new cDefaultHistogramStrategy(*this);}
    //@}

    /** @name Redefined cIHistogramStrategy methods. */
    //@{
    virtual void collect(double value) override;
    virtual void collectWeighted(double value, double weight) override;
    virtual void clear() override {cPrecollectionBasedHistogramStrategy::clear();}
    //@}
};

/**
 * @brief A generic precollection-based histogram strategy that yields a good
 * quality histogram for practical distributions. This strategy is very
 * configurable, and uses the following techniques: precollection; bin size
 * rounding (to 1, 2 or 5 times powers of ten); auto-extension by adding bins
 * if needed; merging of adjacent bins to reduce bin count if needed.
 *
 * It is possible to explicitly set any of the following values (and the rest
 * will be chosen or computed automatically): number of bins; bin size;
 * number of observations to precollect; range extension factor; range lower
 * edge; range upper edge. Especially in INTEGERS mode, if the bins cannot be
 * set up to satisfy all explicitly given constraints (for example, if the
 * explicitly specified range is not an integer multiple of the explicitly
 * specified bin size), an error will be thrown.
 *
 */
// note: when constraints are overdetermined, "best effort" will be made to satisfy them:
// at least as numBins bins created; histogram range to include the [lo,hi) interval, etc.
//TODO do not precollect if range and mode are fully specified!
class SIM_API cAutoRangeHistogramStrategy : public cPrecollectionBasedHistogramStrategy
{
  private:
    const int DEFAULT_NUM_BINS = 30; // to allow merging every 2, 3, 5, 6 adjacent bins during post-processing
    double lo = NAN;  // set NaN for unspecified
    double hi = NAN;  // set NaN for unspecified
    double rangeExtensionFactor = 1.5;
    int desiredNumBins = -1;
    double requestedBinSize = NAN; // user-given
    double binSize = NAN; // actual (computed)
    HistogramMode mode = cHistogram::MODE_AUTO;
    bool binSizeRounding = true;
    bool autoExtend = true;
    bool binMerging = true; // TODO maybe add maxNumBins instead of this flag? i.e. when #bins exceeds maxNumBins, call mergeBins() with appropriate "n" -- prevents binSize getting multiplied by 2^n

  protected:
    virtual void createBins() override;
    virtual void mergeBins();

  private:
    void copy(const cAutoRangeHistogramStrategy& other);

  public:
    /** @name Constructors, copying. */
    //@{
    // lo, hi: specify NaN for either or both or neither!
    explicit cAutoRangeHistogramStrategy(double lo=NAN, double hi=NAN, int numBins=-1, HistogramMode mode=cHistogram::MODE_AUTO) :  //TODO remove numBins from ctor?
        lo(lo), hi(hi), desiredNumBins(numBins), mode(mode) {}
    cAutoRangeHistogramStrategy(const cAutoRangeHistogramStrategy& other): cPrecollectionBasedHistogramStrategy(other) {copy(other);}
    cAutoRangeHistogramStrategy& operator=(const cAutoRangeHistogramStrategy& other);
    virtual cAutoRangeHistogramStrategy *dup() const override {return new cAutoRangeHistogramStrategy(*this);}
    //@}

    /** @name Configuring. */
    //@{
    //TODO all params are hints!! rename???
    double getBinSize() const {return requestedBinSize;}
    void setBinSize(double binSize) {this->requestedBinSize = binSize;}
    bool getBinSizeRounding() const {return binSizeRounding;}
    void setBinSizeRounding(bool binSizeRounding) {this->binSizeRounding = binSizeRounding;}
    void setRange(double lo, double hi) {this->lo = lo; this->hi = hi;}
    double getRangeHi() const {return hi;}
    void setRangeHi(double hi) {this->hi = hi;}
    double getRangeLo() const {return lo;}
    void setRangeLo(double lo) {this->lo = lo;}
    HistogramMode getMode() const {return mode;}
    void setMode(HistogramMode mode) {this->mode = mode;}
    int getNumBins() const {return desiredNumBins;}
    void setNumBins(int numBins) {this->desiredNumBins = numBins;}
    double getRangeExtensionFactor() const {return rangeExtensionFactor;}
    void setRangeExtensionFactor(double rangeExtensionFactor) {this->rangeExtensionFactor = rangeExtensionFactor;}
    void setAutoExtend(bool enable) {this->autoExtend = enable;}
    bool getAutoExtend() const {return autoExtend;}
    void setBinMerging(bool enable) {this->binMerging = enable;}
    bool getBinMerging() const {return binMerging;}
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



