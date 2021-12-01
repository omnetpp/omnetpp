//==========================================================================
//  ENVIRDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Envir library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_ENVIRDEFS_H
#define __OMNETPP_ENVIR_ENVIRDEFS_H

#include "omnetpp/platdep/platdefs.h"
#include "common/exception.h"

namespace omnetpp {
namespace envir {

#if defined(ENVIR_EXPORT)
#  define ENVIR_API OPP_DLLEXPORT
#elif defined(ENVIR_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define ENVIR_API OPP_DLLIMPORT
#else
#  define ENVIR_API
#endif

using omnetpp::common::opp_runtime_error;

}  // namespace envir
}  // namespace omnetpp

#endif
