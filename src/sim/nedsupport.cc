//==========================================================================
//   NEDSUPPORT.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cdynamicexpression.h"
#include "cxmlelement.h"
#include "cnedmathfunction.h"
#include "cnedfunction.h"
#include "cexception.h"
#include "cenvir.h"
#include "cmodule.h"
#include "ccomponenttype.h"
#include "nedsupport.h"
#include "stringutil.h"

NAMESPACE_BEGIN

namespace NEDSupport {

ModuleIndex::ModuleIndex()
{
}

cNEDValue ModuleIndex::evaluate(cComponent *context, cNEDValue args[], int numargs)
{
    ASSERT(numargs==0 && context!=NULL);
    cModule *module = dynamic_cast<cModule *>(context);
    if (!module)
        throw cRuntimeError(context,"cannot evaluate `index' operator in expression: context is not a module");
    return (double) module->getIndex();
}

std::string ModuleIndex::str(std::string args[], int numargs)
{
    return "index";
}

//----

ParameterRef::ParameterRef(const char *paramName, bool ofParent, bool explicitKeyword)
{
    this->paramName = paramName;
    this->ofParent = ofParent;
    this->explicitKeyword = explicitKeyword;
}

cNEDValue ParameterRef::evaluate(cComponent *context, cNEDValue args[], int numargs)
{
    ASSERT(numargs==0 && context!=NULL);
    cComponent *component = ofParent ? context->getParentModule() : context;
    if (!component)
        throw cRuntimeError(context,eENOPARENT);

    // In inner types, a "paramName" should be first tried as the enclosing type's
    // parameter, only then as local parameter.
    // However, "this.paramName" (that is, ofParent==false and explicitKeyword==true)
    // means local parameter, so parent should not be looked up in that case
    if (!ofParent && !explicitKeyword && component->getComponentType()->isInnerType())
        if (component->getParentModule() && component->getParentModule()->hasPar(paramName.c_str()))
            return component->getParentModule()->par(paramName.c_str());

    return component->par(paramName.c_str());
}

std::string ParameterRef::str(std::string args[], int numargs)
{
    if (!explicitKeyword)
        return paramName;
    else if (ofParent)
        return std::string("parent.")+paramName; // not a legal NED syntax
    else
        return std::string("this.")+paramName;
}

//----

SiblingModuleParameterRef::SiblingModuleParameterRef(const char *moduleName, const char *paramName, bool ofParent, bool withModuleIndex)
{
    ASSERT(!opp_isempty(moduleName) && !opp_isempty(paramName) && opp_strcmp(moduleName,"this")!=0);
    this->moduleName = moduleName;
    this->paramName = paramName;
    this->ofParent = ofParent;
    this->withModuleIndex = withModuleIndex;
}

cNEDValue SiblingModuleParameterRef::evaluate(cComponent *context, cNEDValue args[], int numargs)
{
    ASSERT(context!=NULL);
    ASSERT(!withModuleIndex || (withModuleIndex && numargs==1 && args[0].type==cNEDValue::DBL));
    cModule *compoundModule = dynamic_cast<cModule *>(ofParent ? context->getParentModule() : context); // this works for channels too
    if (!compoundModule)
        throw cRuntimeError(context,eENOPARENT);
    int moduleIndex = withModuleIndex ? (int)args[0] : -1;
    cModule *siblingModule = compoundModule->getSubmodule(moduleName.c_str(), moduleIndex);
    if (!siblingModule) {
        std::string modName = moduleIndex==-1 ? moduleName : opp_stringf("%s[%d]", moduleName.c_str(), moduleIndex);
        throw cRuntimeError(context,"cannot find submodule for parameter `%s.%s'", modName.c_str(), paramName.c_str());
    }
    return siblingModule->par(paramName.c_str());
}

std::string SiblingModuleParameterRef::str(std::string args[], int numargs)
{
    if (withModuleIndex)
        return moduleName+"["+args[0]+"]."+paramName;
    else
        return moduleName+"."+paramName;
}

//----

const char *LoopVar::varNames[32];
long LoopVar::vars[32];
int LoopVar::varCount = 0;

long& LoopVar::pushVar(const char *varName)
{
    ASSERT(varCount<32);
    varNames[varCount] = varName;
    return vars[varCount++];
}

void LoopVar::popVar()
{
    ASSERT(varCount>0);
    varCount--;
}

void LoopVar::reset()
{
    varCount = 0;
}

cNEDValue LoopVar::evaluate(cComponent *context, cNEDValue args[], int numargs)
{
    ASSERT(numargs==0);
    const char *var = varName.c_str();
    for (int i=0; i<varCount; i++)
        if (strcmp(var, varNames[i])==0)
            return vars[i];
    throw cRuntimeError(context, "loop variable %s not found", varName.c_str());
}

std::string LoopVar::str(std::string args[], int numargs)
{
    // return varName;
    return std::string("(loopvar)")+varName;  //XXX debugging only
}

//---

Sizeof::Sizeof(const char *ident, bool ofParent, bool explicitKeyword)
{
    this->ident = ident;
    this->ofParent = ofParent;
    this->explicitKeyword = explicitKeyword;
}

cNEDValue Sizeof::evaluate(cComponent *context, cNEDValue args[], int numargs)
{
    ASSERT(numargs==0 && context!=NULL);
    cModule *module = dynamic_cast<cModule *>(ofParent ? context->getParentModule() : context);
    if (!module)
        throw cRuntimeError(context,eENOPARENT);

//FIXME stuff already implemented at the bottom of this file????

//FIXME decide it at buildtime, not now? (info is known then already!)
    // ident might be a gate vector of the *parent* module, or a sibling submodule vector
    // Note: it might NOT mean gate vector of this module
    if (module->hasGate(ident.c_str()))
    {
        return (long) module->gateSize(ident.c_str()); // returns 1 if it's not a vector
    }
    else
    {
        // Find ident among submodules. If there's no such submodule, it may
        // be that such submodule vector never existed, or can be that it's zero
        // size -- we cannot tell, so we have to return 0 (and cannot throw error).
        cModule *siblingModule = module->getSubmodule(ident.c_str(), 0); // returns NULL if submodule is not a vector
        if (!siblingModule && module->getSubmodule(ident.c_str()))
            return 1L; // return 1 if submodule exists but not a vector
        return (long) siblingModule ? siblingModule->size() : 0L;
    }

}

std::string Sizeof::str(std::string args[], int numargs)
{
    const char *prefix = !explicitKeyword ? "" : ofParent ? "parent." : "this."; // "parent" is not a legal NED syntax though
    return std::string("sizeof(") + prefix + ident + ")";
}

};

