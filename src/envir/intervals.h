//==========================================================================
//  INTERVALS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __INTERVALS_H
#define __INTERVALS_H

#include "simkerneldefs.h"
#include "envirdefs.h"
#include "simtime_t.h"

NAMESPACE_BEGIN

/**
 * A list of simulation time intervals represented as an array of (starttime, stoptime)
 * pairs terminated with (0,0), or NULL
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
        bool contains(simtime_t t);
};

NAMESPACE_END

#endif
