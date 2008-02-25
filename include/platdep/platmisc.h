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
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define unlink _unlink
#define itoa _itoa
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

//
// 64-bit file offsets
//
#include "inttypes.h"

typedef int64 file_offset_t;  // off_t on Linux

/*
#if defined _MSC_VER
# define opp_ftell _ftelli64
# define opp_fseek _fseeki64
# if _MSC_VER < 1400
   // Kludge: in Visual C++ 7.1, 64-bit fseek/ftell is not part of the public
   // API, but the functions are there in the CRT. Declare them here.
   int __cdecl _fseeki64 (FILE *str, __int64 offset, int whence);
   __int64 __cdecl _ftelli64 (FILE *stream);
# endif
#else
# define opp_ftell ftello64
# define opp_fseek fseeko64
#endif
*/

//FIXME replace this with the above code!!! once it compiles with 7.1
#if defined _MSC_VER && (_MSC_VER >= 1400)
#define opp_ftell _ftelli64
#define opp_fseek _fseeki64
#define opp_stat_t __stat64 // name of the struct
#define opp_stat _stat64    // name of the function
#define opp_fstat _fstati64
#elif defined _MSC_VER || __MINGW32__ // FIXME: no 64 bit version under mingw?
// for Visual C++ 7.1, fall back to 32-bit functions
#define opp_ftell ftell
#define opp_fseek fseek
#define opp_stat_t stat
#define opp_stat stat
#define opp_fstat fstat
#else
#define opp_ftell ftello64
#define opp_fseek fseeko64
#define opp_stat_t stat64
#define opp_stat stat64
#define opp_fstat fstat64
#endif


#endif

