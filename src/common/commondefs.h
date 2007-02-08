//=========================================================================
//  COMMONDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _COMMONDEFS_H_
#define _COMMONDEFS_H_

#include "platdefs.h"  // for OPP_DLLIMPORT, OPP_DLLEXPORT

// OPP_DLLIMPORT/EXPORT are empty if non-Windows, non-dll, etc.
#ifdef BUILDING_COMMON
#  define COMMON_API  OPP_DLLEXPORT
#else
#  define COMMON_API  OPP_DLLIMPORT
#endif

#endif


