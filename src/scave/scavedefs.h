//=========================================================================
//  SCAVEDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SCAVEDEFS_H
#define __OMNETPP_SCAVE_SCAVEDEFS_H

#include <cstdint>
#include <string>
#include <map>
#include <set>
#include <vector>
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

typedef std::vector<std::string> StringVector;
typedef std::set<std::string> StringSet;
typedef std::map<std::string, std::string> StringMap;
typedef std::vector<std::pair<std::string, std::string>> OrderedKeyValueList;

#define DEFAULT_RESULT_PRECISION  14

using omnetpp::common::BigDecimal;
using omnetpp::common::opp_runtime_error;
using omnetpp::common::NaN;
using omnetpp::common::POSITIVE_INFINITY;
using omnetpp::common::NEGATIVE_INFINITY;

// simulation time
typedef BigDecimal simultime_t;
typedef int64_t eventnumber_t;

}  // namespace scave
}  // namespace omnetpp


#endif


