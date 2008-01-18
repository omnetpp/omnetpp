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

NAMESPACE_BEGIN

#if defined(LAYOUT_EXPORT)
#  define LAYOUT_API OPP_DLLEXPORT
#elif defined(LAYOUT_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define LAYOUT_API OPP_DLLIMPORT
#else
#  define LAYOUT_API
#endif

NAMESPACE_END


#endif


