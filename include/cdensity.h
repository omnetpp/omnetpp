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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDENSITY_H
#define __CDENSITY_H

#include <iostream.h>
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
 *   <LI>The first <I>num_firstvals<I> observations are stored.
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
    long num_firstvals;         // number of "precollected" samples
                                // before transform() is performed.
    unsigned long cell_under,
                  cell_over;    // counting samples fall out of range

    double range_ext_factor;    // the range of histogram is: [min_samples,max_samples] made
                                // range_ext_factor times larger
    int range_mode;             // one of RANGE_xxx constants

    bool transfd;
    double *firstvals;         // pointer to array of "precollected" samples

  protected:
    static void plotline (ostream& os, char* pref, double xval,
                          double count, double a);

  public:
    /**
     * Copy constructor.
     */
    cDensityEstBase(cDensityEstBase& r) : cStdDev(r)
            {setName(r.name());firstvals=NULL;operator=(r);}

    /**
     * Constructor.
     */
    explicit cDensityEstBase(const char *name=NULL);    // name of collected data

    /**
     * Destructor.
     */
    virtual ~cDensityEstBase();

    // redefined functions

    /**
     * Returns pointer to a string containing the class name, "cDensityEstBase".
     */
    virtual const char *className() const {return "cDensityEstBase";}

    /**
     * Constructors, destructor, assignment.
     */
    cDensityEstBase& operator=(cDensityEstBase& res);

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(ostream& os);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cDensityEstBaseIFC";}

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
    // dup() and inspector(..) not needed

    // redefined cStat functions & new ones

    /**
     *
     */
    virtual void clearResult();

    /**
     * Range setting.
     */
    virtual void setRange(double lower, double upper);

    /**
     * Range setting.
     */
    virtual void setRangeAuto(int num_firstvals, double range_ext_fact);

    /**
     * Range setting.
     */
    virtual void setRangeAutoLower(double upper, int num_firstvals, double range_ext_fact);

    /**
     * Range setting.
     */
    virtual void setRangeAutoUpper(double lower, int num_firstvals, double range_ext_fact);

    /**
     * Range setting.
     */
    virtual void setNumFirstVals(int num_firstvals);

    /**
     *
     */
    virtual void collect(double val);

  protected:
    /**
     * MISSINGDOC: cDensityEstBase:void setupRange()
     */
    virtual void setupRange();

    /**
     * MISSINGDOC: cDensityEstBase:void collectTransformed(double)
     */
    virtual void collectTransformed(double val) = 0; // must maintain underflow/overflow cells
  public:

    /**
     * First one returns whether the object is transformed or not; second
     * one forces a transformation.
     */
    virtual bool transformed()   {return transfd;}

    /**
     * First one returns whether the object is transformed or not; second
     * one forces a transformation.
     */
    virtual void transform() = 0; // must set transfd; probably needs to call setupRange()

    /**
     * MISSINGDOC: cDensityEstBase:int cells()
     */
    virtual int cells() = 0;

    /**
     * MISSINGDOC: cDensityEstBase:double basepoint(int)
     */
    virtual double basepoint(int k) = 0;

    /**
     * MISSINGDOC: cDensityEstBase:double cell(int)
     */
    virtual double cell(int k) = 0;

    /**
     * MISSINGDOC: cDensityEstBase:double cellPDF(int)
     */
    virtual double cellPDF(int k);

    /**
     * MISSINGDOC: cDensityEstBase:double random()
     */
    virtual double random() = 0;

    /**
     * Density function and cumulated density function at a given <I>x</I>.
     */
    virtual double pdf(double x) = 0;

    /**
     * Density function and cumulated density function at a given <I>x</I>.
     */
    virtual double cdf(double x) = 0;

    /**
     * Returns number of observations that fall out of the histogram
     * range.
     */
    virtual unsigned long underflowCell() {return cell_under;}

    /**
     * Returns number of observations that fall out of the histogram
     * range.
     */
    virtual unsigned long overflowCell() {return cell_over;}

    /**
     *
     */
    virtual void saveToFile(FILE *);

    /**
     *
     */
    virtual void loadFromFile(FILE *);
};

#endif



