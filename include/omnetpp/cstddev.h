//==========================================================================
//  CSTDDEV.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

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
    long numValues;
    double minValue, maxValue;
    double sumValues, sumSquaredValues;

  private:
    void copy(const cStdDev& other);

  protected:
    void doMerge(const cStatistic *other);

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
    explicit cStdDev(const char *name=nullptr);

    /**
     * Destructor.
     */
    virtual ~cStdDev() {}

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
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
    virtual std::string info() const override;

    /**
     * Produces a multi-line description of the object.
     * See cObject for more details.
     */
    virtual std::string detailedInfo() const override;

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
     * Collects one value.
     */
    virtual void collect(double value) override;

    /**
     * Convenience method, delegates to collect(double).
     */
    virtual void collect(SimTime value) override {collect(value.dbl());}

    /**
     * Updates this object with data coming from another statistics
     * object -- as if this object had collected observations fed
     * into the other object as well. Throws an error if the other
     * object is weighted statistics (see isWeighted()).
     */
    virtual void merge(const cStatistic *other) override;

    /**
     * Returns false, because this class does not collect weighted statistics.
     */
    virtual bool isWeighted() const override  {return false;}

    /**
     * Returns the number of observations collected.
     */
    virtual long getCount() const override  {return numValues;}

    /**
     * Returns the sum of the values.
     */
    virtual double getSum() const override  {return sumValues;}

    /**
     * Returns the squared sum of the collected values.
     */
    virtual double getSqrSum() const override  {return sumSquaredValues;}

    /**
     * Returns the minimum of the collected values, or NaN if none have been collected yet.
     */
    virtual double getMin() const override;

    /**
     * Returns the maximum of the collected values, or NaN if none have been collected yet.
     */
    virtual double getMax() const override;

    /**
     * Returns the mean of the collected values, or NaN if none have been collected yet.
     */
    virtual double getMean() const override;

    /**
     * Returns the standard deviation of the observations, or NaN if less than
     * two observations have been collected.
     */
    virtual double getStddev() const override;

    /**
     * Returns the variance of the observations collected, or NaN if less than
     * two observations have been collected.
     */
    virtual double getVariance() const override;

    /**
     * Since this is unweighted statistics, the sum of weights is getCount().
     */
    virtual double getWeights() const override  {return getCount();}

    /**
     * Since this is unweighted statistics, the sum of weight*value products
     * is getSum().
     */
    virtual double getWeightedSum() const override  {return getSum();}

    /**
     * Since this is unweighted statistics, the sum of squared weights
     * is getCount().
     */
    virtual double getSqrSumWeights() const override  {return getCount();}

    /**
     * Since this is unweighted statistics, the sum of weight*value*value
     * products is getSqrSum().
     */
    virtual double getWeightedSqrSum() const override  {return getSqrSum();}

    /**
     * Returns a number from a normal distribution with the current mean and
     * standard deviation.
     */
    virtual double draw() const override;

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult() override;

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


/**
 * @brief Statistics class to collect doubles and calculate weighted statistics
 * of them. It can be used for example to calculate time average.
 *
 * @ingroup Statistics
 */
class SIM_API cWeightedStdDev : public cStdDev
{
  protected:
    double sumWeights;
    double sumWeightedValues;
    double sumSquaredWeights;
    double sumWeightedSquaredValues;

  private:
    void copy(const cWeightedStdDev& other);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructors, destructor, duplication and assignment.
     */
    cWeightedStdDev(const cWeightedStdDev& r) : cStdDev(r) {copy(r);}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    explicit cWeightedStdDev(const char *name=nullptr) : cStdDev(name)  {sumWeights=sumWeightedValues=sumSquaredWeights=sumWeightedSquaredValues=0.0;}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    virtual ~cWeightedStdDev() {}

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cWeightedStdDev& operator=(const cWeightedStdDev& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cWeightedStdDev *dup() const override  {return new cWeightedStdDev(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const override;

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
     * Collects one value.
     */
    virtual void collect(double value) override  {collect2(value,1.0);}

    /**
     * Convenience method, delegates to collect(double).
     */
    virtual void collect(SimTime value) override {collect(value.dbl());}

    /**
     * Returns true, because this class collects weighted statistics.
     */
    virtual bool isWeighted() const override  {return true;}

    /**
     * Collects one value with a given weight. Negative weights will cause a runtime error.
     */
    virtual void collect2(double value, double weight) override;

    /**
     * Convenience method, delegates to collect2(double, double).
     */
    virtual void collect2(SimTime value, double weight) {collect2(value.dbl(), weight);}

    /**
     * Convenience method, delegates to collect2(double, double).
     */
    virtual void collect2(double value, SimTime weight) {collect2(value, weight.dbl());}

    /**
     * Convenience method, delegates to collect2(double, double).
     */
    virtual void collect2(SimTime value, SimTime weight) {collect2(value.dbl(), weight.dbl());}

    /**
     * Updates this object with data coming from another statistics
     * object -- as if this object had collected observations fed
     * into the other object as well.
     */
    virtual void merge(const cStatistic *other) override;

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult() override;

    /**
     * Returns the weighted mean of the observations collected, or NaN if
     * no observations have been collected (or their weights were 0).
     */
    virtual double getMean() const override;

    /**
     * Returns the variance of the observations collected, or NaN if
     * less than two observations have been collected.
     */
    virtual double getVariance() const override;

    /**
     * Returns the sum of weights.
     */
    virtual double getWeights() const override  {return sumWeights;}

    /**
     * Returns the sum of weight*value products.
     */
    virtual double getWeightedSum() const override  {return sumWeightedValues;}

    /**
     * Returns the sum of squared weights.
     */
    virtual double getSqrSumWeights() const override  {return sumSquaredWeights;}

    /**
     * Returns the sum of weight*value*value products.
     */
    virtual double getWeightedSqrSum() const override  {return sumWeightedSquaredValues;}

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

