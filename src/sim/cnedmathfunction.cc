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
using namespace omnetpp::internal;

namespace omnetpp {

cNedMathFunction::cNedMathFunction(const char *name, MathFuncNoArg f, int ac, const char *category, const char *description) :
    cNoncopyableOwnedObject(name, false),
    f((MathFunc)f), argc(0), category(opp_nulltoempty(category)), description(opp_nulltoempty(description))
{
    if (ac != -1 && ac != 0)
        throw cRuntimeError("Register_NED_Math_Function() or cNedMathFunction: "
                            "attempt to register function \"%s\" with wrong "
                            "number of arguments %d, should be 0", name, ac);
}

cNedMathFunction::cNedMathFunction(const char *name, MathFunc1Arg f, int ac, const char *category, const char *description) :
    cNoncopyableOwnedObject(name, false),
    f((MathFunc)f), argc(1), category(opp_nulltoempty(category)), description(opp_nulltoempty(description))
{
    if (ac != -1 && ac != 1)
        throw cRuntimeError("Register_NED_Math_Function() or cNedMathFunction: "
                            "attempt to register function \"%s\" with wrong "
                            "number of arguments %d, should be 1", name, ac);
}

cNedMathFunction::cNedMathFunction(const char *name, MathFunc2Args f, int ac, const char *category, const char *description) :
    cNoncopyableOwnedObject(name, false),
    f((MathFunc)f), argc(2), category(opp_nulltoempty(category)), description(opp_nulltoempty(description))
{
    if (ac != -1 && ac != 2)
        throw cRuntimeError("Register_NED_Math_Function() or cNedMathFunction: "
                            "attempt to register function \"%s\" with wrong "
                            "number of arguments %d, should be 2", name, ac);
}

cNedMathFunction::cNedMathFunction(const char *name, MathFunc3Args f, int ac, const char *category, const char *description) :
    cNoncopyableOwnedObject(name, false),
    f((MathFunc)f), argc(3), category(opp_nulltoempty(category)), description(opp_nulltoempty(description))
{
    if (ac != -1 && ac != 3)
        throw cRuntimeError("Register_NED_Math_Function() or cNedMathFunction: "
                            "attempt to register function \"%s\" with wrong "
                            "number of arguments %d, should be 3", name, ac);
}

cNedMathFunction::cNedMathFunction(const char *name, MathFunc4Args f, int ac, const char *category, const char *description) :
    cNoncopyableOwnedObject(name, false),
    f((MathFunc)f), argc(4), category(opp_nulltoempty(category)), description(opp_nulltoempty(description))
{
    if (ac != -1 && ac != 4)
        throw cRuntimeError("Register_NED_Math_Function() or cNedMathFunction: "
                            "attempt to register function \"%s\" with wrong "
                            "number of arguments %d, should be 4", name, ac);
}

std::string cNedMathFunction::str() const
{
    std::stringstream out;
    out << "double " << getName() << "(";
    for (int i = 0; i < argc; i++)
        out << (i ? ", " : "") << "double";
    out << ")";
    return out.str();
}

MathFuncNoArg cNedMathFunction::getMathFuncNoArg() const
{
    if (argc != 0)
        throw cRuntimeError(this, "getMathFuncNoArg(): Arg count mismatch (argc=%d)", argc);
    return (MathFuncNoArg)f;
}

MathFunc1Arg cNedMathFunction::getMathFunc1Arg() const
{
    if (argc != 1)
        throw cRuntimeError(this, "getMathFunc1Arg(): Arg count mismatch (argc=%d)", argc);
    return (MathFunc1Arg)f;
}

MathFunc2Args cNedMathFunction::getMathFunc2Args() const
{
    if (argc != 2)
        throw cRuntimeError(this, "getMathFunc2Args(): Arg count mismatch (argc=%d)", argc);
    return (MathFunc2Args)f;
}

MathFunc3Args cNedMathFunction::getMathFunc3Args() const
{
    if (argc != 3)
        throw cRuntimeError(this, "getMathFunc3Args(): Arg count mismatch (argc=%d)", argc);
    return (MathFunc3Args)f;
}

MathFunc4Args cNedMathFunction::getMathFunc4Args() const
{
    if (argc != 4)
        throw cRuntimeError(this, "getMathFunc4Args(): Arg count mismatch (argc=%d)", argc);
    return (MathFunc4Args)f;
}

cNedMathFunction *cNedMathFunction::find(const char *name, int argcount)
{
    cRegistrationList *a = nedFunctions.getInstance();
    for (int i = 0; i < a->size(); i++) {
        cNedMathFunction *f = dynamic_cast<cNedMathFunction *>(a->get(i));
        if (f && f->isName(name) && f->getNumArgs() == argcount)
            return f;
    }
    return nullptr;
}

cNedMathFunction *cNedMathFunction::get(const char *name, int argcount)
{
    cNedMathFunction *p = find(name, argcount);
    if (!p)
        throw cRuntimeError("Math function \"%s\" with %d args not found -- perhaps it wasn't registered "
                            "with the Register_Function() macro, or its code is not linked in",
                            name, argcount);
    return p;
}

cNedMathFunction *cNedMathFunction::findByPointer(MathFunc f)
{
    cRegistrationList *a = nedFunctions.getInstance();
    for (int i = 0; i < a->size(); i++) {
        cNedMathFunction *ff = dynamic_cast<cNedMathFunction *>(a->get(i));
        if (ff && ff->getMathFunc() == f)
            return ff;
    }
    return nullptr;
}

}  // namespace omnetpp