NAMESPACE_END

/*
//TODO make error messages consistent

typedef cNEDcNEDValue cNEDValue; // abbreviation for local use

//
// internal function to support NED: resolves a sizeof(moduleOrGateVectorName) reference
//
cNEDValue cDynamicExpression::getSizeofIdent(cComponent *context, cNEDValue args[], int numargs)
{
    ASSERT(numargs==1 && args[0].type==cNEDValue::STR);
    const char *ident = args[0].s.c_str();

    // ident might be a gate vector of the *parent* module, or a sibling submodule vector
    // Note: it might NOT mean gate vector of this module
    cModule *parentModule = dynamic_cast<cModule *>(context->getParentModule()); // this works for channels too
    if (!parentModule)
        throw cRuntimeError(context, "sizeof(%s) occurs in wrong context", ident);
    if (parentModule->hasGate(ident))
    {
        return (long) parentModule->gateSize(ident); // returns 1 if it's not a vector
    }
    else
    {
        // Find ident among submodules. If there's no such submodule, it may
        // be that such submodule vector never existed, or can be that it's zero
        // size -- we cannot tell, so we have to return 0.
        cModule *siblingModule = parentModule->getSubmodule(ident, 0); // returns NULL if submodule is not a vector
        if (!siblingModule && parentModule->getSubmodule(ident))
            return 1L; // return 1 if submodule exists but not a vector
        return (long) siblingModule ? siblingModule->size() : 0L;
    }
}

//
// internal function to support NED: resolves a sizeof(this.gateVectorName) reference
//
cNEDValue cDynamicExpression::getSizeofGate(cComponent *context, cNEDValue args[], int numargs)
{
    ASSERT(numargs==1 && args[0].type==cNEDValue::STR);
    const char *gateName = args[0].s.c_str();
    cModule *module = dynamic_cast<cModule *>(context);
    if (!module || !module->hasGate(gateName))
        throw cRuntimeError(context, "error evaluating sizeof(): no such gate: `%s'", gateName);
    return (long) module->gateSize(gateName); // returns 1 if it's not a vector
}

//
// internal function to support NED: resolves a sizeof(parent.gateVectorName) reference
//
cNEDValue cDynamicExpression::getSizeofParentModuleGate(cComponent *context, cNEDValue args[], int numargs)
{
    ASSERT(numargs==1 && args[0].type==cNEDValue::STR);
    const char *gateName = args[0].s.c_str();
    cModule *parentModule = dynamic_cast<cModule *>(context->getParentModule()); // this works for channels too
    if (!parentModule)
        throw cRuntimeError(context, "sizeof() occurs in wrong context", gateName);
    if (!parentModule->hasGate(gateName))
        throw cRuntimeError(context, "error evaluating sizeof(): no such gate: `%s'", gateName);
    return (long) parentModule->gateSize(gateName); // returns 1 if it's not a vector
}

//
// internal function to support NED: resolves a sizeof(module.gateName) reference
//
cNEDValue cDynamicExpression::getSizeofSiblingModuleGate(cComponent *context, cNEDValue args[], int numargs)
{
    ASSERT(numargs==2 && args[0].type==cNEDValue::STR && args[1].type==cNEDValue::STR);
    const char *siblingModuleName = args[0].s.c_str();
    const char *gateName = args[1].s.c_str();

    cModule *parentModule = dynamic_cast<cModule *>(context->getParentModule()); // this works for channels too
    if (!parentModule)
        throw cRuntimeError(context, "sizeof() occurs in wrong context", gateName);
    cModule *siblingModule = parentModule->getSubmodule(siblingModuleName); // returns NULL if submodule is not a vector
    if (!siblingModule->hasGate(gateName))
        throw cRuntimeError(context, "error evaluating sizeof(): no such gate: `%s'", gateName);
    return (long) siblingModule->gateSize(gateName); // returns 1 if it's not a vector
}

//
// internal function to support NED: resolves a sizeof(module.gateName) reference
//
cNEDValue cDynamicExpression::getSizeofIndexedSiblingModuleGate(cComponent *context, cNEDValue args[], int numargs)
{
    ASSERT(numargs==3 && args[0].type==cNEDValue::STR && args[1].type==cNEDValue::STR && args[2].type==cNEDValue::DBL);
    const char *gateName = args[1].s.c_str();
    const char *siblingModuleName = args[1].s.c_str();
    int siblingModuleIndex = (int)args[2].dbl;
    cModule *parentModule = dynamic_cast<cModule *>(context->getParentModule()); // this works for channels too
    cModule *siblingModule = parentModule ? parentModule->getSubmodule(siblingModuleName, siblingModuleIndex) : NULL;
    if (!siblingModule)
        throw cRuntimeError(context,"sizeof(): cannot find submodule %[%d]",
                                siblingModuleName, siblingModuleIndex,
                                siblingModuleName, siblingModuleIndex, gateName);
    return (long) siblingModule->gateSize(gateName); // returns 1 if it's not a vector
}
*/
