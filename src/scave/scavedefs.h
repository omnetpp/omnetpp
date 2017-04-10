//=========================================================================
//  SCAVEDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SCAVEDEFS_H
#define __OMNETPP_SCAVE_SCAVEDEFS_H

#include <cstdint> // for int64_t, our equivalent of Java's "long" type
#include "common/bigdecimal.h"
#include "common/commonutil.h"
#include "omnetpp/platdep/platdefs.h"

namespace omnetpp {
namespace scave {

#if defined(SCAVE_EXPORT)
#  define SCAVE_API OPP_DLLEXPORT
#elif defined(SCAVE_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define SCAVE_API OPP_DLLIMPORT
#else
#  define SCAVE_API
#endif

#define DEFAULT_RESULT_PRECISION  14

using omnetpp::common::BigDecimal;
using omnetpp::common::opp_runtime_error;
using omnetpp::common::NaN;
using omnetpp::common::POSITIVE_INFINITY;
using omnetpp::common::NEGATIVE_INFINITY;

// simulation time
typedef BigDecimal simultime_t;
#define SIMTIME_MIN BigDecimal::NegativeInfinity
#define SIMTIME_MAX BigDecimal::PositiveInfinity
typedef int64_t eventnumber_t;


} // namespace scave
}  // namespace omnetpp


#endif


