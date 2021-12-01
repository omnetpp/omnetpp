//==========================================================================
//  SPEEDOMETER.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __OMNETPP_ENVIR_SPEEDOMETER_H
#define __OMNETPP_ENVIR_SPEEDOMETER_H

#include "omnetpp/simkerneldefs.h"
#include "omnetpp/simtime_t.h"
#include "envirdefs.h"

namespace omnetpp {
namespace envir {


/**
 * Speedometer: utility class to measure simulation speed
 */
class ENVIR_API Speedometer
{
  private:
    bool started = false;
    long numEvents;
    simtime_t currentSimtime;
    simtime_t intervalStartSimtime;
    int64_t intervalStartWalltime; // in microseconds, as returned by opp_get_monotonic_clock_usecs()
    double lastIntervalEventsPerSec;
    double lastIntervalEventsPerSimsec;
    double lastIntervalSimsecPerSec;

  public:
    Speedometer() {}

    void start(simtime_t t);
    void addEvent(simtime_t t);
    void beginNewInterval();

    unsigned long getMillisSinceIntervalStart();
    long getNumEventsSinceIntervalStart();

    // return readings from the last interval, or 0 if no interval has been completed yet
    double getEventsPerSec();
    double getEventsPerSimSec();
    double getSimSecPerSec();
};

}  // namespace envir
}  // namespace omnetpp

#endif

