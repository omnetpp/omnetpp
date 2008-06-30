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

#ifndef __SIMTIME_T_H
#define __SIMTIME_T_H

#include <stdlib.h>
#include <string>
#include <math.h>
#include <float.h>
#include "simkerneldefs.h"
#include "simtime.h"

//
// simtime_t: simulation time.
//
// There are two sets of definitions. By default, OMNeT++ 4.x uses
// an int64-based simtime_t (class SimTime), but it also supports
// "double" which was in use in earlier versions (2.x and 3.x).
// This "legacy mode" can be enabled at compile-time by #defining
// USE_DOUBLE_SIMTIME. One use of legacy mode is simulation fingerprint
// verification across 3.x and 4.x.
//
// The macros SIMTIME_STR(), SIMTIME_DBL(), SIMTIME_RAW(), STR_SIMTIME()
// perform conversions to double and to/from string; they should only be
// used in models that should be able to compile with USE_DOUBLE_SIMTIME
// as well. Otherwise, methods of the SimTime class can be directly
// invoked.
//
// It is expected that support for legacy mode (double) will be dropped
// in some future version, together with the SIMTIME_STR(), etc macros.
//
#ifdef USE_DOUBLE_SIMTIME

// "Legacy" module: double simtime_t
typedef double         simtime_t;
typedef const double   const_simtime_t;
#define MAXTIME        DBL_MAX
#define SIMTIME_ZERO   0.0
#define SIMTIME_STR(t) double_to_str(t).c_str()
#define SIMTIME_DBL(t) (t)
#define SIMTIME_RAW(t) (t)
#define STR_SIMTIME(s) strToSimtime(s)
#define SIMTIME_TTOA(buf,t) gcvt(t,16,buf)

#else

// OMNeT++ 4.x native mode: int64-based fixed-point simtime_t (class SimTime)

typedef OPP::SimTime   simtime_t;
typedef const double   const_simtime_t;
#define MAXTIME        OPP::SimTime::getMaxTime()
#define SIMTIME_ZERO   OPP::SimTime()
#define SIMTIME_STR(t) ((t).str().c_str())
#define SIMTIME_DBL(t) ((t).dbl())
#define SIMTIME_RAW(t) ((t).raw())
#define STR_SIMTIME(s) OPP::SimTime::parse(s)
#define SIMTIME_TTOA(buf,t) ((t).str(buf))

#endif  //USE_DOUBLE_SIMTIME

#endif

