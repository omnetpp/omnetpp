//=========================================================================
//  SCAVEDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _SCAVEDEFS_H_
#define _SCAVEDEFS_H_

#include "inttypes.h" // for int64, our equivalent of Java's "long" type
#include "platdefs.h"
#include "bigdecimal.h"

NAMESPACE_BEGIN

#if defined(SCAVE_EXPORT)
#  define SCAVE_API OPP_DLLEXPORT
#elif defined(SCAVE_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define SCAVE_API OPP_DLLIMPORT
#else
#  define SCAVE_API
#endif

#define DEFAULT_PRECISION  14

// simulation time
typedef BigDecimal simultime_t;
#define SIMTIME_MIN BigDecimal::NegativeInfinity
#define SIMTIME_MAX BigDecimal::PositiveInfinity

/**
 * Used by VectorResult as well as filter nodes e.g. Integrate
 */
enum InterpolationMode { UNSPECIFIED, NONE, SAMPLE_HOLD, BACKWARD_SAMPLE_HOLD, LINEAR };


NAMESPACE_END


#endif


