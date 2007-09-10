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
        long *evec; // event numbers
    public:
        XYArray(int l, double *xv, double *yv, BigDecimal *xpv = NULL, long *ev = NULL) {len = l; x = xv; y = yv; xp = xpv; evec = ev;}
        ~XYArray() {delete [] x; delete [] y; delete [] xp; delete [] evec;}
        int length() const  {return len;}
        double getX(int i) const  {return (i>=0 && i<len) ? x[i] : 0;}
        double getY(int i) const  {return (i>=0 && i<len) ? y[i] : 0;}
        BigDecimal getPreciseX(int i) const {return ((xp != NULL && i>=0 && i < len) ? xp[i] : BigDecimal::Nil);}
        long getEventNumber(int i) const {return ((evec != NULL && i>=0 && i<len) ? evec[i] : -1);}
        void sortByX();
};

#endif


