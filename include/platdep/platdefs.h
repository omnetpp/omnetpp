//==========================================================================
//  PLATDEFS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Compiler and platform related checks and defines
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PLATDEFS_H
#define __PLATDEFS_H

#include <stddef.h>

#ifndef _WIN32
#  if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#    define _WIN32
#  endif
#endif

// macros needed for building Windows DLLs
#if defined(_WIN32)
#  define OPP_DLLEXPORT  __declspec(dllexport)
#  define OPP_DLLIMPORT  __declspec(dllimport)
#else
#  define OPP_DLLIMPORT
#  define OPP_DLLEXPORT
#endif

// check VC++ version (9.0 is no longer accepted)
#ifdef _MSC_VER
#if _MSC_VER<1600
#error "OMNEST/OMNeT++ cannot be compiled with Visual C++ 2008 or earlier, at least version 2010 required"
#endif
#endif

#ifdef _MSC_VER
#pragma warning(disable:4800)  // disable "forcing int to bool"
#endif

// gcc 2.9x.x had broken exception handling
#ifdef __GNUC__
#  if  __GNUC__<3
#    error gcc 3.x or later required -- please upgrade
#  endif
#endif

#if defined __GNUC__ && __GNUC__>=4
#define _OPPDEPRECATED __attribute((__deprecated__))
#elif defined _MSC_VER
#define _OPPDEPRECATED __declspec(deprecated)
#else
#define _OPPDEPRECATED
#endif

#ifdef USE_NAMESPACE
#  define NAMESPACE_BEGIN  namespace omnetpp {
#  define NAMESPACE_END    };
#  define USING_NAMESPACE  using namespace omnetpp;
#  define OPP              omnetpp
#  define OPP_PREFIX       "omnetpp::"
#else
#  define NAMESPACE_BEGIN
#  define NAMESPACE_END
#  define USING_NAMESPACE
#  define OPP
#  define OPP_PREFIX
#endif

#endif

