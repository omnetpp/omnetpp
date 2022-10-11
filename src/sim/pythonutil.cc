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

def _set_arg_owned(klass, method_name, arg_index, owned):
    orig_method = getattr(klass, method_name)
    def wrapper(self, *args):
        args[arg_index].__python_owns__ = owned
        return orig_method(self, *args)

    setattr(klass, method_name, wrapper)

def csimplemodule_msg_ownership(klass, name):
    if name == "cSimpleModule":

        _set_arg_owned(klass, "send", 0, False)
        _set_arg_owned(klass, "sendDirect", 0, False)
        _set_arg_owned(klass, "sendDelayed", 0, False)

        _set_arg_owned(klass, "scheduleAt", 1, False)
        _set_arg_owned(klass, "scheduleAfter", 1, False)
        _set_arg_owned(klass, "rescheduleAt", 1, False)
        _set_arg_owned(klass, "rescheduleAfter", 1, False)

        # FIXME: DOES NOT WORK!
        #_set_arg_owned(klass, "handleMessage", 0, True)

cppyy.py.add_pythonization(csimplemodule_msg_ownership, 'omnetpp')


# This is a workaround for: https://github.com/wlav/cppyy/issues/95
def enable_simtime_arithmetic(klass, name):
    if name == "SimTime":
        def add(self, other):
            c = cppyy.gbl.omnetpp.SimTime(self)
            c += other
            return c
        klass.__add__ = add

        def sub(self, other):
            c = cppyy.gbl.omnetpp.SimTime(self)
            c -= other
            return c
        klass.__sub__ = sub

        """ TODO:
        *	__mul__(self, other)
        /	__truediv__(self, other)
        //	__floordiv__(self, other)
        %	__mod__(self, other)
        **	__pow__(self, other)
        >>	__rshift__(self, other)
        <<	__lshift__(self, other)
        &	__and__(self, other)
        |	__or__(self, other)
        ^	__xor__(self, other)
        """

        # also TODO: i... and r... methods?

cppyy.py.add_pythonization(enable_simtime_arithmetic, 'omnetpp')


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


std::string getQualifiedPythonClassName(cNedDeclaration *declaration)
{
    cProperty *pythonClassProp = declaration->getProperties()->get("pythonClass");

    if (!pythonClassProp)
        return "";

    const char *className = opp_emptytodefault(pythonClassProp->getValue(), declaration->getName());

    if (strchr(className, '.') != nullptr) {
        // qualified name: expect the Python module to import to be found in the existing Python path

        return className;
    }
    else {
        // simple name: load the class from the .py file next to the NED file that contains the @pythonClass property

        const char *sourceFile = opp_emptytodefault(pythonClassProp->getValueOriginFile(), declaration->getSourceFileName());
        std::string nedFileName = removeFileExtension(filenameOf(sourceFile).c_str());
        std::string moduleName = opp_join(".", declaration->getPackage().c_str(), nedFileName.c_str());

        return opp_join(".", moduleName.c_str(), className);
    }
}


void *instantiatePythonObject(const char *pythonClassQName)
{
#ifdef WITH_PYTHON
    ASSERT2(strchr(pythonClassQName, '.'), "Python class name must be qualified");

    std::string moduleToImport = opp_substringbeforelast(pythonClassQName, ".");
    std::string classToInstantiate = opp_substringafterlast(pythonClassQName, ".");

    ensurePythonInterpreter();

    try {
        PyObject *globals = PyDict_New();
        PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());

        PyRun_String(("from " + moduleToImport + " import " + classToInstantiate + "\n"
            "inst = " + classToInstantiate + "()\n"
            "inst.__python_owns__ = False\n"
            "import cppyy.ll").c_str(), Py_file_input, globals, globals);
        checkPythonException();

        PyObject *code = Py_CompileString("cppyy.ll.addressof(inst)", "<addressof>", Py_eval_input);
        checkPythonException();

        PyObject *result = PyEval_EvalCode(code, globals, globals);
        checkPythonException();

        void *pythonObject = (void *)PyLong_AsLong(result);
        ASSERT2(pythonObject, "Instance is `nullptr` (Make sure to call `super().__init__()` in the Python class constructor!)");
        return pythonObject;
    }
    catch (std::exception& e) {
        throw cRuntimeError("Cannot instantiate Python class '%s': %s", pythonClassQName, e.what());
    }
#else
    throw cRuntimeError("Embedded Python support was not enabled at build time");
#endif
}

}  // namespace omnetpp

#endif  // WITH_PYTHON
