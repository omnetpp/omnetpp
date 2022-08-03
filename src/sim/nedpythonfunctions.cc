//=========================================================================
//  NEDPYTHONFUNCTIONS.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "omnetpp/platdep/config.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cexception.h"

#ifdef WITH_PYTHON

#include <cstddef>
#include <string>

#include "common/stringutil.h"
#include "omnetpp/cvalue.h"
#include "omnetpp/cvaluearray.h"
#include "omnetpp/cvaluemap.h"

#include "Python.h"

#endif

namespace omnetpp {

using namespace common;

class cComponent;

void nedpythonfunctions_dummy() {} //see util.cc

#ifdef WITH_PYTHON

static const char *CPP_CONTAINER_UNWRAP_CODE = R"(
import cppyy

STRING_TYPES = ["cppyy.gbl.std.string", "cppyy.gbl.std.string_view"]
LIST_TYPES = ["cppyy.gbl.std.vector", "cppyy.gbl.std.queue", "cppyy.gbl.std.deque", "cppyy.gbl.std.list"]
DICT_TYPES = ["cppyy.gbl.std.map", "cppyy.gbl.std.unordered_map"]

def iscppinstance(obj, typestring):
    # TODO: I wish this could be done in a nicer way.
    # See: https://github.com/wlav/cppyy/issues/89
    return typestring in str(type(obj))

def unwrap(obj):
    t = type(obj)

    for stringtype in STRING_TYPES:
        if iscppinstance(obj, stringtype):
            return str(obj)

    for listtype in LIST_TYPES:
        if iscppinstance(obj, listtype):
            return [unwrap(e) for e in list(obj)]

    for dicttype in DICT_TYPES:
        if iscppinstance(obj, dicttype):
            d = dict(obj)
            result = dict()
            for k, v in d.items():
                result[unwrap(k)] = unwrap(v)
            return result

    if isinstance(obj, cppyy.gbl.omnetpp.cValueArray):
        l = list(obj.getArray())
        return [unwrap(e) for e in l]

    if isinstance(obj, cppyy.gbl.omnetpp.cValueMap):
        d = dict(obj.getFields())
        result = dict()
        for k, v in d.items():
            result[unwrap(k)] = unwrap(v)
        return result

    if isinstance(obj, cppyy.gbl.omnetpp.cValue):
        from cppyy.gbl.omnetpp import cValue
        vt = obj.getType()

        if vt == cValue.BOOL:
            return obj.boolValue()
        if vt == cValue.DOUBLE:
            return obj.doubleValue()
        if vt == cValue.INT:
            return obj.intValue()
        if vt == cValue.STRING:
            return obj.stringValue()
        if vt == cValue.OBJECT:
            return unwrap(obj.objectValue())

    if isinstance(obj, cppyy.gbl.omnetpp.cValueHolder):
        return unwrap(obj.get())

    # TODO: other kinds of objects? wrap them in cValue::OBJECT? or a cPythonObject "holder"?

    return None
)";


static const char *COMPONENT_WRAPPER_CODE = R"(
import os

OMNETPP_ROOT = os.getenv("__omnetpp_root_dir")

if not OMNETPP_ROOT:
    raise RuntimeError("The OMNeT++ root directory was not found, make sure the shell is set up appropriately (`. setenv`)!")

import cppyy

def replace_cvalue_str(klass, name):
    """
    Pythonization so str() works on cValue objects of type other than STRING,
    by switching it to the "printer" function instead of the "string getter
    """
    if name == "cValue":
        try:
            klass.__str__ = lambda self: str(self.str())
        except KeyError:
            pass

cppyy.py.add_pythonization(replace_cvalue_str, 'omnetpp')

cppyy.add_include_path(OMNETPP_ROOT + "/include")
cppyy.include("omnetpp.h")

class Accessor:
    """
    Wraps a cComponent object given by its pointer, to give more direct
    access to its parameters as if they were simple members.
    Unfortunately methods and fields declared on any subclass of cComponent
    are not accessible here. See: https://github.com/wlav/cppyy/issues/87
    """
    def __init__(self, contextComponentAddress):
        self.contextComponent = cppyy.bind_object(contextComponentAddress, "omnetpp::cComponent")

    def __getattr__(self, name):
        from cppyy.gbl.omnetpp import cPar

        if self.contextComponent.hasPar(name):
            par = self.contextComponent.par(name)
            t = par.getType()

            if t == cPar.BOOL:
                return par.boolValue()
            if t == cPar.DOUBLE:
                return par.doubleValue()
            if t == cPar.INT:
                return par.intValue()
            if t == cPar.STRING:
                return par.stringValue()
            if t == cPar.OBJECT:
                return unwrap(par.objectValue())
            # TODO: XML?
        else:
            return getattr(self.contextComponent, name)
)";

