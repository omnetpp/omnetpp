//==========================================================================
//  CPSQUARE.H - part of
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

//  Author: Andras Varga, based on code of Babak Fakhamzadeh, TU Delft, Mar-Jun 1996;

#ifndef __OMNETPP_CPSQUARE_H
#define __OMNETPP_CPSQUARE_H

#include "cabstracthistogram.h"

namespace omnetpp {


/**
 * @brief Implements the P<sup>2</sup> algorithm, which calculates quantile
 * values without storing the observations. See the seminal paper titled
 * "The P^2 Algorithm for Dynamic Statistical Computing Calculation of
 * Quantiles and Histograms Without Storing Observations" by Raj Jain and
 * Imrich Chlamtac.
 *
 * @ingroup Statistics
 */
class SIM_API cPSquare : public cAbstractHistogram
{
  protected:
    int numBins;         // number of bins
    long numObs = 0;     // number of observations
    int *n = nullptr;    // array of positions
    double *q = nullptr; // array of heights

    long numNegInfs = 0, numPosInfs = 0;

  protected:
    void copy(const cPSquare& other);
    void ensureStrictlyIncreasingEdges();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cPSquare(const cPSquare& r);

    /**
     * Constructor.
     */
    explicit cPSquare(const char *name=nullptr, int bins=10);

    /**
     * Destructor.
     */
    virtual ~cPSquare();

    /**
     * Assignment operator. The name member is not copied; see cNamedObject::operator=() for details.
     */
    cPSquare& operator=(const cPSquare& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPSquare *dup() const override  {return new cPSquare(*this);}

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

  public:
    /** @name Redefined member functions from cStatistic */
    //@{
    /**
     * Returns true if histogram is already available. This cPSquare implementation
     * always returns true, since the algorithm does not contain a precollection stage.
     */
    virtual bool binsAlreadySetUp() const override {return true;}

    /**
     * Transforms the array of pre-collected values into histogram structure.
     * This cPSquare implementation does nothing.
     */
    virtual void setUpBins() override {}

    /**
     * Collects one observation.
     */
    virtual void collect(double value) override;
    using cStatistic::collect;

    /**
     * Collects one observation with a given weight. Throws an error,
     * as cPSquare does not support weighted statistics.
     */
    virtual void collectWeighted(double value, double weight) override;
    using cStatistic::collectWeighted;

    /**
     * Returns the number of bins used.
     */
    virtual int getNumBins() const override;

    /**
     * Returns the kth bin boundary. Note that because of the P2 algorithm,
     * bin boundaries are shifting during data collection, thus getBinValue() and
     * other methods based on getBinValue() and getBinEdge() return approximate values.
     */
    virtual double getBinEdge(int k) const override;

    /**
     * Returns the number of observations in the kth histogram bin.
     */
    virtual double getBinValue(int k) const override;

    /**
     * Returns number of observations that were below the histogram range,
     * independent of their weights. In cPSquare, this is always the same as the
     * number of negative infinities.
     */
    virtual int64_t getNumUnderflows() const override {return numNegInfs;}

    /**
     * Returns number of observations that were above the histogram range,
     * independent of their weights. In cPSquare, this is always the same as the
     * number of positive infinities.
     */
    virtual int64_t getNumOverflows() const override {return numPosInfs;}

    /**
     * Returns the total weight of the observations that were below the histogram range.
     * In cPSquare, this is always the same as the number of negative infinities.
     */
    virtual double getUnderflowSumWeights() const override {return numNegInfs;}

    /**
     * Returns the total weight of the observations that were above the histogram range.
     * In cPSquare, this is always the same as the number of positive infinities.
     */
    virtual double getOverflowSumWeights() const override {return numPosInfs;}

    /**
     * Returns number of observations that were negative infinity, independent of their weights.
     */
    virtual int64_t getNumNegInfs() const override {return numNegInfs;}

    /**
     * Returns number of observations that were positive infinity, independent of their weights.
     */
    virtual int64_t getNumPosInfs() const override {return numPosInfs;}

    /**
     * Returns the total weight of the observations that were negative infinity.
     * Since cPSquare does not support weighted statistics, this is always the same
     * as the number of observations that were negative infinity.
     */
    virtual double getNegInfSumWeights() const override {return numNegInfs;}

    /**
     * Returns the total weight of the observations that were positive infinity.
     * Since cPSquare does not support weighted statistics, this is always the same
     * as the number of observations that were positive infinity.
     */
    virtual double getPosInfSumWeights() const override {return numPosInfs;}

    /**
     * Generates a random number based on the collected data.
     */
    virtual double draw() const override;

    /**
     * Merging is not supported by this class. This method throws an error.
     */
    virtual void merge(const cStatistic *other) override;

    /**
     * Clears the results collected so far.
     */
    virtual void clear() override;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const override;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *) override;
    //@}
};

}  // namespace omnetpp


#endif

