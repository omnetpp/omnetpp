//==========================================================================
//   CNEDNETWORKBUILDER.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

#include "common/commonutil.h"  // TRACE_CALL()
#include "common/stringutil.h"
#include "common/patternmatcher.h"
#include "nedxml/nedelements.h"
#include "nedxml/nedparser.h"
#include "nedxml/neddtdvalidator.h"
#include "nedxml/nedsyntaxvalidator.h"
#include "nedxml/nedutil.h"
#include "nedxml/errorstore.h"
#include "nedxml/xmlastparser.h"
#include "nedxml/xmlgenerator.h"  // for debugging
#include "omnetpp/cmodule.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cdataratechannel.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cboolparimpl.h"
#include "omnetpp/cintparimpl.h"
#include "omnetpp/cstringparimpl.h"
#include "common/displaystring.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/nedsupport.h"
#include "cnednetworkbuilder.h"
#include "cnedloader.h"
#include "cexpressionbuilder.h"

using namespace omnetpp::common;
using namespace omnetpp::nedsupport;

namespace omnetpp {

Register_PerRunConfigOption(CFGID_MAX_MODULE_NESTING, "max-module-nesting", CFG_INT, "50", "The maximum allowed depth of submodule nesting. This is used to catch accidental infinite recursions in NED.");
Register_PerObjectConfigOption(CFGID_TYPENAME, "typename", KIND_UNSPECIFIED_TYPE, CFG_STRING, nullptr, "Specifies type for submodules and channels declared with 'like <>'.");

#if 0
// for debugging
static void dump(NedElement *node)
{
    generateXML(std::cout, node, false);
    std::cout.flush();
}
#endif

// utility function for exception handling: adds NED file+line to the exception text
static void updateOrRethrowException(std::exception& e, NedElement *context)
{
    const char *loc = context ? context->getSourceLocation() : nullptr;
    if (!opp_isempty(loc)) {
        std::string msg = std::string(e.what()) + ", at " + loc;
        cException *ce = dynamic_cast<cException *>(&e);
        if (ce)
            ce->setMessage(msg.c_str());
        else
            throw cRuntimeError("%s", msg.c_str());  // cannot set msg on existing exception object, throw new one
    }
}

void cNedNetworkBuilder::addParametersAndGatesTo(cComponent *component, cNedDeclaration *decl)
{
    cContextSwitcher __ctx(component);  // params need to be evaluated in the module's context FIXME needed???
    doAddParametersAndGatesTo(component, decl);

    // assign submodule/connection parameters using parent module NED declaration as well
    cModule *parentModule = component->getParentModule();
    if (parentModule) {
        const char *parentNedTypeName = parentModule->getNedTypeName();
        cNedDeclaration *parentDecl = cNedLoader::getInstance()->getDecl(parentNedTypeName);
        if (parentDecl) {  // i.e. parent was created via NED-based componentType
            NedElement *subcomponentNode = component->isModule() ?
                (NedElement *)parentDecl->getSubmoduleElement(component->getName()) :
                (NedElement *)parentDecl->getConnectionElement(((cChannel *)component)->getNedConnectionElementId());
            if (subcomponentNode)
                assignSubcomponentParams(component, subcomponentNode);
        }
    }

    assignParametersFromPatterns(component);
}

void cNedNetworkBuilder::doAddParametersAndGatesTo(cComponent *component, cNedDeclaration *decl)
{
    // TODO for performance: component->reallocParamv(decl->getParameterDeclarations().size());

    // recursively add and assign super types' parameters
    if (decl->numExtendsNames() > 0) {
        const char *superName = decl->getExtendsName(0);
        cNedDeclaration *superDecl = cNedLoader::getInstance()->getDecl(superName);
        doAddParametersAndGatesTo(component, superDecl);
    }

    // add this decl parameters / assignments
    ParametersElement *paramsNode = decl->getParametersElement();
    if (paramsNode) {
        currentDecl = decl;  // switch "context"
        doParams(component, paramsNode, false);
    }

    // add this decl's gates (if there are any)
    GatesElement *gatesNode = decl->getGatesElement();
    if (gatesNode) {
        currentDecl = decl;  // switch "context"
        doGates((cModule *)component, gatesNode, false);
    }
}

cPar::Type cNedNetworkBuilder::translateParamType(int t)
{
    return t == PARTYPE_DOUBLE ? cPar::DOUBLE :
           t == PARTYPE_INT ? cPar::INT :
           t == PARTYPE_STRING ? cPar::STRING :
           t == PARTYPE_BOOL ? cPar::BOOL :
           t == PARTYPE_XML ? cPar::XML :
           (cPar::Type)-1;
}

cGate::Type cNedNetworkBuilder::translateGateType(int t)
{
    return t == GATETYPE_INPUT ? cGate::INPUT :
           t == GATETYPE_OUTPUT ? cGate::OUTPUT :
           t == GATETYPE_INOUT ? cGate::INOUT :
           (cGate::Type)-1;
}

void cNedNetworkBuilder::doParams(cComponent *component, ParametersElement *paramsNode, bool isSubcomponent)
{
    ASSERT(paramsNode != nullptr);
    for (ParamElement *paramNode = paramsNode->getFirstParamChild(); paramNode; paramNode = paramNode->getNextParamSibling())
        if (!paramNode->getIsPattern())
            doParam(component, paramNode, isSubcomponent);

}

void cNedNetworkBuilder::doParam(cComponent *component, ParamElement *paramNode, bool isSubcomponent)
{
    ASSERT(!paramNode->getIsPattern());  // we only deal with non-pattern assignments

    try {
        const char *paramName = paramNode->getName();

        // isSubComponent==false: we are called from cModuleType::addParametersAndGatesTo();
        // isSubComponent==true: we are called from assignSubcomponentParams().
        // if type==NONE, this is an inherited parameter (must have been added already)
        bool isNewParam = !isSubcomponent && paramNode->getType() != PARTYPE_NONE;

        // try to find an impl object with this value; we'll reuse it to optimize memory consumption
        cParImpl *impl = currentDecl->getSharedParImplFor(paramNode);
        if (impl) {
            // reuse impl for this parameter: we've already been at this point
            // in the NED files sometime, so we can reuse what we produced then
            ASSERT(impl->isName(paramName));
            if (isNewParam)
                component->addPar(impl);
            else {
                cPar& par = component->par(paramName);  // must exist already
                if (par.isSet())
                    throw cRuntimeError(component, "Cannot overwrite non-default value of parameter '%s'", paramName);
                par.setImpl(impl);
            }
            return;
        }

        ASSERT(impl == nullptr);
        if (isNewParam) {
            // adding parameter
            impl = cParImpl::createWithType(translateParamType(paramNode->getType()));
            impl->setName(paramName);
            impl->setIsShared(false);
            impl->setIsVolatile(paramNode->getIsVolatile());

            component->addPar(impl);

            cProperties *paramProps = component->par(paramName).getProperties();
            cProperty *unitProp = paramProps->get("unit");
            const char *declUnit = unitProp ? unitProp->getValue(cProperty::DEFAULTKEY) : nullptr;
            impl->setUnit(declUnit);
        }
        else {
            // parameter must exist already. Un-share it so that we can modify its value
            cPar& par = component->par(paramName);
            if (par.isSet())
                throw cRuntimeError(component, "Cannot overwrite non-default value of parameter '%s'", paramName);
            impl = par.copyIfShared();
        }

        ASSERT(!impl->isSet());  // we do not overwrite assigned values

        // put new value into impl
        ExpressionElement *exprNode = paramNode->getFirstExpressionChild();
        if (exprNode) {
            ASSERT(impl == component->par(paramName).impl() && !impl->isShared());
            cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, isSubcomponent);
            cExpressionBuilder::setExpression(impl, dynamicExpr);
            impl->setIsSet(!paramNode->getIsDefault());
        }
        else if (paramNode->getIsDefault()) {
            // default, but no value expression: set parameter to its existing default value
            //
            // Note: this branch ("=default" in NED files) is currently not supported,
            // because it would be complicated to implement in the Inifile Editor.
            //
            if (!impl->containsValue())
                throw cRuntimeError(component, "Cannot apply default value to parameter '%s': It has no default value", paramName);
            impl->setIsSet(true);
        }
        impl->setIsShared(true);
        currentDecl->putSharedParImplFor(paramNode, impl);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, paramNode);
        throw;
    }
}

