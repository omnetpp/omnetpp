//=========================================================================
//  COMMONDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_COMMONDEFS_H
#define __OMNETPP_COMMON_COMMONDEFS_H

#include "omnetpp/platdep/platdefs.h"

namespace omnetpp {
namespace common {

#if defined(COMMON_EXPORT)
#  define COMMON_API OPP_DLLEXPORT
#elif defined(COMMON_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define COMMON_API OPP_DLLIMPORT
#else
#  define COMMON_API
#endif

}  // namespace common
}  // namespace omnetpp


#endif


