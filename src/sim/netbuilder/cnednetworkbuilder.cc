//==========================================================================
//   CNEDNETWORKBUILDER.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <algorithm>

#include "cmodule.h"
#include "cgate.h"
#include "cchannel.h"
#include "cdataratechannel.h"
#include "ccomponenttype.h"
#include "clongparimpl.h"
#include "cboolparimpl.h"
#include "cstringparimpl.h"
#include "cdisplaystring.h"
#include "cconfiguration.h"
#include "cconfigoption.h"
#include "cenvir.h"

#include "nedelements.h"
#include "nederror.h"

#include "nedparser.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedsyntaxvalidator.h"
#include "nedutil.h"
#include "xmlgenerator.h"  // for debugging

#include "cnednetworkbuilder.h"
#include "cnedloader.h"
#include "cexpressionbuilder.h"
#include "nedsupport.h"
#include "commonutil.h"  // TRACE()
#include "stringutil.h"
#include "patternmatcher.h"

NAMESPACE_BEGIN

Register_PerRunConfigOption(CFGID_MAX_MODULE_NESTING, "max-module-nesting", CFG_INT, "50", "The maximum allowed depth of submodule nesting. This is used to catch accidental infinite recursions in NED.");
Register_PerObjectConfigOption(CFGID_TYPENAME, "typename", KIND_UNSPECIFIED_TYPE, CFG_STRING, NULL, "Specifies type for submodules and channels declared with 'like <>'.");


#if 0
// for debugging
static void dump(NEDElement *node)
{
    generateXML(std::cout, node, false);
    std::cout.flush();
}
#endif

// utility function for exception handling: adds NED file+line to the exception text
static void updateOrRethrowException(std::exception& e, NEDElement *context)
{
    const char *loc = context ? context->getSourceLocation() : NULL;
    if (!opp_isempty(loc))
    {
        std::string msg = std::string(e.what()) + ", at " + loc;
        cException *ce = dynamic_cast<cException *>(&e);
        if (ce)
           ce->setMessage(msg.c_str());
        else
           throw cRuntimeError("%s", msg.c_str()); // cannot set msg on existing exception object, throw new one
    }
}

void cNEDNetworkBuilder::addParametersAndGatesTo(cComponent *component, cNEDDeclaration *decl)
{
    cContextSwitcher __ctx(component); // params need to be evaluated in the module's context FIXME needed???
    doAddParametersAndGatesTo(component, decl);

    // assign submodule/connection parameters using parent module NED declaration as well
    cModule *parentModule = component->getParentModule();
    if (parentModule)
    {
        const char *parentNedTypeName = parentModule->getNedTypeName();
        cNEDDeclaration *parentDecl = cNEDLoader::getInstance()->getDecl(parentNedTypeName);
        if (parentDecl)  // i.e. parent was created via NED-based componentType
        {
            NEDElement *subcomponentNode = component->isModule() ?
                (NEDElement *)parentDecl->getSubmoduleElement(component->getName()) :
                (NEDElement *)parentDecl->getConnectionElement(((cChannel*)component)->getNedConnectionElementId());
            if (subcomponentNode)
                assignSubcomponentParams(component, subcomponentNode);
        }
    }

    assignParametersFromPatterns(component);
}

void cNEDNetworkBuilder::doAddParametersAndGatesTo(cComponent *component, cNEDDeclaration *decl)
{
    //TODO for performance: component->reallocParamv(decl->getParameterDeclarations().size());

    // recursively add and assign super types' parameters
    if (decl->numExtendsNames() > 0)
    {
        const char *superName = decl->extendsName(0);
        cNEDDeclaration *superDecl = cNEDLoader::getInstance()->getDecl(superName);
        doAddParametersAndGatesTo(component, superDecl);
    }

    // add this decl parameters / assignments
    ParametersElement *paramsNode = decl->getParametersElement();
    if (paramsNode)
    {
        currentDecl = decl; // switch "context"
        doParams(component, paramsNode, false);
    }

    // add this decl's gates (if there are any)
    GatesElement *gatesNode = decl->getGatesElement();
    if (gatesNode)
    {
        currentDecl = decl; // switch "context"
        doGates((cModule *)component, gatesNode, false);
    }
}

cPar::Type cNEDNetworkBuilder::translateParamType(int t)
{
    return t==NED_PARTYPE_DOUBLE ? cPar::DOUBLE :
           t==NED_PARTYPE_INT ? cPar::LONG :
           t==NED_PARTYPE_STRING ? cPar::STRING :
           t==NED_PARTYPE_BOOL ? cPar::BOOL :
           t==NED_PARTYPE_XML ? cPar::XML :
           (cPar::Type)-1;
}

cGate::Type cNEDNetworkBuilder::translateGateType(int t)
{
    return t==NED_GATETYPE_INPUT ? cGate::INPUT :
           t==NED_GATETYPE_OUTPUT ? cGate::OUTPUT :
           t==NED_GATETYPE_INOUT ? cGate::INOUT :
           (cGate::Type)-1;
}

void cNEDNetworkBuilder::doParams(cComponent *component, ParametersElement *paramsNode, bool isSubcomponent)
{
    ASSERT(paramsNode!=NULL);
    for (ParamElement *paramNode=paramsNode->getFirstParamChild(); paramNode; paramNode=paramNode->getNextParamSibling())
        if (!paramNode->getIsPattern())
            doParam(component, paramNode, isSubcomponent);
}

