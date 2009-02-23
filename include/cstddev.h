//==========================================================================
//  CSTDDEV.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cStdDev: basic statistics (mean, stddev, min, max, etc)
//    cWeightedStdDev: weighted version
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSTDDEV_H
#define __CSTDDEV_H

#include <stdio.h>
#include "cstatistic.h"

NAMESPACE_BEGIN

/**
 * Statistics class to collect min, max, mean, and standard deviation.
 *
 * @ingroup Statistics
 */
class SIM_API cStdDev : public cStatistic
{
  protected:
    long num_vals;
    double min_vals, max_vals;
    double sum_vals, sqrsum_vals;

  protected:
    void doMerge(const cStatistic *other);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cStdDev(const cStdDev& r) : cStatistic() {setName(r.getName());operator=(r);}

    /**
     * Constructor.
     */
    explicit cStdDev(const char *name=NULL);

    /**
     * Destructor.
     */
    virtual ~cStdDev() {}

    /**
     * Assignment operator. The name member doesn't get copied; see cNamedObject's operator=() for more details.
     */
    cStdDev& operator=(const cStdDev& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cStdDev *dup() const  {return new cStdDev(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Produces a multi-line description of the object.
     * See cObject for more details.
     */
    virtual std::string detailedInfo() const;

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer);
    //@}

    /** @name Redefined cStatistic functions. */
    //@{

    /**
     * Collects one value.
     */
    virtual void collect(double value);

    /**
     * Convenience method, delegates to collect(double).
     */
    virtual void collect(SimTime value) {collect(value.dbl());}

    /**
     * Updates this object with data coming from another statistics
     * object -- as if this object had collected observations fed
     * into the other object as well. Throws an error if the other
     * object is weighted statistics (see isWeighted()).
     */
    virtual void merge(const cStatistic *other);

    /**
     * Returns false, because this class does not collect weighted statistics.
     */
    virtual bool isWeighted() const  {return false;}

    /**
     * Returns the number of observations collected.
     */
    virtual long getCount() const  {return num_vals;}

    /**
     * Returns the sum of the values.
     */
    virtual double getSum() const  {return sum_vals;}

    /**
     * Returns the squared sum of the collected values.
     */
    virtual double getSqrSum() const  {return sqrsum_vals;}

    /**
     * Returns the minimum of the collected values.
     */
    virtual double getMin() const  {return min_vals;}

    /**
     * Returns the maximum of the collected values.
     */
    virtual double getMax() const  {return max_vals;}

    /**
     * Returns the mean of the observations.
     * Returns 0.0 if nothing was collected yet.
     */
    virtual double getMean() const  {return num_vals==0 ? 0 : sum_vals/num_vals;}

    /**
     * Returns the standard deviation of the observations.
     */
    virtual double getStddev() const;

    /**
     * Returns the variance of the observations collected.
     */
    virtual double getVariance() const;

    /**
     * Since this is unweighted statistics, the sum of weights is getCount().
     */
    virtual double getWeights() const  {return getCount();}

    /**
     * Since this is unweighted statistics, the sum of weight*value products
     * is getSum().
     */
    virtual double getWeightedSum() const  {return getSum();}

    /**
     * Since this is unweighted statistics, the sum of squared weights
     * is getCount().
     */
    virtual double getSqrSumWeights() const  {return getCount();}

    /**
     * Since this is unweighted statistics, the sum of weight*value*value
     * products is getSqrSum().
     */
    virtual double getWeightedSqrSum() const  {return getSqrSum();}

    /**
     * Returns numbers from a normal distribution with the current mean and
     * standard deviation.
     */
    virtual double random() const;

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult();

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const;

    /**
     * Reads the object data from a file written out by saveToFile()
     * (or written by hand)
     */
    virtual void loadFromFile(FILE *);
    //@}
};



/**
 * Statistics class to collect doubles and calculate weighted statistics
 * of them. It can be used for example to calculate time average.
 *
 * @ingroup Statistics
 */
class SIM_API cWeightedStdDev : public cStdDev
{
  protected:
    double sum_weights;
    double sum_weighted_vals;
    double sum_squared_weights;
    double sum_weights_squared_vals;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructors, destructor, duplication and assignment.
     */
    cWeightedStdDev(const cWeightedStdDev& r) : cStdDev() {setName(r.getName());operator=(r);}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    explicit cWeightedStdDev(const char *name=NULL) : cStdDev(name)  {sum_weights=sum_weighted_vals=sum_squared_weights=sum_weights_squared_vals=0.0;}

    /**
     * Constructors, destructor, duplication and assignment.
     */
    virtual ~cWeightedStdDev() {}

    /**
     * Assignment operator. The name member doesn't get copied; see cNamedObject's operator=() for more details.
     */
    cWeightedStdDev& operator=(const cWeightedStdDev& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cWeightedStdDev *dup() const  {return new cWeightedStdDev(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer);
    //@}

    /** @name Redefined cStatistic functions. */
    //@{

    /**
     * Collects one value.
     */
    virtual void collect(double value)  {collect2(value,1.0);}

    /**
     * Convenience method, delegates to collect(double).
     */
    virtual void collect(SimTime value) {collect(value.dbl());}

    /**
     * Returns true, because this class collects weighted statistics.
     */
    virtual bool isWeighted() const  {return true;}

    /**
     * Collects one value with a given weight.
     */
    virtual void collect2(double value, double weight);

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
    virtual void merge(const cStatistic *other);

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult();

    /**
     * Returns the mean of the observations collected.
     * Returns 0.0 if nothing was collected yet.
     */
    virtual double getMean() const  {return sum_weights==0 ? 0 : sum_weighted_vals / sum_weights;}

    /**
     * Returns the variance of the observations collected.
     */
    virtual double getVariance() const;

    /**
     * Returns the sum of weights.
     */
    virtual double getWeights() const  {return sum_weights;}

    /**
     * Returns the sum of weight*value products.
     */
    virtual double getWeightedSum() const  {return sum_weighted_vals;}

    /**
     * Returns the sum of squared weights.
     */
    virtual double getSqrSumWeights() const  {return sum_squared_weights;}

    /**
     * Returns the sum of weight*value*value products.
     */
    virtual double getWeightedSqrSum() const  {return sum_weights_squared_vals;}

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *);
    //@}
};

NAMESPACE_END

#endif

