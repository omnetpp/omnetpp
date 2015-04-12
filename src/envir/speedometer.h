//==========================================================================
//  SPEEDOMETER.H - part of
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


#ifndef __OMNETPP_SPEEDOMETER_H
#define __OMNETPP_SPEEDOMETER_H

#include "omnetpp/simkerneldefs.h"
#include "omnetpp/simtime_t.h"
#include "omnetpp/platdep/timeutil.h"
#include "envirdefs.h"

NAMESPACE_BEGIN


/**
 * Speedometer: utility class to measure simulation speed
 */
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
    void beginNewInterval();

    unsigned long getMillisSinceIntervalStart();

    double getEventsPerSec();
    double getEventsPerSimSec();
    double getSimSecPerSec();
};

NAMESPACE_END

#endif

