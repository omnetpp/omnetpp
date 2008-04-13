//==========================================================================
//   CNEDNETWORKBUILDER.CC
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

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
#include "cbasicchannel.h"
#include "ccomponenttype.h"
#include "clongparimpl.h"
#include "cboolparimpl.h"
#include "cstringparimpl.h"
#include "cdisplaystring.h"
#include "cconfiguration.h"
#include "cconfigkey.h"

#include "nedelements.h"
#include "nederror.h"

#include "nedparser.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedsyntaxvalidator.h"
#include "xmlgenerator.h"  // for debugging

#include "cneddeclaration.h"
#include "cnednetworkbuilder.h"
#include "cnedloader.h"
#include "cexpressionbuilder.h"
#include "nedsupport.h"
#include "commonutil.h"  // TRACE()
#include "stringutil.h"
#include "patternmatcher.h"

USING_NAMESPACE

Register_PerRunConfigEntry(CFGID_MAX_MODULE_NESTING, "max-module-nesting", CFG_INT, "50", "The maximum allowed depth of submodule nesting. This is used to catch accidental infinite recursions in NED.");
Register_PerObjectConfigEntry(CFGID_TYPE_NAME, "type-name", CFG_STRING, NULL, "Specifies type for submodules and channels declared with 'like <>'.");


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

void cNEDNetworkBuilder::addParametersTo(cComponent *component, cNEDDeclaration *decl)
{
    //TRACE("addParametersTo(%s), decl=%s", component->fullPath().c_str(), decl->name()); //XXX

    //TODO for performance: component->reallocParamv(decl->getParameterDeclarations().size());

    // recursively add and assign super types' parameters
    if (decl->numExtendsNames() > 0)
    {
        const char *superName = decl->extendsName(0);
        cNEDDeclaration *superDecl = cNEDLoader::instance()->getDecl(superName);
        addParametersTo(component, superDecl);
    }

    // add this decl parameters / assignments
    ParametersElement *paramsNode = decl->getParametersElement();
    if (paramsNode)
    {
        currentDecl = decl; // switch "context"
        doParams(component, paramsNode, false);
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
    for (NEDElement *child=paramsNode->getFirstChildWithTag(NED_PARAM); child; child=child->getNextSiblingWithTag(NED_PARAM))
        doParam(component, (ParamElement *)child, isSubcomponent);
}

void cNEDNetworkBuilder::doParam(cComponent *component, ParamElement *paramNode, bool isSubcomponent)
{
    const char *paramName = paramNode->getName();

    // isSubComponent==true: we are called from cModuleType::addParametersTo();
    // isSubComponent==false: we are called from assignSubcomponentParams().
    // if type==NONE, this is an inherited parameter (must have been added already)
    bool isNewParam = !isSubcomponent && paramNode->getType()!=NED_PARTYPE_NONE;
    ASSERT(component->hasPar(paramName) != isNewParam);

    cParImpl *impl = currentDecl->getSharedParImplFor(paramNode);
    if (impl)
    {
        //printf("   +++ reusing param impl for %s\n", paramName);

        // we've already been at this point in the NED files sometime,
        // so we can reuse what we produced then
        ASSERT(impl->isName(paramName));
        if (isNewParam)
            component->addPar(impl);
        else {
            cPar& par = component->par(paramName); // must exist already
            par.setImpl(impl);
        }
        return;
    }

    try {
        if (isNewParam) {
            //printf("   +++ adding param %s\n", paramName);
            impl = cParImpl::createWithType(translateParamType(paramNode->getType()));
            impl->setName(paramName);
            impl->setIsShared(false);
            impl->setIsInput(true);
            impl->setIsVolatile(paramNode->getIsVolatile());

            component->addPar(impl);

            cProperties *paramProps = component->par(paramName).properties();
            cProperty *unitProp = paramProps->get("unit");
            const char *declUnit = unitProp ? unitProp->value(cProperty::DEFAULTKEY) : NULL;
            impl->setUnit(declUnit);
        }
        else {
            impl = component->par(paramName).copyIfShared();
        }

        ExpressionElement *exprNode = paramNode->getFirstExpressionChild();
        if (exprNode)
        {
            //printf("   +++ assigning param %s\n", paramName);
            ASSERT(impl==component->par(paramName).impl() && !impl->isShared());
            cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, isSubcomponent);
            impl->setIsInput(paramNode->getIsDefault());
            cExpressionBuilder::setExpression(impl, dynamicExpr);
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
        const char *gateName = gateNode->getName();
        ExpressionElement *exprNode = gateNode->getFirstExpressionChild();

        // determine gatesize
        int gatesize = -1;
        if (gateNode->getIsVector() && exprNode)
        {
            cParImpl *value = currentDecl->getSharedParImplFor(exprNode);
            if (!value)
            {
                cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, isSubcomponent);
                value = new cLongParImpl();
                value->setName("gatesize-expression");
                cExpressionBuilder::setExpression(value, dynamicExpr);
                currentDecl->putSharedParImplFor(exprNode, value);
            }
            gatesize = value->longValue(module);
        }

        // add gate if it's declared here
        if (!isSubcomponent && gateNode->getType()!=NED_GATETYPE_NONE)
            module->addGate(gateName, translateGateType(gateNode->getType()), gateNode->getIsVector());

        // set gate vector size
        if (gatesize!=-1)
            module->setGateSize(gateName, gatesize);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, gateNode); throw;
    }
}

