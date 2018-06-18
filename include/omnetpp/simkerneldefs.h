//==========================================================================
//  SIMKERNELDEFS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SIMKERNELDEFS_H
#define __OMNETPP_SIMKERNELDEFS_H

#include <cstddef>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include "platdep/platdefs.h"

// OMNeT++ version, (major<<8)|minor
#define OMNETPP_VERSION  0x0504

// OMNeT++ build number
#define OMNETPP_BUILDNUM   1020

// uncomment to switch fingerprint computation algorithm to OMNeT++ 4.x compatible mode
//#define USE_OMNETPP4x_FINGERPRINTS

#if defined(SIM_EXPORT)
#  define SIM_API OPP_DLLEXPORT
#elif defined(SIM_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define SIM_API OPP_DLLIMPORT
#else
#  define SIM_API
#endif

//
// other common defines
//

#ifndef PI
#define PI        3.141592653589793
#endif

#ifndef NDEBUG
/**
 * @brief The \opp version of C's assert() macro. If expr evaluates to false, an exception
 * will be thrown with file/line/function information.
 */
#define ASSERT(expr) \
  ((void) ((expr) ? 0 : (throw omnetpp::cRuntimeError("ASSERT: Condition '%s' does not hold in function '%s' at %s:%d", \
                                   #expr, __FUNCTION__, __FILE__, __LINE__), 0)))

/**
 * @brief The \opp version of C's assert() macro. If expr evaluates to false, an exception
 * will be thrown with file/line/function information and the given text.
 */
#define ASSERT2(expr,text) \
  ((void) ((expr) ? 0 : (throw omnetpp::cRuntimeError("ASSERT: %s in function '%s' at %s:%d", \
                                   text, __FUNCTION__, __FILE__, __LINE__), 0)))
#else
#define ASSERT(expr)        ((void)0)
#define ASSERT2(expr,text)  ((void)0)
#endif


/**
 * @brief Sequence number of events during the simulation. Events are numbered from one.
 * (Event number zero is reserved for network setup and initialization.)
 *
 * @ingroup SimSupport
 */
typedef int64_t eventnumber_t;

/**
 * @brief Type for NED parameter values that store integers. It is guaranteed
 * to be signed and at least as wide as "long".
 *
 * @ingroup SimSupport
 */
typedef int64_t intpar_t;

#endif

