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
#include "clongpar.h"
#include "cboolpar.h"
#include "cstringpar.h"

#include "nedelements.h"
#include "nederror.h"

#include "nedparser.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedsyntaxvalidator.h"
#include "nedsemanticvalidator.h"
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

/*
  FIXME FIXME FIXME
  TODO connection display string
*/

/* for debugging
static void dump(NEDElement *node)
{
    generateXML(std::cout, node, false);
    std::cout.flush();
}
*/

void cNEDNetworkBuilder::addParametersTo(cComponent *component, cNEDDeclaration *decl)
{
    //TRACE("addParametersTo(%s), decl=%s", component->fullPath().c_str(), decl->name()); //XXX

    // recursively add and assign super types' parameters
    if (decl->numExtendsNames() > 0)
    {
        const char *superName = decl->extendsName(0);
        cNEDDeclaration *superDecl = cNEDLoader::instance()->getDecl(superName);
        addParametersTo(component, superDecl);
    }

    // add this decl parameters / assignments
    ParametersNode *paramsNode = decl->getParametersNode();
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

void cNEDNetworkBuilder::doParams(cComponent *component, ParametersNode *paramsNode, bool isSubcomponent)
{
    ASSERT(paramsNode!=NULL);
    for (NEDElement *child=paramsNode->getFirstChildWithTag(NED_PARAM); child; child=child->getNextSiblingWithTag(NED_PARAM))
    {
        ParamNode *paramNode = (ParamNode *)child;
        const char *paramName = paramNode->getName();
        ExpressionNode *exprNode = paramNode->getFirstExpressionChild();

        cParValue *value = NULL;
        if (exprNode)
        {
            value = currentDecl->getCachedExpression(exprNode);
            ASSERT(!value || value->isName(paramName));
            if (!value)
            {
                cPar::Type parType = paramNode->getType()==NED_PARTYPE_NONE
                    ? component->par(paramName).type()
                    : translateParamType(paramNode->getType());

                bool isVolatile = paramNode->getType()==NED_PARTYPE_NONE
                    ? component->par(paramName).isVolatile()
                    : paramNode->getIsVolatile();

                cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, isSubcomponent);
                value = cParValue::createWithType(parType);
                cExpressionBuilder::assign(value, dynamicExpr);
                value->setName(paramName);
                value->setIsShared(true);
                value->setIsInput(paramNode->getIsDefault());
                value->setIsVolatile(isVolatile);
                currentDecl->putCachedExpression(exprNode, value);
            }
        }

        // find or add parameter
        if (isSubcomponent || paramNode->getType()==NED_PARTYPE_NONE)
        {
            if (value)
            {
                // parameter must exist already
                cPar &par = component->par(paramName);
                par.reassign(value);
            }
        }
        else
        {
            // add it now
            if (!value)
            {
                value = cParValue::createWithType(translateParamType(paramNode->getType()));
                value->setName(paramName);
                value->setIsShared(false); //XXX cannot cache: there'no ExpressionNode to piggyback on!!!
                value->setIsInput(true);
                value->setIsVolatile(paramNode->getIsVolatile());
            }
            //XXX printf("   +++ adding param %s\n", paramName);
            component->addPar(value);
        }
    }
}

