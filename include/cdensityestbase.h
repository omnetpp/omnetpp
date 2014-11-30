//==========================================================================
//  CDENSITYESTBASE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cDensityEstBase : common base class for density estimation classes
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDENSITYESTBASE_H
#define __CDENSITYESTBASE_H

#include "cstddev.h"

NAMESPACE_BEGIN

/**
 * Common base class for density estimation classes. Provides several
 * pure virtual functions, so it is an abstract class, no instances
 * can be created.
 *
 * For the histogram classes, you need to specify the number of cells
 * and the range. Range can either be set explicitly or you can choose
 * automatic range determination.
 *
 * Automatic range estimation works in the following way:
 * <OL>
 *   <LI>The first <I>num_firstvals</I> observations are stored.
 *   <LI>After having collected a given number of observations, the actual
 *   histogram is set up. The range (<I>min</I>, <I>max</I>) of the
 *   initial values is expanded <I>range_ext_factor</I> times, and
 *   the result will become the histogram's range (<I>rangemin</I>,
 *   <I>rangemax</I>). Based on the range, the cells are layed out.
 *   Then the initial values that have been stored up to this point
 *   will be transferred into the new histogram structure and their
 *   store is deleted -- this is done by the transform() function.
 * </OL>
 *
 * You may also explicitly specify the lower or upper limit and have
 * the other end of the range estimated automatically. The setRange...()
 * member functions of cDensityEstBase deal with setting
 * up the histogram range. It also provides pure virtual functions
 * transform() etc.
 *
 * Subsequent observations are placed in the histogram structure.
 * If an observation falls out of the histogram range, the <I>underflow</I>
 * or the <I>overflow</I> <I>cell</I> is incremented.
 *
 * layout of the histogram:
 * <PRE><TT>
 *        underflow-cell   ordinary cells          overflow-cell
 *       ...----------|----|----|----|----|----|----|--------- ...
 *                    |                             |
 *                    |                             |
 *                rangemin                       rangemax
 * </TT></PRE>
 *
 * @ingroup Statistics
 */
class SIM_API cDensityEstBase : public cStdDev
{
  public:
    /**
     * Constants for histogram range_mode
     */
    enum RangeMode {
        RANGE_AUTO,      // automatic range setup, using precollected values and range extension (the default)
        RANGE_AUTOLOWER, // like RANGE_AUTO, but upper limit is fixed
        RANGE_AUTOUPPER, // like RANGE_AUTO, but lower limit is fixed
        RANGE_FIXED,     // fixed range (lower, upper)
        RANGE_NOTSET     // not set, but it is OK (cVarHistogram only)
    };

    /**
     * Information about a cell. This struct is not used internally by
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

  protected:
    double rangemin, rangemax;   // range for distribution density collection
    int num_firstvals;         // number of "pre-collected" observations
                                // before transform() is performed.
    unsigned long cell_under;
    unsigned long cell_over;    // for counting observations that fall out of range

    double range_ext_factor;    // the range of histogram is: [min_vals, max_vals] made
                                // range_ext_factor times larger
    RangeMode range_mode;       // one of RANGE_xxx constants

    bool transfd;
    double *firstvals;         // pointer to array of "pre-collected" observations

  private:
    void copy(const cDensityEstBase& other);

  protected:
    static void plotline (std::ostream& os, const char *pref, double xval, double count, double a);

    // part of merge(); to be implemented in subclasses
    virtual void doMergeCellValues(const cDensityEstBase *other) = 0;
  public:
    // internal, for use in sim_std.msg; note: each call overwrites the previous value!
    const Cell& internalGetCellInfo(int k) const;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cDensityEstBase(const cDensityEstBase& other) : cStdDev(other) {firstvals=NULL;copy(other);}

    /**
     * Constructor.
     */
    explicit cDensityEstBase(const char *name=NULL);

    /**
     * Destructor.
     */
    virtual ~cDensityEstBase();

    /**
     * Assignment operator. The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cDensityEstBase& operator=(const cDensityEstBase& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* No dup() because this is an abstract class. */

    /**
     * Produces a multi-line description of the object's contents.
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

    /** @name Redefined cStatistic member functions. */
    //@{

    /**
     * Collects one value. Before the histogram was transformed, this method
     * simply adds the value to the table of pre-collected values.
     * When the number of pre-collected observations reaches a limit, the transform()
     * method is called. After transformation, it calls collectTransformed()
     * to update the stored statistics with this value.
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
     * object is not a cDensityEstBase.
     */
    virtual void merge(const cStatistic *other);

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult();

