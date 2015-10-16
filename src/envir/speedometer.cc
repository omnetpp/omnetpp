//==========================================================================
//  SPEEDOMETER.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

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
    // start() mush have been called already
    assert(started);

    timeval now;
    gettimeofday(&now, nullptr);
    return timeval_msec(now - intervalStartWalltime);
}

void Speedometer::beginNewInterval()
{
    // start() mush have been called already
    assert(started);

    timeval now;
    gettimeofday(&now, nullptr);
    unsigned long elapsed_msecs = timeval_msec(now - intervalStartWalltime);
    if (elapsed_msecs < 10 || numEvents == 0) {
        // if we're called too often, refuse to give readings as probably
        // they'd be very misleading
        lastIntervalEventsPerSec = 0;
        lastIntervalSimsecPerSec = 0;
        lastIntervalEventsPerSimsec = 0;
    }
    else {
        double elapsed_sec = (double)elapsed_msecs/1000.0;
        simtime_t elapsed_simsec = currentSimtime-intervalStartSimtime;

        lastIntervalEventsPerSec = numEvents / elapsed_sec;
        lastIntervalSimsecPerSec = SIMTIME_DBL(elapsed_simsec) / elapsed_sec;
        lastIntervalEventsPerSimsec = (elapsed_simsec == SIMTIME_ZERO) ? 0 : (numEvents / SIMTIME_DBL(elapsed_simsec));
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

