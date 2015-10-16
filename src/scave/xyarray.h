//=========================================================================
//  XYARRAY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_XYARRAY_H
#define __OMNETPP_SCAVE_XYARRAY_H

#include "scavedefs.h"

namespace omnetpp {
namespace scave {

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
        eventnumber_t *evec; // event numbers
    public:
        XYArray(int l, double *xv, double *yv, BigDecimal *xpv = nullptr, eventnumber_t *ev = nullptr) {len = l; x = xv; y = yv; xp = xpv; evec = ev;}
        ~XYArray() {delete [] x; delete [] y; delete [] xp; delete [] evec;}
        int length() const  {return len;}
        double getX(int i) const  {return (i>=0 && i<len) ? x[i] : 0;}
        double getY(int i) const  {return (i>=0 && i<len) ? y[i] : 0;}
        BigDecimal getPreciseX(int i) const {return ((xp != nullptr && i>=0 && i < len) ? xp[i] : BigDecimal::Nil);}
        eventnumber_t getEventNumber(int i) const {return ((evec != nullptr && i>=0 && i<len) ? evec[i] : -1);}
        void sortByX();
};

} // namespace scave
}  // namespace omnetpp


#endif


