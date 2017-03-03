//=========================================================================
//  CMATHFUNCTION.CC - part of
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

#include <cmath>
#include <cstring>
#include <cstdarg>
#include "common/stringutil.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/globals.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cexception.h"

using namespace omnetpp::common;

namespace omnetpp {

cNEDMathFunction::cNEDMathFunction(const char *name, MathFuncNoArg f, int ac, const char *category, const char *description) :
    cNoncopyableOwnedObject(name, false)
{
    this->category = opp_nulltoempty(category);
    this->description = opp_nulltoempty(description);
    this->f = (MathFunc)f;
    argc = 0;
    if (ac != -1 && ac != 0)
        throw cRuntimeError("Register_NED_Math_Function() or cNEDMathFunction: "
                            "attempt to register function \"%s\" with wrong "
                            "number of arguments %d, should be 0", name, ac);
}

cNEDMathFunction::cNEDMathFunction(const char *name, MathFunc1Arg f, int ac, const char *category, const char *description) :
    cNoncopyableOwnedObject(name, false)
{
    this->category = opp_nulltoempty(category);
    this->description = opp_nulltoempty(description);
    this->f = (MathFunc)f;
    argc = 1;
    if (ac != -1 && ac != 1)
        throw cRuntimeError("Register_NED_Math_Function() or cNEDMathFunction: "
                            "attempt to register function \"%s\" with wrong "
                            "number of arguments %d, should be 1", name, ac);
}

cNEDMathFunction::cNEDMathFunction(const char *name, MathFunc2Args f, int ac, const char *category, const char *description) :
    cNoncopyableOwnedObject(name, false)
{
    this->category = opp_nulltoempty(category);
    this->description = opp_nulltoempty(description);
    this->f = (MathFunc)f;
    argc = 2;
    if (ac != -1 && ac != 2)
        throw cRuntimeError("Register_NED_Math_Function() or cNEDMathFunction: "
                            "attempt to register function \"%s\" with wrong "
                            "number of arguments %d, should be 2", name, ac);
}

cNEDMathFunction::cNEDMathFunction(const char *name, MathFunc3Args f, int ac, const char *category, const char *description) :
    cNoncopyableOwnedObject(name, false)
{
    this->category = opp_nulltoempty(category);
    this->description = opp_nulltoempty(description);
    this->f = (MathFunc)f;
    argc = 3;
    if (ac != -1 && ac != 3)
        throw cRuntimeError("Register_NED_Math_Function() or cNEDMathFunction: "
                            "attempt to register function \"%s\" with wrong "
                            "number of arguments %d, should be 3", name, ac);
}

cNEDMathFunction::cNEDMathFunction(const char *name, MathFunc4Args f, int ac, const char *category, const char *description) :
    cNoncopyableOwnedObject(name, false)
{
    this->category = opp_nulltoempty(category);
    this->description = opp_nulltoempty(description);
    this->f = (MathFunc)f;
    argc = 4;
    if (ac != -1 && ac != 4)
        throw cRuntimeError("Register_NED_Math_Function() or cNEDMathFunction: "
                            "attempt to register function \"%s\" with wrong "
                            "number of arguments %d, should be 4", name, ac);
}

std::string cNEDMathFunction::str() const
{
    std::stringstream out;
    out << "double " << getName() << "(";
    for (int i = 0; i < argc; i++)
        out << (i ? ", " : "") << "double";
    out << ")";
    return out.str();
}

MathFuncNoArg cNEDMathFunction::getMathFuncNoArg() const
{
    if (argc != 0)
        throw cRuntimeError(this, "getMathFuncNoArg(): Arg count mismatch (argc=%d)", argc);
    return (MathFuncNoArg)f;
}

MathFunc1Arg cNEDMathFunction::getMathFunc1Arg() const
{
    if (argc != 1)
        throw cRuntimeError(this, "getMathFunc1Arg(): Arg count mismatch (argc=%d)", argc);
    return (MathFunc1Arg)f;
}

MathFunc2Args cNEDMathFunction::getMathFunc2Args() const
{
    if (argc != 2)
        throw cRuntimeError(this, "getMathFunc2Args(): Arg count mismatch (argc=%d)", argc);
    return (MathFunc2Args)f;
}

MathFunc3Args cNEDMathFunction::getMathFunc3Args() const
{
    if (argc != 3)
        throw cRuntimeError(this, "getMathFunc3Args(): Arg count mismatch (argc=%d)", argc);
    return (MathFunc3Args)f;
}

MathFunc4Args cNEDMathFunction::getMathFunc4Args() const
{
    if (argc != 4)
        throw cRuntimeError(this, "getMathFunc4Args(): Arg count mismatch (argc=%d)", argc);
    return (MathFunc4Args)f;
}

cNEDMathFunction *cNEDMathFunction::find(const char *name, int argcount)
{
    cRegistrationList *a = nedFunctions.getInstance();
    for (int i = 0; i < a->size(); i++) {
        cNEDMathFunction *f = dynamic_cast<cNEDMathFunction *>(a->get(i));
        if (f && f->isName(name) && f->getNumArgs() == argcount)
            return f;
    }
    return nullptr;
}

cNEDMathFunction *cNEDMathFunction::get(const char *name, int argcount)
{
    cNEDMathFunction *p = find(name, argcount);
    if (!p)
        throw cRuntimeError("Math function \"%s\" with %d args not found -- perhaps it wasn't registered "
                            "with the Register_Function() macro, or its code is not linked in",
                            name, argcount);
    return p;
}

cNEDMathFunction *cNEDMathFunction::findByPointer(MathFunc f)
{
    cRegistrationList *a = nedFunctions.getInstance();
    for (int i = 0; i < a->size(); i++) {
        cNEDMathFunction *ff = dynamic_cast<cNEDMathFunction *>(a->get(i));
        if (ff && ff->getMathFunc() == f)
            return ff;
    }
    return nullptr;
}

}  // namespace omnetpp

