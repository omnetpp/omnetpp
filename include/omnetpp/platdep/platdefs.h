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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_PLATDEFS_H
#define __OMNETPP_PLATDEFS_H

#include <cstddef>

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
#  define _OPPDEPRECATED __attribute((__deprecated__))
#elif defined _MSC_VER
#  define _OPPDEPRECATED __declspec(deprecated)
#else
#  define _OPPDEPRECATED
#endif

// make the code compile with pre-C++11 compilers
#if __cplusplus < 201103L
#  if !defined nullptr
#    define nullptr  0
#  endif
#  define override
#endif

#if defined __GNUC__
#  define _OPP_GNU_ATTRIBUTE(x)  __attribute__(x)
#else
#  define _OPP_GNU_ATTRIBUTE(x)
#endif

// choose coroutine library if unspecified
#if !defined(USE_WIN32_FIBERS) && !defined(USE_POSIX_COROUTINES) && !defined(USE_PORTABLE_COROUTINES)
#  if defined _WIN32
#    define USE_WIN32_FIBERS
#  elif HAVE_SWAPCONTEXT
#    define USE_POSIX_COROUTINES
#  else
#    define USE_PORTABLE_COROUTINES
#  endif
#endif

#endif

