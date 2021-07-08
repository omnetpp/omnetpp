//==========================================================================
//  CABSTRACTHISTOGRAM.H - part of
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

#ifndef __OMNETPP_CABSTRACTHISTOGRAM_H
#define __OMNETPP_CABSTRACTHISTOGRAM_H

#include "cstddev.h"

namespace omnetpp {

/**
 * @brief Interface and base class for histogram-like density estimation classes.
 */
class SIM_API cAbstractHistogram : public cStdDev
{
  public:
    /**
     * @brief Information about a histogram bin. This struct is not used
     * internally by the histogram classes, only to return information to the user.
     */
    struct Bin
    {
        double lower;  // lower bin bound (inclusive)
        double upper;  // lower bin bound (exclusive)
        double value;  // count or sum of weights (or its estimate)
        double relativeFreq;  // value / total
        Bin() {lower=upper=value=relativeFreq=0;}
        std::string str() const;
    };

  private:
    void copy(const cAbstractHistogram& other) {}

  public:
    // internal, for use in sim_std.msg; note: each call overwrites the previous value!
    const Bin& internalGetBinInfo(int k) const;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cAbstractHistogram(const cAbstractHistogram& other) = default;

    /**
     * Constructor.
     */
    explicit cAbstractHistogram(const char *name=nullptr, bool weighted=false) : cStdDev(name, weighted) {}

    /**
     * Destructor.
     */
    virtual ~cAbstractHistogram() {}

    /**
     * Assignment operator. The name member is not copied; see cNamedObject::operator=() for details.
     */
    cAbstractHistogram& operator=(const cAbstractHistogram& res) {
        if (this == &res)
           return *this;
       cStdDev::operator=(res);
       copy(res);
       return *this;
    }

    virtual cAbstractHistogram *dup() const override {throw cRuntimeError(this, E_CANTDUP);}
    //@}

    /** @name Accessing histogram bins. */
    //@{
    /**
     * Returns true if histogram is already available. See setUpBins().
     */
    virtual bool binsAlreadySetUp() const = 0;

    /**
     * Sets up histogram bins, possibly based on data collected during a
     * precollection phase. Calling this if the bins are already set up
     * raises an error.
     */
    virtual void setUpBins() = 0;

    /**
     * Returns the bin edges of the histogram. There is always one more edge than bin,
     * except when the histogram has not been set up yet, in which case both are zero.
     */
    virtual std::vector<double> getBinEdges() const;

    /**
     * Returns the bin values of the histogram. There is always one less bin than edge,
     * except when the histogram has not been set up yet, in which case both are zero.
     */
    virtual std::vector<double> getBinValues() const;

    /**
     * Returns the number of histogram bins.
     */
    virtual int getNumBins() const = 0;

    /**
     * Returns the kth histogram bin edge. Legal values for k are 0 through
     * getNumBins(), that is, there's one more edge than the number of bins.
     * getBinEdge(0) returns the lower edge of the first bin, and
     * getBinEdge(getNumBins()) returns the upper edge of the last bin.
     * The lower edge is inclusive, the upper edge is exclusive.
     */
    virtual double getBinEdge(int k) const = 0;

    /**
     * Returns the total weight of the observations in the k'th bin of the histogram.
     * (In the unweighted case, every observation is regarded as having the weight 1.0.)
     */
    virtual double getBinValue(int k) const = 0;

    /**
     * Returns the estimated value of the Probability Density Function
     * within bin k. This method simply divides the number of observations
     * in bin k with the bin size and the number of total observations collected.
     */
    virtual double getBinPDF(int k) const;

    /**
     * Returns number of observations that were below the histogram range,
     * independent of their weights. This includes the number of collected
     * negative infinities as well.
     */
    virtual int64_t getNumUnderflows() const = 0;

    /**
     * Returns number of observations that were above the histogram range,
     * independent of their weights. This includes the number of collected
     * positive infinities as well.
     */
    virtual int64_t getNumOverflows() const = 0;

    /**
     * Returns the total weight of the observations that were below the
     * histogram range. This includes the weights of the collected negative
     * infinities as well.
     */
    virtual double getUnderflowSumWeights() const = 0;

    /**
     * Returns the total weight of the observations that were above the
     * histogram range. This includes the weights of the collected positive
     * infinities as well.
     */
    virtual double getOverflowSumWeights() const = 0;

    /**
     * Returns number of observations that were negative infinity, independent of their weights.
     */
    virtual int64_t getNumNegInfs() const = 0;

    /**
     * Returns number of observations that were positive infinity, independent of their weights.
     */
    virtual int64_t getNumPosInfs() const = 0;

    /**
     * Returns the total weight of the observations that were negative infinity.
     */
    virtual double getNegInfSumWeights() const = 0;

    /**
     * Returns the total weight of the observations that were positive infinity.
     */
    virtual double getPosInfSumWeights() const = 0;

    /**
     * Combines the functionality of getBinEdge(), getBinValue() and getBinPDF() into a
     * single call.
     */
    virtual Bin getBinInfo(int k) const;
    //@}

    /** @name Density and cumulated density approximation functions, random number generation. */
    //@{

    /**
     * Returns the estimated value of the Probability Density Function at a given x.
     */
    virtual double getPDF(double x) const;

    /**
     * Returns the estimated value of the Cumulative Density Function at a given x.
     */
    virtual double getCDF(double x) const;

    /**
     * Returns a random number from the distribution represented by the histogram.
     */
    virtual double draw() const override;
    //@}
};

}  // namespace omnetpp


#endif



