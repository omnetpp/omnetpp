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
#include "cenvir.h"
#include "cexception.h"
#include "onstartup.h"
#include "cpar.h"
#include "cmodule.h"
#include "regmacros.h"


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

//--------

//FIXME check arguments are popped in the right order!!!!
//FIXME make error messages consistent

typedef cDynamicExpression::StkValue StkValue; // abbreviation for local use

//
// internal function to support NED: resolves "index" operator
//
static StkValue __moduleIndex(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==0);
    cModule *module = dynamic_cast<cModule *>(context);
    if (!module)
        throw new cRuntimeError(context,"cannot evaluate `index' operator in expression: context is not a module");
    return (double) module->index();
}

Define_NED_Function(__moduleIndex, "", "L");


//
// internal function to support NED: resolves "index" of parent module (currently unused)
//
static StkValue __parentModuleIndex(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==0);
    cModule *parentModule = dynamic_cast<cModule *>(context->owner()); // this works for channels too
    if (!parentModule)
        throw new cRuntimeError(context,"cannot evaluate `index' operator in expression: context is not a module");
    return (double) parentModule->index();
}

Define_NED_Function(__parentModuleIndex, "", "L");


//
// internal function to support NED: resolves a parameter reference
//
static StkValue __parameter(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==1 && args[0].type==StkValue::STR);
    const char *paramName = args[0].str.c_str();
    cPar& par = context->par(paramName);
    return par;
}

Define_NED_Function(__parameter, "S", "*");


//
// internal function to support NED: resolves reference to parent module's parameter
//
static StkValue __parentParameter(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==1 && args[0].type==StkValue::STR);
    const char *paramName = args[0].str.c_str();
    cComponent *parent = dynamic_cast<cComponent *>(context->owner()); // this works for channels too
    if (!parent)
        throw new cRuntimeError(context,"cannot find parameter `%s'", paramName);
    cPar& par = parent->par(paramName);
    return par;
}

Define_NED_Function(__parentParameter, "S", "*");


//
// internal function to support NED: resolves a module.paramName reference
//
static StkValue __siblingModuleParameter(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==2 && args[0].type==StkValue::STR && args[1].type==StkValue::STR);
    const char *paramName = args[0].str.c_str();
    const char *siblingModuleName = args[1].str.c_str();
    cModule *parentModule = dynamic_cast<cModule *>(context->owner()); // this works for channels too
    cModule *siblingModule = parentModule ? parentModule->submodule(siblingModuleName) : NULL;
    if (!siblingModule)
        throw new cRuntimeError(context,"cannot find submodule `%' for parameter `%s.%s'",
                                siblingModuleName, siblingModuleName, paramName);
    cPar& par = siblingModule->par(paramName);
    return par;
}

Define_NED_Function(__siblingModuleParameter, "SS", "*");


//
// internal function to support NED: resolves a module[index].paramName reference
//
static StkValue __indexedSiblingModuleParameter(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==3 && args[0].type==StkValue::STR && args[1].type==StkValue::STR && args[2].type==StkValue::DBL);
    const char *paramName = args[0].str.c_str();
    const char *siblingModuleName = args[1].str.c_str();
    int siblingModuleIndex = (int)args[2].dbl;
    cModule *parentModule = dynamic_cast<cModule *>(context->owner()); // this works for channels too
    cModule *siblingModule = parentModule ? parentModule->submodule(siblingModuleName, siblingModuleIndex) : NULL;
    if (!siblingModule)
        throw new cRuntimeError(context,"cannot find submodule `%[%d]' for parameter `%s[%d].%s'",
                                siblingModuleName, siblingModuleIndex,
                                siblingModuleName, siblingModuleIndex, paramName);
    cPar& par = siblingModule->par(paramName);
    return par;
}

Define_NED_Function(__indexedSiblingModuleParameter, "SSL", "*");


