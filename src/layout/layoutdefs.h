//==========================================================================
//  LAYOUTDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __LAYOUTDEFS_H
#define __LAYOUTDEFS_H

#include "platdefs.h"

#ifdef BUILDING_LAYOUT
#  define LAYOUT_API  OPP_DLLEXPORT
#else
#  define LAYOUT_API  OPP_DLLIMPORT
#endif

#endif


