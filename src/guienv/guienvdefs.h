//==========================================================================
//  GUIENVDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the GUIENV library
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

#ifndef __GUIENVDEFS_H
#define __GUIENVDEFS_H

#include "platdefs.h"

NAMESPACE_BEGIN

#if defined(GUIENV_EXPORT)
#  define GUIENV_API OPP_DLLEXPORT
#elif defined(GUIENV_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define GUIENV_API OPP_DLLIMPORT
#else
#  define GUIENV_API
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

NAMESPACE_END


#endif
