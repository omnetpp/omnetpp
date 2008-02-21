//=========================================================================
//  CNEDFUNCTION.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <string.h>
#include "cnedfunction.h"
#include "globals.h"
#include "cexception.h"
#include "stringutil.h"
#include "opp_ctype.h"

USING_NAMESPACE

//XXX also: wrap invocation of function, so that cNEDFunction checks arg types etc
//XXX also: old Define_Function be implemented with cNEDFunction (ie an adapter func)

cNEDFunction::cNEDFunction(const char *name, NEDFunction f, const char *signature) : cNoncopyableOwnedObject(name,false)
{
    ASSERT(f);
    signature = opp_nulltoempty(signature);

    this->f = f;
    this->sign = opp_nulltoempty(signature);

    parseSignature(signature);
}

void cNEDFunction::parseSignature(const char *signature)
{
    // parse argument list
    const char *s = opp_nulltoempty(signature);
    minargc = -1;
    for (; *s; s++) {
        char c = *s;
        if (strchr("BLDQSX*", c)!=NULL)
            argtypes += c;
        else if (c=='/')
            minargc = argtypes.size();
        else if (c=='-' && *(s+1)=='>') {
            s += 2;
            break;
        }
        else
            throw cRuntimeError("Define_NED_Function(%s): invalid character '%c' in signature \"%s\"", name(), c, signature);
    }
    maxargc = argtypes.size();
    if (minargc==-1)
        minargc = maxargc;

    // parse return type
    if (strchr("BLDQSX*", *s)==NULL)
        throw cRuntimeError("Define_NED_Function(%s): invalid return type '%c' in signature \"%s\"", name(), *s, signature);
    rettype = *s++;
    if (*s)
        throw cRuntimeError("Define_NED_Function(%s): trailing garbage in signature \"%s\"", name(), signature);
}

void cNEDFunction::checkArgs(cDynamicExpression::Value argv[], int argc)
{
    if (argc<minargc || argc>maxargc)
        throw cRuntimeError("%s: called with wrong number of arguments", name());

    for (int i=0; i<argc; i++) {
        char decltype = argtypes[i];
        if (decltype=='D' || decltype=='L') {
            if (argv[i].type != 'D')
                throw cRuntimeError(eEBADARGS, name());
            if (!opp_isempty(argv[i].dblunit))
                throw cRuntimeError(eDIMLESS, name()); //XXX better msg! only arg i is dimless
        }
        else if (decltype=='Q') {
            if (argv[i].type != 'D')
                throw cRuntimeError(eEBADARGS, name());
        }
        else if (decltype!='*' && argv[i].type!=decltype) {
            throw cRuntimeError(eEBADARGS, name());
        }
    }
}

cDynamicExpression::Value cNEDFunction::invoke(cComponent *context, cDynamicExpression::Value argv[], int argc)
{
    checkArgs(argv, argc);
    return f(context, argv, argc);
}

static const char *typeName(char t)
{
    switch (t)
    {
        case 'B': return "bool";
        case 'L': return "long";
        case 'D': return "double";
        case 'Q': return "quantity";
        case 'S': return "string";
        case 'X': return "cXMLElement";
        case '*': return "any";
        default:  return "?";
    }
}

std::string cNEDFunction::info() const
{
    return sign;
}

std::string cNEDFunction::detailedInfo() const
{
    std::stringstream out;
    out << "(";
    for (int i = 0; i < maxArgs(); i++)
        out << (i==minArgs() ? "[" : "") << (i?",":"") << typeName(argType(i));
    out << (minArgs()!=maxArgs() ? "]" : "") << ") -> " << typeName(returnType());
    return out.str();
}

cNEDFunction *cNEDFunction::find(const char *name, int argcount)
{
    cSymTable *a = nedFunctions.instance();
    for (int i=0; i<a->size(); i++) {
        cNEDFunction *f = dynamic_cast<cNEDFunction *>(a->get(i));
        if (f && f->isName(name) && f->minArgs()<=argcount && f->maxArgs()>=argcount)
            return f;
    }
    return NULL;
}

cNEDFunction *cNEDFunction::findByPointer(NEDFunction f)
{
    cSymTable *a = nedFunctions.instance();
    for (int i=0; i<a->size(); i++) {
        cNEDFunction *ff = dynamic_cast<cNEDFunction *>(a->get(i));
        if (ff && ff->functionPointer() == f)
            return ff;
    }
    return NULL;
}


