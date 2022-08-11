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

namespace omnetpp {

// OMNeT++ version, (major<<8)|minor
#define OMNETPP_VERSION  0x0600

// OMNeT++ build number
#define OMNETPP_BUILDNUM   1532

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
 * @ingroup Misc
 */
typedef int64_t eventnumber_t;

/**
 * @brief Message ID. See cMessage::getId().
 *
 * @ingroup Misc
 */
typedef int64_t msgid_t;

/**
 * @brief Transmission ID. See SendOptions::transmissionId(), cMessage::getTransmissionId().
 *
 * @ingroup Misc
 */
typedef int64_t txid_t;

/**
 * @brief Signed integer type which is guaranteed to be at least 64 bits wide.
 * It is used throughout the library as a "large enough" general integer type,
 * for example in NED parameters, during expression evaluation, and as signal value.
 *
 * @ingroup Misc
 */
typedef int64_t intval_t;

/**
 * @brief Unsigned integer type which is guaranteed to be at least 64 bits wide.
 * It is used throughout the library as a "large enough" general unsigned integer type.
 *
 * @ingroup Misc
 */
typedef uint64_t uintval_t;

/**
 * DEPRECATED. Use intval_t instead.
 *
 * @ingroup Misc
 */
[[deprecated]] typedef intval_t intpar_t;

}  // namespace

#endif

