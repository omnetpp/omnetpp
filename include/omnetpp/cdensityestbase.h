//==========================================================================
//  CDENSITYESTBASE.H - part of
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

#ifndef __OMNETPP_CDENSITYESTBASE_H
#define __OMNETPP_CDENSITYESTBASE_H

#include "cstddev.h"

namespace omnetpp {

/**
 * @brief Interface and base class for histogram-like density estimation classes.
 *
 * TODO
 */
class SIM_API cDensityEstBase : public cStdDev
{
  public:
    /**
     * @brief Information about a cell. This struct is not used internally by
     * histogram and histogram-like classes, only to return information
     * to the user.
     */
    struct Cell
    {
        double lower;  // lower cell bound (inclusive)
        double upper;  // lower cell bound (exclusive)
        double value;  // counter (or its estimate)
        double relativeFreq;  // value / total
        Cell() {lower=upper=value=relativeFreq=0;}
    };

  private:
    void copy(const cDensityEstBase& other) {}

  public:
    // internal, for use in sim_std.msg; note: each call overwrites the previous value!
    const Cell& internalGetCellInfo(int k) const;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cDensityEstBase(const cDensityEstBase& other) : cStdDev(other) {}

    /**
     * Constructor.
     */
    explicit cDensityEstBase(const char *name=nullptr, bool weighted=false) : cStdDev(name, weighted) {}

    /**
     * Destructor.
     */
    virtual ~cDensityEstBase() {};

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cDensityEstBase& operator=(const cDensityEstBase& res) {
        if (this == &res)
           return *this;
       cStdDev::operator=(res);
       copy(res);
       return *this;
    }
    //@}


    /** @name Accessing histogram cells. */
    //@{
    /**
     * Returns true if histogram is already available. See transform().
     */
    virtual bool isTransformed() const = 0;

    /**
     * Transforms the array of pre-collected values into histogram structure.
     */
    virtual void transform() = 0;

    /**
     * Returns the number of histogram cells used.
     */
    virtual int getNumCells() const = 0;

    /**
     * Returns the kth histogram cell boundary. Legal values for k are 0 through
     * getNumCells(), that is, there's one more basepoint than the number of cells.
     * getBasepoint(0) returns the lower bound of the first cell, and
     * getBasepoint(getNumCells()) returns the upper bound of the last cell.
     */
    virtual double getBasepoint(int k) const = 0;

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     */
    virtual double getCellValue(int k) const = 0;

    /**
     * Returns the estimated value of the Probability Density Function
     * within the kth cell. This method simply divides the number of observations
     * in cell k with the cell size and the number of total observations collected.
     */
    virtual double getCellPDF(int k) const;

    /**
     * Returns number of observations that were below the histogram range,
     * independent of their weights.
     */
    virtual int64_t getUnderflowCell() const = 0;

    /**
     * Returns number of observations that were above the histogram range,
     * independent of their weights.
     */
    virtual int64_t getOverflowCell() const = 0;

    /**
     * Returns the total weight of the observations that were below the histogram range.
     */
    virtual double getUnderflowSumWeights() const = 0;

    /**
     * Returns the total weight of the observations that were above the histogram range.
     */
    virtual double getOverflowSumWeights() const = 0;

    /**
     * Combines the functionality of getBasepoint(), getCellValue() and getCellPDF() into a
     * single call.
     */
    virtual Cell getCellInfo(int k) const;
    //@}

    /** @name Density and cumulated density approximation functions. */
    //@{

    /**
     * Returns the estimated value of the Probability Density Function at a given x.
     */
    virtual double getPDF(double x) const;

