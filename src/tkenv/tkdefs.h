//==========================================================================
//  TKDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Tkenv library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __TKDEFS_H
#define __TKDEFS_H

#include "platdefs.h"

NAMESPACE_BEGIN

#if defined(TKENV_EXPORT)
#  define TKENV_API OPP_DLLEXPORT
#elif defined(TKENV_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define TKENV_API OPP_DLLIMPORT
#else
#  define TKENV_API
#endif

NAMESPACE_END


#endif