void cNedNetworkBuilder::doGates(cModule *module, GatesElement *gatesNode, bool isSubcomponent)
{
    ASSERT(gatesNode != nullptr);
    for (NedElement *child = gatesNode->getFirstChildWithTag(NED_GATE); child; child = child->getNextSiblingWithTag(NED_GATE))
        doGate(module, (GateElement *)child, isSubcomponent);
}

void cNedNetworkBuilder::doGate(cModule *module, GateElement *gateNode, bool isSubcomponent)
{
    try {
        // add gate if it's declared here
        if (!isSubcomponent && gateNode->getType() != GATETYPE_NONE)
            module->addGate(gateNode->getName(), translateGateType(gateNode->getType()), gateNode->getIsVector());
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, gateNode);
        throw;
    }
}

void cNedNetworkBuilder::assignParametersFromPatterns(cComponent *component)
{
    // go up the parent chain, and apply the parameter pattern assignments
    // to matching, still-unset parameters
    std::string prefix;

    for (cComponent *child = component; true; child = child->getParentModule()) {
        cModule *parent = child->getParentModule();
        if (!parent)
            break;

        // check patterns in compound module "parent"

        // find NED declaration. Note that decl may be nullptr (if parent was not defined via NED but created dynamically)
        const char *nedTypeName = parent->getNedTypeName();
        cNedDeclaration *decl = cNedLoader::getInstance()->getDecl(nedTypeName);

        // first, check patterns in the "submodules:" section
        if (decl && !prefix.empty()) {
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
            ASSERT(srcGate == ((cChannel *)child)->getSourceGate());
            prefix = std::string(srcGate->getFullName()) + "."  + child->getFullName() + "." + prefix;  // prepend with "<srcgate>.channel."
            if (srcGate->getOwner() != parent)
                prefix = std::string(srcGate->getOwner()->getFullName()) + "."  + prefix;  // also prepend with "<submod>."
        }

        // check patterns on the compound module itself
        if (decl) {
            const std::vector<PatternData>& patterns = decl->getParamPatterns();
            if (!patterns.empty())
                doAssignParametersFromPatterns(component, prefix, patterns, false, parent);
        }
    }
}

void cNedNetworkBuilder::doAssignParametersFromPatterns(cComponent *component, const std::string& prefix, const std::vector<PatternData>& patterns, bool isInSubcomponent, cComponent *evalContext)
{
    int numPatterns = patterns.size();
    int numParams = component->getNumParams();
    for (int i = 0; i < numParams; i++) {
        cPar& par = component->par(i);
        if (!par.isSet()) {
            // first match
            std::string paramPath = prefix + par.getFullName();
            for (int j = 0; j < numPatterns; j++) {
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

void cNedNetworkBuilder::doAssignParameterFromPattern(cPar& par, ParamElement *patternNode, bool isInSubcomponent, cComponent *evalContext)
{
    // note: this code should look similar to relevant part of doParam()
    try {
        ASSERT(patternNode->getIsPattern());
        cParImpl *impl = par.copyIfShared();
        ExpressionElement *exprNode = patternNode->getFirstExpressionChild();
        if (exprNode) {
            // assign the parameter
            ASSERT(impl == par.impl() && !impl->isShared());
            cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, isInSubcomponent);
            cExpressionBuilder::setExpression(impl, dynamicExpr);
            par.setEvaluationContext(evalContext);
            impl->setIsSet(!patternNode->getIsDefault());
        }
        else if (patternNode->getIsDefault()) {
            // no expression: set parameter to its existing default value ("par=default;" syntax)
            //
            // Note: this branch ("=default" in NED files) is currently not supported,
            // because it would be complicated to implement in the Inifile Editor.
            if (!impl->containsValue())
                throw cRuntimeError(par.getOwner(), "Cannot apply default value to parameter '%s': It has no default value", par.getName());
            impl->setIsSet(true);
        }
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, patternNode);
        throw;
    }
}

cModule *cNedNetworkBuilder::_submodule(cModule *, const char *submodName, int idx)
{
    SubmodMap::iterator i = submodMap.find(std::string(submodName));
    if (i == submodMap.end())
        return nullptr;

    ModulePtrVector& v = i->second;
    if (idx < 0)
        return (v.size() != 1 || v[0]->isVector()) ? nullptr : v[0];
    else
        return ((unsigned)idx >= v.size()) ? nullptr : v[idx];
}

void cNedNetworkBuilder::doGateSizes(cModule *module, GatesElement *gatesNode, bool isSubcomponent)
{
    ASSERT(gatesNode != nullptr);
    for (NedElement *child = gatesNode->getFirstChildWithTag(NED_GATE); child; child = child->getNextSiblingWithTag(NED_GATE))
        doGateSize(module, (GateElement *)child, isSubcomponent);
}

void cNedNetworkBuilder::doGateSize(cModule *module, GateElement *gateNode, bool isSubcomponent)
{
    try {
        if (gateNode->getIsVector()) {
            // if there's a gatesize expression given, apply it
            ExpressionElement *exprNode = gateNode->getFirstExpressionChild();
            if (exprNode) {
                // ExpressionElement first needs to be compiled into a cParImpl
                // for evaluation; these cParImpl's are cached
                cParImpl *value = currentDecl->getSharedParImplFor(exprNode);
                if (!value) {
                    // not yet seen, compile and cache it
                    cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, isSubcomponent);
                    value = new cIntParImpl();
                    value->setName("gatesize-expression");
                    cExpressionBuilder::setExpression(value, dynamicExpr);
                    currentDecl->putSharedParImplFor(exprNode, value);
                }

                // evaluate the expression, and set the gate vector size
                int gatesize = value->intValue(module);
                module->setGateSize(gateNode->getName(), gatesize);
            }
        }
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, gateNode);
        throw;
    }
}

