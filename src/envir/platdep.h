//==========================================================================
//  PLATDEP.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PLATDEP_H
#define __PLATDEP_H

#include <sys/types.h>
#include <sys/timeb.h>  // ftime(), timeb

// load library. platform-specific extension (.so or .dll) is added by this function.
ENVIR_API bool opp_loadlibrary(const char *libname);

#ifdef __WIN32__
#define ftime _ftime
#define timeb _timeb
#endif

#ifdef __APPLE__
int ftime(struct timeb *tp);
#endif

// (t-t0) in milliseconds (t>=t0 is assumed; overflows only after 49.7 days).
unsigned long opp_difftimebmillis(const struct timeb& t, const struct timeb& t0);
struct timeb operator+(const struct timeb& a, const struct timeb& b);
struct timeb operator-(const struct timeb& a, const struct timeb& b);

#endif

