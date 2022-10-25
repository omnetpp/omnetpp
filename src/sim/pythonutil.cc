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

#ifdef WITH_PYTHONSIM

#include <cstddef>
#include <string>
#include <regex>

#include "Python.h"

#include "common/stringutil.h"
#include "omnetpp/cvalue.h"
#include "omnetpp/cvaluearray.h"
#include "omnetpp/cvaluemap.h"
#include "omnetpp/onstartup.h"
#include "omnetpp/csimulation.h"
#include "envir/startup.h"
#include "sim/netbuilder/cnedloader.h"

namespace omnetpp {

using namespace common;

bool pythonInitializedHere = false;

void ensurePythonInterpreter()
{
    PythonGilLock gilLock;

    ASSERT(Py_IsInitialized());

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

        # handleMessage is done after instantiation

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


def replace_cvalue_str(klass, name):
    """
    Pythonization so str() works on cValue objects of type other than STRING, by
    switching it to the "printer" function instead of the "string getter" one.
    """
    if name == "cValue":
        klass.__str__ = lambda self: str(self.str())

cppyy.py.add_pythonization(replace_cvalue_str, 'omnetpp')


cppyy.add_include_path(OMNETPP_ROOT + "/include")
cppyy.include("omnetpp.h")
)");

        cINedLoader *nedLoader = cSimulation::getActiveSimulation()->getNedLoader();
        if (nedLoader) {
            PyObject *pyPath = PySys_GetObject("path");
            for (auto f : nedLoader->getLoadedNedFolders())
                PyList_Append(pyPath, PyUnicode_FromString(f.c_str()));
        }

        pythonConfigured = true;
    }
}

EXECUTE_ON_SHUTDOWN(
    if (pythonInitializedHere)
        Py_Finalize();
);


static std::string extractExceptionLocation(PyObject *traceback)
{
    std::string location;

    // to not leave the refcount lower than it was
    Py_XINCREF(traceback);

    while (traceback && traceback != Py_None) {
        PyObject *frame = PyObject_GetAttrString(traceback, "tb_frame");
        PyObject *code = PyObject_GetAttrString(frame, "f_code");

        PyObject *filename = PyObject_GetAttrString(code, "co_filename");
        PyObject *filenameStr = PyObject_Str(filename);
        location = PyUnicode_AsUTF8(filenameStr);

        Py_DECREF(frame);
        Py_DECREF(code);
        Py_DECREF(filename);
        Py_DECREF(filenameStr);

        if (location == "<string>") {
            location.clear();
            PyObject *nextTraceback = PyObject_GetAttrString(traceback, "tb_next");
            Py_DECREF(traceback);
            traceback = nextTraceback;
            continue;
        }

        location = filenameOf(location.c_str());

        PyObject *lineno = PyObject_GetAttrString(traceback, "tb_lineno");
        location += ":" + std::to_string(PyLong_AsLong(lineno));
        Py_DECREF(lineno);

        PyObject *name = PyObject_GetAttrString(code, "co_name");
        PyObject *nameStr = PyObject_Str(name);
        const char *nameUtf8 = PyUnicode_AsUTF8(nameStr);
        if (!opp_isempty(nameUtf8)) {
            location += " in ";
            location += nameUtf8;
        }

        Py_DECREF(name);
        Py_DECREF(nameStr);

        break;
    }

    // to not leak the last considered traceback
    Py_XDECREF(traceback);

    return location;
}


void checkPythonException()
{
    PythonGilLock gilLock;

    PyObject *type, *value, *traceback;
    PyErr_Fetch(&type, &value, &traceback);

    if (type || value || traceback) {
        PyErr_NormalizeException(&type, &value, &traceback);
        std::string message = PyUnicode_AsUTF8(PyObject_Str(value));
        std::string location = extractExceptionLocation(traceback);

        if (!location.empty())
            message += " - at " + location;

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
#ifdef WITH_PYTHONSIM
    ASSERT2(strchr(pythonClassQName, '.'), "Python class name must be qualified");

    std::string moduleToImport = opp_substringbeforelast(pythonClassQName, ".");
    std::string classToInstantiate = opp_substringafterlast(pythonClassQName, ".");

    ensurePythonInterpreter();
    PythonGilLock gilLock;

    try {
        PyObject *globals = PyDict_New();
        PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());

        PyRun_String(("from " + moduleToImport + " import " + classToInstantiate + "\n"
            "inst = " + classToInstantiate + "()\n"
            "inst.__python_owns__ = False\n"
            "import cppyy\n"
            // Since regular pythonization callbacks act on the class level,
            // and overrides in the user's subclasses would in fact override
            // any custom wrappers that we could add there, in this case, we
            // add the wrapper at the instance level, where it is effective.
            "if cppyy.gbl.omnetpp.cSimpleModule in type(inst).mro():\n"
            "  inst.__orig_handleMessage = inst.handleMessage\n"
            "  def __new_handleMessage(self, msg):\n"
            "    assert(msg.__python_owns__ == False)\n"
            "    msg.__python_owns__ = True\n"
            "    return self.__orig_handleMessage(msg)\n"
            "  import types\n"
            "  inst.handleMessage = types.MethodType(__new_handleMessage, inst)\n"
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

#endif  // WITH_PYTHONSIM
