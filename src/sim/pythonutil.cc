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

#include <cstddef>
#include <string>
#include <regex>

#include "Python.h"

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

    static bool pythonConfigured = false;

    if (!pythonConfigured) {
        PyRun_SimpleString(R"(
import os
OMNETPP_ROOT = os.getenv("__omnetpp_root_dir")
if not OMNETPP_ROOT:
    raise RuntimeError("The OMNeT++ root directory was not found, make sure the shell is set up appropriately (`source setenv`)!")

import cppyy
cppyy.add_include_path(OMNETPP_ROOT + "/include")
cppyy.include("omnetpp.h")
)");

        PyObject *pyPath = PySys_GetObject("path");
        for (auto f : cNedLoader::getInstance()->getLoadedNedFolders())
            PyList_Append(pyPath, PyUnicode_FromString(f.c_str()));

        pythonConfigured = true;
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
