//==========================================================================
// PYTHONUTIL.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_PYTHONUTIL_H
#define __OMNETPP_PYTHONUTIL_H

#include "omnetpp/platdep/config.h"

#ifdef WITH_PYTHON

#include <string>
#include <Python.h>
#include "common/fileutil.h"
#include "common/stringutil.h"
#include "omnetpp/cexception.h"

namespace omnetpp {

class cNedDeclaration;

void ensurePythonInterpreter();

/**
 * Internal helper. Checks if the Python interpreter has an exception
 * being thrown, and if so, clears it, and rethrows it as a C++ exception.
 */
void checkPythonException();

}  // namespace omnetpp

#endif  // WITH_PYTHON

#endif // __OMNETPP_PYTHONUTIL_H

