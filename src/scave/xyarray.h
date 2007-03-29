//=========================================================================
//  XYARRAY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _XYARRAY_H_
#define _XYARRAY_H_

#include "scavedefs.h"

/**
 * ArrayBuilderNode returns the results in this class.
 */
class SCAVE_API XYArray
{
    private:
        double *x;
        double *y;
        int len;
        BigDecimal *xp; // precise x value
        int lenp;       // length of the xp array
    public:
        XYArray(double *xv, double *yv, int l) {x = xv; y = yv; len = l; xp = NULL; lenp = 0;}
        XYArray(double *xv, double *yv, int l, BigDecimal *xpv, int lp) {x = xv; y = yv; len = l; xp = xpv; lenp = lp;}
        ~XYArray() {delete [] x; delete [] y;}
        int length() const  {return len;}
        double getX(int i) const  {return (i>=0 && i<len) ? x[i] : 0;}
        double getY(int i) const  {return (i>=0 && i<len) ? y[i] : 0;}
        BigDecimal getPreciseX(int i) const {return ((xp != NULL && i>=0 && i < lenp) ? xp[i] : BigDecimal::Nil);}
};

#endif


