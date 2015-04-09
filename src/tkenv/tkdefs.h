//==========================================================================
//  TKDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Tkenv library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __TKDEFS_H
#define __TKDEFS_H

#include "omnetpp/platdep/platdefs.h"
#include "omnetpp/platdep/platmisc.h"   // must precede tk.h otherwise Visual Studio 2013 fails to compile
#include "omnetpp/platdep/timeutil.h"   // must precede <tk.h>, due to collision with <windows.h>
#include <tk.h>                         // included here for all tkenv, to avoid troubles noted above

#if defined(TKENV_EXPORT)
#  define TKENV_API OPP_DLLEXPORT
#elif defined(TKENV_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define TKENV_API OPP_DLLIMPORT
#else
#  define TKENV_API
#endif

#endif
