//==========================================================================
// PYTHONUTIL.CC - part of
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

#include "pythonutil.h"

#ifdef WITH_PYTHON

#include "Python.h"

#include <cstddef>
#include <string>
#include <regex>

#include "common/stringutil.h"
#include "omnetpp/cvalue.h"
#include "omnetpp/cvaluearray.h"
#include "omnetpp/cvaluemap.h"
#include "omnetpp/onstartup.h"
#include "envir/startup.h"
#include "sim/netbuilder/cnedloader.h"

namespace omnetpp {

using namespace common;

static bool pythonInitializedHere = false;

void ensurePythonInterpreter()
{
    if (!Py_IsInitialized()) {
        Py_Initialize();
        pythonInitializedHere = true;
    }
}

EXECUTE_ON_SHUTDOWN(
    if (pythonInitializedHere)
        Py_Finalize();
);

void checkPythonException()
{
    PyObject *type, *value, *traceback;
    PyErr_Fetch(&type, &value, &traceback);

    if (type || value || traceback) {
        PyErr_NormalizeException(&type, &value, &traceback);
        std::string message = PyUnicode_AsUTF8(PyObject_Str(value));
        Py_XDECREF(type);
        Py_XDECREF(value);
        Py_XDECREF(traceback);
        throw cRuntimeError("%s", message.c_str());
    }
}

}  // namespace omnetpp

#endif  // WITH_PYTHON
