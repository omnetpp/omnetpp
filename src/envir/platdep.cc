//==========================================================================
//  PLATDEP.CC - part of
//                             OMNeT++
//             Discrete System Simulation in C++
//
//  Implementation of
//    platform dependent functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "defs.h"   // __WIN32__
#include "util.h"
#include "cexception.h"
#include "cenvir.h"

#if HAVE_DLOPEN
#include <dlfcn.h>
#endif

#ifdef __WIN32__
#include <windows.h>   // LoadLibrary() et al.
#endif


bool opp_loadlibrary(const char *libname)
{
#if HAVE_DLOPEN
     if (!dlopen(libname,RTLD_NOW))
         throw new cException("Cannot load library '%s': %s",libname,dlerror());
     return true;
#elif defined(__WIN32__)
     if (!LoadLibrary(libname))
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
         opp_string msg; msg=(const char *)lpMsgBuf;
         LocalFree( lpMsgBuf );
         msg.buffer()[strlen(msg)-3] = '\0';  // chop ".\r\n"
         throw new cException("Cannot load library '%s': %s",libname,(const char *)msg);
     }
     return true;
#else
     throw new cException("Cannot load '%s': dlopen() syscall not available",libname);
#endif
}


