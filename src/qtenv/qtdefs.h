//==========================================================================
//  QTDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the CMDENV library
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

#ifndef __QTDEFS_H
#define __QTDEFS_H

#include "platdefs.h"

NAMESPACE_BEGIN

#if defined(QTENV_EXPORT)
#  define QTENV_API OPP_DLLEXPORT
#elif defined(QTENV_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define QTENV_API OPP_DLLIMPORT
#else
#  define QTENV_API
#endif


NAMESPACE_END


#endif