    /**
     * Writes the contents of the object into a text file.
     */
    virtual void saveToFile(FILE *) const;

    /**
     * Reads the object data from a file, in the format written out by saveToFile().
     */
    virtual void loadFromFile(FILE *);
    //@}

    /** @name Selecting the method of setting up the histogram range. */
    //@{

    /**
     * Sets the histogram range explicitly to [lower, upper]. When this method is
     * used, setNumFirstVals() is not needed.
     */
    virtual void setRange(double lower, double upper);

    /**
     * Selects a histogram range setup method where the range will be determined
     * entirely from a set of pre-collected values.
     *
     * When called, the histogram range will be determined from the first
     * num_firstvals values, extending their range symmetrically by
     * range_ext_fact. For example, after a call to setRangeAuto(100, 1.3),
     * the histogram will be set up after pre-collecting 100 values, the range
     * being the range of the 100 pre-collected values extended 1.3 times
     * symmetrically.
     */
    virtual void setRangeAuto(int num_firstvals=100, double range_ext_fact=2.0);

    /**
     * Selects a histogram range setup method where the upper bound of the range
     * is fixed and the lower bound is determined from a set of pre-collected values.
     *
     * The lower bound is calculated by extending the range (minimum of observations, upper)
     * range_ext_fact times.
     */
    virtual void setRangeAutoLower(double upper, int num_firstvals=100, double range_ext_fact=2.0);

    /**
     * Selects a histogram range setup method where the lower bound of the range
     * is fixed and the upper bound is determined from a set of pre-collected values.
     *
     * The upper bound is calculated by extending the range (lower, maximum of observations)
     * range_ext_fact times.
     */
    virtual void setRangeAutoUpper(double lower, int num_firstvals=100, double range_ext_fact=2.0);

    /**
     * Sets the number of values to be pre-collected before transformation takes
     * place. See transform().
     */
    virtual void setNumFirstVals(int num_firstvals);

    /**
     * Returns the number of values to be pre-collected before transformation
     * takes place. See transform().
     */
    virtual int getNumFirstVals() const {return num_firstvals;}

    /**
     * Returns the range extension factor, used with histogram range setup.
     * See setRangeAuto() and transform().
     */
    virtual double getRangeExtensionFactor() const {return range_ext_factor;}
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

  public:

    /** @name Transformation. */
    //@{

    /**
     * Returns whether the object is transformed. See transform().
     */
    virtual bool isTransformed() const   {return transfd;}

    /**
     * Transforms the table of pre-collected values into an internal
     * histogram structure. This is a pure virtual function. Implementations
     * of transform() are expected to call setupRange(), and set the
     * transfd flag when transformation was successfully done.
     */
    virtual void transform() = 0;
    //@}

    /** @name Accessing histogram cells. */
    //@{

    /**
     * Returns the number of histogram cells used.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual int getNumCells() const = 0;

    /**
     * Returns the kth cell boundary. Legal values for k are 0 through getNumCells(),
     * that is, there' one more basepoint than the number of cells.
     * Basepoint(0) returns the low end of the first cell, and getBasepoint(getNumCells())
     * returns the high end of the last histogram cell.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double getBasepoint(int k) const = 0;

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     * Before transformation, this method may return zero. See transform().
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double getCellValue(int k) const = 0;

    /**
     * Returns the estimated value of the Probability Density Function
     * within the kth cell. This method simply divides the number of observations
     * in cell k with the cell size and the number of total observations collected.
     * Note that before transformation, getCellValue() and also this method may return zero.
     * See transform().
     */
    virtual double getCellPDF(int k) const;

    /**
     * Returns number of observations that, being too small, fell out of the histogram
     * range.
     */
    virtual unsigned long getUnderflowCell() const {return cell_under;}

    /**
     * Returns number of observations that, being too large, fell out of the histogram
     * range.
     */
    virtual unsigned long getOverflowCell() const {return cell_over;}

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
     * This is a pure virtual function, implementation is provided
     * in subclasses implementing concrete histogram types.
     */
    virtual double getPDF(double x) const = 0;

    /**
     * Returns the estimated value of the Cumulated Density Function at a given x.
     * This is a pure virtual function, implementation is provided
     * in subclasses implementing concrete histogram types.
     */
    virtual double getCDF(double x) const = 0;
    //@}
};

NAMESPACE_END


#endif



