//==========================================================================
//   DEFS.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Defines of global interest
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __DEFS_H
#define __DEFS_H

#include <stddef.h>   // size_t
#include <math.h>     // HUGE_VAL


//=== Windows DLL IMPORT/EXPORT stuff
#ifndef __WIN32__
#  if defined(_WIN32) || defined(WIN32)
#    define __WIN32__
#  endif
#endif

#ifdef __WIN32__
#  if !defined(STATIC_LIBRARY) && !defined(__GNUC__)  /*Cygwin b20.1 doesn't like dllexport stuff*/
#    define OPP_DLLIMPORT  __declspec(dllimport)
#    define OPP_DLLEXPORT  __declspec(dllexport)
#  else
#    define OPP_DLLIMPORT
#    define OPP_DLLEXPORT
#  endif
#else
#  define OPP_DLLIMPORT
#  define OPP_DLLEXPORT
#endif


// OPP_DLLIMPORT/EXPORT are empty if non-Windows, non-dll, etc.
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


//=== NULL
#ifndef NULL
#define NULL ((void*)0)
#endif

#define NO(cXX)   ((cXX *)NULL)

//=== other common defines

#ifndef TRUE
#define TRUE      true
#define FALSE     false
#endif

//// old compilers which don't know bool are no longer supported
// #if (defined(__BORLANDC__) && (__BORLANDC__<=0x410)) || (defined(__WATCOMC__) && (__WATCOMC__<1060))
// typedef int bool;
// #define true   1
// #define false  0
// #endif

#ifndef PI
#define PI        3.141592653589793
#endif

#ifndef Min
#define Min(a,b)     ( (a)<(b) ? (a) : (b) )
#define Max(a,b)     ( (a)>(b) ? (a) : (b) )
#endif

#define sgn(x)       ((x)==0 ? 0 : ((x)<0 ? -1 : 1))

#ifndef NDEBUG
#define ASSERT(expr)  \
  ((void) ((expr) ? 0 : \
           (opp_error("ASSERT: condition %s false, %s line %d", \
                             #expr, __FILE__, __LINE__), 0)))
#else
#define ASSERT(expr)  ((void)0)
#endif

#define CONST_CAST(x)  (char *)(x)

//=== model time
typedef double       simtime_t;
#define MAXTIME      HUGE_VAL

//=== backwards compatibility defines
#define isA()          className()
// cQueue:
#define insertHead(a)  insert(a)
#define peekTail()     tail()
#define peekHead()     head()
#define getTail()      pop()


//=== memory mgmt functions for void* pointers
//    (used by cLinkedList and cPar)
typedef void (*VoidDelFunc)(void *);
typedef void *(*VoidDupFunc)(void *);

//=== used by cPar expressions
typedef double (*MathFunc)(...);
typedef double (*MathFuncNoArg)();
typedef double (*MathFunc1Arg)(double);
typedef double (*MathFunc2Args)(double,double);
typedef double (*MathFunc3Args)(double,double,double);

#endif