void cNEDNetworkBuilder::doParam(cComponent *component, ParamElement *paramNode, bool isSubcomponent)
{
    ASSERT(!paramNode->getIsPattern()); // we only deal with non-pattern assignments

    try {
        const char *paramName = paramNode->getName();

        // isSubComponent==false: we are called from cModuleType::addParametersAndGatesTo();
        // isSubComponent==true: we are called from assignSubcomponentParams().
        // if type==NONE, this is an inherited parameter (must have been added already)
        bool isNewParam = !isSubcomponent && paramNode->getType()!=NED_PARTYPE_NONE;

        // try to find an impl object with this value; we'll reuse it to optimize memory consumption
        cParImpl *impl = currentDecl->getSharedParImplFor(paramNode);
        if (impl)
        {
            // reuse impl for this parameter: we've already been at this point
            // in the NED files sometime, so we can reuse what we produced then
            ASSERT(impl->isName(paramName));
            if (isNewParam)
                component->addPar(impl);
            else {
                cPar& par = component->par(paramName); // must exist already
                if (par.isSet())
                    throw cRuntimeError(component, "Cannot overwrite non-default value of parameter `%s'", paramName);
                par.setImpl(impl);
            }
            return;
        }

        ASSERT(impl==NULL);
        if (isNewParam) {
            // adding parameter
            impl = cParImpl::createWithType(translateParamType(paramNode->getType()));
            impl->setName(paramName);
            impl->setIsShared(false);
            impl->setIsVolatile(paramNode->getIsVolatile());

            component->addPar(impl);

            cProperties *paramProps = component->par(paramName).getProperties();
            cProperty *unitProp = paramProps->get("unit");
            const char *declUnit = unitProp ? unitProp->getValue(cProperty::DEFAULTKEY) : NULL;
            impl->setUnit(declUnit);
        }
        else {
            // parameter must exist already. Un-share it so that we can modify its value
            cPar& par = component->par(paramName);
            if (par.isSet())
                throw cRuntimeError(component, "Cannot overwrite non-default value of parameter `%s'", paramName);
            impl = par.copyIfShared();
        }

        ASSERT(!impl->isSet());  // we do not overwrite assigned values

        // put new value into impl
        ExpressionElement *exprNode = paramNode->getFirstExpressionChild();
        if (exprNode)
        {
            ASSERT(impl==component->par(paramName).impl() && !impl->isShared());
            cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, isSubcomponent);
            cExpressionBuilder::setExpression(impl, dynamicExpr);
            impl->setIsSet(!paramNode->getIsDefault());
        }
        else if (paramNode->getIsDefault())
        {
            // default, but no value expression: set parameter to its existing default value
            //
            // Note: this branch ("=default" in NED files) is currently not supported,
            // because it would be complicated to implement in the Inifile Editor.
            //
            if (!impl->containsValue())
                throw cRuntimeError(component, "Cannot apply default value to parameter `%s': it has no default value", paramName);
            impl->setIsSet(true);
        }
        impl->setIsShared(true);
        currentDecl->putSharedParImplFor(paramNode, impl);

    }
    catch (std::exception& e) {
        updateOrRethrowException(e, paramNode); throw;
    }
}

void cNEDNetworkBuilder::doGates(cModule *module, GatesElement *gatesNode, bool isSubcomponent)
{
    ASSERT(gatesNode!=NULL);
    for (NEDElement *child=gatesNode->getFirstChildWithTag(NED_GATE); child; child=child->getNextSiblingWithTag(NED_GATE))
        doGate(module, (GateElement *)child, isSubcomponent);
}

void cNEDNetworkBuilder::doGate(cModule *module, GateElement *gateNode, bool isSubcomponent)
{
    try {
        // add gate if it's declared here
        if (!isSubcomponent && gateNode->getType()!=NED_GATETYPE_NONE)
            module->addGate(gateNode->getName(), translateGateType(gateNode->getType()), gateNode->getIsVector());
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, gateNode); throw;
    }
}

void cNEDNetworkBuilder::assignParametersFromPatterns(cComponent *component)
{
    // go up the parent chain, and apply the parameter pattern assignments
    // to matching, still-unset parameters
    std::string prefix;

    for (cComponent *child = component; true; child = child->getParentModule())
    {
        cModule *parent = child->getParentModule();
        if (!parent)
            break;

        // check patterns in compound module "parent"

        // find NED declaration. Note that decl may be NULL (if parent was not defined via NED but created dynamically)
        const char *nedTypeName = parent->getNedTypeName();
        cNEDDeclaration *decl = cNEDLoader::getInstance()->getDecl(nedTypeName);

        // first, check patterns in the "submodules:" section
        if (decl && !prefix.empty())
        {
            const std::vector<PatternData>& submodPatterns = decl->getSubmoduleParamPatterns(child->getName());
            if (!submodPatterns.empty())
                doAssignParametersFromPatterns(component, prefix, submodPatterns, true, child);
        }

        // for checking the patterns on the compound module, prefix with submodule name
        if (child->isModule())
            prefix = std::string(child->getFullName()) + "." + prefix;
        else {
            // channel: its path includes the connection source gate's name
            cObject *srcGate = child->getOwner();
            ASSERT(srcGate == ((cChannel*)child)->getSourceGate());
            prefix = std::string(srcGate->getFullName()) + "."  + child->getFullName() + "." + prefix; // prepend with "<srcgate>.channel."
            if (srcGate->getOwner() != parent)
                prefix = std::string(srcGate->getOwner()->getFullName()) + "."  + prefix; // also prepend with "<submod>."
        }

        // check patterns on the compound module itself
        if (decl)
        {
            const std::vector<PatternData>& patterns = decl->getParamPatterns();
            if (!patterns.empty())
                doAssignParametersFromPatterns(component, prefix, patterns, false, parent);
        }
    }
}

void cNEDNetworkBuilder::doAssignParametersFromPatterns(cComponent *component, const std::string& prefix, const std::vector<PatternData>& patterns, bool isInSubcomponent, cComponent *evalContext)
{
    int numPatterns = patterns.size();
    int numParams = component->getNumParams();
    for (int i=0; i<numParams; i++) {
        cPar& par = component->par(i);
        if (!par.isSet()) {
            // first match
            std::string paramPath = prefix + par.getFullName();
            for (int j=0; j<numPatterns; j++) {
                if (patterns[j].matcher->matches(paramPath.c_str())) {
                    // pattern matches the parameter's path, assign the value
                    doAssignParameterFromPattern(par, patterns[j].patternNode, isInSubcomponent, evalContext);
                    if (par.isSet())
                        break;
                }
            }
        }
    }
}

void cNEDNetworkBuilder::doAssignParameterFromPattern(cPar& par, ParamElement *patternNode, bool isInSubcomponent, cComponent *evalContext)
{
    // note: this code should look similar to relevant part of doParam()
    try {
        ASSERT(patternNode->getIsPattern());
        cParImpl *impl = par.copyIfShared();
        ExpressionElement *exprNode = patternNode->getFirstExpressionChild();
        if (exprNode)
        {
            // assign the parameter
            ASSERT(impl==par.impl() && !impl->isShared());
            cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, isInSubcomponent);
            cExpressionBuilder::setExpression(impl, dynamicExpr);
            par.setEvaluationContext(evalContext);
            impl->setIsSet(!patternNode->getIsDefault());
        }
        else if (patternNode->getIsDefault())
        {
            // no expression: set parameter to its existing default value ("par=default;" syntax)
            //
            // Note: this branch ("=default" in NED files) is currently not supported,
            // because it would be complicated to implement in the Inifile Editor.
            if (!impl->containsValue())
                throw cRuntimeError(par.getOwner(), "Cannot apply default value to parameter `%s': it has no default value", par.getName());
            impl->setIsSet(true);
        }
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, patternNode); throw;
    }
}

