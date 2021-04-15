//==========================================================================
//  CSTATISTIC.H - part of
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

#ifndef __OMNETPP_CSTATISTIC_H
#define __OMNETPP_CSTATISTIC_H

#include "crandom.h"
#include "simtime.h"
#include "opp_string.h"

namespace omnetpp {

/**
 * @brief cStatistic is an abstract class for computing statistical properties
 * of a random variable.
 *
 * This class inherits from cRandom, because instances are able to generate
 * random variates from the estimated distribution.
 *
 * @ingroup Statistics
 */
class SIM_API cStatistic : public cRandom
{
  private:
    void copy(const cStatistic& other);

  protected:
    // internal: utility function for implementing loadFromFile() functions
    _OPP_GNU_ATTRIBUTE(format(scanf, 3, 4))
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
    explicit cStatistic(const char *name=nullptr);

    /**
     * Destructor.
     */
    virtual ~cStatistic() {}

    /**
     * Assignment operator. It is present since descendants may refer to it.
     * The name member is not copied; see cNamedObject::operator=() for details.
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
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
    //@}

    /** @name Collecting values. */
    //@{
    /**
     * Returns true if this object collects weighted statistics.
     */
    virtual bool isWeighted() const = 0;

    /**
     * Collects one value.
     */
    virtual void collect(double value) = 0;

    /**
     * Convenience method, delegates to collect(double).
     */
    virtual void collect(SimTime value) {collect(value.dbl());}

    /**
     * Collects one value with a given weight.
     */
    virtual void collectWeighted(double value, double weight);

    /**
     * Convenience method, delegates to collectWeighted(double, double).
     */
    virtual void collectWeighted(SimTime value, double weight) {collectWeighted(value.dbl(), weight);}

    /**
     * Convenience method, delegates to collectWeighted(double, double).
     */
    virtual void collectWeighted(double value, SimTime weight) {collectWeighted(value, weight.dbl());}

    /**
     * Convenience method, delegates to collectWeighted(double, double).
     */
    virtual void collectWeighted(SimTime value, SimTime weight) {collectWeighted(value.dbl(), weight.dbl());}

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
    virtual void clear() = 0;
    //@}

    /** @name Statistics of collected data. */
    //@{

    /**
     * Returns the number of the observations, regardless of their weights.
     */
    virtual int64_t getCount() const = 0;

    /**
     * Returns the sum of the values. Not available for weighted statistics.
     *
     * @see getWeightedSum()
     */
    virtual double getSum() const = 0;

    /**
     * Returns the sum of the squared values. Not available for weighted statistics.
     *
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
    virtual double getSumWeights() const = 0;

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
     * histogram, the method may invoke the setUpBins() on the histogram
     * object, to force it set up histogram bins before recording.
     */
    virtual void record()  {recordAs(nullptr, nullptr);}

    /**
     * Records the statistics into the scalar result file, with the given
     * unit (e.g. "s", "m/s", etc). The name of the recorded statistic
     * will be the object name (see getName()).
     *
     * Note that this operation may have side effect: if this object is a
     * histogram, the method may invoke the setUpBins() on the histogram
     * object, to force it set up histogram bins before recording.
     */
    virtual void recordWithUnit(const char *unit)  {recordAs(nullptr, unit);}

    /**
     * Records the statistics into the scalar result file, with the given name,
     * and optionally, the given unit (e.g. "s", "m/s", etc).
     *
     * Note that this operation may have side effect: if this object is a
     * histogram, the method may invoke the setUpBins() on the histogram
     * object, to force it set up histogram bins before recording.
     */
    virtual void recordAs(const char *name, const char *unit=nullptr);
    //@}
};

}  // namespace omnetpp

#endif

