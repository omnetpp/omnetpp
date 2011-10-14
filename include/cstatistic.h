//==========================================================================
//  CSTATISTIC.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cStatistic : base for statistics
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSTATISTIC_H
#define __CSTATISTIC_H

#include <stdio.h>
#include "cownedobject.h"
#include "simtime.h"

NAMESPACE_BEGIN

class cTransientDetection;
class cAccuracyDetection;


/**
 * Base class of different statistic collecting classes.
 * cStatistic is the base class for all statistical data
 * collection classes. cStatistic itself adds no data members
 * or algorithms to cOwnedObject, it only defines virtual functions
 * that will be redefined in descendants. No instance of cStatistic
 * can be created.
 *
 * @ingroup Statistics
 */
class SIM_API cStatistic : public cOwnedObject
{
  public:
    cTransientDetection *td;    // ptr to associated object
    cAccuracyDetection *ra;     // ptr to associated object
    int genk;                   // index of random number generator to use

  private:
    void copy(const cStatistic& other);

  protected:
    // internal: utility function for implementing loadFromFile() functions
    void freadvarsf (FILE *f,  const char *fmt, ...);
    // internal: for collecting the attributes to record into the scalar file
    virtual void getAttributesToRecord(opp_string_map& attributes) {}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cStatistic(const cStatistic& r);

    /**
     * Constructor, creates an object with the given name
     */
    explicit cStatistic(const char *name=NULL);

    /**
     * Destructor.
     */
    virtual ~cStatistic();

    /**
     * Assignment operator. It is present since descendants may refer to it.
     * The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cStatistic& operator=(const cStatistic& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* Note: No dup() because this is an abstract class! */

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

    /** @name Collecting values. */
    //@{

    /**
     * Collects one value.
     */
    virtual void collect(double value) = 0;

    /**
     * Convenience method, delegates to collect(double).
     */
    virtual void collect(SimTime value) {collect(value.dbl());}

    /**
     * Returns true if this object collects weighted statistics.
     */
    virtual bool isWeighted() const = 0;

    /**
     * Collects one value with a given weight.
     */
    virtual void collect2(double value, double weight);

    /**
     * Updates this object with data coming from another statistics
     * object. The result is as if this object had collected all the
     * observations fed into the other object as well.
     */
    virtual void merge(const cStatistic *other) = 0;

    /**
     * This function should be redefined in derived classes to clear
     * the results collected so far.
     */
    virtual void clearResult() = 0;
    //@}

    /** @name Statistics of collected data. */
    //@{

    /**
     * Returns the number of the observations.
     */
    virtual long getCount() const = 0;

    /**
     * Returns the sum of the values.
     * @see getWeightedSum()
     */
    virtual double getSum() const = 0;

    /**
     * Returns the sum of the squared values.
     * @see getWeightedSqrSum()
     */
    virtual double getSqrSum() const = 0;

    /**
     * Returns the minimum of the values.
     */
    virtual double getMin() const = 0;

    /**
     * Returns the maximum of the values.
     */
    virtual double getMax() const = 0;

    /**
     * Returns the mean of the (weighted) observations.
     */
    virtual double getMean() const = 0;

    /**
     * Returns the standard deviation of the (weighted) observations.
     */
    virtual double getStddev() const = 0;

    /**
     * Returns the variance of the (weighted) observations.
     */
    virtual double getVariance() const = 0;
    //@}

    /** @name Weighted statistics. */
    //@{
    /**
     * Returns the sum of weights.
     */
    virtual double getWeights() const = 0;

    /**
     * Returns the sum of weight*value products.
     */
    virtual double getWeightedSum() const = 0;

    /**
     * Returns the sum of squared weights.
     */
    virtual double getSqrSumWeights() const = 0;

    /**
     * Returns the sum of weight*value*value products.
     */
    virtual double getWeightedSqrSum() const = 0;
    //@}

    /** @name Transient and result accuracy detection. */
    //@{

    /**
     * Assigns transient and accuracy detection objects to the statistic
     * object.
     */
    void addTransientDetection(cTransientDetection *object);

    /**
     * Assigns transient and accuracy detection objects to the statistic
     * object.
     */
    void addAccuracyDetection(cAccuracyDetection *object);

    /**
     * Returns the assigned transient and accuracy detection objects.
     */
    cTransientDetection *getTransientDetectionObject() const  {return td;}

    /**
     * Returns the assigned transient and accuracy detection objects.
     */
    cAccuracyDetection  *getAccuracyDetectionObject() const  {return ra;}
    //@}

    /** @name Generating random numbers based on the collected data */
    //@{

    /**
     * Sets the index of the random number generator to use when the
     * object has to generate a random number based on the statistics
     * stored.
     */
    void setGenK(int gen_nr)   {genk=gen_nr;}

    /**
     * Generates a random number based on the collected data. Uses the random number generator set by setGenK().
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double random() const = 0;
    //@}

    /** @name Writing to text file, reading from text file, recording to scalar file. */
    //@{

    /**
     * Writes the contents of the object into a text file.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual void saveToFile(FILE *) const = 0;

    /**
     * Reads the object data from a file written out by saveToFile().
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual void loadFromFile(FILE *) = 0;

    /**
     * Records the statistics into the scalar result file. (Recorded data
     * includes the number of observations, mean, standard deviation, min,
     * max, potential histogram data, etc.)  The name of the recorded
     * statistic will be the object name (see getFullName()).
     *
     * Note that this operation may have side effect: if this object is a
     * histogram, the method may invoke the transform() on the histogram
     * object, to force it set up histogram cells before recording.
     */
    virtual void record()  {recordAs(NULL, NULL);}

    /**
     * Records the statistics into the scalar result file, with the given
     * unit (e.g. "s", "m/s", etc). The name of the recorded statistic
     * will be the object name (see getName()).
     *
     * Note that this operation may have side effect: if this object is a
     * histogram, the method may invoke the transform() on the histogram
     * object, to force it set up histogram cells before recording.
     */
    virtual void recordWithUnit(const char *unit)  {recordAs(NULL, unit);}

    /**
     * Records the statistics into the scalar result file, with the given name,
     * and optionally, the given unit (e.g. "s", "m/s", etc).
     *
     * Note that this operation may have side effect: if this object is a
     * histogram, the method may invoke the transform() on the histogram
     * object, to force it set up histogram cells before recording.
     */
    virtual void recordAs(const char *name, const char *unit=NULL);
    //@}
};

NAMESPACE_END

#endif