cModule *cNEDNetworkBuilder::_submodule(cModule *, const char *submodName, int idx)
{
    SubmodMap::iterator i = submodMap.find(std::string(submodName));
    if (i==submodMap.end())
        return NULL;

    ModulePtrVector& v = i->second;
    if (idx < 0)
        return (v.size()!=1 || v[0]->isVector()) ? NULL : v[0];
    else
        return ((unsigned)idx>=v.size()) ? NULL : v[idx];
}

void cNEDNetworkBuilder::doGateSizes(cModule *module, GatesElement *gatesNode, bool isSubcomponent)
{
    ASSERT(gatesNode!=NULL);
    for (NEDElement *child=gatesNode->getFirstChildWithTag(NED_GATE); child; child=child->getNextSiblingWithTag(NED_GATE))
        doGateSize(module, (GateElement *)child, isSubcomponent);
}

void cNEDNetworkBuilder::doGateSize(cModule *module, GateElement *gateNode, bool isSubcomponent)
{
    try {
        if (gateNode->getIsVector()) {
            // if there's a gatesize expression given, apply it
            ExpressionElement *exprNode = gateNode->getFirstExpressionChild();
            if (exprNode)
            {
                // ExpressionElement first needs to be compiled into a cParImpl
                // for evaluation; these cParImpl's are cached
                cParImpl *value = currentDecl->getSharedParImplFor(exprNode);
                if (!value)
                {
                    // not yet seen, compile and cache it
                    cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, isSubcomponent);
                    value = new cLongParImpl();
                    value->setName("gatesize-expression");
                    cExpressionBuilder::setExpression(value, dynamicExpr);
                    currentDecl->putSharedParImplFor(exprNode, value);
                }

                // evaluate the expression, and set the gate vector size
                int gatesize = value->longValue(module);
                module->setGateSize(gateNode->getName(), gatesize);
            }
        }
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, gateNode); throw;
    }
}

void cNEDNetworkBuilder::setupGateVectors(cModule *module, cNEDDeclaration *decl)
{
    // recursively add super types' gates
    if (decl->numExtendsNames() > 0)
    {
        const char *superName = decl->extendsName(0);
        cNEDDeclaration *superDecl = cNEDLoader::getInstance()->getDecl(superName);
        setupGateVectors(module, superDecl);
    }

    // add this decl's gatesizes
    GatesElement *gatesNode = decl->getGatesElement();
    if (gatesNode)
    {
        currentDecl = decl; // switch "context"
        doGateSizes(module, gatesNode, false);
    }
}

void cNEDNetworkBuilder::buildInside(cModule *modp, cNEDDeclaration *decl)
{
    if (modp->getId() % 50 == 0)
    {
        // half-hearted attempt to catch "recursive compound module" bug (where
        // a compound module contains itself, directly or via other compound modules)
        int limit = ev.getConfig()->getAsInt(CFGID_MAX_MODULE_NESTING);
        if (limit>0)
        {
            int depth = 0;
            for (cModule *p=modp; p; p=p->getParentModule())
                depth++;
            if (depth > limit)
                throw cRuntimeError(modp, "Submodule nesting too deep (%d) (potential infinite recursion?)", depth);
        }
    }

    // add submodules and connections. Submodules and connections are inherited:
    // we need to start start with the the base classes, and do this compound
    // module last.
    submodMap.clear();
    buildRecursively(modp, decl);

    // check if there are unconnected gates left -- unless unconnected gates were already permitted in the super type
    ConnectionsElement *conns = decl->getConnectionsElement();
    if ((!conns || !conns->getAllowUnconnected()) && !superTypeAllowsUnconnected(decl))
        modp->checkInternalConnections();

    // recursively build the submodules too (top-down)
    currentDecl = decl;
    for (cModule::SubmoduleIterator submod(modp); !submod.end(); submod++)
    {
        cModule *m = submod();
        m->buildInside();
    }
}

void cNEDNetworkBuilder::buildRecursively(cModule *modp, cNEDDeclaration *decl)
{
    // recursively add super types' submodules and connections
    if (decl->numExtendsNames() > 0)
    {
        const char *superName = decl->extendsName(0);
        cNEDDeclaration *superDecl = cNEDLoader::getInstance()->getDecl(superName);
        buildRecursively(modp, superDecl);
    }

    currentDecl = decl; // switch "context"
    addSubmodulesAndConnections(modp);
}

void cNEDNetworkBuilder::addSubmodulesAndConnections(cModule *modp)
{
    SubmodulesElement *submods = currentDecl->getSubmodulesElement();
    if (submods)
        for (SubmoduleElement *submod=submods->getFirstSubmoduleChild(); submod; submod=submod->getNextSubmoduleSibling())
            addSubmodule(modp, submod);

    // loop through connections and add them
    ConnectionsElement *conns = currentDecl->getConnectionsElement();
    if (conns)
        for (NEDElement *child=conns->getFirstChild(); child; child=child->getNextSibling())
            if (child->getTagCode()==NED_CONNECTION || child->getTagCode()==NED_CONNECTION_GROUP)
                addConnectionOrConnectionGroup(modp, child);
}

bool cNEDNetworkBuilder::superTypeAllowsUnconnected(cNEDDeclaration *decl) const
{
    // follow through the inheritance chain, and return true if we find an "allowunconnected" anywhere
    while (decl->numExtendsNames() > 0)
    {
        const char *superName = decl->extendsName(0);
        decl = cNEDLoader::getInstance()->getDecl(superName);
        ASSERT(decl); // all super classes must be loaded before we start building
        ConnectionsElement *conns = decl->getConnectionsElement();
        if (conns && conns->getAllowUnconnected())
            return true;
    }
    return false;
}

std::string cNEDNetworkBuilder::resolveComponentType(const NEDLookupContext& context, const char *nedTypeName)
{
    // Resolve a NED module/channel type name, for a submodule or channel
    // instance. Lookup is based on component names registered in the simkernel,
    // NOT on the NED files loaded. This allows the user to instantiate
    // cModuleTypes/cChannelTypes which are not declared in NED.
    ComponentTypeNames qnames;
    return cNEDLoader::getInstance()->resolveNedType(context, nedTypeName, &qnames);
}

cModuleType *cNEDNetworkBuilder::findAndCheckModuleType(const char *modTypeName, cModule *modp, const char *submodName)
{
    //TODO cache the result to speed up further lookups
    NEDLookupContext context(currentDecl->getTree(), currentDecl->getFullName());
    std::string qname = resolveComponentType(context, modTypeName);
    if (qname.empty())
        throw cRuntimeError(modp, "Submodule %s: cannot resolve module type `%s' (not in the loaded NED files?)",
                            submodName, modTypeName);
    cComponentType *componenttype = cComponentType::find(qname.c_str());
    if (!dynamic_cast<cModuleType *>(componenttype))
        throw cRuntimeError(modp, "Submodule %s: `%s' is not a module type",
                            submodName, qname.c_str());
    return (cModuleType *)componenttype;
}