cModule *cNEDNetworkBuilder::_submodule(cModule *, const char *submodname, int idx)
{
    SubmodMap::iterator i = submodMap.find(std::string(submodname));
    if (i==submodMap.end())
        return NULL;

    ModulePtrVector& v = i->second;
    if (idx<0)
        return (v.size()!=1 || v[0]->isVector()) ? NULL : v[0];
    else
        return ((unsigned)idx>=v.size()) ? NULL : v[idx];
}

void cNEDNetworkBuilder::addGatesTo(cModule *module, cNEDDeclaration *decl)
{
    //TRACE("addGatesTo(%s), decl=%s", module->fullPath().c_str(), decl->name()); //XXX

    // recursively add and assign super types' gates
    if (decl->numExtendsNames() > 0)
    {
        const char *superName = decl->extendsName(0);
        cNEDDeclaration *superDecl = cNEDLoader::instance()->getDecl(superName);
        addGatesTo(module, superDecl);
    }

    // add this decl gates / gatesizes
    GatesElement *gatesNode = decl->getGatesElement();
    if (gatesNode)
    {
        currentDecl = decl; // switch "context"
        doGates(module, gatesNode, false);
    }
}

void cNEDNetworkBuilder::buildInside(cModule *modp, cNEDDeclaration *decl)
{
    //TRACE("buildinside(%s), decl=%s", modp->fullPath().c_str(), decl->name());  //XXX

    if (modp->id() % 50 == 0)
    {
        // half-hearted attempt to catch "recursive compound module" bug (where
        // a compound module contains itself, directly or via other compound modules)
        int limit = ev.config()->getAsInt(CFGID_MAX_MODULE_NESTING);
        if (limit>0)
        {
            int depth = 0;
            for (cModule *p=modp; p; p=p->parentModule())
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
        cNEDDeclaration *superDecl = cNEDLoader::instance()->getDecl(superName);
        buildRecursively(modp, superDecl);
    }

    currentDecl = decl; // switch "context"
    addSubmodulesAndConnections(modp);
}

void cNEDNetworkBuilder::addSubmodulesAndConnections(cModule *modp)
{
    //TRACE("addSubmodulesAndConnections(%s), decl=%s", modp->fullPath().c_str(), currentDecl->name()); //XXX
    //dump(currentDecl->getTree()); XXX

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
        decl = cNEDLoader::instance()->getDecl(superName);
        ASSERT(decl); // all super classes must be loaded before we start building
        ConnectionsElement *conns = decl->getConnectionsElement();
        if (conns && conns->getAllowUnconnected())
            return true;
    }
    return false;
}

