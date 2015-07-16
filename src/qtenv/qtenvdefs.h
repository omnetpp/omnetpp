//==========================================================================
//  QTENVDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Tkenv library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_QTENVDEFS_H
#define __OMNETPP_QTENV_QTENVDEFS_H

#include "tk-dummy.h"
#include "omnetpp/platdep/platdefs.h"
#include "omnetpp/platdep/platmisc.h"   // must precede tk.h otherwise Visual Studio 2013 fails to compile
#include "omnetpp/platdep/timeutil.h"   // must precede <tk.h>, due to collision with <windows.h>

// a simple "class cObject;" is not enough for Q_DECLARE_METATYPE, it has to be fully defined
#include "omnetpp/cobject.h"
#include "omnetpp/cmessage.h"

// this prevents the definition of "emit" as a macro
// it is needed because OMNeT uses "emit" as a function name
// if you want to use "emit" the Qt way after including this header
// just define "emit" as empty, like in qobjectdefs.h
#define QT_NO_EMIT

#include <QMetaType>
#include <QPair>

typedef QPair<OPP::cObject*, int> ActionDataPair;

Q_DECLARE_METATYPE(OPP::cObject*)
Q_DECLARE_METATYPE(ActionDataPair)
Q_DECLARE_METATYPE(OPP::cMessage*)


#if defined(TKENV_EXPORT)
#  define QTENV_API OPP_DLLEXPORT
#elif defined(TKENV_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define QTENV_API OPP_DLLIMPORT
#else
#  define QTENV_API
#endif

#define TCL_NULL  (void*)nullptr  // workaround: gcc 4.8.2 on a 64-bit architecture pushed a 32-bit 0 on the stack as last arg for Tcl_VarEval(), causing it to crash

#endif
