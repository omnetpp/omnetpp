//==========================================================================
//  SPEEDOMETER.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include "speedometer.h"

USING_NAMESPACE


Speedometer::Speedometer()
{
    started = false;
}

void Speedometer::start(simtime_t t)
{
    // begin 1st interval
    events = 0;
    gettimeofday(&intvstart_walltime, NULL);
    intvstart_simtime = t;

    last_eventspersec = 0;
    last_eventspersimsec = 0;
    last_simsecpersec = 0;

    started = true;
}

void Speedometer::addEvent(simtime_t t)
{
    // start() mush have been called already
    assert(started);

    events++;
    current_simtime = t;
}

unsigned long Speedometer::getMillisSinceIntervalStart()
{
    // start() mush have been called already
    assert(started);

    timeval now;
    gettimeofday(&now, NULL);
    return timeval_msec(now - intvstart_walltime);
}

void Speedometer::beginNewInterval()
{
    // start() mush have been called already
    assert(started);

    timeval now;
    gettimeofday(&now, NULL);
    unsigned long elapsed_msecs = timeval_msec(now - intvstart_walltime);
    if (elapsed_msecs<10 || events==0)
    {
        // if we're called too often, refuse to give readings as probably
        // they'd be very misleading
        last_eventspersec = 0;
        last_simsecpersec = 0;
        last_eventspersimsec = 0;
    }
    else
    {
        double elapsed_sec = (double)elapsed_msecs/1000.0;
        simtime_t elapsed_simsec = current_simtime-intvstart_simtime;

        last_eventspersec = events / elapsed_sec;
        last_simsecpersec = SIMTIME_DBL(elapsed_simsec) / elapsed_sec;
        last_eventspersimsec = (elapsed_simsec==0) ? 0 : (events / SIMTIME_DBL(elapsed_simsec));
    }
    events = 0;
    intvstart_walltime = now;
    intvstart_simtime = current_simtime;
}

double Speedometer::getEventsPerSec()
{
    if (!started) return 0;
    return last_eventspersec;
}

double Speedometer::getEventsPerSimSec()
{
    if (!started) return 0;
    return last_eventspersimsec;
}

double Speedometer::getSimSecPerSec()
{
    if (!started) return 0;
    return last_simsecpersec;
}

