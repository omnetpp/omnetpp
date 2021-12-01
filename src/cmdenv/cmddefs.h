//==========================================================================
//  CMDDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the CMDENV library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CMDENV_CMDDEFS_H
#define __OMNETPP_CMDENV_CMDDEFS_H

#include "omnetpp/platdep/platdefs.h"

namespace omnetpp {
namespace cmdenv {

#if defined(CMDENV_EXPORT)
#  define CMDENV_API OPP_DLLEXPORT
#elif defined(CMDENV_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define CMDENV_API OPP_DLLIMPORT
#else
#  define CMDENV_API
#endif

}  // namespace cmdenv
}  // namespace omnetpp


#endif
