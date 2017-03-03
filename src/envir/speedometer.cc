//==========================================================================
//  SPEEDOMETER.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cassert>
#include "speedometer.h"

namespace omnetpp {
namespace envir {

Speedometer::Speedometer()
{
    started = false;
}

void Speedometer::start(simtime_t t)
{
    // begin 1st interval
    numEvents = 0;
    gettimeofday(&intervalStartWalltime, nullptr);
    intervalStartSimtime = t;

    lastIntervalEventsPerSec = 0;
    lastIntervalEventsPerSimsec = 0;
    lastIntervalSimsecPerSec = 0;

    started = true;
}

void Speedometer::addEvent(simtime_t t)
{
    // start() mush have been called already
    assert(started);

    numEvents++;
    currentSimtime = t;
}

unsigned long Speedometer::getMillisSinceIntervalStart()
{
    // start() must have been called already
    assert(started);

    timeval now;
    gettimeofday(&now, nullptr);
    return timeval_msec(now - intervalStartWalltime);
}

long Speedometer::getNumEventsSinceIntervalStart()
{
    return numEvents;
}

void Speedometer::beginNewInterval()
{
    // start() must have been called already
    assert(started);

    timeval now;
    gettimeofday(&now, nullptr);
    unsigned long elapsedMillis = timeval_msec(now - intervalStartWalltime);
    if (elapsedMillis < 10 || numEvents == 0) {
        // if we're called too often, refuse to give readings as probably
        // they'd be very misleading
        lastIntervalEventsPerSec = 0;
        lastIntervalSimsecPerSec = 0;
        lastIntervalEventsPerSimsec = 0;
    }
    else {
        double elapsedSecs = (double)elapsedMillis/1000.0;
        simtime_t elapsedSimsecs = currentSimtime-intervalStartSimtime;

        lastIntervalEventsPerSec = numEvents / elapsedSecs;
        lastIntervalSimsecPerSec = SIMTIME_DBL(elapsedSimsecs) / elapsedSecs;
        lastIntervalEventsPerSimsec = (elapsedSimsecs == SIMTIME_ZERO) ? 0 : (numEvents / SIMTIME_DBL(elapsedSimsecs));
    }
    numEvents = 0;
    intervalStartWalltime = now;
    intervalStartSimtime = currentSimtime;
}

double Speedometer::getEventsPerSec()
{
    if (!started)
        return 0;
    return lastIntervalEventsPerSec;
}

double Speedometer::getEventsPerSimSec()
{
    if (!started)
        return 0;
    return lastIntervalEventsPerSimsec;
}

double Speedometer::getSimSecPerSec()
{
    if (!started)
        return 0;
    return lastIntervalSimsecPerSec;
}

}  // namespace envir
}  // namespace omnetpp