std::string cNEDNetworkBuilder::resolveComponentType(const NEDLookupContext& context, const char *nedtypename)
{
    // Resolve a NED module/channel type name, for a submodule or channel
    // instance. Lookup is based on component names registered in the simkernel,
    // NOT on the NED files loaded. This allows the user to instantiate
    // cModuleTypes/cChannelTypes which are not declared in NED.
    ComponentTypeNames qnames;
    return cNEDLoader::instance()->resolveNedType(context, nedtypename, &qnames);
}

cModuleType *cNEDNetworkBuilder::findAndCheckModuleType(const char *modTypeName, cModule *modp, const char *submodname)
{
    //FIXME cache the result to speed up further lookups
    NEDLookupContext context(currentDecl->getTree(), currentDecl->fullName());
    std::string qname = resolveComponentType(context, modTypeName);
    if (qname.empty())
        throw cRuntimeError(modp, "Submodule %s: cannot resolve module type `%s' (not in the loaded NED files?)",
                            submodname, modTypeName);
    cComponentType *componenttype = cComponentType::find(qname.c_str());
    if (!dynamic_cast<cModuleType *>(componenttype))
        throw cRuntimeError(modp, "Submodule %s: `%s' is not a module type",
                            submodname, qname.c_str());
    return (cModuleType *)componenttype;
}

cModuleType *cNEDNetworkBuilder::findAndCheckModuleTypeLike(const char *modTypeName, const char *likeType, cModule *modp, const char *submodname)
{
    //FIXME cache the result to speed up further lookups

    // resolve the interface
    NEDLookupContext context(currentDecl->getTree(), currentDecl->fullName());
    std::string interfaceqname = cNEDLoader::instance()->resolveNedType(context, likeType);
    cNEDDeclaration *interfacedecl = interfaceqname.empty() ? NULL : (cNEDDeclaration *)cNEDLoader::instance()->lookup(interfaceqname.c_str());
    if (!interfacedecl)
        throw cRuntimeError(modp, "Submodule %s: cannot resolve module interface `%s'",
                            submodname, likeType);
    if (interfacedecl->getTree()->getTagCode()!=NED_MODULE_INTERFACE)
        throw cRuntimeError(modp, "Submodule %s: `%s' is not a module interface",
                            submodname, interfaceqname.c_str());

    // search for module type that implements the interface
    std::vector<std::string> candidates = findTypeWithInterface(modTypeName, interfaceqname.c_str());
    if (candidates.empty())
        throw cRuntimeError(modp, "Submodule %s: no module type named `%s' found that implements module interface %s (not in the loaded NED files?)",
                            submodname, modTypeName, interfaceqname.c_str());
    if (candidates.size() > 1)
        throw cRuntimeError(modp, "Submodule %s: more than one module types named `%s' found that implement module interface %s (use fully qualified name to disambiguate)",
                            submodname, modTypeName, interfaceqname.c_str());

    cComponentType *componenttype = cComponentType::find(candidates[0].c_str());
    if (!dynamic_cast<cModuleType *>(componenttype))
        throw cRuntimeError(modp, "Submodule %s: `%s' is not a module type",
                            submodname, candidates[0].c_str());
    return (cModuleType *)componenttype;
}

