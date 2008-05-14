//==========================================================================
//  ENVIRDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Envir library
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENVIRDEFS_H
#define __ENVIRDEFS_H

#include "platdefs.h"

NAMESPACE_BEGIN

#if defined(ENVIR_EXPORT)
#  define ENVIR_API OPP_DLLEXPORT
#elif defined(ENVIR_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define ENVIR_API OPP_DLLIMPORT
#else
#  define ENVIR_API
#endif


NAMESPACE_END


#endif
