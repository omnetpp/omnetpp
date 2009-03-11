//==========================================================================
//  JAVAENVDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Javaenv library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __JAVAENVDEFS_H
#define __JAVAENVDEFS_H

#include "simkerneldefs.h"  // for OPP_DLLIMPORT, OPP_DLLEXPORT

// OPP_DLLIMPORT/EXPORT are empty if non-Windows, non-dll, etc.
#ifdef BUILDING_JAVAENV
#  define JAVAENV_API  OPP_DLLEXPORT
#else
#  define JAVAENV_API  OPP_DLLIMPORT
#endif


//XXX temporary
class DBGTrace
{
    private:
        const char *f;
    public:
        DBGTrace(const char *s) {f = s; fprintf(stderr, "entering %s\n", f); fflush(stderr);}
        ~DBGTrace() {fprintf(stderr, "leaving %s\n", f); fflush(stderr);}
};

//#define TRACE(x) DBGTrace _(x)
#define TRACE(x)

#endif