void cNedNetworkBuilder::setupGateVectors(cModule *module, cNedDeclaration *decl)
{
    // recursively add super types' gates
    if (decl->numExtendsNames() > 0) {
        const char *superName = decl->getExtendsName(0);
        cNedDeclaration *superDecl = cNedLoader::getInstance()->getDecl(superName);
        setupGateVectors(module, superDecl);
    }

    // add this decl's gatesizes
    GatesElement *gatesNode = decl->getGatesElement();
    if (gatesNode) {
        currentDecl = decl;  // switch "context"
        doGateSizes(module, gatesNode, false);
    }
}

void cNedNetworkBuilder::buildInside(cModule *modp, cNedDeclaration *decl)
{
    if (modp->getId() % 50 == 0) {
        // half-hearted attempt to catch "recursive compound module" bug (where
        // a compound module contains itself, directly or via other compound modules)
        int limit = getEnvir()->getConfig()->getAsInt(CFGID_MAX_MODULE_NESTING);
        if (limit > 0) {
            int depth = 0;
            for (cModule *p = modp; p; p = p->getParentModule())
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
    for (cModule::SubmoduleIterator it(modp); !it.end(); ++it)
        (*it)->buildInside();
}

void cNedNetworkBuilder::buildRecursively(cModule *modp, cNedDeclaration *decl)
{
    // recursively add super types' submodules and connections
    if (decl->numExtendsNames() > 0) {
        const char *superName = decl->getExtendsName(0);
        cNedDeclaration *superDecl = cNedLoader::getInstance()->getDecl(superName);
        buildRecursively(modp, superDecl);
    }

    currentDecl = decl;  // switch "context"
    addSubmodulesAndConnections(modp);
}

void cNedNetworkBuilder::addSubmodulesAndConnections(cModule *modp)
{
    SubmodulesElement *submods = currentDecl->getSubmodulesElement();
    if (submods)
        for (SubmoduleElement *submod = submods->getFirstSubmoduleChild(); submod; submod = submod->getNextSubmoduleSibling())
            addSubmodule(modp, submod);


    // loop through connections and add them
    ConnectionsElement *conns = currentDecl->getConnectionsElement();
    if (conns)
        for (NedElement *child = conns->getFirstChild(); child; child = child->getNextSibling())
            if (child->getTagCode() == NED_CONNECTION || child->getTagCode() == NED_CONNECTION_GROUP)
                addConnectionOrConnectionGroup(modp, child);

}

bool cNedNetworkBuilder::superTypeAllowsUnconnected(cNedDeclaration *decl) const
{
    // follow through the inheritance chain, and return true if we find an "allowunconnected" anywhere
    while (decl->numExtendsNames() > 0) {
        const char *superName = decl->getExtendsName(0);
        decl = cNedLoader::getInstance()->getDecl(superName);
        ASSERT(decl);  // all super classes must be loaded before we start building
        ConnectionsElement *conns = decl->getConnectionsElement();
        if (conns && conns->getAllowUnconnected())
            return true;
    }
    return false;
}

std::string cNedNetworkBuilder::resolveComponentType(const NedLookupContext& context, const char *nedTypeName)
{
    // Resolve a NED module/channel type name, for a submodule or channel
    // instance. Lookup is based on component names registered in the simkernel,
    // NOT on the NED files loaded. This allows the user to instantiate
    // cModuleTypes/cChannelTypes which are not declared in NED.
    ComponentTypeNames qnames;
    return cNedLoader::getInstance()->resolveNedType(context, nedTypeName, &qnames);
}

cModuleType *cNedNetworkBuilder::findAndCheckModuleType(const char *modTypeName, cModule *modp, const char *submodName)
{
    // TODO cache the result to speed up further lookups
    NedLookupContext context(currentDecl->getTree(), currentDecl->getFullName());
    std::string qname = resolveComponentType(context, modTypeName);
    if (qname.empty())
        throw cRuntimeError(modp, "Submodule %s: Cannot resolve module type '%s' (not in the loaded NED files?)",
                submodName, modTypeName);
    cComponentType *componentType = cComponentType::find(qname.c_str());
    if (!dynamic_cast<cModuleType *>(componentType))
        throw cRuntimeError(modp, "Submodule %s: '%s' is not a module type",
                submodName, qname.c_str());
    return (cModuleType *)componentType;
}

cModuleType *cNedNetworkBuilder::findAndCheckModuleTypeLike(const char *modTypeName, const char *likeType, cModule *modp, const char *submodName)
{
    // TODO cache the result to speed up further lookups

    // resolve the interface
    NedLookupContext context(currentDecl->getTree(), currentDecl->getFullName());
    std::string interfaceQName = cNedLoader::getInstance()->resolveNedType(context, likeType);
    cNedDeclaration *interfaceDecl = interfaceQName.empty() ? nullptr : (cNedDeclaration *)cNedLoader::getInstance()->lookup(interfaceQName.c_str());
    if (!interfaceDecl)
        throw cRuntimeError(modp, "Submodule %s: Cannot resolve module interface '%s'",
                submodName, likeType);
    if (interfaceDecl->getTree()->getTagCode() != NED_MODULE_INTERFACE)
        throw cRuntimeError(modp, "Submodule %s: '%s' is not a module interface",
                submodName, interfaceQName.c_str());

    // search for module type that implements the interface
    std::vector<std::string> candidates = findTypeWithInterface(modTypeName, interfaceQName.c_str());
    if (candidates.empty())
        throw cRuntimeError(modp, "Submodule %s: No module type named '%s' found that implements module interface %s (not in the loaded NED files?)",
                submodName, modTypeName, interfaceQName.c_str());
    if (candidates.size() > 1)
        throw cRuntimeError(modp, "Submodule %s: More than one module types named '%s' found that implement module interface %s (use fully qualified name to disambiguate)",
                submodName, modTypeName, interfaceQName.c_str());

    cComponentType *componenttype = cComponentType::find(candidates[0].c_str());
    if (!dynamic_cast<cModuleType *>(componenttype))
        throw cRuntimeError(modp, "Submodule %s: '%s' is not a module type",
                submodName, candidates[0].c_str());
    return (cModuleType *)componenttype;
}

std::vector<std::string> cNedNetworkBuilder::findTypeWithInterface(const char *nedTypeName, const char *interfaceQName)
{
    std::vector<std::string> candidates;

    // try to interpret it as a fully qualified name
    ComponentTypeNames qnames;
    if (qnames.contains(nedTypeName)) {
        cNedDeclaration *decl = cNedLoader::getInstance()->getDecl(nedTypeName);
        ASSERT(decl);
        if (decl->supportsInterface(interfaceQName)) {
            candidates.push_back(nedTypeName);
            return candidates;
        }
    }

    if (!strchr(nedTypeName, '.')) {
        // no dot: name is an unqualified name (simple name). See how many NED types
        // implement the given interface; there should be exactly one
        std::string dot_nedtypename = std::string(".")+nedTypeName;
        for (int i = 0; i < qnames.size(); i++) {
            const char *qname = qnames.get(i);
            if (opp_stringendswith(qname, dot_nedtypename.c_str()) || strcmp(qname, nedTypeName) == 0) {
                cNedDeclaration *decl = cNedLoader::getInstance()->getDecl(qname);
                ASSERT(decl);
                if (decl->supportsInterface(interfaceQName))
                    candidates.push_back(qname);
            }
        }
    }
    return candidates;
}

std::string cNedNetworkBuilder::getSubmoduleTypeName(cModule *modp, SubmoduleElement *submod, int index)
{
    // note: this code is nearly identical to getChannelTypeName(), with subtle differences
    const char *submodName = submod->getName();
    if (opp_isempty(submod->getLikeType())) {
        return submod->getType();
    }
    else {
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
        std::string submodTypeName;
        bool isDefault;
        bool typenameFound = getSubmoduleOrChannelTypeNameFromDeepAssignments(modp, submodFullName, submodTypeName, isDefault);
        if (typenameFound && !isDefault)
            return submodTypeName;

        // then, use **.typename option in the configuration if exists
        std::string key = modp->getFullPath() + "." + submodName;
        if (index != -1)
            key = opp_stringf("%s[%d]", key.c_str(), index);
        const char *NOTFOUNDVALUE = "\x1";
        std::string configTypename = getEnvir()->getConfig()->getAsString(key.c_str(), CFGID_TYPENAME, NOTFOUNDVALUE);
        if (configTypename != NOTFOUNDVALUE)
            return configTypename;

        // then, use the **.typename=default(expression) value from NED deep param assignments
        if (typenameFound)
            return submodTypeName;

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

bool cNedNetworkBuilder::getSubmoduleOrChannelTypeNameFromDeepAssignments(cModule *modp, const std::string& submodOrChannelKey, std::string& outTypeName, bool& outIsDefault)
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
    cNedDeclaration *decl = cNedLoader::getInstance()->getDecl(nedTypeName);

    bool found = false;
    outTypeName = "";
    outIsDefault = false;

    for (cComponent *mod = modp; true;  /**/) {
        // check patterns on the 'mod' compound module
        if (decl) {
            const std::vector<PatternData>& patterns = decl->getParamPatterns();
            if (!patterns.empty()) {
                int numPatterns = patterns.size();
                for (int j = 0; j < numPatterns; j++) {
                    if (patterns[j].matcher->matches(key.c_str())) {
                        // return the value if it's not enclosed in 'default()', otherwise remember it
                        ExpressionElement *typeNameExpr = findExpression(patterns[j].patternNode, "value");
                        if (typeNameExpr) {
                            outTypeName = evaluateAsString(typeNameExpr, mod, false);
                            if (patterns[j].patternNode->getIsDefault())
                                found = true;
                            else
                                return true;
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
        cNedDeclaration *parentDecl = cNedLoader::getInstance()->getDecl(nedTypeName);
        if (parentDecl) {
            const std::vector<PatternData>& submodPatterns = parentDecl->getSubmoduleParamPatterns(mod->getName());
            if (!submodPatterns.empty()) {
                int numPatterns = submodPatterns.size();
                for (int j = 0; j < numPatterns; j++) {
                    if (submodPatterns[j].matcher->matches(key.c_str())) {
                        // return the value if it's not enclosed in 'default()', otherwise remember it
                        ExpressionElement *typeNameExpr = findExpression(submodPatterns[j].patternNode, "value");
                        if (typeNameExpr) {
                            outTypeName = evaluateAsString(typeNameExpr, mod, true);
                            if (submodPatterns[j].patternNode->getIsDefault())
                                found = true;
                            else
                                return true;
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
    return found;
}

void cNedNetworkBuilder::addSubmodule(cModule *compoundModule, SubmoduleElement *submod)
{
    // if there is a @dynamic or @dynamic(true), do not instantiate the submodule
    ParametersElement *paramsNode = submod->getFirstParametersChild();
    if (paramsNode)
        for (PropertyElement *prop = paramsNode->getFirstPropertyChild(); prop != nullptr; prop = prop->getNextPropertySibling())
            if (omnetpp::opp_strcmp(prop->getName(), "dynamic") == 0 && ASTNodeUtil::propertyAsBool(prop) == true)
                return;

    // find vector size expression
    const char *submodName = submod->getName();
    bool usesLike = !opp_isempty(submod->getLikeType());
    ExpressionElement *vectorSizeExpr = findExpression(submod, "vector-size");

    // create submodule
    if (!vectorSizeExpr) {
        cModuleType *submodType;
        try {
            std::string submodTypeName = getSubmoduleTypeName(compoundModule, submod);

            // handle conditional submodule
            ConditionElement *condition = submod->getFirstConditionChild();
            if (condition) {
                ExpressionElement *conditionExpr = findExpression(condition, "condition");
                ASSERT(conditionExpr);
                NedExpressionContext context(compoundModule, NedExpressionContext::SUBMODULE_CONDITION, submodTypeName.c_str());
                if (evaluateAsBool(conditionExpr, &context, false) == false)
                    return;
            }

            submodType = usesLike ?
                findAndCheckModuleTypeLike(submodTypeName.c_str(), submod->getLikeType(), compoundModule, submodName) :
                findAndCheckModuleType(submodTypeName.c_str(), compoundModule, submodName);
        }
        catch (std::exception& e) {
            updateOrRethrowException(e, submod);
            throw;
        }
        cModule *submodp = submodType->create(submodName, compoundModule);
        ModulePtrVector& v = submodMap[submodName];
        v.push_back(submodp);

        cContextSwitcher __ctx(submodp);  // params need to be evaluated in the module's context
        submodp->finalizeParameters();  // also sets up gate sizes declared inside the type
        setupSubmoduleGateVectors(submodp, submod);
    }
    else {
        // handle conditional submodule vector
        ConditionElement *condition = submod->getFirstConditionChild();
        if (condition) {
            ExpressionElement *conditionExpr = findExpression(condition, "condition");
            ASSERT(conditionExpr);
            // note: "typename" may NOT occur in vector submodules' condition, because type is
            // per-element, and we want to evaluate the condition only once for the whole vector
            NedExpressionContext context(compoundModule, NedExpressionContext::SUBMODULE_ARRAY_CONDITION, nullptr);
            if (evaluateAsBool(conditionExpr, &context, false) == false)
                return;
        }

        // note: we don't try to resolve moduleType if vector size is zero
        int vectorsize = (int)evaluateAsLong(vectorSizeExpr, compoundModule, false);
        ModulePtrVector& v = submodMap[submodName];
        cModuleType *submodType = nullptr;
        for (int i = 0; i < vectorsize; i++) {
            if (!submodType || usesLike) {
                try {
                    std::string submodTypeName = getSubmoduleTypeName(compoundModule, submod, i);
                    submodType = usesLike ?
                        findAndCheckModuleTypeLike(submodTypeName.c_str(), submod->getLikeType(), compoundModule, submodName) :
                        findAndCheckModuleType(submodTypeName.c_str(), compoundModule, submodName);
                }
                catch (std::exception& e) {
                    updateOrRethrowException(e, submod);
                    throw;
                }
            }
            cModule *submodp = submodType->create(submodName, compoundModule, vectorsize, i);
            v.push_back(submodp);

            cContextSwitcher __ctx(submodp);  // params need to be evaluated in the module's context
            submodp->finalizeParameters();  // also sets up gate sizes declared inside the type
            setupSubmoduleGateVectors(submodp, submod);
        }
    }

    // Note: buildInside() will be called when connections have been built out
    // on this level too.
}

void cNedNetworkBuilder::assignSubcomponentParams(cComponent *subcomponent, NedElement *subcomponentNode)
{
    ParametersElement *paramsNode = (ParametersElement *)subcomponentNode->getFirstChildWithTag(NED_PARAMETERS);
    if (paramsNode)
        doParams(subcomponent, paramsNode, true);
}

void cNedNetworkBuilder::setupSubmoduleGateVectors(cModule *submodule, NedElement *submoduleNode)
{
    GatesElement *gatesNode = (GatesElement *)submoduleNode->getFirstChildWithTag(NED_GATES);
    if (gatesNode)
        doGateSizes(submodule, gatesNode, true);
}

void cNedNetworkBuilder::addConnectionOrConnectionGroup(cModule *modp, NedElement *connOrConnGroup)
{
    // this is tricky: elements representing "for" and "if" in NED are children
    // of the ConnectionElement or ConnectionGroupElement. So, first we have to go through
    // and execute the LoopElement and ConditionElement children recursively to get
    // nested loops etc, then after (inside) the last one create the connection(s)
    // themselves, which is (are) then parent of the LoopNode/ConditionNode.
    LoopVar::reset();
    doConnOrConnGroupBody(modp, connOrConnGroup, connOrConnGroup->getFirstChild());
}

void cNedNetworkBuilder::doConnOrConnGroupBody(cModule *modp, NedElement *connOrConnGroup, NedElement *loopOrCondition)
{
    // find first "for" or "if" at loopOrCondition (or among its next siblings)
    while (loopOrCondition && loopOrCondition->getTagCode() != NED_LOOP && loopOrCondition->getTagCode() != NED_CONDITION)
        loopOrCondition = loopOrCondition->getNextSibling();

    // if there's a "for" or "if", do that, otherwise create the connection(s) themselves
    if (loopOrCondition)
        doLoopOrCondition(modp, loopOrCondition);
    else
        doAddConnOrConnGroup(modp, connOrConnGroup);
}

void cNedNetworkBuilder::doLoopOrCondition(cModule *modp, NedElement *loopOrCondition)
{
    if (loopOrCondition->getTagCode() == NED_CONDITION) {
        // check condition
        ConditionElement *condition = (ConditionElement *)loopOrCondition;
        ExpressionElement *condexpr = findExpression(condition, "condition");
        if (condexpr && evaluateAsBool(condexpr, modp, false) == true) {
            // do the body of the "if": either further "for"'s and "if"'s, or
            // the connection(group) itself that we are children of.
            doConnOrConnGroupBody(modp, loopOrCondition->getParent(), loopOrCondition->getNextSibling());
        }
    }
    else if (loopOrCondition->getTagCode() == NED_LOOP) {
        LoopElement *loop = (LoopElement *)loopOrCondition;
        long start = evaluateAsLong(findExpression(loop, "from-value"), modp, false);
        long end = evaluateAsLong(findExpression(loop, "to-value"), modp, false);

        // do loop
        long& i = LoopVar::pushVar(loop->getParamName());
        for (i = start; i <= end; i++) {
            // do the body of the "if": either further "for"'s and "if"'s, or
            // the connection(group) itself that we are children of.
            doConnOrConnGroupBody(modp, loopOrCondition->getParent(), loopOrCondition->getNextSibling());
        }
        LoopVar::popVar();
    }
    else {
        ASSERT(false);
    }
}

void cNedNetworkBuilder::doAddConnOrConnGroup(cModule *modp, NedElement *connOrConnGroup)
{
    if (connOrConnGroup->getTagCode() == NED_CONNECTION) {
        doAddConnection(modp, (ConnectionElement *)connOrConnGroup);
    }
    else if (connOrConnGroup->getTagCode() == NED_CONNECTION_GROUP) {
        ConnectionGroupElement *conngroup = (ConnectionGroupElement *)connOrConnGroup;
        for (ConnectionElement *conn = conngroup->getFirstConnectionChild(); conn; conn = conn->getNextConnectionSibling()) {
            doConnOrConnGroupBody(modp, conn, conn->getFirstChild());
        }
    }
    else {
        ASSERT(false);
    }
}

void cNedNetworkBuilder::doAddConnection(cModule *modp, ConnectionElement *conn)
{
// FIXME spurious error message comes when trying to connect INOUT gate with "-->"
    try {
        if (!conn->getIsBidirectional()) {
            // find gates and create connection
            cGate *srcGate = resolveGate(modp, conn->getSrcModule(), findExpression(conn, "src-module-index"),
                        conn->getSrcGate(), findExpression(conn, "src-gate-index"),
                        conn->getSrcGateSubg(), conn->getSrcGatePlusplus());
            cGate *destGate = resolveGate(modp, conn->getDestModule(), findExpression(conn, "dest-module-index"),
                        conn->getDestGate(), findExpression(conn, "dest-gate-index"),
                        conn->getDestGateSubg(), conn->getDestGatePlusplus());

            // check directions
            cGate *errorGate = nullptr;
            if (srcGate->getOwnerModule() == modp ? srcGate->getType() != cGate::INPUT : srcGate->getType() != cGate::OUTPUT)
                errorGate = srcGate;
            if (destGate->getOwnerModule() == modp ? destGate->getType() != cGate::OUTPUT : destGate->getType() != cGate::INPUT)
                errorGate = destGate;
            if (errorGate)
                throw cRuntimeError(modp, "Gate %s is being connected in the wrong direction",
                        errorGate->getFullPath().c_str());

            doConnectGates(modp, srcGate, destGate, conn);
        }
        else {
            // find gates and create connection in both ways
            if (conn->getSrcGateSubg() != SUBGATE_NONE || conn->getDestGateSubg() != SUBGATE_NONE)
                throw cRuntimeError(modp, "gate$i or gate$o used with bidirectional connections");

            // now: 1 is input, 2 is output, except for parent module gates where it is the other way round
            // (because we connect xx.out --> yy.in, but xx.out --> out!)
            cGate *srcGate1, *srcGate2, *destGate1, *destGate2;
            resolveInoutGate(modp, conn->getSrcModule(), findExpression(conn, "src-module-index"),
                    conn->getSrcGate(), findExpression(conn, "src-gate-index"),
                    conn->getSrcGatePlusplus(),
                    srcGate1, srcGate2);
            resolveInoutGate(modp, conn->getDestModule(), findExpression(conn, "dest-module-index"),
                    conn->getDestGate(), findExpression(conn, "dest-gate-index"),
                    conn->getDestGatePlusplus(),
                    destGate1, destGate2);

            doConnectGates(modp, srcGate2, destGate1, conn);
            doConnectGates(modp, destGate2, srcGate1, conn);
        }
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, conn);
        throw;
    }
}

void cNedNetworkBuilder::doConnectGates(cModule *modp, cGate *srcGate, cGate *destGate, ConnectionElement *conn)
{
    if (opp_isempty(conn->getName()) && opp_isempty(conn->getType()) &&
        opp_isempty(conn->getLikeType()) && !conn->getFirstParametersChild())
    {
        srcGate->connectTo(destGate);
    }
    else {
        cChannel *channel = createChannel(conn, modp, srcGate);
        channel->setNedConnectionElementId(conn->getId());  // so that properties will be found
        srcGate->connectTo(destGate, channel);
        assignSubcomponentParams(channel, conn);
        channel->finalizeParameters();
    }
}

cGate *cNedNetworkBuilder::resolveGate(cModule *compoundModule,
        const char *moduleName, ExpressionElement *moduleIndexExpr,
        const char *gateName, ExpressionElement *gateIndexExpr,
        int subgate, bool isPlusPlus)
{
    if (isPlusPlus && gateIndexExpr)
        throw cRuntimeError(compoundModule, "Both '++' and gate index expression used in a connection");

    cModule *module = resolveModuleForConnection(compoundModule, moduleName, moduleIndexExpr);

    // add "$i" or "$o" suffix to gate name if needed
    std::string tmp;
    const char *gateName2 = gateName;
    if (subgate != SUBGATE_NONE) {
        const char *suffix = subgate == SUBGATE_I ? "$i" : "$o";
        tmp = gateName;
        tmp += suffix;
        gateName2 = tmp.c_str();
    }

    // look up gate
    cGate *gate = nullptr;
    if (!gateIndexExpr && !isPlusPlus) {
        gate = module->gate(gateName2);
    }
    else if (isPlusPlus) {
        if (module == compoundModule) {
            gate = module->getOrCreateFirstUnconnectedGate(gateName2, 0, true, false);  // inside, don't expand
            if (!gate)
                throw cRuntimeError(module, "%s[] gates are all connected, no gate left for '++' operator", gateName);
        }
        else {
            gate = module->getOrCreateFirstUnconnectedGate(gateName2, 0, false, true);  // outside, expand
            ASSERT(gate != nullptr);
        }
    }
    else {  // (gateIndexExpr)
        int gateIndex = (int)evaluateAsLong(gateIndexExpr, compoundModule, false);
        gate = module->gate(gateName2, gateIndex);
    }
    return gate;
}

void cNedNetworkBuilder::resolveInoutGate(cModule *compoundModule,
        const char *moduleName, ExpressionElement *moduleIndexExpr,
        const char *gateName, ExpressionElement *gateIndexExpr,
        bool isPlusPlus,
        cGate *& outGate1, cGate *& outGate2)
{
    if (isPlusPlus && gateIndexExpr)
        throw cRuntimeError(compoundModule, "Both '++' and gate index expression used in a connection");

    cModule *module = resolveModuleForConnection(compoundModule, moduleName, moduleIndexExpr);

    outGate1 = outGate2 = nullptr;
    if (!gateIndexExpr && !isPlusPlus) {
        // optimization possibility: add gatePair() method to cModule to spare one lookup
        outGate1 = module->gateHalf(gateName, cGate::INPUT);
        outGate2 = module->gateHalf(gateName, cGate::OUTPUT);
    }
    else if (isPlusPlus) {
        if (module == compoundModule) {
            module->getOrCreateFirstUnconnectedGatePair(gateName, true, false, outGate1, outGate2);  // inside, don't expand
            if (!outGate1 || !outGate2)
                throw cRuntimeError(compoundModule, "%s[] gates are all connected, no gate left for '++' operator", gateName);
        }
        else {
            module->getOrCreateFirstUnconnectedGatePair(gateName, false, true, outGate1, outGate2);  // outside, expand
            ASSERT(outGate1 && outGate2);
        }
    }
    else {  // (gateIndexExpr)
            // optimization possibility: add gatePair() method to cModule to spare one lookup
        int gateIndex = (int)evaluateAsLong(gateIndexExpr, compoundModule, false);
        outGate1 = module->gateHalf(gateName, cGate::INPUT, gateIndex);
        outGate2 = module->gateHalf(gateName, cGate::OUTPUT, gateIndex);
    }

    if (module == compoundModule) {
        std::swap(outGate1, outGate2);
    }
}

cModule *cNedNetworkBuilder::resolveModuleForConnection(cModule *compoundModule, const char *moduleName, ExpressionElement *moduleIndexExpr)
{
    if (opp_isempty(moduleName)) {
        return compoundModule;
    }
    else {
        int moduleIndex = !moduleIndexExpr ? 0 : (int)evaluateAsLong(moduleIndexExpr, compoundModule, false);
        cModule *module = _submodule(compoundModule, moduleName, moduleIndex);
        if (!module) {
            if (!moduleIndexExpr)
                throw cRuntimeError(module, "No submodule '%s' to be connected", moduleName);
            else
                throw cRuntimeError(module, "No submodule '%s[%d]' to be connected", moduleName, moduleIndex);
        }
        return module;
    }
}

std::string cNedNetworkBuilder::getChannelTypeName(cModule *parentmodp, cGate *srcGate, ConnectionElement *conn, const char *channelName)
{
    // note: this code is nearly identical to getSubmoduleTypeName(), with subtle differences
    if (opp_isempty(conn->getLikeType())) {
        if (!opp_isempty(conn->getType())) {
            return conn->getType();
        }
        else {
            bool hasDelayChannelParams = false, hasOtherParams = false;
            ParametersElement *channelParams = conn->getFirstParametersChild();
            if (channelParams) {
                for (ParamElement *param = channelParams->getFirstParamChild(); param; param = param->getNextParamSibling())
                    if (strcmp(param->getName(), "delay") == 0 || strcmp(param->getName(), "disabled") == 0)
                        hasDelayChannelParams = true;
                    else
                        hasOtherParams = true;

            }

            // choose one of the three built-in channel types, based on the channel's parameters
            return hasOtherParams ? "ned.DatarateChannel" : hasDelayChannelParams ? "ned.DelayChannel" : "ned.IdealChannel";
        }
    }
    else {
        // first, try to use expression between angle braces from the NED file
        if (!conn->getIsDefault()) {
            if (!opp_isempty(conn->getLikeExpr()))
                return parentmodp->par(conn->getLikeExpr()).stringValue();
            ExpressionElement *likeExprElement = findExpression(conn, "like-expr");
            if (likeExprElement)
                return evaluateAsString(likeExprElement, parentmodp, false);
        }

        // produce key to identify channel within the parent module, e.g. "in[3].channel" or "queue.out.channel"
        std::string channelKey;
        if (srcGate->getOwnerModule() != parentmodp)
            channelKey = std::string(srcGate->getOwnerModule()->getFullName()) + ".";  // src submodule name
        channelKey += std::string(srcGate->getFullName()) + "." + channelName;

        // then, use **.typename from NED deep param assignments (using channelKey above)
        std::string connTypeName;
        bool isDefault;
        bool typenameFound = getSubmoduleOrChannelTypeNameFromDeepAssignments(parentmodp, channelKey, connTypeName, isDefault);
        if (typenameFound && !isDefault)
            return connTypeName;

        // then, use **.typename option in the configuration if exists
        std::string key = srcGate->getFullPath() + "." + channelName;
        const char *NOTFOUNDVALUE = "\x1";
        std::string configTypename = getEnvir()->getConfig()->getAsString(key.c_str(), CFGID_TYPENAME, NOTFOUNDVALUE);
        if (configTypename != NOTFOUNDVALUE)
            return configTypename;

        // then, use default() expression from NED deep param assignments
        if (typenameFound)
            return connTypeName;

        // last, use default(expression) between angle braces from the NED file
        if (conn->getIsDefault()) {
            if (!opp_isempty(conn->getLikeExpr()))
                return parentmodp->par(conn->getLikeExpr()).stringValue();
            ExpressionElement *likeExprElement = findExpression(conn, "like-expr");
            if (likeExprElement)
                return evaluateAsString(likeExprElement, parentmodp, false);
        }
        throw cRuntimeError(parentmodp, "Unable to determine type name for channel: Missing config entry %s.%s and no default value", key.c_str(), CFGID_TYPENAME->getName());
    }
}

cChannel *cNedNetworkBuilder::createChannel(ConnectionElement *conn, cModule *parentmodp, cGate *srcGate)
{
    // resolve channel type
    const char *channelName = conn->getName();
    if (opp_isempty(channelName))
        channelName = nullptr;  // use nullptr to indicate "no name"

    cChannelType *channelType;
    try {
        // note: for **.channelname.liketype= lookups we cannot take the channel type @defaultname into account, because we don't have the type yet!
        std::string channelTypeName = getChannelTypeName(parentmodp, srcGate, conn, (channelName ? channelName : "channel"));
        bool usesLike = !opp_isempty(conn->getLikeType());
        channelType = usesLike ?
            findAndCheckChannelTypeLike(channelTypeName.c_str(), conn->getLikeType(), parentmodp) :
            findAndCheckChannelType(channelTypeName.c_str(), parentmodp);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, conn);
        throw;
    }
    // create channel object
    cChannel *channelp = channelType->create(channelName);  // it will choose a name if it's nullptr
    return channelp;
}

cChannelType *cNedNetworkBuilder::findAndCheckChannelType(const char *channelTypeName, cModule *modp)
{
    // TODO cache the result to speed up further lookups
    NedLookupContext context(currentDecl->getTree(), currentDecl->getFullName());
    std::string qname = resolveComponentType(context, channelTypeName);
    if (qname.empty())
        throw cRuntimeError(modp, "Cannot resolve channel type '%s' (not in the loaded NED files?)", channelTypeName);

    cComponentType *componentType = cComponentType::find(qname.c_str());
    if (!dynamic_cast<cChannelType *>(componentType))
        throw cRuntimeError(modp, "'%s' is not a channel type", qname.c_str());
    return (cChannelType *)componentType;
}

cChannelType *cNedNetworkBuilder::findAndCheckChannelTypeLike(const char *channelTypeName, const char *likeType, cModule *modp)
{
    // TODO cache the result to speed up further lookups

    // resolve the interface
    NedLookupContext context(currentDecl->getTree(), currentDecl->getFullName());
    std::string interfaceQName = cNedLoader::getInstance()->resolveNedType(context, likeType);
    cNedDeclaration *interfaceDecl = interfaceQName.empty() ? nullptr : (cNedDeclaration *)cNedLoader::getInstance()->lookup(interfaceQName.c_str());
    if (!interfaceDecl)
        throw cRuntimeError(modp, "Cannot resolve channel interface '%s'", likeType);
    if (interfaceDecl->getTree()->getTagCode() != NED_CHANNEL_INTERFACE)
        throw cRuntimeError(modp, "'%s' is not a channel interface", interfaceQName.c_str());

    // search for channel type that implements the interface
    std::vector<std::string> candidates = findTypeWithInterface(channelTypeName, interfaceQName.c_str());
    if (candidates.empty())
        throw cRuntimeError(modp, "No channel type named '%s' found that implements channel interface %s (not in the loaded NED files?)",
                channelTypeName, interfaceQName.c_str());
    if (candidates.size() > 1)
        throw cRuntimeError(modp, "More than one channel types named '%s' found that implement channel interface %s (use fully qualified name to disambiguate)",
                channelTypeName, interfaceQName.c_str());

    cComponentType *componenttype = cComponentType::find(candidates[0].c_str());
    if (!dynamic_cast<cChannelType *>(componenttype))
        throw cRuntimeError(modp, "'%s' is not a channel type", candidates[0].c_str());
    return (cChannelType *)componenttype;
}

ExpressionElement *cNedNetworkBuilder::findExpression(NedElement *node, const char *exprname)
{
    // find expression with given name in node
    if (!node)
        return nullptr;
    for (NedElement *child = node->getFirstChildWithTag(NED_EXPRESSION); child; child = child->getNextSiblingWithTag(NED_EXPRESSION)) {
        ExpressionElement *expr = (ExpressionElement *)child;
        if (!strcmp(expr->getTarget(), exprname))
            return expr;
    }
    return nullptr;
}

cDynamicExpression *cNedNetworkBuilder::getOrCreateExpression(ExpressionElement *exprNode, bool inSubcomponentScope)
{
    return cNedLoader::getInstance()->getCompiledExpression(exprNode, inSubcomponentScope);
}

long cNedNetworkBuilder::evaluateAsLong(ExpressionElement *exprNode, cComponent *contextComponent, bool inSubcomponentScope)
{
    cExpression::Context context(contextComponent);
    return evaluateAsLong(exprNode, &context, inSubcomponentScope);
}

bool cNedNetworkBuilder::evaluateAsBool(ExpressionElement *exprNode, cComponent *contextComponent, bool inSubcomponentScope)
{
    cExpression::Context context(contextComponent);
    return evaluateAsBool(exprNode, &context, inSubcomponentScope);
}

std::string cNedNetworkBuilder::evaluateAsString(ExpressionElement *exprNode, cComponent *contextComponent, bool inSubcomponentScope)
{
    cExpression::Context context(contextComponent);
    return evaluateAsString(exprNode, &context, inSubcomponentScope);
}

long cNedNetworkBuilder::evaluateAsLong(ExpressionElement *exprNode, cExpression::Context *context, bool inSubcomponentScope)
{
    try {
        cDynamicExpression *expr = getOrCreateExpression(exprNode, inSubcomponentScope);
        return expr->intValue(context);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, exprNode);
        throw;
    }
}

bool cNedNetworkBuilder::evaluateAsBool(ExpressionElement *exprNode, cExpression::Context *context, bool inSubcomponentScope)
{
    try {
        cDynamicExpression *expr = getOrCreateExpression(exprNode, inSubcomponentScope);
        return expr->boolValue(context);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, exprNode);
        throw;
    }
}

std::string cNedNetworkBuilder::evaluateAsString(ExpressionElement *exprNode, cExpression::Context *context, bool inSubcomponentScope)
{
    try {
        cDynamicExpression *expr = getOrCreateExpression(exprNode, inSubcomponentScope);
        return expr->stringValue(context);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, exprNode);
        throw;
    }
}

}  // namespace omnetpp

