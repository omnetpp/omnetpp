//==========================================================================
//   NEDSUPPORT.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/stringutil.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/nedsupport.h"

using namespace omnetpp::common;

namespace omnetpp {

namespace nedsupport {

ModuleIndex::ModuleIndex()
{
}

cNedValue ModuleIndex::evaluate(Context *context, cNedValue args[], int numargs)
{
    ASSERT(numargs == 0 && context != nullptr && context->component != nullptr);
    cModule *module = dynamic_cast<cModule *>(context->component);
    if (!module)
        throw cRuntimeError(context->component, "Cannot evaluate 'index' operator in expression: Context is not a module");
    return (intpar_t)module->getIndex();
}

std::string ModuleIndex::str(std::string args[], int numargs)
{
    return "index";
}

//----

Exists::Exists(const char *ident, bool ofParent, bool explicitKeyword)
{
    this->ident = ident;
    this->ofParent = ofParent;
    this->explicitKeyword = explicitKeyword;
}


cNedValue Exists::evaluate(Context *context, cNedValue args[], int numargs)
{
    ASSERT(numargs == 0 && context != nullptr && context->component != nullptr);
    cModule *module = ofParent ? context->component->getParentModule() : dynamic_cast<cModule *>(context->component);
    if (!module)
        throw cRuntimeError(context->component, E_ENOPARENT);

    // Find ident among submodules. If there's no such submodule, it may be that such
    // submodule vector never existed, or can be that it's zero size -- we cannot tell
    bool exists = module->getSubmodule(ident.c_str()) || module->getSubmodule(ident.c_str(), 0);
    return exists;
}

std::string Exists::str(std::string args[], int numargs)
{
    std::string prefix = !explicitKeyword ? "" : ofParent ? "parent." : "this.";
    return "exists(" + prefix + ident + ")";
}

//----

Typename::Typename()
{
}

cNedValue Typename::evaluate(Context *context, cNedValue args[], int numargs)
{
    ASSERT(numargs == 0 && context != nullptr && context->component != nullptr);
    NedExpressionContext *nedContext = dynamic_cast<NedExpressionContext*>(context);
    if (!nedContext)
        return context->component->getNedTypeName();
    switch(nedContext->role) {
        case NedExpressionContext::SUBMODULE_CONDITION: return nedContext->computedTypename;
        case NedExpressionContext::SUBMODULE_ARRAY_CONDITION: throw cRuntimeError("typename is not allowed in the condition of a submodule vector");
        default: return context->component->getNedTypeName();
    }

}

std::string Typename::str(std::string args[], int numargs)
{
    return "typename";
}

//----

ParameterRef::ParameterRef(const char *paramName, bool ofParent, bool explicitKeyword)
{
    this->paramName = paramName;
    this->ofParent = ofParent;
    this->explicitKeyword = explicitKeyword;
}

cNedValue ParameterRef::evaluate(Context *context, cNedValue args[], int numargs)
{
    ASSERT(numargs == 0 && context != nullptr && context->component != nullptr);
    cComponent *component = ofParent ? context->component->getParentModule() : context->component;
    if (!component)
        throw cRuntimeError(context->component, E_ENOPARENT);

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
    std::string prefix = !explicitKeyword ? "" : ofParent ? "parent." : "this.";
    return prefix + paramName;
}

//----

SiblingModuleParameterRef::SiblingModuleParameterRef(const char *moduleName, const char *paramName, bool ofParent, bool withModuleIndex)
{
    ASSERT(!opp_isempty(moduleName) && !opp_isempty(paramName) && omnetpp::opp_strcmp(moduleName, "this") != 0);
    this->moduleName = moduleName;
    this->paramName = paramName;
    this->ofParent = ofParent;
    this->withModuleIndex = withModuleIndex;
}

cNedValue SiblingModuleParameterRef::evaluate(Context *context, cNedValue args[], int numargs)
{
    ASSERT(context != nullptr && context->component != nullptr);
    ASSERT(!withModuleIndex || (withModuleIndex && numargs == 1 && args[0].type == cNedValue::INT));
    cModule *compoundModule = dynamic_cast<cModule *>(ofParent ? context->component->getParentModule() : context->component);  // this works for channels too
    if (!compoundModule)
        throw cRuntimeError(context->component, E_ENOPARENT);
    int moduleIndex = withModuleIndex ? (int)args[0] : -1;
    cModule *siblingModule = compoundModule->getSubmodule(moduleName.c_str(), moduleIndex);
    if (!siblingModule) {
        std::string modName = moduleIndex == -1 ? moduleName : opp_stringf("%s[%d]", moduleName.c_str(), moduleIndex);
        throw cRuntimeError(context->component, "Cannot find submodule for parameter '%s.%s'", modName.c_str(), paramName.c_str());
    }
    return siblingModule->par(paramName.c_str());
}

std::string SiblingModuleParameterRef::str(std::string args[], int numargs)
{
    if (withModuleIndex)
        return moduleName + "[" + args[0] + "]." + paramName;
    else
        return moduleName + "." + paramName;
}

//----

const char *LoopVar::varNames[32];
long LoopVar::vars[32];
int LoopVar::varCount = 0;

long& LoopVar::pushVar(const char *varName)
{
    ASSERT(varCount < 32);
    varNames[varCount] = varName;
    return vars[varCount++];
}

void LoopVar::popVar()
{
    ASSERT(varCount > 0);
    varCount--;
}

void LoopVar::reset()
{
    varCount = 0;
}

cNedValue LoopVar::evaluate(Context *context, cNedValue args[], int numargs)
{
    ASSERT(numargs == 0 && context != nullptr && context->component != nullptr);
    const char *var = varName.c_str();
    for (int i = 0; i < varCount; i++)
        if (strcmp(var, varNames[i]) == 0)
            return (intpar_t)vars[i];

    throw cRuntimeError(context->component, "Loop variable %s not found", varName.c_str());
}

std::string LoopVar::str(std::string args[], int numargs)
{
    // return varName;
    return std::string("(loopvar)") + varName;  // for debugging only
}

//---

Sizeof::Sizeof(const char *ident, bool ofParent, bool explicitKeyword)
{
    this->ident = ident;
    this->ofParent = ofParent;
    this->explicitKeyword = explicitKeyword;
}

cNedValue Sizeof::evaluate(Context *context, cNedValue args[], int numargs)
{
    ASSERT(numargs == 0 && context != nullptr && context->component != nullptr);
    cModule *module = dynamic_cast<cModule *>(ofParent ? context->component->getParentModule() : context->component);
    if (!module)
        throw cRuntimeError(context->component, E_ENOPARENT);

    // ident might be a gate vector of the *parent* module, or a sibling submodule vector
    // Note: it might NOT mean gate vector of this module
    if (module->hasGate(ident.c_str())) {
        return (intpar_t)module->gateSize(ident.c_str());  // returns 1 if it's not a vector
    }
    else {
        // Find ident among submodules. If there's no such submodule, it may
        // be that such submodule vector never existed, or can be that it's zero
        // size -- we cannot tell, so we have to return 0 (and cannot throw error).
        cModule *siblingModule = module->getSubmodule(ident.c_str(), 0);  // returns nullptr if submodule is not a vector
        if (!siblingModule && module->getSubmodule(ident.c_str()))
            return (intpar_t)1;  // return 1 if submodule exists but not a vector
        return (intpar_t)(siblingModule ? siblingModule->getVectorSize() : 0L);
    }
}

std::string Sizeof::str(std::string args[], int numargs)
{
    std::string prefix = !explicitKeyword ? "" : ofParent ? "parent." : "this.";
    return "sizeof(" + prefix + ident + ")";
}

};

}  // namespace omnetpp
