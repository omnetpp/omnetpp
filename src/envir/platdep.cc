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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "defs.h"   // __WIN32__
#include "util.h"
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
     {
         opp_error("Cannot load library '%s': %s",libname,dlerror());
         return false;
     }
     return true;
#elif defined(__WIN32__)
     if (!LoadLibrary(libname))
     {
         // Some nice microsoftism to produce an error msg :-(
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
         char *msg=(char *)lpMsgBuf;
         msg[strlen(msg)-3]=0;  // chop ".\r\n"
         opp_error("Cannot load library '%s': %s",libname,msg);
         LocalFree( lpMsgBuf );
         return false;
     }
     return true;
#else
     opp_error("Cannot load '%s': dlopen() syscall not available",libname);
     return false;
#endif
}