    /**
     * Returns the estimated value of the Cumulative Density Function at a given x.
     */
    virtual double getCDF(double x) const;
    //@}
};


/**
 * @brief Base class for histogram-like density estimation classes.
 *
 * DEPRECATED CLASS, do not use for new classes.
 *
 * For the histogram classes, you need to specify the number of cells
 * and the range. Range can either be set explicitly or you can choose
 * automatic range determination.
 *
 * Automatic range estimation works in the following way:
 *  1. The first <I>numPrecollect</I> observations are stored.
 *  2. After having collected a given number of observations, the actual
 *     histogram is set up. The range (min, max) of the  initial values
 *     is expanded rangeExtensionFactoror times, and the result will become
 *     the histogram's range (rangeMin, rangeMax). Based on the range,
 *     the cells are layed out.
 *  3. Then the initial values that have been stored up to this point
 *     will be transferred into the new histogram structure and their
 *     store is deleted -- this is done by the transform() function.
 *
 * You may also explicitly specify the lower or upper limit and have
 * the other end of the range estimated automatically. The setRange...()
 * member functions of cPrecollectionBasedDensityEst deal with setting
 * up the histogram range. It also provides pure virtual functions
 * transform() etc.
 *
 * Subsequent observations are placed in the histogram structure.
 * If an observation falls out of the histogram range, the underflow
 * or overflow counter is incremented.
 *
 * @ingroup Statistics
 */
class SIM_API cPrecollectionBasedDensityEst : public cDensityEstBase
{
  public:
    /**
     * @brief Constants for histogram range_mode
     */
    enum RangeMode {
        RANGE_AUTO,      // automatic range setup, using precollected values and range extension (the default)
        RANGE_AUTOLOWER, // like RANGE_AUTO, but upper limit is fixed
        RANGE_AUTOUPPER, // like RANGE_AUTO, but lower limit is fixed
        RANGE_FIXED,     // fixed range (lower, upper)
        RANGE_NOTSET     // not set, but it is OK (cVarHistogram only)
    };

  protected:
    // we can precollect observations to automatically determine the range before histogram cells are set up
    bool transformed;   // whether precollected values have been transformed into a histogram
    double *precollectedValues;
    double *precollectedWeights; // nullptr for unweighted
    int numPrecollected;

    // range for distribution density collection
    RangeMode rangeMode;
    double rangeExtFactor;    // we extend the range of precollected values by rangeExtFactor
    double rangeMin, rangeMax;

    // variables for counting out-of-range observations
    int64_t numUnderflows;
    int64_t numOverflows;
    double underflowSumWeights;
    double overflowSumWeights;

  private:
    void copy(const cPrecollectionBasedDensityEst& other);

  protected:
    static void plotline (std::ostream& os, const char *pref, double xval, double count, double a);

    // part of merge(); to be implemented in subclasses
    virtual void doMergeCellValues(const cPrecollectionBasedDensityEst *other) = 0;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cPrecollectionBasedDensityEst(const cPrecollectionBasedDensityEst& other) : cDensityEstBase(other) {precollectedValues=nullptr; precollectedWeights=nullptr; copy(other);}

    /**
     * Constructor.
     */
    explicit cPrecollectionBasedDensityEst(const char *name=nullptr, bool weighted=false);

    /**
     * Destructor.
     */
    virtual ~cPrecollectionBasedDensityEst();

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cPrecollectionBasedDensityEst& operator=(const cPrecollectionBasedDensityEst& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* No dup() because this is an abstract class. */

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

    /** @name Redefined cStatistic member functions. */
    //@{

    /**
     * Collects one value. Before the histogram was transformed, this method
     * simply adds the value to the table of pre-collected values.
     * When the number of pre-collected observations reaches a limit, the transform()
     * method is called. After transformation, it calls collectTransformed()
     * to update the stored statistics with this value.
     */
    virtual void collect(double value) override;
    using cStatistic::collect;

    /**
     * Collects one observation with a given weight. The weight must not be
     * negative. (Zero-weight observations are allowed, but will not affect
     * mean and stddev.)
     */
    virtual void collect2(double value, double weight) override;
    using cStatistic::collect2;

    /**
     * Updates this object with data coming from another statistics
     * object -- as if this object had collected observations fed
     * into the other object as well. Throws an error if the other
     * object is not a cPrecollectionBasedDensityEst.
     */
    virtual void merge(const cStatistic *other) override;

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult() override;

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const override;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *) override;
    //@}

