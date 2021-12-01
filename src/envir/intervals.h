//==========================================================================
//  INTERVALS.H - part of
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

#ifndef __OMNETPP_ENVIR_INTERVALS_H
#define __OMNETPP_ENVIR_INTERVALS_H

#include "omnetpp/simkerneldefs.h"
#include "omnetpp/simtime_t.h"
#include "envirdefs.h"

namespace omnetpp {
namespace envir {

/**
 * A list of simulation time intervals represented as an array of (starttime, stoptime)
 * pairs terminated with (0,0), or nullptr
 */
class ENVIR_API Intervals
{
    public:
        struct Interval {
            simtime_t startTime;
            simtime_t stopTime;  // 0 if unspecified
            Interval() { startTime = stopTime = 0; }
        };

    private:
        Interval *intervals;

    public:
        Intervals();
        ~Intervals();
        void parse(const char *text);
        bool contains(simtime_t t) const;
        bool empty() const;
};

}  // namespace envir
}  // namespace omnetpp

#endif
