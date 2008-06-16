//==========================================================================
//  FSUTILS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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

#ifndef __FSUTILS_H
#define __FSUTILS_H

#include "envirdefs.h"
#include "simutil.h"

NAMESPACE_BEGIN

/**
 * Utility function: dynamically loads a DLL (Windows) or shared object
 * (.so, on Unix systems) and registers the module types, channel types,
 * networks etc. in it. The file name should be given without extension;
 * ".dll" or ".so" will be appended, depending on the platform.
 *
 * On failure, cRuntimeError will be thrown.
 */
ENVIR_API void loadExtensionLibrary(const char *lib);

NAMESPACE_END


#endif

