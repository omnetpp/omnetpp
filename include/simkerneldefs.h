//==========================================================================
//  SIMKERNELDEFS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Defines of for the simulation kernel
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __SIMKERNELDEFS_H
#define __SIMKERNELDEFS_H

#include <stddef.h>
#include <float.h>
#include <math.h>
#include "platdep/platdefs.h"
#include "platdep/inttypes.h"

// OMNeT++ version -- must match NEDC_VERSION and MSGC_VERSION in nedxml!
//XXX move to platdefs.h? and remove NEDC_VERSION
#define OMNETPP_VERSION 0x0400

#if defined(SIM_EXPORT)
#  define SIM_API OPP_DLLEXPORT
#elif defined(SIM_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define SIM_API OPP_DLLIMPORT
#else
#  define SIM_API
#endif

// NULL
#ifndef NULL
#define NULL ((void*)0)
#endif

// maximum lengths for getClassName() and getFullPath() strings
#define MAX_CLASSNAME       100        //FIXME remove!!!
#define MAX_OBJECTFULLPATH  1024

//
// other common defines
//

#ifndef PI
#define PI        3.141592653589793
#endif

#ifndef Min
#define Min(a,b)     ( (a)<(b) ? (a) : (b) )
#define Max(a,b)     ( (a)>(b) ? (a) : (b) )
#endif

#define sgn(x)       ((x)==0 ? 0 : ((x)<0 ? -1 : 1))


#ifndef NDEBUG
#define ASSERT(expr) \
  ((void) ((expr) ? 0 : (opp_error("ASSERT: condition %s false in function %s, %s line %d", \
                                   #expr, __FUNCTION__, __FILE__, __LINE__), 0)))
#define ASSERT2(expr,text) \
  ((void) ((expr) ? 0 : (opp_error("ASSERT: %s in function %s, %s line %d", \
                                   text, __FUNCTION__, __FILE__, __LINE__), 0)))
#else
#define ASSERT(expr)        ((void)0)
#define ASSERT2(expr,text)  ((void)0)
#endif


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

#include <stdlib.h>
#include <string>

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
NAMESPACE_BEGIN
class SimTime;
NAMESPACE_END
#include "simtime.h"

typedef OPP::SimTime   simtime_t;
typedef const double   const_simtime_t;
#define MAXTIME        OPP::SimTime::getMaxTime()
#define SIMTIME_ZERO   OPP::SimTime()
#define SIMTIME_STR(t) ((t).str().c_str())
#define SIMTIME_DBL(t) ((t).dbl())
#define SIMTIME_RAW(t) ((t).raw())
#define STR_SIMTIME(s) OPP::SimTime::parse(s)
#define SIMTIME_TTOA(buf,t) ((t).str(buf))

#endif


/**
 * Prototype for functions that are called by some objects (cMsgPar, cLinkedList)
 * to free up user-defined data structures.
 * @ingroup EnumsTypes
 */
typedef void (*VoidDelFunc)(void *);

/**
 * Prototype for functions that are called by some objects (cMsgPar, cLinkedList)
 * to duplicate user-defined data structures.
 * @ingroup EnumsTypes
 */
typedef void *(*VoidDupFunc)(void *);


#endif