cModuleType *cNEDNetworkBuilder::findAndCheckModuleTypeLike(const char *modTypeName, const char *likeType, cModule *modp, const char *submodName)
{
    //TODO cache the result to speed up further lookups

    // resolve the interface
    NEDLookupContext context(currentDecl->getTree(), currentDecl->getFullName());
    std::string interfaceQName = cNEDLoader::getInstance()->resolveNedType(context, likeType);
    cNEDDeclaration *interfacedecl = interfaceQName.empty() ? NULL : (cNEDDeclaration *)cNEDLoader::getInstance()->lookup(interfaceQName.c_str());
    if (!interfacedecl)
        throw cRuntimeError(modp, "Submodule %s: cannot resolve module interface `%s'",
                            submodName, likeType);
    if (interfacedecl->getTree()->getTagCode()!=NED_MODULE_INTERFACE)
        throw cRuntimeError(modp, "Submodule %s: `%s' is not a module interface",
                            submodName, interfaceQName.c_str());

    // search for module type that implements the interface
    std::vector<std::string> candidates = findTypeWithInterface(modTypeName, interfaceQName.c_str());
    if (candidates.empty())
        throw cRuntimeError(modp, "Submodule %s: no module type named `%s' found that implements module interface %s (not in the loaded NED files?)",
                            submodName, modTypeName, interfaceQName.c_str());
    if (candidates.size() > 1)
        throw cRuntimeError(modp, "Submodule %s: more than one module types named `%s' found that implement module interface %s (use fully qualified name to disambiguate)",
                            submodName, modTypeName, interfaceQName.c_str());

    cComponentType *componenttype = cComponentType::find(candidates[0].c_str());
    if (!dynamic_cast<cModuleType *>(componenttype))
        throw cRuntimeError(modp, "Submodule %s: `%s' is not a module type",
                            submodName, candidates[0].c_str());
    return (cModuleType *)componenttype;
}

std::vector<std::string> cNEDNetworkBuilder::findTypeWithInterface(const char *nedTypeName, const char *interfaceQName)
{
    std::vector<std::string> candidates;

    // try to interpret it as a fully qualified name
    ComponentTypeNames qnames;
    if (qnames.contains(nedTypeName)) {
        cNEDDeclaration *decl = cNEDLoader::getInstance()->getDecl(nedTypeName);
        ASSERT(decl);
        if (decl->supportsInterface(interfaceQName)) {
            candidates.push_back(nedTypeName);
            return candidates;
        }
    }

    if (!strchr(nedTypeName, '.'))
    {
        // no dot: name is an unqualified name (simple name). See how many NED types
        // implement the given interface; there should be exactly one
        std::string dot_nedtypename = std::string(".")+nedTypeName;
        for (int i=0; i<qnames.size(); i++) {
            const char *qname = qnames.get(i);
            if (opp_stringendswith(qname, dot_nedtypename.c_str()) || strcmp(qname, nedTypeName)==0) {
                cNEDDeclaration *decl = cNEDLoader::getInstance()->getDecl(qname);
                ASSERT(decl);
                if (decl->supportsInterface(interfaceQName))
                    candidates.push_back(qname);
            }
        }
    }
    return candidates;
}

std::string cNEDNetworkBuilder::getSubmoduleTypeName(cModule *modp, SubmoduleElement *submod, int index)
{
    // note: this code is nearly identical to getChannelTypeName(), with subtle differences
    const char *submodName = submod->getName();
    if (opp_isempty(submod->getLikeType()))
    {
        return submod->getType();
    }
    else
    {
        // first, try to use expression between angle braces from the NED file
        if (!submod->getIsDefault()) {
            if (!opp_isempty(submod->getLikeExpr()))
                return modp->par(submod->getLikeExpr()).stringValue();
            ExpressionElement *likeExprElement = findExpression(submod, "like-expr");
            if (likeExprElement)
                return evaluateAsString(likeExprElement, modp, false);
        }

        std::string submodFullName = submodName;
        if (index != -1)
            submodFullName += opp_stringf("[%d]", index);

        // then, use **.typename from NED deep param assignments
        std::string defaultDeepSubmodTypeName;
        bool deepSubmodTypeNameIsDefault;
        std::string submodTypeName = getSubmoduleOrChannelTypeNameFromDeepAssignments(modp, submodFullName, deepSubmodTypeNameIsDefault);
        if (!submodTypeName.empty()) {
            if (!deepSubmodTypeNameIsDefault)
                return submodTypeName;
            else
                defaultDeepSubmodTypeName = submodTypeName;
        }

        // then, use **.typename option in the configuration if exists
        std::string key = modp->getFullPath() + "." + submodName;
        if (index != -1)
            key = opp_stringf("%s[%d]", key.c_str(), index);
        submodTypeName = ev.getConfig()->getAsString(key.c_str(), CFGID_TYPENAME);
        if (!submodTypeName.empty())
            return submodTypeName;

        // then, use **.typename=default() expressions from NED deep param assignments
        if (!defaultDeepSubmodTypeName.empty())
            return defaultDeepSubmodTypeName;

        // last, use default(expression) between angle braces from the NED file
        if (submod->getIsDefault()) {
            if (!opp_isempty(submod->getLikeExpr()))
                return modp->par(submod->getLikeExpr()).stringValue();
            ExpressionElement *likeExprElement = findExpression(submod, "like-expr");
            if (likeExprElement)
                return evaluateAsString(likeExprElement, modp, false);
        }
        throw cRuntimeError(modp, "Unable to determine type name for submodule %s, missing entry %s.%s and no default value", submodName, key.c_str(), CFGID_TYPENAME->getName());
    }
}

