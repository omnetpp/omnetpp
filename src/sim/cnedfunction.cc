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


cNEDFunction::cNEDFunction(const char *name, NEDFunction f, const char *argtypes, const char *rettype) : cNoncopyableObject(name,false)
{
    this->f = f;

    if (!argtypes) argtypes = "";
    if (!rettype) rettype = "";
    if (strspn(argtypes,"BLDSX*")!=strlen(argtypes))
        throw new cRuntimeError("Define_NED_Function(%s): invalid type character in argtypes string \"%s\"", name, argtypes);
    if (strlen(rettype)!=1 || strspn(rettype,"BLDSX*")!=strlen(rettype))
        throw new cRuntimeError("Define_NED_Function(%s): invalid return type \"%s\"", name, argtypes);

    this->rettype = rettype[0];
    this->argtypes = argtypes;
    this->numargs = strlen(argtypes);
}

cNEDFunction *cNEDFunction::find(const char *name, int argcount)
{
    cArray *a = nedFunctions.instance();
    for (int i=0; i<a->items(); i++)
    {
        cNEDFunction *f = dynamic_cast<cNEDFunction *>(a->get(i));
        if (f && f->isName(name) && f->numArgs()==argcount)
            return f;
    }
    return NULL;
}

cNEDFunction *cNEDFunction::findByPointer(NEDFunction f)
{
    cArray *a = nedFunctions.instance();
    for (int i=0; i<a->items(); i++)
    {
        cNEDFunction *ff = dynamic_cast<cNEDFunction *>(a->get(i));
        if (ff && ff->functionPointer() == f)
            return ff;
    }
    return NULL;
}


