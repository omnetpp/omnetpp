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
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "speedmtr.h"


Speedometer::Speedometer()
{
    started = false;
}

void Speedometer::addEvent(simtime_t t)
{
    if (!started)
    {
        // begin 1st interval
        events = 0;
        intvstart_clock = clock();
        intvstart_simtime = t;

        last_eventspersec = 0;
        last_eventspersimsec = 0;
        last_simsecpersec = 0;

        started = true;
    }

    events++;
    current_simtime = t;
}

double Speedometer::secondsInThisInterval()
{
    long elapsed_clocks = clock() - intvstart_clock;
    return (double)elapsed_clocks / CLOCKS_PER_SEC;
}

void Speedometer::beginNewInterval()
{
    clock_t now = clock();
    long elapsed_clocks = now - intvstart_clock;
    if (elapsed_clocks==0) elapsed_clocks=1; // avoid division by zero
    double elapsed_sec = (double)elapsed_clocks / CLOCKS_PER_SEC;
    simtime_t elapsed_simsec = current_simtime-intvstart_simtime;

    last_eventspersec = events / elapsed_sec;  // elapsed_sec is never zero here
    last_simsecpersec = elapsed_simsec / elapsed_sec;
    last_eventspersimsec = (elapsed_simsec==0) ? 0 : (events / elapsed_simsec);

    events = 0;
    intvstart_clock = now;
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

