//==========================================================================
//  PLATMISC.H - part of
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

#ifndef __PLATMISC_H
#define __PLATMISC_H

//
// Common Unix functionality
//
#ifdef _WIN32

#include <process.h>
#include <io.h>
#include <direct.h>
#include <stdlib.h> // _MAX_PATH

#define getpid _getpid
#define getcwd _getcwd
#define chdir  _chdir
#define usleep(x) _sleep((x)/1000)
#define mkdir(x,y) _mkdir(x)

#ifndef __MINGW32__
#define vsnprintf _vsnprintf
#endif

#else

#include <sys/types.h>
#include <unistd.h>  // getpid(), getcwd(), etc

#if HAVE_DLOPEN
#include <dlfcn.h>
#endif

#endif

#include <exception>
#include <stdexcept>

#ifdef _MSC_VER
#define strcasecmp  stricmp
#define strncasecmp strnicmp
#endif

//
// Getting Windows error strings
//
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef min
#undef max
#include <string>

// errorCode usually comes from GetLastError()
inline std::string opp_getWindowsError(DWORD errorCode)
{
     // Some nice microsoftism to produce an error msg...
     LPVOID lpMsgBuf;
     FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    errorCode,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) &lpMsgBuf,
                    0,
                    NULL );
     int len = strlen((const char *)lpMsgBuf);
     std::string ret((const char *)lpMsgBuf, 0, len>3 ? len-3 : len); // chop ".\r\n"
     LocalFree( lpMsgBuf );
     return ret;
}
#endif


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
//XXX     if (!LoadLibrary(libfname.c_str()))
     if (!LoadLibrary((char *)libfname.c_str()))
         throw std::runtime_error(std::string("Cannot load library '")+libfname+"': "+opp_getWindowsError(GetLastError()));
     return true;
#else
     throw std::runtime_error(std::string("Cannot load library '")+libname+"': dlopen() syscall not available");
#endif
}

#endif