std::vector<std::string> cNEDNetworkBuilder::findTypeWithInterface(const char *nedtypename, const char *interfaceqname)
{
    std::vector<std::string> candidates;

    // try to interpret it as a fully qualified name
    ComponentTypeNames qnames;
    if (qnames.contains(nedtypename)) {
        cNEDDeclaration *decl = cNEDLoader::instance()->getDecl(nedtypename);
        ASSERT(decl);
        if (decl->supportsInterface(interfaceqname)) {
            candidates.push_back(nedtypename);
            return candidates;
        }
    }

    if (!strchr(nedtypename, '.'))
    {
        // no dot: name is an unqualified name (simple name). See how many NED types
        // implement the given interface; there should be exactly one
        std::string dot_nedtypename = std::string(".")+nedtypename;
        for (int i=0; i<qnames.size(); i++) {
            const char *qname = qnames.get(i);
            if (opp_stringendswith(qname, dot_nedtypename.c_str()) || strcmp(qname, nedtypename)==0) {
                cNEDDeclaration *decl = cNEDLoader::instance()->getDecl(qname);
                ASSERT(decl);
                if (decl->supportsInterface(interfaceqname))
                    candidates.push_back(qname);
            }
        }
    }
    return candidates;
}

std::string cNEDNetworkBuilder::getSubmoduleTypeName(cModule *modp, SubmoduleElement *submod, int index)
{
    const char *submodname = submod->getName();
    std::string submodtypename;
    if (opp_isempty(submod->getLikeType())) {
        submodtypename = submod->getType();
    }
    else
    {
        // type may be given either in ExpressionElement child or "like-param" attribute
        if (!opp_isempty(submod->getLikeParam())) {
            submodtypename = modp->par(submod->getLikeParam()).stringValue();
        }
        else {
            ExpressionElement *likeParamExpr = findExpression(submod, "like-param");
            if (likeParamExpr)
                submodtypename = evaluateAsString(likeParamExpr, modp, false);
            else {
                std::string key = modp->fullPath() + "." + submodname;
                if (index != -1)
                    key = opp_stringf("%s[%d]", key.c_str(), index);
                submodtypename = ev.config()->getAsString(key.c_str(), CFGID_TYPE_NAME);
                if (submodtypename.empty())
                    throw cRuntimeError(modp, "Unable to determine type name for submodule %s, missing entry %s.%s", submodname, key.c_str(), CFGID_TYPE_NAME->name());
            }
        }
    }

    return submodtypename;
}

