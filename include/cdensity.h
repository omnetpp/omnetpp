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
// cDensityEstBase -  common base class for density estimation classes
//
// Contains:
//    * contains ways to define range/number of cells etc
//    * does precollection and range setup for histogram-type estimators
//    * contains underflow/overflow counters
//    * declares pure virtual functions for:
//         getting the cells of historgram-type estimators
//         calculating PDF/CDF
//         generating random number from the distribution
//
// layout of the histogram:
//
//        underflow-cell   ordinary cells          overflow-cell
//       ...----------|----|----|----|----|----|----|--------- ...
//                    |                             |
//                    |                             |
//                rangemin                       rangemax
//


/**
 * FIXME: 
 * cDensityEstBase -  common base class for density estimation classes
 * 
 * Contains:
 *    * contains ways to define range/number of cells etc
 *    * does precollection and range setup for histogram-type estimators
 *    * contains underflow/overflow counters
 *    * declares pure virtual functions for:
 *         getting the cells of historgram-type estimators
 *         calculating PDF/CDF
 *         generating random number from the distribution
 * 
 * layout of the histogram:
 * 
 *        underflow-cell   ordinary cells          overflow-cell
 *       ...----------|----|----|----|----|----|----|--------- ...
 *                    |                             |
 *                    |                             |
 *                rangemin                       rangemax
 * 
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

    /**
     * FIXME: to range_mode:
     * before transform() is performed.
     * range_ext_factor times larger
     */
           double count, double a);

  public:

    /**
     * Constructors, destructor, assignment.
     */
    cDensityEstBase(cDensityEstBase& r) : cStdDev(r)
            {setName(r.name());firstvals=NULL;operator=(r);}

    /**
     * Constructors, destructor, assignment.
     */
    explicit cDensityEstBase(const char *name=NULL);    // name of collected data

    /**
     * Constructors, destructor, assignment.
     */
    virtual ~cDensityEstBase();

    // redefined functions

    /**
     * 
     */
    virtual const char *className() const {return "cDensityEstBase";}

    /**
     * Constructors, destructor, assignment.
     */
    cDensityEstBase& operator=(cDensityEstBase& res);

    /**
     * 
     */
    virtual void writeContents(ostream& os);

    /**
     * MISSINGDOC: cDensityEstBase:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cDensityEstBaseIFC";}

    /**
     * MISSINGDOC: cDensityEstBase:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cDensityEstBase:int netUnpack()
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

