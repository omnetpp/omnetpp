//==========================================================================
//  LAYOUTDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_LAYOUT_LAYOUTDEFS_H
#define __OMNETPP_LAYOUT_LAYOUTDEFS_H

#include "omnetpp/platdep/platdefs.h"
#include "common/commonutil.h"

namespace omnetpp {
namespace layout {

#if defined(LAYOUT_EXPORT)
#  define LAYOUT_API OPP_DLLEXPORT
#elif defined(LAYOUT_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define LAYOUT_API OPP_DLLIMPORT
#else
#  define LAYOUT_API
#endif

using omnetpp::common::opp_runtime_error;
using omnetpp::common::NaN;
using omnetpp::common::isNaN;
using omnetpp::common::POSITIVE_INFINITY;
using omnetpp::common::NEGATIVE_INFINITY;

}  // namespace layout
}  // namespace omnetpp


#endif


