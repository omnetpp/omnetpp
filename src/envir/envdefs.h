//==========================================================================
//  ENVDEFS.H - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//  General defines for the Envir library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENVDEFS_H
#define __ENVDEFS_H

#include "defs.h"  // for OPP_DLLIMPORT, OPP_DLLEXPORT

#ifdef _WIN32
#  ifdef BUILDING_ENVIR
#    define ENVIR_API  OPP_DLLEXPORT
#  else
#    define ENVIR_API  OPP_DLLIMPORT
#  endif
#else
#  define ENVIR_API
#endif


#endif
