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
class XYArray
{
    private:
        double *x;
        double *y;
        size_t len;
    public:
        XYArray(double *xv, double *yv, size_t l) {x = xv; y = yv; len = l;}
        ~XYArray() {delete [] x; delete [] y;}
        int length() const  {return len;}
        double getX(int i) const  {return (i>=0 && i<len) ? x[i] : 0;}
        double getY(int i) const  {return (i>=0 && i<len) ? y[i] : 0;}
};

#endif


