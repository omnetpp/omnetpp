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

USING_NAMESPACE


cNEDFunction::cNEDFunction(const char *name, NEDFunction f, const char *argtypes, const char *rettype) : cNoncopyableOwnedObject(name,false)
{
    this->f = f;

    if (!argtypes) argtypes = "";
    if (!rettype) rettype = "";
    if (strspn(argtypes,"BLDSX*")!=strlen(argtypes))
        throw cRuntimeError("Define_NED_Function(%s): invalid type character in argtypes string \"%s\"", name, argtypes);
    if (strlen(rettype)!=1 || strspn(rettype,"BLDSX*")!=strlen(rettype))
        throw cRuntimeError("Define_NED_Function(%s): invalid return type \"%s\"", name, argtypes);

    this->rettype = rettype[0];
    this->argtypes = argtypes;
    this->numargs = strlen(argtypes);
}

static const char *typeName(char t)
{
    switch (t)
    {
        case 'B': return "bool";
        case 'L': return "long";
        case 'D': return "double";
        case 'S': return "string";
        case 'X': return "cXMLElement";
        case '*': return "any";
        default:  return "?";
    }
}

std::string cNEDFunction::info() const
{
    std::stringstream out;
    out << "(";
    for (int i = 0; i < (int)strlen(argtypes.c_str()); i++)
        out << (i?",":"") << typeName(argtypes[i]);
    out << ") -> " << typeName(rettype);
    return out.str();
}

cNEDFunction *cNEDFunction::find(const char *name, int argcount)
{
    cSymTable *a = nedFunctions.instance();
    for (int i = 0; i< a->size(); i++)
    {
        cNEDFunction *f = dynamic_cast<cNEDFunction *>(a->get(i));
        if (f && f->isName(name) && f->numArgs()==argcount)
            return f;
    }
    return NULL;
}

cNEDFunction *cNEDFunction::findByPointer(NEDFunction f)
{
    cSymTable *a = nedFunctions.instance();
    for (int i=0; i<a->size(); i++)
    {
        cNEDFunction *ff = dynamic_cast<cNEDFunction *>(a->get(i));
        if (ff && ff->functionPointer() == f)
            return ff;
    }
    return NULL;
}


