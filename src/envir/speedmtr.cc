//==========================================================================
//  SPEEDMTR.CC - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//  Implementation of
//    Speedometer
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>

#ifdef __APPLE__
#include <sys/time.h>
static int ftime(struct timeb *tp)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    tp->time = tv.tv_sec;
    tp->millitm = tv.tv_usec / 1000;
    tp->timezone = 0;
    tp->dstflag = 0;
}
#endif

#include "speedmtr.h"


Speedometer::Speedometer()
{
    started = false;
}

void Speedometer::start(simtime_t t)
{
    // begin 1st interval
    events = 0;
    ftime(&intvstart_walltime);
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

unsigned long Speedometer::millisecsInThisInterval()
{
    // start() mush have been called already
    assert(started);

    struct timeb now;
    ftime(&now);
    return opp_difftimebmillis(now, intvstart_walltime);
}

void Speedometer::beginNewInterval()
{
    // start() mush have been called already
    assert(started);

    struct timeb now;
    ftime(&now);
    unsigned long elapsed_msecs = opp_difftimebmillis(now, intvstart_walltime);
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
        last_simsecpersec = elapsed_simsec / elapsed_sec;
        last_eventspersimsec = (elapsed_simsec==0) ? 0 : (events / elapsed_simsec);
    }
    events = 0;
    intvstart_walltime = now;
    intvstart_simtime = current_simtime;
}

double Speedometer::eventsPerSec()
{
    if (!started) return 0;
    return last_eventspersec;
}

double Speedometer::eventsPerSimSec()
{
    if (!started) return 0;
    return last_eventspersimsec;
}

double Speedometer::simSecPerSec()
{
    if (!started) return 0;
    return last_simsecpersec;
}

