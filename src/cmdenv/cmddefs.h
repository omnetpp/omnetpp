//==========================================================================
//  CMDDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the CMDENV library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMDDEFS_H
#define __CMDDEFS_H

#include "platdefs.h"

NAMESPACE_BEGIN

#if defined(CMDENV_EXPORT)
#  define CMDENV_API OPP_DLLEXPORT
#elif defined(CMDENV_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define CMDENV_API OPP_DLLIMPORT
#else
#  define CMDENV_API
#endif


NAMESPACE_END


#endif
