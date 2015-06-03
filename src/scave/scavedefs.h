//=========================================================================
//  SCAVEDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVEDEFS_H
#define __OMNETPP_SCAVEDEFS_H

#include <stdint.h> // for int64_t, our equivalent of Java's "long" type (note: <cstdint> is C++11)
#include "common/bigdecimal.h"
#include "common/commonutil.h"
#include "omnetpp/platdep/platdefs.h"

NAMESPACE_BEGIN
namespace scave {

#if defined(SCAVE_EXPORT)
#  define SCAVE_API OPP_DLLEXPORT
#elif defined(SCAVE_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define SCAVE_API OPP_DLLIMPORT
#else
#  define SCAVE_API
#endif

#define DEFAULT_PRECISION  14

using OPP::common::BigDecimal;
using OPP::common::opp_runtime_error;
using OPP::common::NaN;
using OPP::common::POSITIVE_INFINITY;
using OPP::common::NEGATIVE_INFINITY;

// simulation time
typedef BigDecimal simultime_t;
#define SIMTIME_MIN BigDecimal::NegativeInfinity
#define SIMTIME_MAX BigDecimal::PositiveInfinity
typedef int64_t eventnumber_t;


} // namespace scave
NAMESPACE_END


#endif


