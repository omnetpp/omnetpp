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

#ifdef WITH_PYTHONSIM

#include <string>
#include <Python.h>
#include "common/fileutil.h"
#include "common/stringutil.h"
#include "omnetpp/cexception.h"

#if PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION < 10
#define Py_IsNone(x)  ((x) == Py_None)
#endif

namespace omnetpp {

class cNedDeclaration;

void ensurePythonInterpreter();

/**
 * Internal helper. Checks if the Python interpreter has an exception
 * being thrown, and if so, clears it, and rethrows it as a C++ exception.
 */
void checkPythonException();

std::string getQualifiedPythonClassName(cNedDeclaration *declaration);

void *instantiatePythonObject(const char *pythonClassQName);

template <class T>
T *instantiatePythonObjectChecked(const char *pythonClassQName) {
    void *result = instantiatePythonObject(pythonClassQName);
    // TODO: add an actual type check somehow
    return (T*)result;
}


class SIM_API PythonGilLock {
    PyGILState_STATE state;

public:
    PythonGilLock()
      : state(PyGILState_Ensure())
    {
      // nothing
    }

    PythonGilLock(const PythonGilLock&) = delete;
    PythonGilLock(PythonGilLock&&) = delete;
    PythonGilLock& operator=(const PythonGilLock&) = delete;
    PythonGilLock& operator=(PythonGilLock&&) = delete;

    ~PythonGilLock() {
        PyGILState_Release(state);
    }
};


}  // namespace omnetpp

#endif  // WITH_PYTHONSIM

#endif // __OMNETPP_PYTHONUTIL_H

