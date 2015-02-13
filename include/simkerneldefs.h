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
#include <stdint.h>
#include "platdep/platdefs.h"

// OMNeT++ version -- must match NEDC_VERSION and MSGC_VERSION in nedxml
#define OMNETPP_VERSION 0x0500

// temporarily use 4.x fingerprints -- TODO remove before first 5.0 alpha release!
//#define USE_OMNETPP4x_FINGERPRINTS

#if defined(SIM_EXPORT)
#  define SIM_API OPP_DLLEXPORT
#elif defined(SIM_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define SIM_API OPP_DLLIMPORT
#else
#  define SIM_API
#endif

// NULL
#ifndef NULL
#define NULL 0
#endif

//
// other common defines
//

#ifndef PI
#define PI        3.141592653589793
#endif

#ifndef NDEBUG
/**
 * The \opp version of C's assert() macro. If expr evaluates to false, an exception
 * will be thrown with file/line/function information.
 */
#define ASSERT(expr) \
  ((void) ((expr) ? 0 : (opp_error("ASSERT: condition %s false in function %s, %s line %d", \
                                   #expr, __FUNCTION__, __FILE__, __LINE__), 0)))

/**
 * The \opp version of C's assert() macro. If expr evaluates to false, an exception
 * will be thrown with file/line/function information and the given text.
 */
#define ASSERT2(expr,text) \
  ((void) ((expr) ? 0 : (opp_error("ASSERT: %s in function %s, %s line %d", \
                                   text, __FUNCTION__, __FILE__, __LINE__), 0)))
#else
#define ASSERT(expr)        ((void)0)
#define ASSERT2(expr,text)  ((void)0)
#endif


/**
 * Sequence number of events during the simulation. Events are numbered from one.
 * (Event number zero is reserved for network setup and initialization.)
 *
 * @ingroup EnumsTypes
 */
typedef int64_t eventnumber_t;

/**
 * Prototype for functions that are called by some objects (e.g. cMsgPar)
 * to free up user-defined data structures.
 * @ingroup EnumsTypes
 */
typedef void (*VoidDelFunc)(void *);

/**
 * Prototype for functions that are called by some objects (e.g. cMsgPar)
 * to duplicate user-defined data structures.
 * @ingroup EnumsTypes
 */
typedef void *(*VoidDupFunc)(void *);


#endif

