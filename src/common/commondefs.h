//=========================================================================
//  COMMONDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _COMMONDEFS_H_
#define _COMMONDEFS_H_

#include "platdefs.h"

NAMESPACE_BEGIN

#if defined(COMMON_EXPORT)
#  define COMMON_API OPP_DLLEXPORT
#elif defined(COMMON_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define COMMON_API OPP_DLLIMPORT
#else
#  define COMMON_API
#endif

NAMESPACE_END


#endif


