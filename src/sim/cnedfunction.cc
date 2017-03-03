//=========================================================================
//  CNEDFUNCTION.CC - part of
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
#include <sstream>
#include <algorithm>
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "common/opp_ctype.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/globals.h"
#include "omnetpp/cexception.h"

using namespace omnetpp::common;

namespace omnetpp {

cNEDFunction::cNEDFunction(NEDFunction f, const char *signature,
        const char *category, const char *description) :
    cNoncopyableOwnedObject(nullptr, false)
{
    ASSERT(f);

    this->f = f;
    this->signature = opp_nulltoempty(signature);
    this->category = opp_nulltoempty(category);
    this->description = opp_nulltoempty(description);

    parseSignature(signature);
}

static bool contains(const std::string& str, const std::string& substr)
{
    return str.find(substr) != std::string::npos;
}

static char parseType(const std::string& str)
{
    if (str == "bool")
        return 'B';
    if (str == "int" || str == "long")
        return 'L';
    if (str == "double")
        return 'D';
    if (str == "quantity")
        return 'Q';
    if (str == "string")
        return 'S';
    if (str == "xml")
        return 'X';
    if (str == "any")
        return '*';
    return 0;
}

static bool splitTypeAndName(const std::string& pair, char& type, std::string& name)
{
    std::vector<std::string> v = StringTokenizer(pair.c_str()).asVector();
    if (v.size() != 2)
        return false;

    type = parseType(v[0]);
    if (type == 0)
        return false;

    name = v[1];
    for (const char *s = name.c_str(); *s; s++)
        if (!opp_isalnum(*s) && *s != '_' && *s != '?')  // '?': optional arg
            return false;


    return true;
}

static const char *syntaxErrorMessage =
    "Define_NED_Function(): syntax error in signature \"%s\": "
    "should be <returntype> name(<argtype> argname, ...), "
    "where a type can be one of 'bool', 'int', 'double', 'quantity', "
    "'string', 'xml' and 'any'; names of optional args end in '?'; "
    "append ',...' to accept any number of additional args of any type";

void cNEDFunction::parseSignature(const char *signature)
{
    std::string str = opp_nulltoempty(signature);
    std::string typeAndName = opp_trim(opp_substringbefore(str, "("));
    char type;
    std::string name;
    if (!splitTypeAndName(typeAndName, type, name))
        throw cRuntimeError(syntaxErrorMessage, signature);
    setName(name.c_str());
    returnType = type;

    std::string rest = opp_trim(opp_substringafter(str, "("));
    bool missingRParen = !contains(rest, ")");
    std::string argList = opp_trim(opp_substringbefore(rest, ")"));
    std::string trailingGarbage = opp_trim(opp_substringafter(rest, ")"));
    if (missingRParen || trailingGarbage.size() != 0)
        throw cRuntimeError(syntaxErrorMessage, signature);

    minArgc = -1;
    hasVarargs_ = false;
    std::vector<std::string> args = StringTokenizer(argList.c_str(), ",").asVector();
    for (int i = 0; i < (int)args.size(); i++) {
        if (opp_trim(args[i]) == "...") {
            if (i != (int)args.size() - 1)
                throw cRuntimeError(syntaxErrorMessage, signature);  // "..." must be the last one
            hasVarargs_ = true;
        }
        else {
            char argType;
            std::string argName;
            if (!splitTypeAndName(args[i], argType, argName))
                throw cRuntimeError(syntaxErrorMessage, signature);
            argTypes += argType;
            if (contains(argName, "?") && minArgc == -1)
                minArgc = i;
        }
    }
    maxArgc = argTypes.size();
    if (minArgc == -1)
        minArgc = maxArgc;
}

void cNEDFunction::checkArgs(cNEDValue argv[], int argc)
{
    if (argc < minArgc || (argc > maxArgc && !hasVarargs_))
        throw cRuntimeError("%s: Called with wrong number of arguments", getName());

    int n = std::min(argc, maxArgc);
    for (int i = 0; i < n; i++) {
        char declType = argTypes[i];
        if (declType == 'D' || declType == 'L') {
            if (argv[i].type != cNEDValue::DBL)
                throw cRuntimeError(E_EBADARGS, getName());
            if (!opp_isempty(argv[i].getUnit()))
                throw cRuntimeError(E_DIMLESS, getName());  //XXX better msg! only arg i is dimless
        }
        else if (declType == 'Q') {
            if (argv[i].type != cNEDValue::DBL)
                throw cRuntimeError(E_EBADARGS, getName());
        }
        else if (declType != '*' && argv[i].type != declType) {
            throw cRuntimeError(E_EBADARGS, getName());
        }
    }
}

cNEDValue cNEDFunction::invoke(cComponent *context, cNEDValue argv[], int argc)
{
    checkArgs(argv, argc);
    return f(context, argv, argc);
}

std::string cNEDFunction::str() const
{
    return getSignature();
}

cNEDFunction *cNEDFunction::find(const char *name)
{
    return dynamic_cast<cNEDFunction *>(nedFunctions.getInstance()->find(name));
}

cNEDFunction *cNEDFunction::get(const char *name)
{
    cNEDFunction *p = find(name);
    if (!p)
        throw cRuntimeError("NED function \"%s\" not found -- perhaps it wasn't registered "
                            "with the Define_NED_Function() macro, or its code is not linked in",
                            name);
    return p;
}

cNEDFunction *cNEDFunction::findByPointer(NEDFunction f)
{
    cRegistrationList *a = nedFunctions.getInstance();
    for (int i = 0; i < a->size(); i++) {
        cNEDFunction *ff = dynamic_cast<cNEDFunction *>(a->get(i));
        if (ff && ff->getFunctionPointer() == f)
            return ff;
    }
    return nullptr;
}

}  // namespace omnetpp

