//==========================================================================
//  SIMTIME_T.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SIMTIME_T_H
#define __OMNETPP_SIMTIME_T_H

#include <stdlib.h>
#include <string>
#include <math.h>
#include <float.h>
#include "simkerneldefs.h"
#include "simtime.h"

#ifdef USE_DOUBLE_SIMTIME
#error USE_DOUBLE_SIMTIME is no longer supported -- use OMNeT++ 4.x if you need it
#endif

/**
 * simtime_t: simulation time
 */
typedef OPP::SimTime   simtime_t;

typedef const simtime_t& simtime_t_cref;  // in many configurations (e.g. debug builds), it is more efficient to return a SimTime from a function as a const ref than by value
typedef const double   const_simtime_t;

#define MAXTIME        OPP::SimTime::getMaxTime()
#define SIMTIME_ZERO   OPP::SimTime::ZERO
#define SIMTIME_STR(t) ((t).str().c_str())
#define SIMTIME_DBL(t) ((t).dbl())
#define SIMTIME_RAW(t) ((t).raw())
#define STR_SIMTIME(s) OPP::SimTime::parse(s)
#define SIMTIME_TTOA(buf,t) ((t).str(buf))

#endif