/** Internal helper. Checks if the Python interpreter has an exception
 * being thrown, and if so, clears it, and rethrows it as a C++ exception.
 */
static void checkPythonException()
{
    PyObject *type, *value, *traceback;
    PyErr_Fetch(&type, &value, &traceback);

    if (type || value || traceback) {
        PyErr_NormalizeException(&type, &value, &traceback);
        std::string message = PyUnicode_AsUTF8(PyObject_Str(value));
        Py_XDECREF(type);
        Py_XDECREF(value);
        Py_XDECREF(traceback);
        throw cRuntimeError("Python expression evaluation failed: %s", message.c_str());
    }
}

// Internal helper. Unwraps a Python object, that is actually
// a C++ object wrapped by Cppyy, into a "simple" Python object.
// Recurses into container hierarchies. If the given object is
// not a Cppyy wrapper, returns `None`. Ideally this wouldn't be
// necessary, but see: https://github.com/wlav/cppyy/issues/88
static PyObject *unwrapCppObject(PyObject *obj)
{
    PyObject *globals = PyDict_New();
    PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());
    PyDict_SetItemString(globals, "obj", obj);

    PyRun_String(CPP_CONTAINER_UNWRAP_CODE, Py_file_input, globals, globals);
    checkPythonException();

    PyObject *compiled = Py_CompileString("unwrap(obj)", "<string>", Py_eval_input);
    checkPythonException();

    PyObject *result = PyEval_EvalCode(compiled, globals, globals);
    checkPythonException();

    return result; // may be `None`!
}

// Internal helper. Converts a Python object to a cValue if possible.
// If not, throws an exception.
static cValue pyObjectToValue(PyObject *obj)
{
    // handle a few simple types first
    if (Py_IsNone(obj))
        return cValue((cObject*)nullptr); // UNDEF is not accepted by parameters of type `object`
    if (PyBool_Check(obj))
        return cValue(PyObject_IsTrue(obj) ? true : false);
    if (PyLong_Check(obj))
        return cValue(PyLong_AsLong(obj));
    if (PyFloat_Check(obj))
        return cValue(PyFloat_AsDouble(obj));
    if (PyUnicode_Check(obj))
        return cValue(PyUnicode_AsUTF8(obj));

    // then container types that are also representable in NED
    if (PyList_Check(obj)) {
        cValueArray *arr = new cValueArray();
        size_t size = PyList_Size(obj);
        for (size_t i = 0; i < size; i++)
            arr->add(pyObjectToValue(PyList_GetItem(obj, i)));
        return cValue(arr);
    }

    if (PyDict_Check(obj)) {
        cValueMap *map = new cValueMap();
        PyObject *key, *value;
        ssize_t pos = 0;
        while (PyDict_Next(obj, &pos, &key, &value)) {
            PyObject *unwrappedKey = unwrapCppObject(key);
            checkPythonException();

            if (!Py_IsNone(unwrappedKey))
                key = unwrappedKey;

            if (!PyUnicode_Check(key)) {
                PyObject *t = PyObject_Type(key);
                PyObject *s = PyObject_Str(t);
                const char *str = PyUnicode_AsUTF8(s);
                cRuntimeError exc("Keys in a NED dictionary must be strings, not %s", str);
                Py_XDECREF(t);
                Py_XDECREF(s);
                throw exc;
            }

            map->set(PyUnicode_AsUTF8(key), pyObjectToValue(value));
        }
        return cValue(map);
    }

    // finally try to unwrap any Cppyy-wrapped C++ objects into "simple" Python objects

    PyObject *unwrapped = unwrapCppObject(obj);

    if (!Py_IsNone(unwrapped))
        return pyObjectToValue(unwrapped);

    // and just give up if this didn't work either

    PyObject *s = PyObject_Str(obj);
    const char *str = PyUnicode_AsUTF8(s);

    PyObject *t = PyObject_Type(obj);
    PyObject *ts = PyObject_Str(t);
    const char *t_str = PyUnicode_AsUTF8(ts);

    cRuntimeError exc("Python object could not be converted to cValue: (%s) '%s'", t_str, str);

    Py_XDECREF(s);
    Py_XDECREF(t);
    Py_XDECREF(ts);

    throw exc;
}

