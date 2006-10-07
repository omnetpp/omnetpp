//==========================================================================
//  PLATDEP/LOADLIB.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PLATDEP_LOADLIB_H
#define __PLATDEP_LOADLIB_H

#if HAVE_DLOPEN
#include <dlfcn.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>   // LoadLibrary() etc
#endif

#include <exception>
#ifndef _MSC_VER
#include <stdexcept>   // std::runtime_exception (with MSVC, it's in <exception>)
#endif
#include "platdep/misc.h"  // opp_getWindowsError()


//
// Loading a dll or so file dynamically
//
inline bool opp_loadlibrary(const char *libname)
{
#if HAVE_DLOPEN
     std::string libfname(libname);
     libfname += ".so";
     if (!dlopen(libfname.c_str(),RTLD_NOW))
         throw std::runtime_error(std::string("Cannot load library '")+libfname+"': "+dlerror());
     return true;
#elif defined(_WIN32)
     std::string libfname(libname);
     libfname += ".dll";
     if (!LoadLibrary(libfname.c_str()))
         throw std::runtime_error(std::string("Cannot load library '")+libfname+"': "+opp_getWindowsError(GetLastError()));
     return true;
#else
     throw std::runtime_error(std::string("Cannot load library '")+libname+"': dlopen() syscall not available");
#endif
}

#endif

