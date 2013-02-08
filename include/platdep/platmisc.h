//==========================================================================
//  PLATMISC.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

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

#if defined(__MINGW32__)
#include <unistd.h>  // getpid(), getcwd(), usleep() etc.
#endif

#define mkdir(x,y) _mkdir(x)

// unistd.h contains usleep only on mingw 4.4 or later (minor version 16), otherwise use Windows.h Sleep()
#if defined(_MSC_VER) || __MINGW32_MINOR_VERSION < 16
#define usleep(x) Sleep((x)/1000)
#endif

#else // !_WIN32

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>  // getpid(), getcwd(), etc

#if HAVE_DLOPEN
#include <dlfcn.h>
#endif

#endif // _WIN32

#include <exception>
#include <stdexcept>

#ifdef _MSC_VER
#define snprintf _snprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define unlink _unlink
#define putenv _putenv
#define fileno _fileno
#define getpid _getpid
#define getcwd _getcwd
#define chdir  _chdir
#define gcvt _gcvt
#endif

//
// Include windows header -- needed for getting Windows error strings,
// and also by ccoroutine.
//
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400  // needed for the Fiber API
#endif
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

#ifdef _MSC_VER
#define va_copy(dst, src) ((void)((dst) = (src)))
#endif


//
// gcvt -- FreeBSD does not have it
//
#if defined __FreeBSD__
inline char *gcvt(double value, int ndigit, char *buf)
{
    sprintf(buf, "%.*g", ndigit, value);
    return buf;
}
#endif


//
// 64-bit file offsets
//
#include "intxtypes.h"

typedef int64 file_offset_t;  // off_t on Linux

#if defined _MSC_VER
  #define opp_ftell _ftelli64
  #define opp_fseek _fseeki64
  #define opp_stat_t __stat64 // name of the struct
  #define opp_stat _stat64    // name of the function
  #define opp_fstat _fstati64
#elif defined __MINGW32__
  #define opp_ftell ftello64
  #define opp_fseek fseeko64
  #define opp_stat_t _stati64 // name of the struct
  #define opp_stat _stati64    // name of the function
  #define opp_fstat _fstati64
#elif defined __APPLE__ || defined __FreeBSD__
  #define opp_ftell ftello
  #define opp_fseek fseeko
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

