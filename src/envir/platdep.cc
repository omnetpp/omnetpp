//==========================================================================
//  PLATDEP.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Implementation of
//    platform dependent functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "defs.h"   // __WIN32__
#include "platdep.h"
#include "cexception.h"
#include "cenvir.h"
#include "string"

#if HAVE_DLOPEN
#include <dlfcn.h>
#endif

#ifdef __WIN32__
#include <windows.h>   // LoadLibrary() etc
#endif

#if defined(__WIN32__)
std::string getWindowsError()
{
     // Some nice microsoftism to produce an error msg...
     LPVOID lpMsgBuf;
     FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    GetLastError(),
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

bool opp_loadlibrary(const char *libname)
{
#if HAVE_DLOPEN
     std::string libfname(libname);
     libfname += ".so";
     if (!dlopen(libfname.c_str(),RTLD_NOW))
         throw new cException("Cannot load library '%s': %s",libfname.c_str(),dlerror());
     return true;
#elif defined(__WIN32__)
     std::string libfname(libname);
     libfname += ".dll";
     if (!LoadLibrary(libfname.c_str()))
         throw new cException("Cannot load library '%s': %s", libfname.c_str(), getWindowsError().c_str());
     return true;
#else
     throw new cException("Cannot load library '%s': dlopen() syscall not available", libname);
#endif
}


unsigned long opp_difftimebmillis(const struct timeb& t, const struct timeb& t0)
{
    // with 32-bit longs it only overflows after 49.7 DAYs
    return (t.time - t0.time)*1000 + (t.millitm - t0.millitm);
}

struct timeb operator+(const struct timeb& a, const struct timeb& b)
{
    struct timeb r;
    r.time = a.time + b.time;
    r.millitm = a.millitm + b.millitm;
    if (r.millitm>=1000)
    {
        r.time++;
        r.millitm -= 1000;
    }
    return r;
}

struct timeb operator-(const struct timeb& a, const struct timeb& b)
{
    struct timeb r;
    r.time = a.time - b.time;
    r.millitm = a.millitm - b.millitm;
    if (a.millitm < b.millitm)
    {
        r.time--;
        r.millitm += 1000;
    }
    return r;
}