std::string cNEDNetworkBuilder::getSubmoduleOrChannelTypeNameFromDeepAssignments(cModule *modp, const std::string& submodOrChannelKey, bool& outIsDefault)
{
    // strategy: go up the parent chain, and find patterns that match "<submodfullname>.typename".
    // we return the first (innermost) non-"default()" value, or the last (outermost) "default()" value
    // (and set outIsDefault accordingly.)

    // note: we start at the compound module's parameters section: the submodule's
    // own body is ignored, i.e. we don't accept something like
    //   foo: <> like IFoo {typename = "Foo";}
    // because we'd have to evaluate the expression in the context of a module
    // that doesn't exist yet, and we're not prepared for that.

    // note: this function is based on assignParametersFromPatterns()

    std::string key = submodOrChannelKey + ".typename";

    // find NED declaration of parent module, if exists (there is no NED decl for dynamically created modules)
    const char *nedTypeName = modp->getNedTypeName();
    cNEDDeclaration *decl = cNEDLoader::getInstance()->getDecl(nedTypeName);

    std::string defaultTypeName;
    outIsDefault = false;

    for (cComponent *mod = modp; true; /**/)
    {
        // check patterns on the 'mod' compound module
        if (decl)
        {
            const std::vector<PatternData>& patterns = decl->getParamPatterns();
            if (!patterns.empty())
            {
                int numPatterns = patterns.size();
                for (int j=0; j<numPatterns; j++) {
                    if (patterns[j].matcher->matches(key.c_str())) {
                        // return the value if it's not enclosed in 'default()', otherwise remember it
                        ExpressionElement *typeNameExpr = findExpression(patterns[j].patternNode, "value");
                        if (typeNameExpr) {
                            std::string typeName = evaluateAsString(typeNameExpr, mod, false);
                            if (patterns[j].patternNode->getIsDefault())
                                defaultTypeName = typeName;
                            else
                                return typeName;
                        }
                    }
                }
            }
        }

        // go one level up, and check patterns on 'mod' as a submodule
        cModule *parent = mod->getParentModule();
        if (!parent)
            break;

        // it is stored in the parent type
        const char *nedTypeName = parent->getNedTypeName();
        cNEDDeclaration *parentDecl = cNEDLoader::getInstance()->getDecl(nedTypeName);
        if (parentDecl)
        {
            const std::vector<PatternData>& submodPatterns = parentDecl->getSubmoduleParamPatterns(mod->getName());
            if (!submodPatterns.empty())
            {
                int numPatterns = submodPatterns.size();
                for (int j=0; j<numPatterns; j++) {
                    if (submodPatterns[j].matcher->matches(key.c_str())) {
                        // return the value if it's not enclosed in 'default()', otherwise remember it
                        ExpressionElement *typeNameExpr = findExpression(submodPatterns[j].patternNode, "value");
                        if (typeNameExpr) {
                            std::string typeName = evaluateAsString(typeNameExpr, mod, true);
                            if (submodPatterns[j].patternNode->getIsDefault())
                                defaultTypeName = typeName;
                            else
                                return typeName;
                        }
                    }
                }
            }
        }

        // go one level up
        key = std::string(mod->getFullName()) + "." + key;
        mod = mod->getParentModule();
        decl = parentDecl;
    }

    outIsDefault = true;
    return defaultTypeName;
}

void cNEDNetworkBuilder::addSubmodule(cModule *modp, SubmoduleElement *submod)
{
    // if there is a @dynamic or @dynamic(true), do not instantiate the submodule
    ParametersElement *paramsNode = submod->getFirstParametersChild();
    if (paramsNode)
        for (PropertyElement *prop = paramsNode->getFirstPropertyChild(); prop!=NULL; prop = prop->getNextPropertySibling())
            if (opp_strcmp(prop->getName(), "dynamic")==0 && NEDElementUtil::propertyAsBool(prop)==true)
                return;

    // handle conditional submodule
    ConditionElement *condition = submod->getFirstConditionChild();
    if (condition)
    {
        ExpressionElement *condexpr = findExpression(condition, "condition");
        if (condexpr && evaluateAsBool(condexpr, modp, false)==false)
            return;
    }

    // create submodule
    const char *submodName = submod->getName();
    bool usesLike = !opp_isempty(submod->getLikeType());
    ExpressionElement *vectorsizeexpr = findExpression(submod, "vector-size");

    if (!vectorsizeexpr)
    {
        cModuleType *submodType;
        try {
            std::string submodTypeName = getSubmoduleTypeName(modp, submod);
            submodType = usesLike ?
                findAndCheckModuleTypeLike(submodTypeName.c_str(), submod->getLikeType(), modp, submodName) :
                findAndCheckModuleType(submodTypeName.c_str(), modp, submodName);
        }
        catch (std::exception& e) {
            updateOrRethrowException(e, submod); throw;
        }

        cModule *submodp = submodType->create(submodName, modp);
        ModulePtrVector& v = submodMap[submodName];
        v.push_back(submodp);

        cContextSwitcher __ctx(submodp); // params need to be evaluated in the module's context
        submodp->finalizeParameters(); // also sets up gate sizes declared inside the type
        setupSubmoduleGateVectors(submodp, submod);
    }
    else
    {
        // note: we don't try to resolve moduleType if vector size is zero
        int vectorsize = (int) evaluateAsLong(vectorsizeexpr, modp, false);
        ModulePtrVector& v = submodMap[submodName];
        cModuleType *submodType = NULL;
        for (int i=0; i<vectorsize; i++) {
            if (!submodType || usesLike) {
                try {
                    std::string submodTypeName = getSubmoduleTypeName(modp, submod, i);
                    submodType = usesLike ?
                        findAndCheckModuleTypeLike(submodTypeName.c_str(), submod->getLikeType(), modp, submodName) :
                        findAndCheckModuleType(submodTypeName.c_str(), modp, submodName);
                }
                catch (std::exception& e) {
                    updateOrRethrowException(e, submod); throw;
                }
            }
            cModule *submodp = submodType->create(submodName, modp, vectorsize, i);
            v.push_back(submodp);

            cContextSwitcher __ctx(submodp); // params need to be evaluated in the module's context
            submodp->finalizeParameters(); // also sets up gate sizes declared inside the type
            setupSubmoduleGateVectors(submodp, submod);
        }
    }

    // Note: buildInside() will be called when connections have been built out
    // on this level too.
}

void cNEDNetworkBuilder::assignSubcomponentParams(cComponent *subcomponent, NEDElement *subcomponentNode)
{
    ParametersElement *paramsNode = (ParametersElement *) subcomponentNode->getFirstChildWithTag(NED_PARAMETERS);
    if (paramsNode)
        doParams(subcomponent, paramsNode, true);
}

void cNEDNetworkBuilder::setupSubmoduleGateVectors(cModule *submodule, NEDElement *submoduleNode)
{
    GatesElement *gatesNode = (GatesElement *) submoduleNode->getFirstChildWithTag(NED_GATES);
    if (gatesNode)
        doGateSizes(submodule, gatesNode, true);
}

