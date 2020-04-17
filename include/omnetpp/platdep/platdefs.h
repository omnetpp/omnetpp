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

#ifdef _MSC_VER
#pragma warning(disable:4800)  // disable "forcing int to bool"
#endif

#if defined __GNUC__
#  define _OPP_GNU_ATTRIBUTE(x)  __attribute__(x)
#else
#  define _OPP_GNU_ATTRIBUTE(x)
#endif

// attributes on enumerators were added in C++17, but some compilers (GCC >= 6 for example) accept them in c++14 mode as well
#if defined(__cpp_enumerator_attributes) && (__cpp_enumerator_attributes >= 201411)
#  define OPP_DEPRECATED_ENUMERATOR(message) [[deprecated(message)]]
#else
#  define OPP_DEPRECATED_ENUMERATOR(message)
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

