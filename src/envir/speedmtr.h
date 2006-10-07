//==========================================================================
//  SPEEDMTR.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Implementation of
//    Speedometer -- utility class to measure simulation speed
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __SPEEDMTR_H
#define __SPEEDMTR_H

#include "defs.h"
#include "envdefs.h"
#include "platdep/time.h"


//
// Speedometer: utility class to measure simulation speed
//
class ENVIR_API Speedometer
{
  private:
    bool started;
    long events;
    simtime_t current_simtime;
    simtime_t intvstart_simtime;
    timeval intvstart_walltime;
    double last_eventspersec;
    double last_eventspersimsec;
    double last_simsecpersec;
  public:
    Speedometer();
    void start(simtime_t t);
    void addEvent(simtime_t t);
    unsigned long millisecsInThisInterval();
    void beginNewInterval();
    double eventsPerSec();
    double eventsPerSimSec();
    double simSecPerSec();
};

#endif