void cNEDNetworkBuilder::addConnectionOrConnectionGroup(cModule *modp, NEDElement *connOrConnGroup)
{
    // this is tricky: elements representing "for" and "if" in NED are children
    // of the ConnectionElement or ConnectionGroupElement. So, first we have to go through
    // and execute the LoopElement and ConditionElement children recursively to get
    // nested loops etc, then after (inside) the last one create the connection(s)
    // themselves, which is (are) then parent of the LoopNode/ConditionNode.
    NEDSupport::LoopVar::reset();
    doConnOrConnGroupBody(modp, connOrConnGroup, connOrConnGroup->getFirstChild());
}

void cNEDNetworkBuilder::doConnOrConnGroupBody(cModule *modp, NEDElement *connOrConnGroup, NEDElement *loopOrCondition)
{
    // find first "for" or "if" at loopOrCondition (or among its next siblings)
    while (loopOrCondition && loopOrCondition->getTagCode()!=NED_LOOP && loopOrCondition->getTagCode()!=NED_CONDITION)
        loopOrCondition = loopOrCondition->getNextSibling();

    // if there's a "for" or "if", do that, otherwise create the connection(s) themselves
    if (loopOrCondition)
        doLoopOrCondition(modp, loopOrCondition);
    else
        doAddConnOrConnGroup(modp, connOrConnGroup);
}


void cNEDNetworkBuilder::doLoopOrCondition(cModule *modp, NEDElement *loopOrCondition)
{
    if (loopOrCondition->getTagCode()==NED_CONDITION)
    {
        // check condition
        ConditionElement *condition = (ConditionElement *)loopOrCondition;
        ExpressionElement *condexpr = findExpression(condition, "condition");
        if (condexpr && evaluateAsBool(condexpr, modp, false)==true)
        {
            // do the body of the "if": either further "for"'s and "if"'s, or
            // the connection(group) itself that we are children of.
            doConnOrConnGroupBody(modp, loopOrCondition->getParent(), loopOrCondition->getNextSibling());
        }
    }
    else if (loopOrCondition->getTagCode()==NED_LOOP)
    {
        LoopElement *loop = (LoopElement *)loopOrCondition;
        long start = evaluateAsLong(findExpression(loop, "from-value"), modp, false);
        long end = evaluateAsLong(findExpression(loop, "to-value"), modp, false);

        // do loop
        long& i = NEDSupport::LoopVar::pushVar(loop->getParamName());
        for (i=start; i<=end; i++)
        {
            // do the body of the "if": either further "for"'s and "if"'s, or
            // the connection(group) itself that we are children of.
            doConnOrConnGroupBody(modp, loopOrCondition->getParent(), loopOrCondition->getNextSibling());
        }
        NEDSupport::LoopVar::popVar();
    }
    else
    {
        ASSERT(false);
    }
}

void cNEDNetworkBuilder::doAddConnOrConnGroup(cModule *modp, NEDElement *connOrConnGroup)
{
    if (connOrConnGroup->getTagCode()==NED_CONNECTION)
    {
        doAddConnection(modp, (ConnectionElement *)connOrConnGroup);
    }
    else if (connOrConnGroup->getTagCode()==NED_CONNECTION_GROUP)
    {
        ConnectionGroupElement *conngroup = (ConnectionGroupElement *)connOrConnGroup;
        for (ConnectionElement *conn=conngroup->getFirstConnectionChild(); conn; conn=conn->getNextConnectionSibling())
        {
            doConnOrConnGroupBody(modp, conn, conn->getFirstChild());
        }
    }
    else
    {
        ASSERT(false);
    }
}

void cNEDNetworkBuilder::doAddConnection(cModule *modp, ConnectionElement *conn)
{
//FIXME spurious error message comes when trying to connect INOUT gate with "-->"
    try
    {
        if (!conn->getIsBidirectional())
        {
            // find gates and create connection
            cGate *srcg = resolveGate(modp, conn->getSrcModule(), findExpression(conn, "src-module-index"),
                                            conn->getSrcGate(), findExpression(conn, "src-gate-index"),
                                            conn->getSrcGateSubg(), conn->getSrcGatePlusplus());
            cGate *destg = resolveGate(modp, conn->getDestModule(), findExpression(conn, "dest-module-index"),
                                             conn->getDestGate(), findExpression(conn, "dest-gate-index"),
                                             conn->getDestGateSubg(), conn->getDestGatePlusplus());

            // check directions
            cGate *errg = NULL;
            if (srcg->getOwnerModule()==modp ? srcg->getType()!=cGate::INPUT : srcg->getType()!=cGate::OUTPUT)
                errg = srcg;
            if (destg->getOwnerModule()==modp ? destg->getType()!=cGate::OUTPUT : destg->getType()!=cGate::INPUT)
                errg = destg;
            if (errg)
                throw cRuntimeError(modp, "Gate %s is being connected in the wrong direction",
                                    errg->getFullPath().c_str());

            doConnectGates(modp, srcg, destg, conn);
        }
        else
        {
            // find gates and create connection in both ways
            if (conn->getSrcGateSubg()!=NED_SUBGATE_NONE || conn->getDestGateSubg()!=NED_SUBGATE_NONE)
                throw cRuntimeError(modp, "gate$i or gate$o used with bidirectional connections");

            // now: 1 is input, 2 is output, except for parent module gates where it is the other way round
            // (because we connect xx.out --> yy.in, but xx.out --> out!)
            cGate *srcg1, *srcg2, *destg1, *destg2;
            resolveInoutGate(modp, conn->getSrcModule(), findExpression(conn, "src-module-index"),
                             conn->getSrcGate(), findExpression(conn, "src-gate-index"),
                             conn->getSrcGatePlusplus(),
                             srcg1, srcg2);
            resolveInoutGate(modp, conn->getDestModule(), findExpression(conn, "dest-module-index"),
                             conn->getDestGate(), findExpression(conn, "dest-gate-index"),
                             conn->getDestGatePlusplus(),
                             destg1, destg2);

            doConnectGates(modp, srcg2, destg1, conn);
            doConnectGates(modp, destg2, srcg1, conn);
        }
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, conn); throw;
    }
}

void cNEDNetworkBuilder::doConnectGates(cModule *modp, cGate *srcg, cGate *destg, ConnectionElement *conn)
{
    if (opp_isempty(conn->getName()) && opp_isempty(conn->getType()) &&
        opp_isempty(conn->getLikeType()) && !conn->getFirstParametersChild())
    {
        srcg->connectTo(destg);
    }
    else
    {
        cChannel *channel = createChannel(conn, modp, srcg);
        channel->setNedConnectionElementId(conn->getId()); // so that properties will be found
        srcg->connectTo(destg, channel);
        assignSubcomponentParams(channel, conn);
        channel->finalizeParameters();
    }
}