    /** @name Selecting the method of setting up the histogram range. */
    //@{

    /**
     * Sets the histogram range explicitly to [lower, upper]. When this method is
     * used, setNumPrecollectedValues() is not needed.
     */
    virtual void setRange(double lower, double upper);

    /**
     * Selects a histogram range setup method where the range will be determined
     * entirely from a set of pre-collected values.
     *
     * When called, the histogram range will be determined from the first
     * numPrecollect values, extending their range symmetrically by
     * rangeExtensionFactor. For example, after a call to setRangeAuto(100, 1.3),
     * the histogram will be set up after pre-collecting 100 values, the range
     * being the range of the 100 pre-collected values extended 1.3 times
     * symmetrically.
     */
    virtual void setRangeAuto(int numPrecollect=100, double rangeExtensionFactor=2.0);

    /**
     * Selects a histogram range setup method where the upper bound of the range
     * is fixed and the lower bound is determined from a set of pre-collected values.
     *
     * The lower bound is calculated by extending the range (minimum of observations, upper)
     * rangeExtensionFactor times.
     */
    virtual void setRangeAutoLower(double upper, int numPrecollect=100, double rangeExtensionFactor=2.0);

    /**
     * Selects a histogram range setup method where the lower bound of the range
     * is fixed and the upper bound is determined from a set of pre-collected values.
     *
     * The upper bound is calculated by extending the range (lower, maximum of observations)
     * rangeExtensionFactor times.
     */
    virtual void setRangeAutoUpper(double lower, int numPrecollect=100, double rangeExtensionFactor=2.0);

    /**
     * Sets the number of values to be pre-collected before transformation takes
     * place. See transform().
     */
    virtual void setNumPrecollectedValues(int numPrecollect);

    /**
     * Returns the number of values to be pre-collected before transformation
     * takes place. See transform().
     */
    virtual int getNumPrecollectedValues() const {return numPrecollected;}

    /**
     * Returns the range extension factor, used with histogram range setup.
     * See setRangeAuto() and transform().
     */
    virtual double getRangeExtensionFactor() const {return rangeExtFactor;}
    //@}

  protected:
    /**
     * Called internally by transform(), this method should determine and set up
     * the histogram range, based on the pre-collected data and the range setup
     * method selected by calls to the setRange(), setRangeAuto(), setRangeAutoLower(),
     * setRangeAutoUpper() methods.
     */
    virtual void setupRange();

    /**
     * Called internally by collect(), this method collects a value
     * after the histogram has been transformed.
     * Updating the underflow/overflow cells must be handled within this function.
     * This is a pure virtual function; it must be redefined in subclasses.
     */
    virtual void collectTransformed(double value) = 0;

    /**
     * Called internally by collect(), this method collects a value
     * after the histogram has been transformed.
     * Updating the underflow/overflow cells must be handled within this function.
     * This is a pure virtual function; it must be redefined in subclasses.
     */
    virtual void collectTransformed2(double value, double weight) = 0;

  public:
    /** @name Redefined methods */
    //@{
    /**
     * Returns whether the object is transformed. See transform().
     */
    virtual bool isTransformed() const override {return transformed;}

    /**
     * Returns number of observations that were below the histogram range,
     * independent of their weights.
     */
    virtual int64_t getUnderflowCell() const override {return numUnderflows;}

    /**
     * Returns number of observations that were above the histogram range,
     * independent of their weights.
     */
    virtual int64_t getOverflowCell() const override {return numOverflows;}

    /**
     * Returns the total weight of the observations that were below the histogram range.
     */
    virtual double getUnderflowSumWeights() const override {return underflowSumWeights;}

    /**
     * Returns the total weight of the observations that were above the histogram range.
     */
    virtual double getOverflowSumWeights() const override {return overflowSumWeights;}
    //@}
};

}  // namespace omnetpp


#endif