// Internal helper. Converts a cValue to a Python object if possible.
// If not, throws an exception.
static PyObject *valueToPyObject(const cValue& val)
{
    switch (val.getType()) {
        case cValue::UNDEF:   return Py_None;
        case cValue::BOOL:    return PyBool_FromLong(val.boolValue());
        case cValue::INT:     return PyLong_FromLong(val.intValue());
        case cValue::DOUBLE:  return PyFloat_FromDouble(val.doubleValue());
        case cValue::STRING:  return PyUnicode_FromString(val.stringValue());
        case cValue::POINTER: {
            cObject *obj = val.objectValue();

            if (!obj)
                return Py_None;

            if (cValueArray *arr = dynamic_cast<cValueArray *>(obj)) {
                PyObject *list = PyList_New(arr->size());
                const std::vector<cValue>& values = arr->getArray();

                for (int i = 0; i < values.size(); ++i) {
                    PyObject *item = valueToPyObject(values[i]);
                    PyList_SetItem(list, i, item);
                }

                return list;
            }

            if (cValueMap *map = dynamic_cast<cValueMap *>(obj)) {
                PyObject *dict = PyDict_New();

                for (int i = 0; i < map->size(); ++i) {
                    PyObject *value = valueToPyObject(map->getEntry(i).second);
                    PyDict_SetItem(dict, PyUnicode_FromString(map->getEntry(i).first.c_str()), value);
                }

                return dict;
            }

            throw cRuntimeError("cValue of type POINTER could not be converted to Python object");
        }
    }
}

// Internal helper. Creates a dictionary to be used as a `globals`
// context for evaluating and executing Python code in. It has
// the interpreter builtins and our helper code already defined.
PyObject *makeGlobalsWithAccessor(cComponent *contextComponent)
{
    PyObject *globals = PyDict_New();
    PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());

    PyRun_String(CPP_CONTAINER_UNWRAP_CODE, Py_file_input, globals, globals);
    PyRun_String(COMPONENT_WRAPPER_CODE, Py_file_input, globals, globals);
    checkPythonException();

    PyObject *accessorClass = PyDict_GetItemString(globals, "Accessor");
    ASSERT(!Py_IsNone(accessorClass));
    checkPythonException();

    PyObject *args = PyTuple_New(1);
    PyTuple_SetItem(args, 0, PyLong_FromVoidPtr(reinterpret_cast<void*>(contextComponent)));
    checkPythonException();

    PyObject *accessor = PyObject_Call(accessorClass, args, nullptr);
    checkPythonException();

    PyDict_SetItemString(globals, "this", accessor);
    checkPythonException();

    return globals;
}

#endif // WITH_PYTHON


cValue nedf_pyeval(cComponent *contextComponent, cValue argv[], int argc)
{
#ifdef WITH_PYTHON
    std::string code = argv[0].stringValue();

    if (argc > 1)
        code = "lambda " + code;

    PyObject *compiled = Py_CompileString(code.c_str(), "<string>", Py_eval_input);
    checkPythonException();

    PyObject *globals = makeGlobalsWithAccessor(contextComponent);

    PyObject *result = PyEval_EvalCode(compiled, globals, globals);
    checkPythonException();

    if (argc > 1) {
        // in this case, the result is a lambda we should call
        PyObject *args = PyTuple_New(argc-1);

        for (int i = 1; i < argc; ++i)
            PyTuple_SetItem(args, i-1, valueToPyObject(argv[i]));

        result = PyObject_Call(result, args, NULL);
        checkPythonException();
    }

    return pyObjectToValue(result);
#else
    throw cRuntimeError("Embedded Python support was not enabled at build time");
#endif
}

cValue nedf_pycode(cComponent *contextComponent, cValue argv[], int argc)
{
#ifdef WITH_PYTHON
    std::string code = argv[0].stringValue();
    // Using *args, so we don't have to also create a list
    // inside the argument pack tuple when calling.
    code = "def fun(*args):\n" + opp_indentlines(code, "    ");

    PyObject *globals = makeGlobalsWithAccessor(contextComponent);

    // always returns `None`
    PyRun_String(code.c_str(), Py_file_input, globals, globals);
    checkPythonException();

    PyObject *fun = PyDict_GetItemString(globals, "fun");

    if (!fun)
        throw cRuntimeError("Internal error: Defined internal function not found in locals");

    PyObject *args = PyTuple_New(argc-1);

    for (int i = 1; i < argc; ++i)
        PyTuple_SetItem(args, i-1, valueToPyObject(argv[i]));

    PyObject *result = PyObject_CallObject(fun, args);
    checkPythonException();

    return pyObjectToValue(result);
#else
    throw cRuntimeError("Embedded Python support was not enabled at build time");
#endif
}


Define_NED_Function2(nedf_pyeval, "any pyeval(string s, ...)", "python", "evaluates the string as a Python expression; or as if it was a Python lambda, then calls it");
Define_NED_Function2(nedf_pycode, "any pycode(string s, ...)", "python", "evaluates the string as if it was a Python function body");


}  // namespace omnetpp
