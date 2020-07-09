//==========================================================================
//  CSTDDEV.H - part of
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

#ifndef __OMNETPP_CSTDDEV_H
#define __OMNETPP_CSTDDEV_H

#include <cstdio>
#include "cstatistic.h"

namespace omnetpp {

/**
 * @brief Statistics class to collect min, max, mean, and standard deviation.
 *
 * @ingroup Statistics
 */
class SIM_API cStdDev : public cStatistic
{
  protected:
    bool weighted;
    double minValue;
    double maxValue;
    int64_t numValues; // the actual count of observations, independent of their weights
    double sumWeights;  // equals count in the unweighted case
    double sumWeightedValues; // equals sum in the unweighted case
    double sumSquaredWeights; // equals count in the unweighted case
    double sumWeightedSquaredValues; // sum of squared values in the unweighted case

  private:
    void copy(const cStdDev& other);

  protected:
    void assertUnweighted() const;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cStdDev(const cStdDev& r) : cStatistic(r) {copy(r);}

    /**
     * Constructor.
     */
    explicit cStdDev(const char *name=nullptr, bool weighted=false);

    /**
     * Destructor.
     */
    virtual ~cStdDev() {}

    /**
     * Assignment operator. The name member is not copied; see cNamedObject::operator=() for details.
     */
    cStdDev& operator=(const cStdDev& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cStdDev *dup() const override  {return new cStdDev(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;

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

    /** @name Redefined cStatistic functions. */
    //@{
    /**
     * Returns false if this object represents unweighted statistics,
     * and true if weighted.
     */
    virtual bool isWeighted() const override  {return weighted;}

    /**
     * Collects one observation.
     */
    virtual void collect(double value) override;
    using cStatistic::collect;

    /**
     * Collects one observation with a given weight. The weight must not be
     * negative. (Zero-weight observations are allowed, but will not affect
     * mean and stddev.)
     */
    virtual void collectWeighted(double value, double weight) override;
    using cStatistic::collectWeighted;

    /**
     * Merge another statistics object into this one.
     */
    virtual void merge(const cStatistic *other) override;

    /**
     * Returns the number of values collected, regardless of their weights.
     */
    virtual int64_t getCount() const override  {return numValues;}

    /**
     * Returns the sum of the observations. The sum is only collected
     * if the object represents unweighted statistics, i.e. it is an
     * error to call this function for weighted statistics.
     */
    virtual double getSum() const override  {assertUnweighted(); return sumWeightedValues;}

    /**
     * Returns the sum of squares of the collected observations.
     * This is only collected if the object represents unweighted statistics,
     * i.e. it is an error to call this function for weighted statistics.
     */
    virtual double getSqrSum() const override  {assertUnweighted(); return sumWeightedSquaredValues;}

    /**
     * Returns the minimum of the collected observations, or NaN if none
     * have been collected yet.
     */
    virtual double getMin() const override;

    /**
     * Returns the maximum of the collected observations, or NaN if none
     * have been collected yet.
     */
    virtual double getMax() const override;

    /**
     * Returns the (weighted/unweighted) mean of the collected observations, or NaN
     * if none have been collected yet.
     */
    virtual double getMean() const override;

    /**
     * Returns the estimated (weighted/unweighted) standard deviation of the
     * observations, or NaN if less than two observations have been collected.
     */
    virtual double getStddev() const override;

    /**
     * Returns the variance of the observations collected, or NaN if less than
     * two observations have been collected.
     */
    virtual double getVariance() const override;

    /**
     * Returns the sum of weights. (For unweighted statistics, all weights are
     * taken to be 1.0.)
     */
    virtual double getSumWeights() const override  {return sumWeights;}

    /**
     * Returns the sum of weight*value products. (For unweighted statistics, all
     * weights are taken to be 1.0.)
     */
    virtual double getWeightedSum() const override  {return sumWeightedValues;}

    /**
     * Returns the sum of squared weights. (For unweighted statistics, all weights
     * are taken to be 1.0.)
     */
    virtual double getSqrSumWeights() const override  {return sumSquaredWeights;}

    /**
     * Returns the sum of weight*value*value products. (For unweighted statistics,
     * all weights are taken to be 1.0.)
     */
    virtual double getWeightedSqrSum() const override  {return sumWeightedSquaredValues;}

    /**
     * Returns a number from a normal distribution with the current mean and
     * standard deviation.
     */
    virtual double draw() const override;

    /**
     * Clears the results collected so far.
     */
    virtual void clear() override;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const override;

    /**
     * Reads the object data from a file written out by saveToFile()
     * (or written by hand)
     */
    virtual void loadFromFile(FILE *) override;
    //@}
};

}  // namespace omnetpp

#endif