cGate *cNEDNetworkBuilder::resolveGate(cModule *parentmodp,
                                       const char *modname, ExpressionElement *modindexp,
                                       const char *gatename, ExpressionElement *gateindexp,
                                       int subg, bool isplusplus)
{
    if (isplusplus && gateindexp)
        throw cRuntimeError(parentmodp, "Both `++' and gate index expression used in a connection");

    cModule *modp = resolveModuleForConnection(parentmodp, modname, modindexp);

    // add "$i" or "$o" suffix to gate name if needed
    std::string tmp;
    const char *gatename2 = gatename;
    if (subg!=NED_SUBGATE_NONE)
    {
        const char *suffix = subg==NED_SUBGATE_I ? "$i" : "$o";
        tmp = gatename;
        tmp += suffix;
        gatename2 = tmp.c_str();
    }

    // look up gate
    cGate *gatep = NULL;
    if (!gateindexp && !isplusplus)
    {
        gatep = modp->gate(gatename2);
    }
    else if (isplusplus)
    {
        if (modp == parentmodp)
        {
            gatep = modp->getOrCreateFirstUnconnectedGate(gatename2, 0, true, false); // inside, don't expand
            if (!gatep)
                throw cRuntimeError(modp, "%s[] gates are all connected, no gate left for `++' operator", gatename);
        }
        else
        {
            gatep = modp->getOrCreateFirstUnconnectedGate(gatename2, 0, false, true); // outside, expand
            ASSERT(gatep!=NULL);
        }
    }
    else // (gateindexp)
    {
        int gateindex = (int) evaluateAsLong(gateindexp, parentmodp, false);
        gatep = modp->gate(gatename2, gateindex);
    }
    return gatep;
}

void cNEDNetworkBuilder::resolveInoutGate(cModule *parentmodp,
                                          const char *modname, ExpressionElement *modindexp,
                                          const char *gatename, ExpressionElement *gateindexp,
                                          bool isplusplus,
                                          cGate *&gate1, cGate *&gate2)
{
    if (isplusplus && gateindexp)
        throw cRuntimeError(parentmodp, "Both `++' and gate index expression used in a connection");

    cModule *modp = resolveModuleForConnection(parentmodp, modname, modindexp);

    gate1 = gate2 = NULL;
    if (!gateindexp && !isplusplus)
    {
        // optimization possibility: add gatePair() method to cModule to spare one lookup
        gate1 = modp->gateHalf(gatename, cGate::INPUT);
        gate2 = modp->gateHalf(gatename, cGate::OUTPUT);
    }
    else if (isplusplus)
    {
        if (modp == parentmodp)
        {
            modp->getOrCreateFirstUnconnectedGatePair(gatename, true, false, gate1, gate2); // inside, don't expand
            if (!gate1 || !gate2)
                throw cRuntimeError(parentmodp, "%s[] gates are all connected, no gate left for `++' operator", gatename);
        }
        else
        {
            modp->getOrCreateFirstUnconnectedGatePair(gatename, false, true, gate1, gate2); // outside, expand
            ASSERT(gate1 && gate2);
        }
    }
    else // (gateindexp)
    {
        // optimization possiblity: add gatePair() method to cModule to spare one lookup
        int gateindex = (int) evaluateAsLong(gateindexp, parentmodp, false);
        gate1 = modp->gateHalf(gatename, cGate::INPUT, gateindex);
        gate2 = modp->gateHalf(gatename, cGate::OUTPUT, gateindex);
    }

    if (modp==parentmodp)
    {
        std::swap(gate1, gate2);
    }
}

cModule *cNEDNetworkBuilder::resolveModuleForConnection(cModule *parentmodp, const char *modname, ExpressionElement *modindexp)
{
    if (opp_isempty(modname))
    {
        return parentmodp;
    }
    else
    {
        int modindex = !modindexp ? 0 : (int) evaluateAsLong(modindexp, parentmodp, false);
        cModule *modp = _submodule(parentmodp, modname,modindex);
        if (!modp)
        {
            if (!modindexp)
                throw cRuntimeError(modp, "No submodule `%s' to be connected", modname);
            else
                throw cRuntimeError(modp, "No submodule `%s[%d]' to be connected", modname, modindex);
        }
        return modp;
    }
}

std::string cNEDNetworkBuilder::getChannelTypeName(cModule *parentmodp, cGate *srcgate, ConnectionElement *conn, const char *channelName)
{
    // note: this code is nearly identical to getSubmoduleTypeName(), with subtle differences
    if (opp_isempty(conn->getLikeType()))
    {
        if (!opp_isempty(conn->getType()))
        {
            return conn->getType();
        }
        else
        {
            bool hasDelayChannelParams = false, hasOtherParams = false;
            ParametersElement *channelparams = conn->getFirstParametersChild();
            if (channelparams) {
                for (ParamElement *param=channelparams->getFirstParamChild(); param; param=param->getNextParamSibling())
                    if (strcmp(param->getName(),"delay")==0 || strcmp(param->getName(),"disabled")==0)
                        hasDelayChannelParams = true;
                    else
                        hasOtherParams = true;
            }

            // choose one of the three built-in channel types, based on the channel's parameters
            return hasOtherParams ? "ned.DatarateChannel" : hasDelayChannelParams ? "ned.DelayChannel" : "ned.IdealChannel";
        }
    }
    else
    {
        // first, try to use expression betweeen angle braces from the NED file
        if (!conn->getIsDefault()) {
            if (!opp_isempty(conn->getLikeExpr()))
                return parentmodp->par(conn->getLikeExpr()).stringValue();
            ExpressionElement *likeExprElement = findExpression(conn, "like-expr");
            if (likeExprElement)
                return evaluateAsString(likeExprElement, parentmodp, false);
        }

        // produce key to identify channel within the parent module, e.g. "in[3].channel" or "queue.out.channel"
        std::string channelKey;
        if (srcgate->getOwnerModule() != parentmodp)
            channelKey = std::string(srcgate->getOwnerModule()->getFullName()) + ".";  // src submodule name
        channelKey += std::string(srcgate->getFullName()) + "." + channelName;

        // then, use **.typename from NED deep param assignments (using channelKey above)
        std::string defaultDeepConnTypeName;
        bool deepConnTypeNameIsDefault;
        std::string connTypeName = getSubmoduleOrChannelTypeNameFromDeepAssignments(parentmodp, channelKey, deepConnTypeNameIsDefault);
        if (!connTypeName.empty()) {
            if (!deepConnTypeNameIsDefault)
                return connTypeName;
            else
                defaultDeepConnTypeName = connTypeName;
        }

        // then, use **.typename option in the configuration if exists
        std::string key = srcgate->getFullPath() + "." + channelName;
        std::string channelTypeName = ev.getConfig()->getAsString(key.c_str(), CFGID_TYPENAME);
        if (!channelTypeName.empty())
            return channelTypeName;

        // then, use default() expression from NED deep param assignments
        if (!defaultDeepConnTypeName.empty())
            return defaultDeepConnTypeName;

        // last, use default(expression) betweeen angle braces from the NED file
        if (conn->getIsDefault()) {
            if (!opp_isempty(conn->getLikeExpr()))
                return parentmodp->par(conn->getLikeExpr()).stringValue();
            ExpressionElement *likeExprElement = findExpression(conn, "like-expr");
            if (likeExprElement)
                return evaluateAsString(likeExprElement, parentmodp, false);
        }
        throw cRuntimeError(parentmodp, "Unable to determine type name for channel: missing config entry %s.%s and no default value", key.c_str(), CFGID_TYPENAME->getName());
    }
}

