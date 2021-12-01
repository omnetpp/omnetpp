//==========================================================================
//  FSUTILS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_FSUTILS_H
#define __OMNETPP_ENVIR_FSUTILS_H

#include "omnetpp/simutil.h"
#include "envirdefs.h"

namespace omnetpp {
namespace envir {

/**
 * Utility function: dynamically loads a DLL (Windows) or shared object
 * (.so, on Unix systems) and registers the module types, channel types,
 * networks etc. in it. The file name should be given without extension;
 * ".dll" or ".so" will be appended, depending on the platform.
 *
 * On failure, cRuntimeError will be thrown.
 */
ENVIR_API void loadExtensionLibrary(const char *lib);

}  // namespace envir
}  // namespace omnetpp


#endif

