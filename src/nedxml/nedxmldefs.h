//==========================================================================
//  NEDXMLDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Envir library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDXML_NEDXMLDEFS_H
#define __OMNETPP_NEDXML_NEDXMLDEFS_H

#include "omnetpp/platdep/platdefs.h"

NAMESPACE_BEGIN
namespace nedxml {

#if defined(NEDXML_EXPORT)
#  define NEDXML_API OPP_DLLEXPORT
#elif defined(NEDXML_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define NEDXML_API OPP_DLLIMPORT
#else
#  define NEDXML_API
#endif

} // namespace nedxml
NAMESPACE_END


#endif


