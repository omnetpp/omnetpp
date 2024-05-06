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
#include "omnetpp/ccontextswitcher.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/cboolparimpl.h"
#include "omnetpp/cintparimpl.h"
#include "omnetpp/cstringparimpl.h"
#include "omnetpp/cobjectparimpl.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/fileline.h"
#include "../nedsupport.h"
#include "cnednetworkbuilder.h"
#include "cnedloader.h"

using namespace omnetpp::common;
using namespace omnetpp::nedsupport;
using namespace omnetpp::internal;

namespace omnetpp {

using omnetpp::FileLine;

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
    std::string loc = context ? context->getSourceLocation().str() : "";
    if (!loc.empty()) {
        std::string msg = std::string(e.what()) + ", at " + loc;
        cException *ce = dynamic_cast<cException *>(&e);
        if (ce)
            ce->setMessage(msg.c_str());
        else
            throw cRuntimeError("%s", msg.c_str());  // cannot set msg on existing exception object, throw new one
    }
}

bool cNedNetworkBuilder::getBooleanProperty(NedElement *componentNode, const char *name)
{
    ParametersElement *paramsNode = dynamic_cast<ParametersElement*>(componentNode->getFirstChildWithTag(NED_PARAMETERS));
    if (paramsNode)
        for (PropertyElement *prop = paramsNode->getFirstPropertyChild(); prop != nullptr; prop = prop->getNextPropertySibling())
            if (opp_strcmp(prop->getName(), name) == 0 && ASTNodeUtil::propertyAsBool(prop) == true)
                return true;
    return false;
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
        doParams(component, paramsNode, false, false);
    }

    // add this decl's gates (if there are any)
    GatesElement *gatesNode = decl->getGatesElement();
    if (gatesNode) {
        currentDecl = decl;  // switch "context"
        doGates((cModule *)component, gatesNode);
    }
}

cPar::Type cNedNetworkBuilder::translateParamType(int t)
{
    return t == PARTYPE_DOUBLE ? cPar::DOUBLE :
           t == PARTYPE_INT ? cPar::INT :
           t == PARTYPE_STRING ? cPar::STRING :
           t == PARTYPE_BOOL ? cPar::BOOL :
           t == PARTYPE_OBJECT ? cPar::OBJECT :
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

void cNedNetworkBuilder::doParams(cComponent *component, ParametersElement *paramsNode, bool isSubcomponent, bool isParametricSubcomponent)
{
    ASSERT(paramsNode != nullptr);
    for (ParamElement *paramNode = paramsNode->getFirstParamChild(); paramNode; paramNode = paramNode->getNextParamSibling())
        if (!paramNode->getIsPattern())
            doParam(component, paramNode, isSubcomponent, isParametricSubcomponent);
}

void cNedNetworkBuilder::doParam(cComponent *component, ParamElement *paramNode, bool isSubcomponent, bool isParametricSubcomponent)
{
    ASSERT(!paramNode->getIsPattern());  // we only deal with non-pattern assignments

    try {
        const char *paramName = paramNode->getName();

        // isSubComponent==false: we are called from cModuleType::addParametersAndGatesTo();
        // isSubComponent==true: we are called from assignSubcomponentParams().
        // if type==NONE, this is an inherited parameter (must have been added already)
        bool isNewParam = paramNode->getType() != PARTYPE_NONE;
        if (isNewParam && isSubcomponent)
            throw cRuntimeError(component, "Cannot add new parameter '%s' in a submodule or connection", paramName); // this error is normally discovered already during NED validation

        // if a "like"-submodule (or channel) contains a NED parameter assignment and the submodule
        // doesn't actually have such parameter, ignore it (do not raise an error). It is possibly
        // for another NED type that implements the same module interface -- there's no way to know.
        if (isSubcomponent && isParametricSubcomponent && !component->hasPar(paramName))
            return;

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

            bool isMutable = false;
            if (cProperty *mutableProp = paramProps->get("mutable"))
                isMutable = opp_strcmp(mutableProp->getValue(cProperty::DEFAULTKEY), "false") != 0;
            impl->setIsMutable(isMutable);

            cProperty *unitProp = paramProps->get("unit");
            const char *declUnit = unitProp ? unitProp->getValue(cProperty::DEFAULTKEY) : nullptr;
            impl->setUnit(declUnit);

            cProperty *enumProp = paramProps->get("enum");
            if (enumProp) {
                std::vector<std::string> enumValues;
                for (int i = 0; i < enumProp->getNumValues(""); i++)
                    enumValues.push_back(enumProp->getValue("", i));
                if (cStringParImpl *stringParImpl = dynamic_cast<cStringParImpl*>(impl))
                    stringParImpl->setEnumValues(enumValues);
                else
                    throw cRuntimeError(component, "The @enum property is only allowed on string parameters");
            }

            if (impl->getType() == cPar::OBJECT) {
                cProperty *classProp = paramProps->get("class");
                const char *declClass = classProp ? classProp->getValue(cProperty::DEFAULTKEY) : nullptr;
                cObjectParImpl *objParImpl = dynamic_cast<cObjectParImpl*>(impl);
                ASSERT(objParImpl);
                objParImpl->setExpectedType(declClass);
            }
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
        ExprRef valueExpr(paramNode, ParamElement::ATT_VALUE);
        if (!valueExpr.empty()) {
            ASSERT(impl == component->par(paramName).impl() && !impl->isShared());
            try {
                cDynamicExpression *expr = new cDynamicExpression();
                expr->parseNedExpr(valueExpr.getExprText());
                impl->setBaseDirectory(paramNode->getSourceFileDirectory());
                impl->setExpression(expr);
                auto nodeLoc = paramNode->getSourceLocation();
                omnetpp::FileLine loc(nodeLoc.file.c_str(), nodeLoc.line);
                if (expr->isAConstant())
                    impl->convertToConst(nullptr);
                else
                    expr->setSourceLocation(loc); // note: if we do it earlier, file:line may show up TWICE in the error message: one added inside convertToConst(), the other in the 'catch' block at the bottom of this function
                impl->setIsSet(!paramNode->getIsDefault());
                impl->setSourceLocation(loc);
            }
            catch (std::exception& e) {
                throw cRuntimeError(component, "Error setting up parameter '%s': %s", paramName, e.what());
            }
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

void cNedNetworkBuilder::doGates(cModule *module, GatesElement *gatesNode)
{
    ASSERT(gatesNode != nullptr);
    for (NedElement *child = gatesNode->getFirstChildWithTag(NED_GATE); child; child = child->getNextSiblingWithTag(NED_GATE))
        doGate(module, (GateElement *)child);
}

void cNedNetworkBuilder::doGate(cModule *module, GateElement *gateNode)
{
    try {
        if (gateNode->getType() != GATETYPE_NONE) {  // type=GATETYPE_NONE -> element refers to existing gate
            const char *name = gateNode->getName();
            cGate::Type type = translateGateType(gateNode->getType());
            if (!gateNode->getIsVector())
                module->addGate(name, type);
            else
                module->addGateVector(name, type, 0); // create with zero size initially, resize later
        }
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
                doAssignParametersFromPatterns(component, prefix, submodPatterns, child);
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
                doAssignParametersFromPatterns(component, prefix, patterns, parent);
        }
    }
}

void cNedNetworkBuilder::doAssignParametersFromPatterns(cComponent *component, const std::string& prefix, const std::vector<PatternData>& patterns, cComponent *evalContext)
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
                    doAssignParameterFromPattern(par, patterns[j].patternNode, evalContext);
                    if (par.isSet())
                        break;
                }
            }
        }
    }
}

