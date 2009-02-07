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
#include "cnedfunction.h"
#include "globals.h"
#include "cexception.h"
#include "stringutil.h"
#include "stringtokenizer.h"
#include "opp_ctype.h"

USING_NAMESPACE

//XXX also: wrap invocation of function, so that cNEDFunction checks arg types etc
//XXX also: old Define_Function be implemented with cNEDFunction (ie an adapter func)

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

static std::string substringBefore(const std::string& str, const std::string& substr)
{
    size_t pos = str.find(substr);
    return pos==std::string::npos ? "" : str.substr(0,pos);
}

static std::string substringAfter(const std::string& str, const std::string& substr)
{
    size_t pos = str.find(substr);
    return pos==std::string::npos ? "" : str.substr(pos+substr.size());
}

static char parseType(const std::string& str)
{
    if (str=="bool")
        return 'B';
    if (str=="long")
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
    name = v[1];
    return type!=0;
}

static const char *syntaxErrorMessage =
        "Define_NED_Function(): syntax error in signature \"%s\": "
        "should be <rettype> name(<argtype> argname,...), "
        "where types can be bool, long, double, quantity, string, xml, any; "
        "names of optional args end in '?'";

void cNEDFunction::parseSignature(const char *signature)
{
    std::string str = opp_nulltoempty(signature);
    std::string typeAndName = opp_trim(substringBefore(str, "(").c_str());
    char type;
    std::string name;
    if (!splitTypeAndName(typeAndName, type, name))
        throw cRuntimeError(syntaxErrorMessage, signature);
    setName(name.c_str());
    rettype = type;

    std::string rest = opp_trim(substringAfter(str, "(").c_str());
    bool missingRParen = !contains(rest, ")");
    std::string argList = opp_trim(substringBefore(rest, ")").c_str());
    std::string trailingGarbage = opp_trim(substringAfter(rest, ")").c_str());
    if (missingRParen || trailingGarbage.size()!=0)
        throw cRuntimeError(syntaxErrorMessage, signature);

    minargc = -1;
    std::vector<std::string> args = StringTokenizer(argList.c_str(), ",").asVector();
    for (int i=0; i < (int)args.size(); i++)
    {
        char argType;
        std::string argName;
        if (!splitTypeAndName(args[i], argType, argName))
            throw cRuntimeError(syntaxErrorMessage, signature);
        argtypes += argType;
        if (contains(argName,"?") && minargc==-1)
            minargc = i;
    }
    maxargc = argtypes.size();
    if (minargc==-1)
        minargc = maxargc;
}

void cNEDFunction::checkArgs(cDynamicExpression::Value argv[], int argc)
{
    if (argc<minargc || argc>maxargc)
        throw cRuntimeError("%s: called with wrong number of arguments", getName());

    for (int i=0; i<argc; i++) {
        char declType = argtypes[i];
        if (declType=='D' || declType=='L') {
            if (argv[i].type != 'D')
                throw cRuntimeError(eEBADARGS, getName());
            if (!opp_isempty(argv[i].dblunit))
                throw cRuntimeError(eDIMLESS, getName()); //XXX better msg! only arg i is dimless
        }
        else if (declType=='Q') {
            if (argv[i].type != 'D')
                throw cRuntimeError(eEBADARGS, getName());
        }
        else if (declType!='*' && argv[i].type!=declType) {
            throw cRuntimeError(eEBADARGS, getName());
        }
    }
}

cDynamicExpression::Value cNEDFunction::invoke(cComponent *context, cDynamicExpression::Value argv[], int argc)
{
    checkArgs(argv, argc);
    return f(context, argv, argc);
}

static const char *getTypeName(char t)
{
    switch (t)
    {
        case 'B': return "bool";
        case 'L': return "long";
        case 'D': return "double";
        case 'Q': return "quantity";
        case 'S': return "string";
        case 'X': return "xml";
        case '*': return "any";
        default:  return "?";
    }
}

std::string cNEDFunction::info() const
{
    return getSignature();
}

cNEDFunction *cNEDFunction::find(const char *name, int argcount)
{
    cRegistrationList *a = nedFunctions.getInstance();
    for (int i=0; i<a->size(); i++) {
        cNEDFunction *f = dynamic_cast<cNEDFunction *>(a->get(i));
        if (f && f->isName(name) && f->getMinArgs()<=argcount && f->getMaxArgs()>=argcount)
            return f;
    }
    return NULL;
}

cNEDFunction *cNEDFunction::get(const char *name, int argcount)
{
    cNEDFunction *p = find(name, argcount);
    if (!p)
        throw cRuntimeError("NED function \"%s\" with %d args not found -- perhaps it wasn't registered "
                            "with the Define_NED_Function() macro, or its code is not linked in",
                            name, argcount);
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


