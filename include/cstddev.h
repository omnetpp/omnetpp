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
  Copyright (C) 1992-2005 Andras Varga

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

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cStdDev(const cStdDev& r) : cStatistic() {setName(r.name());operator=(r);}

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
     * Produces a one-line description of object contents.
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
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Redefined cStatistic functions. */
    //@{

    /**
     * Collects one value.
     */
    virtual void collect(double value);

#ifndef USE_DOUBLE_SIMTIME
    /**
     * Convenience method, delegates to collect(double).
     */
    virtual void collect(simtime_t value) {collect(value.dbl());}
#endif

    /**
     * Returns the number of observations collected.
     */
    virtual long count() const  {return num_vals;}

    /**
     * Returns the sum of the values.
     */
    virtual double sum() const  {return sum_vals;}

    /**
     * Returns the squared sum of the collected values.
     */
    virtual double sqrSum() const  {return sqrsum_vals;}

    /**
     * Returns the minimum of the collected values.
     */
    virtual double min() const  {return min_vals;}

    /**
     * Returns the maximum of the collected values.
     */
    virtual double max() const  {return max_vals;}

    /**
     * Returns the mean of the observations.
     * Returns 0.0 if nothing was collected yet.
     */
    virtual double mean() const  {return num_vals==0 ? 0 : sum_vals/num_vals;}

    /**
     * Returns the standard deviation of the observations.
     */
    virtual double stddev() const;

    /**
     * Returns the variance of the observations collected.
     */
    virtual double variance() const;

    /**
     * Since this is unweighted statistics, the sum of weights is count().
     */
    virtual double weights() const  {return count();}

    /**
     * Since this is unweighted statistics, the sum of weight*value products
     * is sum().
     */
    virtual double weightedSum() const  {return sum();}

    /**
     * Since this is unweighted statistics, the sum of squared weights
     * is count().
     */
    virtual double sqrSumWeights() const  {return count();}

    /**
     * Since this is unweighted statistics, the sum of weight*value*value
     * products is sqrSum().
     */
    virtual double weightedSqrSum() const  {return sqrSum();}

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
    cWeightedStdDev(const cWeightedStdDev& r) : cStdDev() {setName(r.name());operator=(r);}

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
     * Produces a one-line description of object contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Redefined cStatistic functions. */
    //@{

    /**
     * Collects one value.
     */
    virtual void collect(double value)  {collect2(value,1.0);}

#ifndef USE_DOUBLE_SIMTIME
    /**
     * Convenience method, delegates to collect(double).
     */
    virtual void collect(simtime_t value) {collect(value.dbl());}
#endif

    /**
     * Collects one value with a given weight.
     */
    virtual void collect2(double value, double weight);

#ifndef USE_DOUBLE_SIMTIME
    /**
     * Convenience method, delegates to collect2(double, double).
     */
    virtual void collect2(simtime_t value, double weight) {collect2(value.dbl(), weight);}

    /**
     * Convenience method, delegates to collect2(double, double).
     */
    virtual void collect2(double value, simtime_t weight) {collect2(value, weight.dbl());}

    /**
     * Convenience method, delegates to collect2(double, double).
     */
    virtual void collect2(simtime_t value, simtime_t weight) {collect2(value.dbl(), weight.dbl());}
#endif

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult();

    /**
     * Returns the mean of the observations collected.
     * Returns 0.0 if nothing was collected yet.
     */
    virtual double mean() const  {return sum_weights==0 ? 0 : sum_weighted_vals / sum_weights;}

    /**
     * Returns the variance of the observations collected.
     */
    virtual double variance() const;

    /**
     * Returns the sum of weights.
     */
    virtual double weights() const  {return sum_weights;}

    /**
     * Returns the sum of weight*value products.
     */
    virtual double weightedSum() const  {return sum_weighted_vals;}

    /**
     * Returns the sum of squared weights.
     */
    virtual double sqrSumWeights() const  {return sum_squared_weights;}

    /**
     * Returns the sum of weight*value*value products.
     */
    virtual double weightedSqrSum() const  {return sum_weights_squared_vals;}

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

