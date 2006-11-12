//==========================================================================
//  DEFS.H - part of
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

#ifndef __DEFS_H
#define __DEFS_H

#include <stddef.h>   // size_t
#include <math.h>     // HUGE_VAL

// OMNeT++ version -- must match NEDC_VERSION in nedxml source, and opp_msgc version!
#define OMNETPP_VERSION 0x0304

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


//=== NULL
#ifndef NULL
#define NULL ((void*)0)
#endif


// maximum lengths for className(), fullPath() and old info(buf) strings
#define MAX_CLASSNAME       100
#define MAX_OBJECTFULLPATH  1024
#define MAX_OBJECTINFO      500

// in case someone still needs the old name
#define FULLPATHBUF_SIZE  MAX_OBJECTFULLPATH

// disable VC8.0 warnings on Unix syscalls
#ifdef _MSC_VER
#pragma warning(disable:4996)
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
#    error gcc 3.x required -- please upgrade
#  endif
#endif

#ifdef __GNUC__
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
 * Modelled time.
 */
typedef double       simtime_t;

#define MAXTIME      HUGE_VAL


//
// memory mgmt functions for void* pointers  (used by cLinkedList and cPar)
//

/**
 * Prototype for functions that are called by some objects (cPar, cLinkedList)
 * to free up user-defined data structures.
 * @ingroup EnumsTypes
 */
typedef void (*VoidDelFunc)(void *);

/**
 * Prototype for functions that are called by some objects (cPar, cLinkedList)
 * to duplicate user-defined data structures.
 * @ingroup EnumsTypes
 */
typedef void *(*VoidDupFunc)(void *);

//
// used by cPar expressions
//

/**
 * Prototype for mathematical functions that can be used in reverse
 * Polish expressions (see ExprElem).
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc)(...);

/**
 * Prototype for mathematical functions taking no arguments
 * that can be used in reverse Polish expressions (see ExprElem).
 * @ingroup EnumsTypes
 */
typedef double (*MathFuncNoArg)();

/**
 * Prototype for mathematical functions taking one argument
 * that can be used in reverse Polish expressions (see ExprElem).
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc1Arg)(double);

/**
 * Prototype for mathematical functions taking two arguments
 * that can be used in reverse Polish expressions (see ExprElem).
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc2Args)(double,double);

/**
 * Prototype for mathematical functions taking three arguments
 * that can be used in reverse Polish expressions (see ExprElem).
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc3Args)(double,double,double);

/**
 * Prototype for mathematical functions taking four arguments
 * that can be used in reverse Polish expressions (see ExprElem).
 * @ingroup EnumsTypes
 */
typedef double (*MathFunc4Args)(double,double,double,double);

#endif