void cNEDNetworkBuilder::addSubmodule(cModule *modp, SubmoduleElement *submod)
{
    // create submodule
    const char *submodname = submod->getName();
    bool usesLike = !opp_isempty(submod->getLikeType());
    ExpressionElement *vectorsizeexpr = findExpression(submod, "vector-size");

    if (!vectorsizeexpr)
    {
        cModuleType *submodtype;
        try {
            std::string submodtypename = getSubmoduleTypeName(modp, submod);
            submodtype = usesLike ?
                findAndCheckModuleTypeLike(submodtypename.c_str(), submod->getLikeType(), modp, submodname) :
                findAndCheckModuleType(submodtypename.c_str(), modp, submodname);
        }
        catch (std::exception& e) {
            updateOrRethrowException(e, submod); throw;
        }

        cModule *submodp = submodtype->create(submodname, modp);
        ModulePtrVector& v = submodMap[submodname];
        v.push_back(submodp);

        cContextSwitcher __ctx(submodp); // params need to be evaluated in the module's context
        assignSubcomponentParams(submodp, submod);
        submodp->finalizeParameters(); // also sets up type's gates
        setupGateVectors(submodp, submod);
    }
    else
    {
        // note: we don't try to resolve moduleType if vector size is zero
        int vectorsize = (int) evaluateAsLong(vectorsizeexpr, modp, false);
        ModulePtrVector& v = submodMap[submodname];
        cModuleType *submodtype = NULL;
        for (int i=0; i<vectorsize; i++) {
            if (!submodtype || usesLike) {
                try {
                    std::string submodtypename = getSubmoduleTypeName(modp, submod, i);
                    submodtype = usesLike ?
                        findAndCheckModuleTypeLike(submodtypename.c_str(), submod->getLikeType(), modp, submodname) :
                        findAndCheckModuleType(submodtypename.c_str(), modp, submodname);
                }
                catch (std::exception& e) {
                    updateOrRethrowException(e, submod); throw;
                }
            }
            cModule *submodp = submodtype->create(submodname, modp, vectorsize, i);
            v.push_back(submodp);

            cContextSwitcher __ctx(submodp); // params need to be evaluated in the module's context
            assignSubcomponentParams(submodp, submod);
            submodp->finalizeParameters(); // also sets up type's gates
            setupGateVectors(submodp, submod);
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

void cNEDNetworkBuilder::setupGateVectors(cModule *submodule, NEDElement *submoduleNode)
{
    GatesElement *gatesNode = (GatesElement *) submoduleNode->getFirstChildWithTag(NED_GATES);
    if (gatesNode)
        doGates(submodule, gatesNode, true);
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
        if (conn->getArrowDirection()!=NED_ARROWDIR_BIDIR)
        {
            // find gates and create connection
            cGate *srcg = resolveGate(modp, conn->getSrcModule(), findExpression(conn, "src-module-index"),
                                            conn->getSrcGate(), findExpression(conn, "src-gate-index"),
                                            conn->getSrcGateSubg(), conn->getSrcGatePlusplus());
            cGate *destg = resolveGate(modp, conn->getDestModule(), findExpression(conn, "dest-module-index"),
                                             conn->getDestGate(), findExpression(conn, "dest-gate-index"),
                                             conn->getDestGateSubg(), conn->getDestGatePlusplus());

            //printf("creating connection: %s --> %s\n", srcg->fullPath().c_str(), destg->fullPath().c_str());

            // check directions
            cGate *errg = NULL;
            if (srcg->ownerModule()==modp ? srcg->type()!=cGate::INPUT : srcg->type()!=cGate::OUTPUT)
                errg = srcg;
            if (destg->ownerModule()==modp ? destg->type()!=cGate::OUTPUT : destg->type()!=cGate::INPUT)
                errg = destg;
            if (errg)
                throw cRuntimeError(modp, "Gate %s is being connected in the wrong direction",
                                    errg->fullPath().c_str());

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

            //printf("Creating bidir conn: %s --> %s\n", srcg2->fullPath().c_str(), destg1->fullPath().c_str());
            //printf("                and: %s <-- %s\n", srcg1->fullPath().c_str(), destg2->fullPath().c_str());

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
    // connect
    ChannelSpecElement *channelspec = conn->getFirstChannelSpecChild();
    if (!channelspec)
    {
        srcg->connectTo(destg);
    }
    else
    {
        cChannel *channel = createChannel(channelspec, modp, srcg);
        channel->setConnectionId(conn->getId()); // so that properties will be found
        srcg->connectTo(destg, channel);
        assignSubcomponentParams(channel, channelspec);
        channel->finalizeParameters();
    }
}

cGate *cNEDNetworkBuilder::resolveGate(cModule *parentmodp,
                                       const char *modname, ExpressionElement *modindexp,
                                       const char *gatename, ExpressionElement *gateindexp,
                                       int subg, bool isplusplus)
{
    //TRACE("resolveGate(mod=%s, %s.%s, subg=%d, plusplus=%d)", parentmodp->fullPath().c_str(), modname, gatename, subg, isplusplus);

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

std::string cNEDNetworkBuilder::getChannelTypeName(cModule *parentmodp, cGate *srcgate, ChannelSpecElement *channelspec, const char *channelname)
{
    std::string channeltypename;
    if (opp_isempty(channelspec->getLikeType())) {
        //TODO create cIdealChannel if there are no parameters?
        channeltypename = opp_isempty(channelspec->getType()) ? "ned.BasicChannel" : channelspec->getType();
    }
    else
    {
        // type may be given either in ExpressionElement child or "like-param" attribute
        if (!opp_isempty(channelspec->getLikeParam())) {
            channeltypename = parentmodp->par(channelspec->getLikeParam()).stringValue();
        }
        else {
            ExpressionElement *likeParamExpr = findExpression(channelspec, "like-param");
            if (likeParamExpr)
                channeltypename = evaluateAsString(likeParamExpr, parentmodp, false);
            else {
                std::string key = srcgate->fullPath() + "." + channelname;
                channeltypename = ev.config()->getAsString(key.c_str(), CFGID_TYPE_NAME);
                if (channeltypename.empty())
                    throw cRuntimeError(parentmodp, "Unable to determine type name for channel: missing config entry %s.%s", key.c_str(), CFGID_TYPE_NAME->name());
            }
        }
    }

    return channeltypename;
}

cChannel *cNEDNetworkBuilder::createChannel(ChannelSpecElement *channelspec, cModule *parentmodp, cGate *srcgate)
{
    // resolve channel type
    const char *channelname = "channel";
    cChannelType *channeltype;
    try {
        std::string channeltypename = getChannelTypeName(parentmodp, srcgate, channelspec, channelname);
        bool usesLike = !opp_isempty(channelspec->getLikeType());
        channeltype = usesLike ?
            findAndCheckChannelTypeLike(channeltypename.c_str(), channelspec->getLikeType(), parentmodp) :
            findAndCheckChannelType(channeltypename.c_str(), parentmodp);
    }
    catch (std::exception& e) {
        updateOrRethrowException(e, channelspec); throw;
    }

    // create channel object
    cChannel *channelp = channeltype->create(channelname, parentmodp);
    return channelp;
}

cChannelType *cNEDNetworkBuilder::findAndCheckChannelType(const char *channeltypename, cModule *modp)
{
    //FIXME cache the result to speed up further lookups
    NEDLookupContext context(currentDecl->getTree(), currentDecl->fullName());
    std::string qname = resolveComponentType(context, channeltypename);
    if (qname.empty())
        throw cRuntimeError(modp, "Cannot resolve channel type `%s' (not in the loaded NED files?)", channeltypename);

    cComponentType *componenttype = cComponentType::find(qname.c_str());
    if (!dynamic_cast<cChannelType *>(componenttype))
        throw cRuntimeError(modp, "`%s' is not a channel type", qname.c_str());
    return (cChannelType *)componenttype;
}

cChannelType *cNEDNetworkBuilder::findAndCheckChannelTypeLike(const char *channeltypename, const char *likeType, cModule *modp)
{
    //FIXME cache the result to speed up further lookups

    // resolve the interface
    NEDLookupContext context(currentDecl->getTree(), currentDecl->fullName());
    std::string interfaceqname = cNEDLoader::instance()->resolveNedType(context, likeType);
    cNEDDeclaration *interfacedecl = interfaceqname.empty() ? NULL : (cNEDDeclaration *)cNEDLoader::instance()->lookup(interfaceqname.c_str());
    if (!interfacedecl)
        throw cRuntimeError(modp, "Cannot resolve channel interface `%s'", likeType);
    if (interfacedecl->getTree()->getTagCode()!=NED_CHANNEL_INTERFACE)
        throw cRuntimeError(modp, "`%s' is not a channel interface", interfaceqname.c_str());

    // search for channel type that implements the interface
    std::vector<std::string> candidates = findTypeWithInterface(channeltypename, interfaceqname.c_str());
    if (candidates.empty())
        throw cRuntimeError(modp, "No channel type named `%s' found that implements channel interface %s (not in the loaded NED files?)",
                            channeltypename, interfaceqname.c_str());
    if (candidates.size() > 1)
        throw cRuntimeError(modp, "More than one channel types named `%s' found that implement channel interface %s (use fully qualified name to disambiguate)",
                            channeltypename, interfaceqname.c_str());

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