cChannel *cNEDNetworkBuilder::createChannel(ConnectionElement *conn, cModule *parentmodp, cGate *srcgate)
{
    // resolve channel type
    const char *channelName = conn->getName();
    if (opp_isempty(channelName)) 
		channelName = NULL; // use NULL to indicate "no name"

    cChannelType *channeltype;
    try {
        // note: for **.channelname.liketype= lookups we cannot take the channel type @defaultname into account, because we don't have the type yet!
        std::string channelTypeName = getChannelTypeName(parentmodp, srcgate, conn, (channelName ? channelName : "channel"));
        bool usesLike = !opp_isempty(conn->getLikeType());
        channeltype = usesLike ?
            findAndCheckChannelTypeLike(channelTypeName.c_str(), conn->getLikeType(), parentmodp) :
            findAndCheckChannelType(channelTypeName.c_str(), parentmodp);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, conn); throw;
    }

    // create channel object
    cChannel *channelp = channeltype->create(channelName); // it will choose a name if it's NULL
    return channelp;
}

cChannelType *cNEDNetworkBuilder::findAndCheckChannelType(const char *channelTypeName, cModule *modp)
{
    //TODO cache the result to speed up further lookups
    NEDLookupContext context(currentDecl->getTree(), currentDecl->getFullName());
    std::string qname = resolveComponentType(context, channelTypeName);
    if (qname.empty())
        throw cRuntimeError(modp, "Cannot resolve channel type `%s' (not in the loaded NED files?)", channelTypeName);

    cComponentType *componenttype = cComponentType::find(qname.c_str());
    if (!dynamic_cast<cChannelType *>(componenttype))
        throw cRuntimeError(modp, "`%s' is not a channel type", qname.c_str());
    return (cChannelType *)componenttype;
}

cChannelType *cNEDNetworkBuilder::findAndCheckChannelTypeLike(const char *channelTypeName, const char *likeType, cModule *modp)
{
    //TODO cache the result to speed up further lookups

    // resolve the interface
    NEDLookupContext context(currentDecl->getTree(), currentDecl->getFullName());
    std::string interfaceQName = cNEDLoader::getInstance()->resolveNedType(context, likeType);
    cNEDDeclaration *interfacedecl = interfaceQName.empty() ? NULL : (cNEDDeclaration *)cNEDLoader::getInstance()->lookup(interfaceQName.c_str());
    if (!interfacedecl)
        throw cRuntimeError(modp, "Cannot resolve channel interface `%s'", likeType);
    if (interfacedecl->getTree()->getTagCode()!=NED_CHANNEL_INTERFACE)
        throw cRuntimeError(modp, "`%s' is not a channel interface", interfaceQName.c_str());

    // search for channel type that implements the interface
    std::vector<std::string> candidates = findTypeWithInterface(channelTypeName, interfaceQName.c_str());
    if (candidates.empty())
        throw cRuntimeError(modp, "No channel type named `%s' found that implements channel interface %s (not in the loaded NED files?)",
                            channelTypeName, interfaceQName.c_str());
    if (candidates.size() > 1)
        throw cRuntimeError(modp, "More than one channel types named `%s' found that implement channel interface %s (use fully qualified name to disambiguate)",
                            channelTypeName, interfaceQName.c_str());

    cComponentType *componenttype = cComponentType::find(candidates[0].c_str());
    if (!dynamic_cast<cChannelType *>(componenttype))
        throw cRuntimeError(modp, "`%s' is not a channel type", candidates[0].c_str());
    return (cChannelType *)componenttype;
}

ExpressionElement *cNEDNetworkBuilder::findExpression(NEDElement *node, const char *exprname)
{
    // find expression with given name in node
    if (!node)
        return NULL;
    for (NEDElement *child=node->getFirstChildWithTag(NED_EXPRESSION); child; child = child->getNextSiblingWithTag(NED_EXPRESSION))
    {
        ExpressionElement *expr = (ExpressionElement *)child;
        if (!strcmp(expr->getTarget(),exprname))
            return expr;
    }
    return NULL;
}

cParImpl *cNEDNetworkBuilder::getOrCreateExpression(ExpressionElement *exprNode, cPar::Type type, const char *unit, bool inSubcomponentScope)
{
    cParImpl *p = currentDecl->getSharedParImplFor(exprNode);
    if (!p)
    {
        cDynamicExpression *e = cExpressionBuilder().process(exprNode, inSubcomponentScope);
        p = cParImpl::createWithType(type);
        cExpressionBuilder::setExpression(p, e);
        p->setUnit(unit);

        currentDecl->putSharedParImplFor(exprNode, p);
    }
    return p;
}

long cNEDNetworkBuilder::evaluateAsLong(ExpressionElement *exprNode, cComponent *context, bool inSubcomponentScope)
{
    try {
        cParImpl *p = getOrCreateExpression(exprNode, cPar::LONG, NULL, inSubcomponentScope);
        return p->longValue(context);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, exprNode); throw;
    }
}

bool cNEDNetworkBuilder::evaluateAsBool(ExpressionElement *exprNode, cComponent *context, bool inSubcomponentScope)
{
    try {
        cParImpl *p = getOrCreateExpression(exprNode, cPar::BOOL, NULL, inSubcomponentScope);
        return p->boolValue(context);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, exprNode); throw;
    }
}

std::string cNEDNetworkBuilder::evaluateAsString(ExpressionElement *exprNode, cComponent *context, bool inSubcomponentScope)
{
    try {
        cParImpl *p = getOrCreateExpression(exprNode, cPar::STRING, NULL, inSubcomponentScope);
        return p->stdstringValue(context);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, exprNode); throw;
    }
}

NAMESPACE_END

