//==========================================================================
//   PLATDEP.H - header for
//                             OMNeT++
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

ENVIR_API bool opp_loadlibrary(const char *libname);

// TBD try and collect all similar platform dependencies here
#ifdef __WIN32__
#define ftime _ftime
#define timeb _timeb
#endif

// (t-t0) in milliseconds (t>=t0 is assumed; overflows only after 49.7 days).
unsigned long opp_difftimebmillis(const struct timeb& t, const struct timeb& t0);
struct timeb operator+(const struct timeb& a, const struct timeb& b); // FIXME to better place! (utils?)
struct timeb operator-(const struct timeb& a, const struct timeb& b);

#endif

