//==========================================================================
//  CMDDEFS.H - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//  General defines for the CMDENV library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMDDEFS_H
#define __CMDDEFS_H

#include "defs.h"  // for OPP_DLLIMPORT, OPP_DLLEXPORT

// OPP_DLLIMPORT/EXPORT are empty if not needed
#ifdef BUILDING_CMDENV
#  define CMDENV_API  OPP_DLLEXPORT
#else
#  define CMDENV_API  OPP_DLLIMPORT
#endif


#endif
