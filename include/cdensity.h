//==========================================================================
//   CDENSITY.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cDensityEstBase : common base class for density estimation classes
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDENSITY_H
#define __CDENSITY_H

#include "cstat.h"

//==========================================================================

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
 *   <LI>After having collected a given number of samples, the actual
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
    // to range_mode:
    enum { RANGE_INVALID,   // --> needs to be set
           RANGE_FIXED,     // --> fixed range (lower,upper)
           RANGE_AUTO,
           RANGE_AUTOLOWER, // --> will be determined from firstvals[],
           RANGE_AUTOUPPER, //        using min and/or max and range_ext_fact
           RANGE_NOTSET     // --> not set, but it's OK (cVarHistogram only)
         };

  protected:
    double rangemin,rangemax;   // range for distribution density collection
    long num_firstvals;         // number of "pre-collected" samples
                                // before transform() is performed.
    unsigned long cell_under;
    unsigned long cell_over;    // for counting observations that fall out of range

    double range_ext_factor;    // the range of histogram is: [min_samples,max_samples] made
                                // range_ext_factor times larger
    int range_mode;             // one of RANGE_xxx constants

    bool transfd;
    double *firstvals;         // pointer to array of "pre-collected" samples

  protected:
    static void plotline (ostream& os, char* pref, double xval,
                          double count, double a);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cDensityEstBase(const cDensityEstBase& r) : cStdDev(r)
            {setName(r.name());firstvals=NULL;operator=(r);}

    /**
     * Constructor.
     */
    explicit cDensityEstBase(const char *name=NULL);

    /**
     * Destructor.
     */
    virtual ~cDensityEstBase();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cDensityEstBase& operator=(const cDensityEstBase& res);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /* No dup() because this is an abstract class. */

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(ostream& os);

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

    /** @name Redefined cStatistic member functions. */
    //@{

    /**
     * Collects one value. Before the histogram was transformed, this method
     * simply adds the value to the table of pre-collected values.
     * When the number of pre-collected samples reaches a limit, the transform()
     * method is called. After transformation, it calls collectTransformed()
     * to update the stored statistics with this value.
     */
    virtual void collect(double val);

    /**
     * Clears the results collected so far.
     */
    virtual void clearResult();

    /**
     * Generates a random number based on the collected data. Uses the random number
     * generator set by setGenK().
     *
     * This is a pure virtual function; it must be redefined in subclasses.
     */
    virtual double random() const = 0;  // FIXME: redundant!!!

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
    virtual void setRangeAuto(int num_firstvals, double range_ext_fact);

    /**
     * Selects a histogram range setup method where the upper bound of the range
     * is fixed and the lower bound is determined from a set of pre-collected values.
     *
     * FIXME: details!
     */
    virtual void setRangeAutoLower(double upper, int num_firstvals, double range_ext_fact);

    /**
     * Selects a histogram range setup method where the lower bound of the range
     * is fixed and the upper bound is determined from a set of pre-collected values.
     *
     * FIXME: details!
     */
    virtual void setRangeAutoUpper(double lower, int num_firstvals, double range_ext_fact);

    /**
     * Sets the number of values to be pre-collected before transformation takes
     * place. See transform().
     */
    virtual void setNumFirstVals(int num_firstvals);
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
    virtual void collectTransformed(double val) = 0;

  public:

    /** @name Transformation. */
    //@{

    /**
     * Returns whether the object is transformed. See transform().
     */
    virtual bool transformed() const   {return transfd;}

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
    virtual int cells() const = 0;

    /**
     * Returns the kth cell boundary. Legal values for k are 0 through cells(),
     * that is, there' one more basepoint than the number of cells.
     * Basepoint(0) returns the low end of the first cell, and basepoint(cells())
     * returns the high end of the last histogram cell.
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double basepoint(int k) const = 0;

    /**
     * Returns the number of observations that fell into the kth histogram cell.
     * Before transformation, this method may return zero. See transform().
     * This method is pure virtual, implementation is provided in subclasses.
     */
    virtual double cell(int k) const = 0;

    /**
     * Returns the estimated value of the Probability Density Function
     * within the kth cell. This method simply divides the number of observations
     * in cell k with the cell size and the number of total observations collected.
     * Note that before transformation, cell() and also this method may return zero.
     * See transform().
     */
    virtual double cellPDF(int k) const;

    /**
     * Returns number of observations that, being too small, fell out of the histogram
     * range.
     */
    virtual unsigned long underflowCell() const {return cell_under;}

    /**
     * Returns number of observations that, being too large, fell out of the histogram
     * range.
     */
    virtual unsigned long overflowCell() const {return cell_over;}
    //@}

    /** @name Density and cumulated density approximation functions. */
    //@{

    /**
     * Returns the estimated value of the Probability Density Function at a given x.
     * This is a pure virtual function, implementation is provided
     * in subclasses implementing concrete histogram types.
     */
    virtual double pdf(double x) const = 0;

    /**
     * Returns the estimated value of the Cumulated Density Function at a given x.
     * This is a pure virtual function, implementation is provided
     * in subclasses implementing concrete histogram types.
     */
    virtual double cdf(double x) const = 0;
    //@}
};

#endif



