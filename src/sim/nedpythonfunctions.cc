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

#ifdef WITH_PYTHON
#include "pythonutil.h"

#include <cstddef>
#include <string>
#include <regex>

#include "common/stringutil.h"
#include "omnetpp/cvalue.h"
#include "omnetpp/cvaluearray.h"
#include "omnetpp/cvaluemap.h"
#endif

#include "omnetpp/cnedfunction.h"
#include "omnetpp/cexception.h"

namespace omnetpp {

using namespace common;

class cComponent;

void nedpythonfunctions_dummy() {} //see util.cc

#ifdef WITH_PYTHON

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
        return cValue((omnetpp::intval_t)PyLong_AsLong(obj));
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

    return globals;
}

#endif // WITH_PYTHON


cValue nedf_pyeval(cComponent *contextComponent, cValue argv[], int argc)
{
#ifdef WITH_PYTHON
    try {
        ensurePythonInterpreter();

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
    }
    catch (cRuntimeError& e) {
        e.prependMessage("Error evaluating Python expression: ");
        throw;
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error evaluating Python expression: %s", e.what());
    }
#else
    throw cRuntimeError("Embedded Python support was not enabled at build time");
#endif
}

cValue nedf_pycode(cComponent *contextComponent, cValue argv[], int argc)
{
#ifdef WITH_PYTHON
    try {
        ensurePythonInterpreter();

        std::string code = argv[0].stringValue();

        std::smatch match;

        std::string ident = "([a-zA-Z_][a-zA-Z0-9_]*)";
        std::string identList = "(" + ident + "(\\s*,\\s*" + ident + ")*)?";
        std::regex_match(code, match, std::regex("(^\\s*" + identList + "\\s*:\\s*).*"));

        if (match.size() >= 2) {
            std::string header = match[1];
            std::string arglist = match[2];
            if (!std::regex_search(header, std::regex("\\btry\\b"))) {
                code = code.substr(header.length());
                code = "def fun(" + arglist + "):\n" + opp_indentlines(code, "    ");
            }
        }
        else {
            // Using *args, so we don't have to also create a list
            // inside the argument pack tuple when calling.
            code = "def fun(*args):\n" + opp_indentlines(code, "    ");
        }

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
    }
    catch (cRuntimeError& e) {
        e.prependMessage("Error executing Python code: ");
        throw;
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error executing Python code: %s", e.what());
    }
#else
    throw cRuntimeError("Embedded Python support was not enabled at build time");
#endif
}


Define_NED_Function2(nedf_pyeval, "any pyeval(string s, ...)", "python", "evaluates the string as a Python expression; or as if it was a Python lambda, then calls it");
Define_NED_Function2(nedf_pycode, "any pycode(string s, ...)", "python", "evaluates the string as if it was a Python function body");


}  // namespace omnetpp