//
// internal function to support NED: resolves a sizeof(moduleOrGateVectorName) reference
//
static StkValue __sizeofIdent(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==1 && args[0].type==StkValue::STR);
    const char *ident = args[0].str.c_str();

    // ident might be a gate vector of the *parent* module, or a sibling submodule vector
    // Note: it might NOT mean gate vector of this module
    cModule *parentModule = dynamic_cast<cModule *>(context->owner()); // this works for channels too
    if (!parentModule)
        throw new cRuntimeError(context, "sizeof(%s) occurs in wrong context", ident);
    if (parentModule->hasGate(ident))
    {
        return (long) parentModule->gateSize(ident); // returns 1 if it's not a vector
    }
    else
    {
        // Find ident among submodules. If there's no such submodule, it may
        // be that such submodule vector never existed, or can be that it's zero
        // size -- we cannot tell, so we have to return 0.
        cModule *siblingModule = parentModule->submodule(ident, 0); // returns NULL if submodule is not a vector
        if (!siblingModule && parentModule->submodule(ident))
            return 1L; // return 1 if submodule exists but not a vector
        return (long) siblingModule ? siblingModule->size() : 0L;
    }
}

Define_NED_Function(__sizeofIdent, "S", "L");


//
// internal function to support NED: resolves a sizeof(this.gateVectorName) reference
//
static StkValue __sizeofGate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==1 && args[0].type==StkValue::STR);
    const char *gateName = args[0].str.c_str();
    cModule *module = dynamic_cast<cModule *>(context);
    if (!module || !module->hasGate(gateName))
        throw new cRuntimeError(context, "error evaluating sizeof(): no such gate: `%s'", gateName);
    return (long) module->gateSize(gateName); // returns 1 if it's not a vector
}

Define_NED_Function(__sizeofGate, "S", "L");


//
// internal function to support NED: resolves a sizeof(parent.gateVectorName) reference
//
static StkValue __sizeofParentModuleGate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==1 && args[0].type==StkValue::STR);
    const char *gateName = args[0].str.c_str();
    cModule *parentModule = dynamic_cast<cModule *>(context->owner()); // this works for channels too
    if (!parentModule)
        throw new cRuntimeError(context, "sizeof() occurs in wrong context", gateName);
    if (!parentModule->hasGate(gateName))
        throw new cRuntimeError(context, "error evaluating sizeof(): no such gate: `%s'", gateName);
    return (long) parentModule->gateSize(gateName); // returns 1 if it's not a vector
}

Define_NED_Function(__sizeofParentModuleGate, "S", "L");


//
// internal function to support NED: resolves a sizeof(module.gateName) reference
//
StkValue __sizeofSiblingModuleGate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==2 && args[0].type==StkValue::STR && args[1].type==StkValue::STR);
    const char *siblingModuleName = args[0].str.c_str();
    const char *gateName = args[1].str.c_str();

    cModule *parentModule = dynamic_cast<cModule *>(context->owner()); // this works for channels too
    if (!parentModule)
        throw new cRuntimeError(context, "sizeof() occurs in wrong context", gateName);
    cModule *siblingModule = parentModule->submodule(siblingModuleName); // returns NULL if submodule is not a vector
    if (!siblingModule->hasGate(gateName))
        throw new cRuntimeError(context, "error evaluating sizeof(): no such gate: `%s'", gateName);
    return (long) siblingModule->gateSize(gateName); // returns 1 if it's not a vector
}

Define_NED_Function(__sizeofSiblingModuleGate, "SS", "L");

//
// internal function to support NED: resolves a sizeof(module.gateName) reference
//
StkValue __sizeofIndexedSiblingModuleGate(cComponent *context, StkValue args[], int numargs)
{
    ASSERT(numargs==3 && args[0].type==StkValue::STR && args[1].type==StkValue::STR && args[2].type==StkValue::DBL);
    const char *gateName = args[1].str.c_str();
    const char *siblingModuleName = args[1].str.c_str();
    int siblingModuleIndex = (int)args[2].dbl;
    cModule *parentModule = dynamic_cast<cModule *>(context->owner()); // this works for channels too
    cModule *siblingModule = parentModule ? parentModule->submodule(siblingModuleName, siblingModuleIndex) : NULL;
    if (!siblingModule)
        throw new cRuntimeError(context,"sizeof(): cannot find submodule %[%d]",
                                siblingModuleName, siblingModuleIndex,
                                siblingModuleName, siblingModuleIndex, gateName);
    return (long) siblingModule->gateSize(gateName); // returns 1 if it's not a vector
}

Define_NED_Function(__sizeofIndexedSiblingModuleGate, "SSL", "L");

