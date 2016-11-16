//==========================================================================
//  PLATMISC.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_PLATMISC_H
#define __OMNETPP_PLATMISC_H

#include <stdint.h>  // int64_t (Note: <cstdint> is C++11)
#include <climits>   // __WORDSIZE
#include "platdefs.h"

//
// Breaking into the debugger
//
#if defined _MSC_VER
#define DEBUG_TRAP  __asm {int 3}  // Visual C++: debug interrupt
#elif defined _WIN32 and defined __GNUC__
#define DEBUG_TRAP  asm("int $3\n")  // MinGW or Cygwin: debug interrupt with GNU syntax
#elif defined(__linux__) && (defined(__i386__) || defined(__x86_64__))
#define DEBUG_TRAP  __asm__("int3")
#else
#include <csignal>
#define DEBUG_TRAP  raise(SIGTRAP)
#endif

//
// Common Unix functionality
//
#ifdef _WIN32

#include <process.h>
#include <io.h>
#include <direct.h>
#include <cstdlib> // _MAX_PATH

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
#define fdopen _fdopen
typedef int pid_t;
#define strtoll _strtoi64
#define strtoull _strtoui64
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
#undef ERROR
#include <string>

// errorCode usually comes from GetLastError()
inline std::string opp_getWindowsError(DWORD errorCode)
{
     // Some nice microsoftism to produce an error msg...
     LPVOID lpMsgBuf;
     FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    nullptr,
                    errorCode,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) &lpMsgBuf,
                    0,
                    nullptr );
     int len = strlen((const char *)lpMsgBuf);
     std::string ret((const char *)lpMsgBuf, 0, len>3 ? len-3 : len); // chop ".\r\n"
     LocalFree( lpMsgBuf );
     return ret;
}
#endif

#if defined _MSC_VER && _MSC_VER<1800
#define va_copy(dst, src) ((void)((dst) = (src)))
#endif


//
// gcvt -- FreeBSD does not have it
//
#if defined __FreeBSD__ || defined __ANDROID__
inline char *gcvt(double value, int ndigit, char *buf)
{
    sprintf(buf, "%.*g", ndigit, value);
    return buf;
}
#endif


//
// 64-bit file offsets
//
typedef int64_t file_offset_t;  // off_t on Linux

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
#elif defined __APPLE__ || defined __FreeBSD__ || defined __ANDROID__
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

//
// printf type specifier for int64_t.
//
// Note: %I64d is only used with VC++ 7.1 and MinGW gcc 3.4.x; once we
// drop support for these compilers, this macro can be dropped altogether
//
// Note2: on 64-bit platforms, gcc defines int64_t to be long, so
// we need to use %ld (%lld generates warnings). We recognize 64-bit
// platforms by __WORDSIZE (from <bits/wordsize.h>, #included by <limits.h>)
// (except MinGW-w64 where wordsize is not defined as 64 and long is only 32-bit,
// so we need 'll' there
//
// Note3: It would be probably better to use the PRId64 macro here which is available
// in C++11. See: http://en.cppreference.com/w/cpp/types/integer
//
#if defined(_MSC_VER) || (defined(__MINGW32__) && !defined(__MINGW64__))
#   define INT64_PRINTF_FORMAT   "I64"
#elif __WORDSIZE == 64 && !defined(__APPLE__)
#   define INT64_PRINTF_FORMAT   "l"
#   define INT64_IS_SAME_AS_LONG
#else
#   define INT64_PRINTF_FORMAT   "ll"
#endif

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#endif

