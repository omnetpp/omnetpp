//==========================================================================
//  ENVDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Envir library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENVDEFS_H
#define __ENVDEFS_H

#include "platdefs.h"

#if defined(ENVIR_EXPORT)
#  define ENVIR_API OPP_DLLEXPORT
#elif defined(ENVIR_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define ENVIR_API OPP_DLLIMPORT
#else
#  define ENVIR_API
#endif


#endif
