//==========================================================================
//  NEDXMLDEFS.H - part of
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

#ifndef __NEDXMLDEFS_H
#define __NEDXMLDEFS_H

#include "platdefs.h"

#if defined(NEDXML_EXPORT)
#  define NEDXML_API OPP_DLLEXPORT
#elif defined(NEDXML_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define NEDXML_API OPP_DLLIMPORT
#else
#  define NEDXML_API
#endif

#endif