void cNEDNetworkBuilder::doGates(cModule *module, GatesNode *gatesNode, bool isSubcomponent)
{
    ASSERT(gatesNode!=NULL);
    for (NEDElement *child=gatesNode->getFirstChildWithTag(NED_GATE); child; child=child->getNextSiblingWithTag(NED_GATE))
    {
        GateNode *gateNode = (GateNode *)child;
        const char *gateName = gateNode->getName();
        ExpressionNode *exprNode = gateNode->getFirstExpressionChild();

        // determine gatesize
        int gatesize = -1;
        if (gateNode->getIsVector() && exprNode)
        {
            cParValue *value = currentDecl->getCachedExpression(exprNode);
            if (!value)
            {
                cDynamicExpression *dynamicExpr = cExpressionBuilder().process(exprNode, isSubcomponent);
                value = new cLongPar();
                value->setName("gatesize-expression");
                cExpressionBuilder::assign(value, dynamicExpr);
                currentDecl->putCachedExpression(exprNode, value);
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
    GatesNode *gatesNode = decl->getGatesNode();
    if (gatesNode)
    {
        currentDecl = decl; // switch "context"
        doGates(module, gatesNode, false);
    }
}

void cNEDNetworkBuilder::buildInside(cModule *modp, cNEDDeclaration *decl)
{
    //TRACE("buildinside(%s), decl=%s", modp->fullPath().c_str(), decl->name());  //XXX

    // add submodules and connections. Submodules and connections are inherited:
    // we need to start start with the the base classes, and do this compound
    // module last.
    submodMap.clear();
    buildRecursively(modp, decl);

    // recursively build the submodules too (top-down)
    currentDecl = decl;
    for (cSubModIterator submod(*modp); !submod.end(); submod++)
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

    SubmodulesNode *submods = currentDecl->getSubmodulesNode();
    if (submods)
    {
        for (SubmoduleNode *submod=submods->getFirstSubmoduleChild(); submod; submod=submod->getNextSubmoduleNodeSibling())
        {
            addSubmodule(modp, submod);
        }
    }

    // loop through connections and add them
    ConnectionsNode *conns = currentDecl->getConnectionsNode();
    if (conns)
    {
        for (NEDElement *child=conns->getFirstChild(); child; child=child->getNextSibling())
        {
            if (child->getTagCode()==NED_CONNECTION || child->getTagCode()==NED_CONNECTION_GROUP)
                addConnectionOrConnectionGroup(modp, child);
        }
    }

    // check if there are unconnected gates left -- unless unconnected gates were already permitted in the super type
    if ((!conns || !conns->getAllowUnconnected()) && !superTypeAllowsUnconnected())
        modp->checkInternalConnections();
}

bool cNEDNetworkBuilder::superTypeAllowsUnconnected() const
{
    // follow through the inheritance chain, and return true if we find an "allowunconnected" anywhere
    cNEDDeclaration *decl = currentDecl;
    while (decl->numExtendsNames() > 0)
    {
        const char *superName = decl->extendsName(0);
        decl = cNEDLoader::instance()->getDecl(superName);
        ASSERT(decl); // all super classes must be loaded before we start building
        ConnectionsNode *conns = decl->getConnectionsNode();
        if (conns && conns->getAllowUnconnected())
            return true;
    }
    return false;
}

cModuleType *cNEDNetworkBuilder::findAndCheckModuleType(const char *modTypeName, cModule *modp, const char *submodname)
{
    //FIXME cache the result to speed up further lookups
    NEDLookupContext context(currentDecl->getTree(), currentDecl->fullName());
    std::string qname = cNEDLoader::instance()->resolveComponentType(context, modTypeName);
    if (qname.empty())
        throw cRuntimeError("dynamic module builder: module type definition `%s' for submodule %s "
                            "in (%s)%s not found (not in the loaded NED files?)",
                            modTypeName, submodname, modp->className(), modp->fullPath().c_str());
    cComponentType *componenttype = cComponentType::find(qname.c_str());
    if (!dynamic_cast<cModuleType *>(componenttype))
        throw cRuntimeError("dynamic module builder: module type definition `%s' for submodule %s "
                            "in (%s)%s not found (type is not a module type)",
                            modTypeName, submodname, modp->className(), modp->fullPath().c_str());
    return (cModuleType *)componenttype;
}

cModuleType *cNEDNetworkBuilder::findAndCheckModuleTypeLike(const char *likeType, const char *modTypeName, cModule *modp, const char *submodname)
{
    //FIXME cache the result to speed up further lookups
    //FIXME actually use this function!!!

    // resolve the interface
    NEDLookupContext context(currentDecl->getTree(), currentDecl->fullName());
    std::string interfaceqname = cNEDLoader::instance()->resolveNedType(context, likeType);
    cNEDDeclaration *interfacedecl = interfaceqname.empty() ? NULL : (cNEDDeclaration *)cNEDLoader::instance()->lookup(interfaceqname.c_str());
    if (!interfacedecl || interfacedecl->getTree()->getTagCode()!=NED_MODULE_INTERFACE)
        throw cRuntimeError("dynamic module builder: interface type `%s' for submodule %s in (%s)%s could not be resolved",
                            likeType, submodname, modp->className(), modp->fullPath().c_str());

    std::vector<std::string> candidates = findTypeWithInterface(modTypeName, interfaceqname.c_str());
    if (candidates.empty())
        throw cRuntimeError("dynamic module builder: empty!");  //FIXME
    if (candidates.size() > 1)
        throw cRuntimeError("dynamic module builder: more than one!");  //FIXME

    cComponentType *componenttype = cComponentType::find(candidates[0].c_str());
    if (!dynamic_cast<cModuleType *>(componenttype))
        throw cRuntimeError("dynamic module builder: module type definition `%s' for 'like' submodule %s "
                            "in (%s)%s not found (type is not a module type)",
                            modTypeName, submodname, modp->className(), modp->fullPath().c_str());
    return (cModuleType *)componenttype;
}

std::vector<std::string> cNEDNetworkBuilder::findTypeWithInterface(const char *nedtypename, const char *interfaceqname)
{
    std::vector<std::string> candidates;
    
    // try to interpret it as a fully qualified name
    cNEDLoader::ComponentTypeNames qnames;
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

void cNEDNetworkBuilder::addSubmodule(cModule *modp, SubmoduleNode *submod)
{
    // create submodule
    const char *submodname = submod->getName();
    std::string submodtypename;
    if (opp_isempty(submod->getLikeType()))
    {
        submodtypename = submod->getType();
    }
    else
    {
        // type may be given either in ExpressionNode child or "like-param" attribute
        if (!opp_isempty(submod->getLikeParam()))
        {
            submodtypename = modp->par(submod->getLikeParam()).stringValue();
        }
        else
        {
            ExpressionNode *likeParamExpr = findExpression(submod, "like-param");
            ASSERT(likeParamExpr); // either attr or expr must be there
            //FIXME if module vector: store it as expression, don't evaluate it now,
            //      because it might be random etc!!!
            submodtypename = evaluateAsString(likeParamExpr, modp, false);
        }

    }

    ExpressionNode *vectorsizeexpr = findExpression(submod, "vector-size");

    if (!vectorsizeexpr)
    {
        cModuleType *submodtype = findAndCheckModuleType(submodtypename.c_str(), modp, submodname);
        //FIXME: check submodtype contains likeType as interfaceName!
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
        for (int i=0; i<vectorsize; i++)
        {
            if (!submodtype)
                submodtype = findAndCheckModuleType(submodtypename.c_str(), modp, submodname);
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

void cNEDNetworkBuilder::setConnDisplayString(cGate *srcgatep)
{
    // @display property comes from the channel, but set on the gate
    if (srcgatep->channel())
    {
        cProperties *props = srcgatep->channel()->properties();
        cProperty *prop = props->get("display");
        const char *propValue = prop ? prop->value(cProperty::DEFAULTKEY) : NULL;
        if (propValue)
            srcgatep->setDisplayString(propValue);
    }
}

void cNEDNetworkBuilder::assignSubcomponentParams(cComponent *subcomponent, NEDElement *subcomponentNode)
{
    ParametersNode *paramsNode = (ParametersNode *) subcomponentNode->getFirstChildWithTag(NED_PARAMETERS);
    if (paramsNode)
        doParams(subcomponent, paramsNode, true);
}

void cNEDNetworkBuilder::setupGateVectors(cModule *submodule, NEDElement *submoduleNode)
{
    GatesNode *gatesNode = (GatesNode *) submoduleNode->getFirstChildWithTag(NED_GATES);
    if (gatesNode)
        doGates(submodule, gatesNode, true);
}

void cNEDNetworkBuilder::addConnectionOrConnectionGroup(cModule *modp, NEDElement *connOrConnGroup)
{
    // this is tricky: elements representing "for" and "if" in NED are children
    // of the ConnectionNode or ConnectionGroupNode. So, first we have to go through
    // and execute the LoopNode and ConditionNode children recursively to get
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
        ConditionNode *condition = (ConditionNode *)loopOrCondition;
        ExpressionNode *condexpr = findExpression(condition, "condition");
        if (condexpr && evaluateAsBool(condexpr, modp, false)==true)
        {
            // do the body of the "if": either further "for"'s and "if"'s, or
            // the connection(group) itself that we are children of.
            doConnOrConnGroupBody(modp, loopOrCondition->getParent(), loopOrCondition->getNextSibling());
        }
    }
    else if (loopOrCondition->getTagCode()==NED_LOOP)
    {
        LoopNode *loop = (LoopNode *)loopOrCondition;
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
        doAddConnection(modp, (ConnectionNode *)connOrConnGroup);
    }
    else if (connOrConnGroup->getTagCode()==NED_CONNECTION_GROUP)
    {
        ConnectionGroupNode *conngroup = (ConnectionGroupNode *)connOrConnGroup;
        for (ConnectionNode *conn=conngroup->getFirstConnectionChild(); conn; conn=conn->getNextConnectionNodeSibling())
        {
            doConnOrConnGroupBody(modp, conn, conn->getFirstChild());
        }
    }
    else
    {
        ASSERT(false);
    }
}

void cNEDNetworkBuilder::doAddConnection(cModule *modp, ConnectionNode *conn)
{
//FIXME spurious error message comes when trying to connect INOUT gate with "-->"
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
            throw cRuntimeError("dynamic module builder: gate %s in (%s)%s is being "
                                "connected the wrong way: directions don't match",
                                errg->fullPath().c_str(), modp->className(), modp->fullPath().c_str());

        doConnectGates(modp, srcg, destg, conn);
    }
    else
    {
        // find gates and create connection in both ways
        if (conn->getSrcGateSubg()!=NED_SUBGATE_NONE || conn->getDestGateSubg()!=NED_SUBGATE_NONE)
            throw cRuntimeError("dynamic module builder: error is module (%s)%s: gate$i or gate$o syntax "
                                "cannot be used with bidirectional connections",
                                modp->className(), modp->fullPath().c_str());

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

void cNEDNetworkBuilder::doConnectGates(cModule *modp, cGate *srcg, cGate *destg, ConnectionNode *conn)
{
    // connect
    ChannelSpecNode *channelspec = conn->getFirstChannelSpecChild();
    if (!channelspec)
    {
        srcg->connectTo(destg);
    }
    else
    {
        cChannel *channel = createChannel(channelspec, modp);
        srcg->connectTo(destg, channel);
        assignSubcomponentParams(channel, channelspec);
        channel->finalizeParameters();

        //XXX display string
    }
}

cGate *cNEDNetworkBuilder::resolveGate(cModule *parentmodp,
                                       const char *modname, ExpressionNode *modindexp,
                                       const char *gatename, ExpressionNode *gateindexp,
                                       int subg, bool isplusplus)
{
    //TRACE("resolveGate(mod=%s, %s.%s, subg=%d, plusplus=%d)", parentmodp->fullPath().c_str(), modname, gatename, subg, isplusplus);

    if (isplusplus && gateindexp)
        throw cRuntimeError("dynamic module builder: \"++\" and gate index expression cannot exist together");

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
                throw cRuntimeError("%s.%s[] gates are all connected, no gate left for `++' operator", modp->fullPath().c_str(), gatename);
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
                                          const char *modname, ExpressionNode *modindexp,
                                          const char *gatename, ExpressionNode *gateindexp,
                                          bool isplusplus,
                                          cGate *&gate1, cGate *&gate2)
{
    if (isplusplus && gateindexp)
        throw cRuntimeError("dynamic module builder: \"++\" and gate index expression cannot exist together");

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
                throw cRuntimeError("%s.%s[] gates are all connected, no gate left for `++' operator", modp->fullPath().c_str(), gatename);
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

cModule *cNEDNetworkBuilder::resolveModuleForConnection(cModule *parentmodp, const char *modname, ExpressionNode *modindexp)
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
                throw cRuntimeError("dynamic module builder: submodule `%s' in (%s)%s not found",
                                        modname, parentmodp->className(), parentmodp->fullPath().c_str());
            else
                throw cRuntimeError("dynamic module builder: submodule `%s[%d]' in (%s)%s not found",
                                        modname, modindex, parentmodp->className(), parentmodp->fullPath().c_str());
        }
        return modp;
    }
}

cChannel *cNEDNetworkBuilder::createChannel(ChannelSpecNode *channelspec, cModule *parentmodp)
{
    // create channel object
    cChannel *channelp = NULL;
    std::string channeltypename;
    if (opp_isempty(channelspec->getLikeType()))
    {
        channeltypename = opp_isempty(channelspec->getType()) ? "ned.cBasicChannel" : channelspec->getType();
    }
    else
    {
        // type may be given either in ExpressionNode child or "like-param" attribute
        if (!opp_isempty(channelspec->getLikeParam()))
        {
            channeltypename = parentmodp->par(channelspec->getLikeParam()).stringValue();
        }
        else
        {
            ExpressionNode *likeParamExpr = findExpression(channelspec, "like-param");
            ASSERT(likeParamExpr); // either attr or expr must be there
            channeltypename = evaluateAsString(likeParamExpr, parentmodp, false);
        }
        //XXX check actual type fulfills likeType
    }

    cChannelType *channeltype = findAndCheckChannelType(channeltypename.c_str(), parentmodp);
    channelp = channeltype->create("channel", parentmodp); //FIXME must give unique names, otherwise channel properties() won't work!!!

    return channelp;
}

cChannelType *cNEDNetworkBuilder::findAndCheckChannelType(const char *channeltypename, cModule *modp)
{
    //FIXME cache the result to speed up further lookups
    NEDLookupContext context(currentDecl->getTree(), currentDecl->fullName());
    std::string qname = cNEDLoader::instance()->resolveComponentType(context, channeltypename);
    if (qname.empty())
        throw cRuntimeError("dynamic network builder: channel type definition `%s' not found "
                            "(not in the loaded NED files?)", channeltypename);  //FIXME "in module %s"
    cComponentType *componenttype = cComponentType::find(qname.c_str());
    if (!dynamic_cast<cChannelType *>(componenttype))
        throw cRuntimeError("dynamic network builder: channel type definition `%s' not found "
                            "(type is not a channel type)", channeltypename);  //FIXME "in module %s"
    return (cChannelType *)componenttype;
}

cChannelType *cNEDNetworkBuilder::findAndCheckChannelTypeLike(const char *likeType, const char *channeltypename, cModule *modp)
{
    //FIXME cache the result to speed up further lookups
    //FIXME actually use this function!!!

    // resolve the interface
    NEDLookupContext context(currentDecl->getTree(), currentDecl->fullName());
    std::string interfaceqname = cNEDLoader::instance()->resolveNedType(context, likeType);
    cNEDDeclaration *interfacedecl = interfaceqname.empty() ? NULL : (cNEDDeclaration *)cNEDLoader::instance()->lookup(interfaceqname.c_str());
    if (!interfacedecl || interfacedecl->getTree()->getTagCode()!=NED_CHANNEL_INTERFACE)
        throw cRuntimeError("dynamic module builder: interface type `%s' for channel in (%s)%s could not be resolved",
                            likeType, modp->className(), modp->fullPath().c_str());

    std::vector<std::string> candidates = findTypeWithInterface(channeltypename, interfaceqname.c_str());
    if (candidates.empty())
        throw cRuntimeError("dynamic module builder: empty!");  //FIXME
    if (candidates.size() > 1)
        throw cRuntimeError("dynamic module builder: more than one!");  //FIXME

    cComponentType *componenttype = cComponentType::find(candidates[0].c_str());
    if (!dynamic_cast<cChannelType *>(componenttype))
        throw cRuntimeError("dynamic module builder: channel type definition `%s' for 'like' channel "
                            "in (%s)%s not found (type is not a channel type)",
                            channeltypename, modp->className(), modp->fullPath().c_str());
    return (cChannelType *)componenttype;
}

ExpressionNode *cNEDNetworkBuilder::findExpression(NEDElement *node, const char *exprname)
{
    // find expression with given name in node
    if (!node)
        return NULL;
    for (NEDElement *child=node->getFirstChildWithTag(NED_EXPRESSION); child; child = child->getNextSiblingWithTag(NED_EXPRESSION))
    {
        ExpressionNode *expr = (ExpressionNode *)child;
        if (!strcmp(expr->getTarget(),exprname))
            return expr;
    }
    return NULL;
}

cParValue *cNEDNetworkBuilder::getOrCreateExpression(ExpressionNode *exprNode, cPar::Type type, bool inSubcomponentScope)
{
    cParValue *p = currentDecl->getCachedExpression(exprNode);
    if (!p)
    {
        cDynamicExpression *e = cExpressionBuilder().process(exprNode, inSubcomponentScope);
        p = cParValue::createWithType(type);
        cExpressionBuilder::assign(p, e);
        currentDecl->putCachedExpression(exprNode, p);
    }
    return p;
}

long cNEDNetworkBuilder::evaluateAsLong(ExpressionNode *exprNode, cComponent *context, bool inSubcomponentScope)
{
    cParValue *p = getOrCreateExpression(exprNode, cPar::LONG, inSubcomponentScope);
    return p->longValue(context);
}

bool cNEDNetworkBuilder::evaluateAsBool(ExpressionNode *exprNode, cComponent *context, bool inSubcomponentScope)
{
    cParValue *p = getOrCreateExpression(exprNode, cPar::BOOL, inSubcomponentScope);
    return p->boolValue(context);
}

std::string cNEDNetworkBuilder::evaluateAsString(ExpressionNode *exprNode, cComponent *context, bool inSubcomponentScope)
{
    cParValue *p = getOrCreateExpression(exprNode, cPar::STRING, inSubcomponentScope);
    return p->stdstringValue(context);
}

