//==========================================================================
//  SIMKERNELDEFS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Defines of global interest
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __SIMKERNELDEFS_H
#define __SIMKERNELDEFS_H

#include <stddef.h>     // size_t
#include <math.h>       // HUGE_VAL

// OMNeT++ version -- must match NEDC_VERSION and MSGC_VERSION in nedxml!
#define OMNETPP_VERSION 0x0400

//=== Windows DLL IMPORT/EXPORT stuff
#ifndef __WIN32__
#  if defined(_WIN32) || defined(WIN32)
#    define __WIN32__
#  endif
#endif

// OPP_DLLIMPORT/EXPORT are empty if not needed
#if defined(__WIN32__)
#  define OPP_DLLEXPORT  __declspec(dllexport)
#  if defined(WIN32_DLL)
#    define OPP_DLLIMPORT  __declspec(dllimport)
#  else
#    define OPP_DLLIMPORT
#  endif
#else
#  define OPP_DLLIMPORT
#  define OPP_DLLEXPORT
#endif

// SIM_API, ENVIR_API etc are also empty if not needed
#ifdef BUILDING_SIM
#  define SIM_API  OPP_DLLEXPORT
#else
#  define SIM_API  OPP_DLLIMPORT
#endif

// we need this because cenvir.h is in our directory
#ifdef BUILDING_ENVIR
#  define ENVIR_API  OPP_DLLEXPORT
#else
#  define ENVIR_API  OPP_DLLIMPORT
#endif


// NULL
#ifndef NULL
#define NULL ((void*)0)
#endif


// maximum lengths for className(), fullPath() and old info(buf) strings
#define MAX_CLASSNAME       100
#define MAX_OBJECTFULLPATH  1024
#define MAX_OBJECTINFO      500

// in case someone still needs the old name
#define FULLPATHBUF_SIZE  MAX_OBJECTFULLPATH

// check VC++ version (6.0 is no longer accepted)
#ifdef _MSC_VER
#if _MSC_VER<1300
#error "OMNEST/OMNeT++ cannot be compiled with Visual C++ 6.0 or earlier, at least version 7.0 required"
#endif
#endif

#ifdef _MSC_VER
#pragma warning(disable:4800)  // disable "forcing int to bool"
#pragma warning(disable:4996)  // disable VC8.0 warnings on Unix syscalls
#endif

//=== other common defines

#ifndef PI
#define PI        3.141592653589793
#endif

#ifndef Min
#define Min(a,b)     ( (a)<(b) ? (a) : (b) )
#define Max(a,b)     ( (a)>(b) ? (a) : (b) )
#endif

#define sgn(x)       ((x)==0 ? 0 : ((x)<0 ? -1 : 1))

// gcc 2.9x.x had broken exception handling
#ifdef __GNUC__
#  if  __GNUC__<3
#    error gcc 3.x or later required -- please upgrade
#  endif
#endif

#if defined __GNUC__ && __GNUC__>=4
#define _MAYBEUNUSED   __attribute((__unused__))
#define _OPPDEPRECATED __attribute((__deprecated__))
#else
#define _MAYBEUNUSED
#define _OPPDEPRECATED
#endif

#ifndef NDEBUG
#define ASSERT(expr)  \
  ((void) ((expr) ? 0 : \
           (opp_error("ASSERT: condition %s false, %s line %d", \
                             #expr, __FILE__, __LINE__), 0)))
#else
#define ASSERT(expr)  ((void)0)
#endif


/**
 * Model time.
 */
//XXX #define SIMTIME_DOUBLE
#ifdef SIMTIME_DOUBLE

typedef double   simtime_t;
#define MAXTIME  HUGE_VAL
#define SIMTIME_STR(t) simtimeToStr(t)
#define SIMTIME_DBL(t) (t)
#define SIMTIME_RAW(t) (t)

#else

class SimTime;
#include "simtime.h"
typedef SimTime  simtime_t;
#define MAXTIME  SimTime::maxTime()
#define SIMTIME_STR(t) ((t).str().c_str())
#define SIMTIME_DBL(t) ((t).dbl())
#define SIMTIME_RAW(t) ((t).raw())

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

