//==========================================================================
//  FSUTILS.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "platmisc.h"
#include "fileutil.h"
#include "fsutils.h"
#include "onstartup.h"
#include "cexception.h"

NAMESPACE_BEGIN

static bool opp_loadlibrary(const char *libname)
{
     bool withdir = strchr(libname, '/')!=NULL || strchr(libname, '\\')!=NULL;
     std::string dir, filenameonly;
     splitFileName(libname, dir, filenameonly);
     bool withext = strchr(filenameonly.c_str(), '.')!=NULL;

#if HAVE_DLOPEN
     std::string libfullname;
     if (withext)
         libfullname = libname;  // unchanged
     else if (withdir)
         libfullname = dir + "/lib" + filenameonly + SHARED_LIB_SUFFIX;
     else
         libfullname = std::string("lib") + filenameonly + SHARED_LIB_SUFFIX;
     if (!dlopen(libfullname.c_str(), RTLD_NOW|RTLD_GLOBAL))
         throw std::runtime_error(std::string("Cannot load library '")+libfullname+"': "+dlerror());
     return true;

#elif defined(_WIN32)
     std::string libfullname;
     if (withext)
         libfullname = libname;  // unchanged
     else
         libfullname = std::string(libname) + ".dll";
     (void)withdir;
     if (!LoadLibrary((char *)libfullname.c_str()))
         throw std::runtime_error(std::string("Cannot load library '")+libfullname+"': "+opp_getWindowsError(GetLastError()));
     return true;

#else
     throw std::runtime_error(std::string("Cannot load library '")+libname+"': dlopen() syscall not available");
#endif
}

void loadExtensionLibrary(const char *lib)
{
    try {
        opp_loadlibrary(lib);
        ExecuteOnStartup::executeAll();
    }
    catch (std::runtime_error e) {
        throw cRuntimeError(e.what());
    }
}

NAMESPACE_END
