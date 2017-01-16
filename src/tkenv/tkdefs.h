//==========================================================================
//  TKDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Tkenv library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_TKENV_TKDEFS_H
#define __OMNETPP_TKENV_TKDEFS_H

#include <tk.h>                         // included here for all tkenv, to avoid troubles noted above
#include "omnetpp/platdep/platdefs.h"
#include "omnetpp/platdep/platmisc.h"   // must precede tk.h otherwise Visual Studio 2013 fails to compile

#if defined(TKENV_EXPORT)
#  define TKENV_API OPP_DLLEXPORT
#elif defined(TKENV_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define TKENV_API OPP_DLLIMPORT
#else
#  define TKENV_API
#endif

#define TCL_NULL  (void*)nullptr  // workaround: gcc 4.8.2 on a 64-bit architecture pushed a 32-bit 0 on the stack as last arg for Tcl_VarEval(), causing it to crash

#endif
