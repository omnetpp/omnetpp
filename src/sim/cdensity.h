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
  Copyright (C) 1992,99 Andras Varga
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

class cDensityEstBase : public cStdDev
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
    static void plotline (ostream& os,  char* pref, double xval,
           double count, double a);

  public:
    cDensityEstBase(cDensityEstBase& r) : cStdDev(r)
            {setName(r.name());firstvals=NULL;operator=(r);}
    explicit cDensityEstBase(char *s=NULL);    // name of collected data
    virtual ~cDensityEstBase();

    // redefined functions
    virtual char *className()  {return "cDensityEstBase";}
    cDensityEstBase& operator=(cDensityEstBase& res);
    virtual void writeContents(ostream& os);
    virtual char *inspectorFactoryName() {return "cDensityEstBaseIFC";}
    virtual int netPack();
    virtual int netUnpack();
    // dup() and inspector(..) not needed

    // redefined cStat functions & new ones
    virtual void clearResult();

    virtual void setRange(double lower, double upper);
    virtual void setRangeAuto(int num_firstvals, double range_ext_fact);
    virtual void setRangeAutoLower(double upper, int num_firstvals, double range_ext_fact);
    virtual void setRangeAutoUpper(double lower, int num_firstvals, double range_ext_fact);
    virtual void setNumFirstVals(int num_firstvals);

    virtual void collect(double val);
  protected:
    virtual void setupRange();
    virtual void collectTransformed(double val) = 0; // must maintain underflow/overflow cells
  public:
    virtual bool transformed()   {return transfd;}
    virtual void transform() = 0; // must set transfd; probably needs to call setupRange()

    virtual int cells() = 0;
    virtual double basepoint(int k) = 0;
    virtual double cell(int k) = 0;
    virtual double cellPDF(int k);

    virtual double random() = 0;

    virtual double pdf(double x) = 0;
    virtual double cdf(double x) = 0;

    virtual unsigned long underflowCell() {return cell_under;}
    virtual unsigned long overflowCell() {return cell_over;}

    virtual void saveToFile(FILE *);
    virtual void loadFromFile(FILE *);
};

#endif