void cNedNetworkBuilder::doAssignParameterFromPattern(cPar& par, ParamElement *patternNode, cComponent *evalContext)
{
    // note: this code should look similar to relevant part of doParam()
    try {
        ASSERT(patternNode->getIsPattern());
        cParImpl *impl = par.copyIfShared();
        ExprRef valueExpr(patternNode, ParamElement::ATT_VALUE);
        if (!valueExpr.empty()) {
            // assign the parameter
            ASSERT(impl == par.impl() && !impl->isShared());
            cDynamicExpression *expr = new cDynamicExpression();
            expr->parseNedExpr(valueExpr.getExprText());
            impl->setBaseDirectory(patternNode->getSourceFileDirectory());
            impl->setExpression(expr);
            auto nodeLoc = patternNode->getSourceLocation();
            omnetpp::FileLine loc(nodeLoc.file.c_str(), nodeLoc.line);
            if (expr->isAConstant())
                impl->convertToConst(nullptr);
            else
                expr->setSourceLocation(loc); // note: if we do it earlier, file:line may show up TWICE in the error message: one added inside convertToConst(), the other in the 'catch' block at the bottom of this function
            par.setEvaluationContext(evalContext);
            impl->setIsSet(!patternNode->getIsDefault());
            impl->setSourceLocation(loc);
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

void cNedNetworkBuilder::doGateSizes(cModule *module, GatesElement *gatesNode)
{
    ASSERT(gatesNode != nullptr);
    for (NedElement *child = gatesNode->getFirstChildWithTag(NED_GATE); child; child = child->getNextSiblingWithTag(NED_GATE))
        doGateSize(module, (GateElement *)child);
}

void cNedNetworkBuilder::doGateSize(cModule *module, GateElement *gateNode)
{
    try {
        if (gateNode->getIsVector()) {
            // if there's a gatesize expression given, apply it
            ExprRef vectorSizeExpr(gateNode, GateElement::ATT_VECTOR_SIZE);
            if (!vectorSizeExpr.empty()) {
                // The expression first needs to be compiled into a cParImpl
                // for evaluation; these cParImpl's are cached
                cParImpl *value = currentDecl->getSharedParImplFor(gateNode);
                if (!value) {
                    // not yet seen, compile and cache it
                    cDynamicExpression *dynamicExpr = new cDynamicExpression();
                    dynamicExpr->parseNedExpr(vectorSizeExpr.getExprText());
                    value = new cIntParImpl();
                    value->setName("gatesize-expression");
                    value->setBaseDirectory(gateNode->getSourceFileDirectory());
                    value->setExpression(dynamicExpr);
                    if (dynamicExpr->isAConstant())
                        value->convertToConst(nullptr);
                    currentDecl->putSharedParImplFor(gateNode, value);
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
        doGateSizes(module, gatesNode);
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
    buildRecursively(modp, decl);

    // check if there are unconnected gates left -- unless unconnected gates were already permitted in the super type
    ConnectionsElement *connectionsNode = decl->getConnectionsElement();
    if ((!connectionsNode || !connectionsNode->getAllowUnconnected()) && !superTypeAllowsUnconnected(decl))
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
    SubmodulesElement *submodulesNode = currentDecl->getSubmodulesElement();
    if (submodulesNode)
        for (SubmoduleElement *submoduleNode = submodulesNode->getFirstSubmoduleChild(); submoduleNode; submoduleNode = submoduleNode->getNextSubmoduleSibling())
            addSubmodule(modp, submoduleNode);

    // loop through connections and add them
    ConnectionsElement *connectionsNode = currentDecl->getConnectionsElement();
    if (connectionsNode)
        for (NedElement *childNode = connectionsNode->getFirstChild(); childNode; childNode = childNode->getNextSibling())
            if (childNode->getTagCode() == NED_CONNECTION || childNode->getTagCode() == NED_CONNECTION_GROUP)
                addConnectionOrConnectionGroup(modp, childNode);

}

bool cNedNetworkBuilder::superTypeAllowsUnconnected(cNedDeclaration *decl) const
{
    // follow through the inheritance chain, and return true if we find an "allowunconnected" anywhere
    while (decl->numExtendsNames() > 0) {
        const char *superName = decl->getExtendsName(0);
        decl = cNedLoader::getInstance()->getDecl(superName);
        ASSERT(decl);  // all super classes must be loaded before we start building
        ConnectionsElement *connectionsNode = decl->getConnectionsElement();
        if (connectionsNode && connectionsNode->getAllowUnconnected())
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
    return cNedLoader::getInstance()->lookupNedType(context, nedTypeName, ComponentTypeNames());
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
    std::string interfaceQName = cNedLoader::getInstance()->lookupNedType(context, likeType);
    cNedDeclaration *interfaceDecl = interfaceQName.empty() ? nullptr : (cNedDeclaration *)cNedLoader::getInstance()->lookup(interfaceQName.c_str());
    if (!interfaceDecl)
        throw cRuntimeError(modp, "Submodule %s: Cannot resolve module interface '%s'",
                submodName, likeType);
    if (interfaceDecl->getTree()->getTagCode() != NED_MODULE_INTERFACE)
        throw cRuntimeError(modp, "Submodule %s: '%s' is not a module interface",
                submodName, interfaceQName.c_str());

    // handle @omittedTypename
    if (opp_isempty(modTypeName)) {
        cProperties *properties = currentDecl->getSubmoduleProperties(submodName, interfaceQName.c_str());
        cProperty *omittedTypeNameProperty = properties->get("omittedTypename");
        if (omittedTypeNameProperty) {
            const char *omittedTypeName = omittedTypeNameProperty->getValue();
            if (opp_isempty(omittedTypeName))
                return nullptr; // meaning "don't create this submodule"
            modTypeName = omittedTypeName;
        }
    }

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

std::string cNedNetworkBuilder::getSubmoduleTypeName(cModule *modp, SubmoduleElement *submoduleNode, int index)
{
    // note: this code is nearly identical to getChannelTypeName(), with subtle differences
    const char *submodName = submoduleNode->getName();
    if (opp_isempty(submoduleNode->getLikeType())) {
        return submoduleNode->getType();
    }
    else {
        // first, try to use expression between angle braces from the NED file
        if (!submoduleNode->getIsDefault()) {
            ExprRef likeExpr(submoduleNode, SubmoduleElement::ATT_LIKE_EXPR);
            if (!likeExpr.empty())
                return evaluateAsString(likeExpr, modp);
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
        if (submoduleNode->getIsDefault()) {
            ExprRef likeExpr(submoduleNode, SubmoduleElement::ATT_LIKE_EXPR);
            if (!likeExpr.empty())
                return evaluateAsString(likeExpr, modp);
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
                        ExprRef typeNameExpr(patterns[j].patternNode, ParamElement::ATT_VALUE);
                        if (!typeNameExpr.empty()) {
                            outTypeName = evaluateAsString(typeNameExpr, mod);
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
                        ExprRef typeNameExpr(submodPatterns[j].patternNode, ParamElement::ATT_VALUE);
                        if (!typeNameExpr.empty()) {
                            outTypeName = evaluateAsString(typeNameExpr, mod);
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

void cNedNetworkBuilder::addSubmodule(cModule *compoundModule, SubmoduleElement *submoduleNode)
{
    // if there is a @dynamic or @dynamic(true), do not instantiate the submodule
    if (getBooleanProperty(submoduleNode, "dynamic"))
        return;

    // find vector size expression
    const char *submodName = submoduleNode->getName();
    bool usesLike = !opp_isempty(submoduleNode->getLikeType());
    ExprRef vectorSizeExpr(submoduleNode, SubmoduleElement::ATT_VECTOR_SIZE);

    // create submodule
    if (vectorSizeExpr.empty()) {
        cModuleType *submodType;
        try {
            std::string submodTypeName = getSubmoduleTypeName(compoundModule, submoduleNode);

            // handle conditional submodule
            ConditionElement *conditionNode = submoduleNode->getFirstConditionChild();
            if (conditionNode) {
                ExprRef conditionExpr(conditionNode, ConditionElement::ATT_CONDITION);
                const char *baseDirectory = compoundModule->getComponentType()->getSourceFileDirectory();
                NedExpressionContext context(compoundModule, baseDirectory, NedExpressionContext::SUBMODULE_CONDITION, submodTypeName.c_str());
                if (evaluateAsBool(conditionExpr, &context) == false)
                    return;
            }

            submodType = usesLike ?
                findAndCheckModuleTypeLike(submodTypeName.c_str(), submoduleNode->getLikeType(), compoundModule, submodName) :
                findAndCheckModuleType(submodTypeName.c_str(), compoundModule, submodName);
            if (submodType == nullptr)
                return;
        }
        catch (std::exception& e) {
            updateOrRethrowException(e, submoduleNode);
            throw;
        }

        cModule *submodp = submodType->create(submodName, compoundModule);
        cContextSwitcher __ctx(submodp);  // params need to be evaluated in the module's context
        submodp->finalizeParameters();  // also sets up gate sizes declared inside the type
        setupSubmoduleGateVectors(submodp, submoduleNode);
    }
    else {
        // handle conditional submodule vector
        ConditionElement *conditionNode = submoduleNode->getFirstConditionChild();
        if (conditionNode) {
            ExprRef conditionExpr(conditionNode, ConditionElement::ATT_CONDITION);
            // note: "typename" may NOT occur in vector submodules' condition, because type is
            // per-element, and we want to evaluate the condition only once for the whole vector
            const char *baseDirectory = compoundModule->getComponentType()->getSourceFileDirectory();
            NedExpressionContext context(compoundModule, baseDirectory, NedExpressionContext::SUBMODULE_ARRAY_CONDITION, nullptr);
            if (evaluateAsBool(conditionExpr, &context) == false)
                return;
        }

        // note: we don't try to resolve moduleType if vector size is zero
        int vectorSize = (int)evaluateAsLong(vectorSizeExpr, compoundModule);
        compoundModule->addSubmoduleVector(submodName, vectorSize);
        cModuleType *submodType = nullptr;
        for (int index = 0; index < vectorSize; index++) {
            if (!submodType || usesLike) {
                try {
                    std::string submodTypeName = getSubmoduleTypeName(compoundModule, submoduleNode, index);
                    submodType = usesLike ?
                        findAndCheckModuleTypeLike(submodTypeName.c_str(), submoduleNode->getLikeType(), compoundModule, submodName) :
                        findAndCheckModuleType(submodTypeName.c_str(), compoundModule, submodName);
                }
                catch (std::exception& e) {
                    updateOrRethrowException(e, submoduleNode);
                    throw;
                }
            }
            if (submodType != nullptr) {  // note: this way we can create "holey" arrays!
                cModule *submodp = submodType->create(submodName, compoundModule, index);
                cContextSwitcher __ctx(submodp);  // params need to be evaluated in the module's context
                submodp->finalizeParameters();  // also sets up gate sizes declared inside the type
                setupSubmoduleGateVectors(submodp, submoduleNode);
            }
        }
    }

    // Note: buildInside() will be called when connections have been built out
    // on this level too.
}

void cNedNetworkBuilder::assignSubcomponentParams(cComponent *subcomponent, NedElement *subcomponentNode)
{
    ParametersElement *paramsNode = (ParametersElement *)subcomponentNode->getFirstChildWithTag(NED_PARAMETERS);
    if (paramsNode) {
        const char *likeType = subcomponentNode->getAttribute("like-type");
        bool isParametricSubcomponent = !opp_isempty(likeType);
        doParams(subcomponent, paramsNode, true, isParametricSubcomponent);
    }
}

void cNedNetworkBuilder::setupSubmoduleGateVectors(cModule *submodule, NedElement *submoduleNode)
{
    GatesElement *gatesNode = (GatesElement *)submoduleNode->getFirstChildWithTag(NED_GATES);
    if (gatesNode)
        doGateSizes(submodule, gatesNode);
}

void cNedNetworkBuilder::addConnectionOrConnectionGroup(cModule *modp, NedElement *connOrConnGroupNode)
{
    // this is tricky: elements representing "for" and "if" in NED are children
    // of the ConnectionElement or ConnectionGroupElement. So, first we have to go through
    // and execute the LoopElement and ConditionElement children recursively to get
    // nested loops etc, then after (inside) the last one create the connection(s)
    // themselves, which is (are) then parent of the LoopNode/ConditionNode.
    LoopVar::clear();
    doConnOrConnGroupBody(modp, connOrConnGroupNode, connOrConnGroupNode->getFirstChild());
}

void cNedNetworkBuilder::doConnOrConnGroupBody(cModule *modp, NedElement *connOrConnGroupNode, NedElement *loopOrConditionNode)
{
    // find first "for" or "if" at loopOrCondition (or among its next siblings)
    while (loopOrConditionNode && loopOrConditionNode->getTagCode() != NED_LOOP && loopOrConditionNode->getTagCode() != NED_CONDITION)
        loopOrConditionNode = loopOrConditionNode->getNextSibling();

    // if there's a "for" or "if", do that, otherwise create the connection(s) themselves
    if (loopOrConditionNode)
        doLoopOrCondition(modp, loopOrConditionNode);
    else
        doAddConnOrConnGroup(modp, connOrConnGroupNode);
}

void cNedNetworkBuilder::doLoopOrCondition(cModule *modp, NedElement *loopOrConditionNode)
{
    if (loopOrConditionNode->getTagCode() == NED_CONDITION) {
        // check condition
        ConditionElement *conditionNode = (ConditionElement *)loopOrConditionNode;
        ExprRef conditionExpr(conditionNode, ConditionElement::ATT_CONDITION);
        if (evaluateAsBool(conditionExpr, modp) == true) {
            // do the body of the "if": either further "for"'s and "if"'s, or
            // the connection(group) itself that we are children of.
            doConnOrConnGroupBody(modp, loopOrConditionNode->getParent(), loopOrConditionNode->getNextSibling());
        }
    }
    else if (loopOrConditionNode->getTagCode() == NED_LOOP) {
        LoopElement *loopNode = (LoopElement *)loopOrConditionNode;
        ExprRef startExpr(loopNode, LoopElement::ATT_FROM_VALUE);
        ExprRef endExpr(loopNode, LoopElement::ATT_TO_VALUE);
        long start = evaluateAsLong(startExpr, modp);
        long end = evaluateAsLong(endExpr, modp);

        // do loop
        long& i = LoopVar::pushVar(loopNode->getParamName());
        for (i = start; i <= end; i++) {
            // do the body of the "if": either further "for"'s and "if"'s, or
            // the connection(group) itself that we are children of.
            doConnOrConnGroupBody(modp, loopOrConditionNode->getParent(), loopOrConditionNode->getNextSibling());
        }
        LoopVar::popVar();
    }
    else {
        ASSERT(false);
    }
}

void cNedNetworkBuilder::doAddConnOrConnGroup(cModule *modp, NedElement *connOrConnGroupNode)
{
    if (connOrConnGroupNode->getTagCode() == NED_CONNECTION) {
        doAddConnection(modp, (ConnectionElement *)connOrConnGroupNode);
    }
    else if (connOrConnGroupNode->getTagCode() == NED_CONNECTION_GROUP) {
        ConnectionGroupElement *connGroupNode = (ConnectionGroupElement *)connOrConnGroupNode;
        for (ConnectionElement *connNode = connGroupNode->getFirstConnectionChild(); connNode; connNode = connNode->getNextConnectionSibling())
            doConnOrConnGroupBody(modp, connNode, connNode->getFirstChild());
    }
    else {
        ASSERT(false);
    }
}

void cNedNetworkBuilder::doAddConnection(cModule *modp, ConnectionElement *connectionNode)
{
// FIXME spurious error message comes when trying to connect INOUT gate with "-->"
    try {
        if (!connectionNode->getIsBidirectional()) {
            // find gates and create connection
            ExprRef srcModuleIndexExpr(connectionNode, ConnectionElement::ATT_SRC_MODULE_INDEX);
            ExprRef srcGateIndexExpr(connectionNode, ConnectionElement::ATT_SRC_GATE_INDEX);
            ExprRef destModuleIndexExpr(connectionNode, ConnectionElement::ATT_DEST_MODULE_INDEX);
            ExprRef destGateIndexExpr(connectionNode, ConnectionElement::ATT_DEST_GATE_INDEX);
            cGate *srcGate = resolveGate(modp, connectionNode->getSrcModule(), srcModuleIndexExpr,
                        connectionNode->getSrcGate(), srcGateIndexExpr,
                        connectionNode->getSrcGateSubg(), connectionNode->getSrcGatePlusplus());
            cGate *destGate = resolveGate(modp, connectionNode->getDestModule(), destModuleIndexExpr,
                        connectionNode->getDestGate(), destGateIndexExpr,
                        connectionNode->getDestGateSubg(), connectionNode->getDestGatePlusplus());

            // check directions
            cGate *errorGate = nullptr;
            if (srcGate->getOwnerModule() == modp ? srcGate->getType() != cGate::INPUT : srcGate->getType() != cGate::OUTPUT)
                errorGate = srcGate;
            if (destGate->getOwnerModule() == modp ? destGate->getType() != cGate::OUTPUT : destGate->getType() != cGate::INPUT)
                errorGate = destGate;
            if (errorGate)
                throw cRuntimeError(modp, "Gate %s is being connected in the wrong direction",
                        errorGate->getFullPath().c_str());

            doConnectGates(modp, srcGate, destGate, connectionNode);
        }
        else {
            // find gates and create connection in both ways
            if (connectionNode->getSrcGateSubg() != SUBGATE_NONE || connectionNode->getDestGateSubg() != SUBGATE_NONE)
                throw cRuntimeError(modp, "gate$i or gate$o used with bidirectional connections");

            // now: 1 is input, 2 is output, except for parent module gates where it is the other way round
            // (because we connect xx.out --> yy.in, but xx.out --> out!)
            cGate *srcGate1, *srcGate2, *destGate1, *destGate2;
            ExprRef srcModuleIndexExpr(connectionNode, ConnectionElement::ATT_SRC_MODULE_INDEX);
            ExprRef srcGateIndexExpr(connectionNode, ConnectionElement::ATT_SRC_GATE_INDEX);
            ExprRef destModuleIndexExpr(connectionNode, ConnectionElement::ATT_DEST_MODULE_INDEX);
            ExprRef destGateIndexExpr(connectionNode, ConnectionElement::ATT_DEST_GATE_INDEX);
            resolveInoutGate(modp, connectionNode->getSrcModule(), srcModuleIndexExpr,
                    connectionNode->getSrcGate(), srcGateIndexExpr,
                    connectionNode->getSrcGatePlusplus(),
                    srcGate1, srcGate2);
            resolveInoutGate(modp, connectionNode->getDestModule(), destModuleIndexExpr,
                    connectionNode->getDestGate(), destGateIndexExpr,
                    connectionNode->getDestGatePlusplus(),
                    destGate1, destGate2);

            doConnectGates(modp, srcGate2, destGate1, connectionNode);
            doConnectGates(modp, destGate2, srcGate1, connectionNode);
        }
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, connectionNode);
        throw;
    }
}

void cNedNetworkBuilder::doConnectGates(cModule *modp, cGate *srcGate, cGate *destGate, ConnectionElement *connectionNode)
{
    if (opp_isempty(connectionNode->getName()) && opp_isempty(connectionNode->getType()) &&
        opp_isempty(connectionNode->getLikeType()) && !connectionNode->getFirstParametersChild())
    {
        srcGate->connectTo(destGate);
    }
    else {
        if (srcGate->getNextGate() || destGate->getPreviousGate()) {
            bool reconnectAllowed = getBooleanProperty(connectionNode, "reconnect");
            if (reconnectAllowed) {
                if (srcGate->getNextGate())
                    srcGate->disconnect();
                if (destGate->getPreviousGate())
                    destGate->getPreviousGate()->disconnect();
            }
        }
        cChannel *channel = createChannel(connectionNode, modp, srcGate);
        if (channel == nullptr)
            srcGate->connectTo(destGate);
        else {
            channel->setNedConnectionElementId(connectionNode->getId());  // so that properties will be found
            srcGate->connectTo(destGate, channel);
            assignSubcomponentParams(channel, connectionNode);
            channel->finalizeParameters();
        }
    }
}

cGate *cNedNetworkBuilder::resolveGate(cModule *compoundModule,
        const char *moduleName, const ExprRef& moduleIndexExpr,
        const char *gateName, const ExprRef& gateIndexExpr,
        int subgate, bool isPlusPlus)
{
    if (isPlusPlus && !gateIndexExpr.empty())
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
    if (gateIndexExpr.empty() && !isPlusPlus) {
        gate = module->gate(gateName2);
    }
    else if (isPlusPlus) {
        if (module == compoundModule) {
            gate = module->getOrCreateFirstUnconnectedGate(gateName2, 0, true, false);  // inside, don't expand
            if (!gate)
                throw cRuntimeError(module, "%s[] gates are all connected, no gate left for '++' operator "
                                            "when connecting the gates on the inside (note that the network is built top-down)",
                                            gateName);
        }
        else {
            gate = module->getOrCreateFirstUnconnectedGate(gateName2, 0, false, true);  // outside, expand
            ASSERT(gate != nullptr);
        }
    }
    else {  // (gateIndexExpr)
        int gateIndex = (int)evaluateAsLong(gateIndexExpr, compoundModule);
        gate = module->gate(gateName2, gateIndex);
    }
    return gate;
}

void cNedNetworkBuilder::resolveInoutGate(cModule *compoundModule,
        const char *moduleName, const ExprRef& moduleIndexExpr,
        const char *gateName, const ExprRef& gateIndexExpr,
        bool isPlusPlus,
        cGate *& outGate1, cGate *& outGate2)
{
    if (isPlusPlus && !gateIndexExpr.empty())
        throw cRuntimeError(compoundModule, "Both '++' and gate index expression used in a connection");

    cModule *module = resolveModuleForConnection(compoundModule, moduleName, moduleIndexExpr);

    outGate1 = outGate2 = nullptr;
    if (gateIndexExpr.empty() && !isPlusPlus) {
        // optimization possibility: add gatePair() method to cModule to spare one lookup
        outGate1 = module->gateHalf(gateName, cGate::INPUT);
        outGate2 = module->gateHalf(gateName, cGate::OUTPUT);
    }
    else if (isPlusPlus) {
        if (module == compoundModule) {
            module->getOrCreateFirstUnconnectedGatePair(gateName, true, false, outGate1, outGate2);  // inside, don't expand
            if (!outGate1 || !outGate2)
                throw cRuntimeError(compoundModule, "%s[] gates are all connected, no gate left for '++' operator "
                                                    "when connecting the gates on the inside (note that the network is built top-down)",
                                                    gateName);
        }
        else {
            module->getOrCreateFirstUnconnectedGatePair(gateName, false, true, outGate1, outGate2);  // outside, expand
            ASSERT(outGate1 && outGate2);
        }
    }
    else {  // (gateIndexExpr)
        // optimization possibility: add gatePair() method to cModule to spare one lookup
        int gateIndex = (int)evaluateAsLong(gateIndexExpr, compoundModule);
        outGate1 = module->gateHalf(gateName, cGate::INPUT, gateIndex);
        outGate2 = module->gateHalf(gateName, cGate::OUTPUT, gateIndex);
    }

    if (module == compoundModule) {
        std::swap(outGate1, outGate2);
    }
}

cModule *cNedNetworkBuilder::resolveModuleForConnection(cModule *compoundModule, const char *moduleName, const ExprRef& moduleIndexExpr)
{
    if (opp_isempty(moduleName)) {
        return compoundModule;
    }
    else {
        int moduleIndex = moduleIndexExpr.empty() ? -1 : (int)evaluateAsLong(moduleIndexExpr, compoundModule);
        cModule *module = compoundModule->getSubmodule(moduleName, moduleIndex);
        if (!module) {
            if (moduleIndexExpr.empty())
                throw cRuntimeError(module, "No submodule '%s' to be connected", moduleName);
            else
                throw cRuntimeError(module, "No submodule '%s[%d]' to be connected", moduleName, moduleIndex);
        }
        return module;
    }
}

std::string cNedNetworkBuilder::getChannelTypeName(cModule *parentmodp, cGate *srcGate, ConnectionElement *connectionNode, const char *channelName)
{
    // note: this code is nearly identical to getSubmoduleTypeName(), with subtle differences
    if (opp_isempty(connectionNode->getLikeType())) {
        if (!opp_isempty(connectionNode->getType())) {
            return connectionNode->getType();
        }
        else {
            bool hasDelayChannelParams = false, hasOtherParams = false;
            ParametersElement *parametersNode = connectionNode->getFirstParametersChild();
            if (parametersNode) {
                for (ParamElement *paramNode = parametersNode->getFirstParamChild(); paramNode; paramNode = paramNode->getNextParamSibling())
                    if (strcmp(paramNode->getName(), "delay") == 0 || strcmp(paramNode->getName(), "disabled") == 0)
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
        if (!connectionNode->getIsDefault()) {
            ExprRef likeExpr(connectionNode, ConnectionElement::ATT_LIKE_EXPR);
            if (!likeExpr.empty())
                return evaluateAsString(likeExpr, parentmodp);
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
        if (connectionNode->getIsDefault()) {
            ExprRef likeExpr(connectionNode, ConnectionElement::ATT_LIKE_EXPR);
            if (!likeExpr.empty())
                return evaluateAsString(likeExpr, parentmodp);
        }
        throw cRuntimeError(parentmodp, "Unable to determine type name for channel: Missing config entry %s.%s and no default value", key.c_str(), CFGID_TYPENAME->getName());
    }
}

cChannel *cNedNetworkBuilder::createChannel(ConnectionElement *connectionNode, cModule *parentmodp, cGate *srcGate)
{
    // resolve channel type
    const char *channelName = connectionNode->getName();
    if (opp_isempty(channelName))
        channelName = nullptr;  // use nullptr to indicate "no name"

    cChannelType *channelType;
    try {
        // note: for **.channelname.liketype= lookups we cannot take the channel type @defaultname into account, because we don't have the type yet!
        std::string channelTypeName = getChannelTypeName(parentmodp, srcGate, connectionNode, (channelName ? channelName : "channel"));
        bool usesLike = !opp_isempty(connectionNode->getLikeType());
        channelType = usesLike ?
            findAndCheckChannelTypeLike(channelTypeName.c_str(), connectionNode->getLikeType(), parentmodp, connectionNode->getId()) :
            findAndCheckChannelType(channelTypeName.c_str(), parentmodp);
        if (channelType == nullptr)
            return nullptr; // "omitted"
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, connectionNode);
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

cChannelType *cNedNetworkBuilder::findAndCheckChannelTypeLike(const char *channelTypeName, const char *likeType, cModule *modp, int connectionId)
{
    // TODO cache the result to speed up further lookups

    // resolve the interface
    NedLookupContext context(currentDecl->getTree(), currentDecl->getFullName());
    std::string interfaceQName = cNedLoader::getInstance()->lookupNedType(context, likeType);
    cNedDeclaration *interfaceDecl = interfaceQName.empty() ? nullptr : (cNedDeclaration *)cNedLoader::getInstance()->lookup(interfaceQName.c_str());
    if (!interfaceDecl)
        throw cRuntimeError(modp, "Cannot resolve channel interface '%s'", likeType);
    if (interfaceDecl->getTree()->getTagCode() != NED_CHANNEL_INTERFACE)
        throw cRuntimeError(modp, "'%s' is not a channel interface", interfaceQName.c_str());

    // handle @omittedTypename
    if (opp_isempty(channelTypeName)) {
        cProperties *properties = currentDecl->getConnectionProperties(connectionId, interfaceQName.c_str());
        cProperty *omittedTypeNameProperty = properties->get("omittedTypename");
        if (omittedTypeNameProperty) {
            const char *omittedTypeName = omittedTypeNameProperty->getValue();
            if (opp_isempty(omittedTypeName))
                return nullptr; // meaning "no channel"
            channelTypeName = omittedTypeName;
        }
    }

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

cDynamicExpression *cNedNetworkBuilder::getOrCreateExpression(const ExprRef& exprRef)
{
    return cNedLoader::getInstance()->getCompiledExpression(exprRef);
}

long cNedNetworkBuilder::evaluateAsLong(const ExprRef& exprRef, cComponent *contextComponent)
{
    const char *baseDirectory = contextComponent->getComponentType()->getSourceFileDirectory();
    cExpression::Context context(contextComponent, baseDirectory);
    return evaluateAsLong(exprRef, &context);
}

bool cNedNetworkBuilder::evaluateAsBool(const ExprRef& exprRef, cComponent *contextComponent)
{
    const char *baseDirectory = contextComponent->getComponentType()->getSourceFileDirectory();
    cExpression::Context context(contextComponent, baseDirectory);
    return evaluateAsBool(exprRef, &context);
}

std::string cNedNetworkBuilder::evaluateAsString(const ExprRef& exprRef, cComponent *contextComponent)
{
    const char *baseDirectory = contextComponent->getComponentType()->getSourceFileDirectory();
    cExpression::Context context(contextComponent, baseDirectory);
    return evaluateAsString(exprRef, &context);
}

long cNedNetworkBuilder::evaluateAsLong(const ExprRef& exprRef, cExpression::Context *context)
{
    try {
        cDynamicExpression *expr = getOrCreateExpression(exprRef);
        return expr->intValue(context);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, exprRef.getNode());
        throw;
    }
}

bool cNedNetworkBuilder::evaluateAsBool(const ExprRef& exprRef, cExpression::Context *context)
{
    try {
        cDynamicExpression *expr = getOrCreateExpression(exprRef);
        return expr->boolValue(context);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, exprRef.getNode());
        throw;
    }
}

std::string cNedNetworkBuilder::evaluateAsString(const ExprRef& exprRef, cExpression::Context *context)
{
    try {
        cDynamicExpression *expr = getOrCreateExpression(exprRef);
        return expr->stringValue(context);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, exprRef.getNode());
        throw;
    }
}

}  // namespace omnetpp

