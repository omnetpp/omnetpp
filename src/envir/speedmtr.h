//==========================================================================
//  SPEEDMTR.H - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//  Implementation of
//    Speedometer -- utility class to measure simulation speed
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __SPEEDMTR_H
#define __SPEEDMTR_H

#include <time.h>
#include "defs.h"
#include "envdefs.h"


//
// Speedometer: utility class to measure simulation speed
//
class ENVIR_API Speedometer
{
    bool started;
    long events;
    simtime_t current_simtime;
    simtime_t intvstart_simtime;
    clock_t intvstart_clock;
    double last_eventspersec;
    double last_eventspersimsec;
    double last_simsecpersec;

  public:
    Speedometer();
    void addEvent(simtime_t t);
    double secondsInThisInterval();
    void beginNewInterval();
    double eventsPerSec();
    double eventsPerSimSec();
    double simSecPerSec();
};

#endif

