//=========================================================================
//  CNEDFUNCTION.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <string.h>
#include <sstream>
#include <algorithm>
#include "cnedfunction.h"
#include "globals.h"
#include "cexception.h"
#include "stringutil.h"
#include "stringtokenizer.h"
#include "opp_ctype.h"

NAMESPACE_BEGIN


cNEDFunction::cNEDFunction(NEDFunction f, const char *signature,
                           const char *category, const char *description) :
  cNoncopyableOwnedObject(NULL,false)
{
    ASSERT(f);
    signature = opp_nulltoempty(signature);

    this->f = f;
    this->sign = opp_nulltoempty(signature);

    this->categ = opp_nulltoempty(category);
    this->desc = opp_nulltoempty(description);

    parseSignature(signature);
}

static bool contains(const std::string& str, const std::string& substr)
{
    return str.find(substr) != std::string::npos;
}

static char parseType(const std::string& str)
{
    if (str=="bool")
        return 'B';
    if (str=="int" || str=="long")
        return 'L';
    if (str=="double")
        return 'D';
    if (str=="quantity")
        return 'Q';
    if (str=="string")
        return 'S';
    if (str=="xml")
        return 'X';
    if (str=="any")
        return '*';
    return 0;
}

static bool splitTypeAndName(const std::string& pair, char& type, std::string& name)
{
    std::vector<std::string> v = StringTokenizer(pair.c_str()).asVector();
    if (v.size()!=2)
        return false;

    type = parseType(v[0]);
    if (type == 0)
        return false;

    name = v[1];
    for (const char *s = name.c_str(); *s; s++)
        if (!opp_isalnum(*s) && *s != '_' && *s != '?') // '?': optional arg
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
    std::string typeAndName = opp_trim(opp_substringbefore(str, "(").c_str());
    char type;
    std::string name;
    if (!splitTypeAndName(typeAndName, type, name))
        throw cRuntimeError(syntaxErrorMessage, signature);
    setName(name.c_str());
    rettype = type;

    std::string rest = opp_trim(opp_substringafter(str, "(").c_str());
    bool missingRParen = !contains(rest, ")");
    std::string argList = opp_trim(opp_substringbefore(rest, ")").c_str());
    std::string trailingGarbage = opp_trim(opp_substringafter(rest, ")").c_str());
    if (missingRParen || trailingGarbage.size()!=0)
        throw cRuntimeError(syntaxErrorMessage, signature);

    minargc = -1;
    hasvarargs = false;
    std::vector<std::string> args = StringTokenizer(argList.c_str(), ",").asVector();
    for (int i=0; i < (int)args.size(); i++)
    {
        if (opp_trim(args[i].c_str()) == "...") {
            if (i != (int)args.size()-1)
                throw cRuntimeError(syntaxErrorMessage, signature); // "..." must be the last one
            hasvarargs = true;
        }
        else {
            char argType;
            std::string argName;
            if (!splitTypeAndName(args[i], argType, argName))
                throw cRuntimeError(syntaxErrorMessage, signature);
            argtypes += argType;
            if (contains(argName,"?") && minargc==-1)
                minargc = i;
        }
    }
    maxargc = argtypes.size();
    if (minargc==-1)
        minargc = maxargc;
}

void cNEDFunction::checkArgs(cNEDValue argv[], int argc)
{
    if (argc < minargc || (argc > maxargc && !hasvarargs))
        throw cRuntimeError("%s: called with wrong number of arguments", getName());

    int n = std::min(argc, maxargc);
    for (int i=0; i<n; i++) {
        char declType = argtypes[i];
        if (declType=='D' || declType=='L') {
            if (argv[i].type != cNEDValue::DBL)
                throw cRuntimeError(eEBADARGS, getName());
            if (!opp_isempty(argv[i].getUnit()))
                throw cRuntimeError(eDIMLESS, getName()); //XXX better msg! only arg i is dimless
        }
        else if (declType=='Q') {
            if (argv[i].type != cNEDValue::DBL)
                throw cRuntimeError(eEBADARGS, getName());
        }
        else if (declType!='*' && argv[i].type!=declType) {
            throw cRuntimeError(eEBADARGS, getName());
        }
    }
}

cNEDValue cNEDFunction::invoke(cComponent *context, cNEDValue argv[], int argc)
{
    checkArgs(argv, argc);
    return f(context, argv, argc);
}

std::string cNEDFunction::info() const
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
    for (int i=0; i<a->size(); i++) {
        cNEDFunction *ff = dynamic_cast<cNEDFunction *>(a->get(i));
        if (ff && ff->getFunctionPointer() == f)
            return ff;
    }
    return NULL;
}

NAMESPACE_END

