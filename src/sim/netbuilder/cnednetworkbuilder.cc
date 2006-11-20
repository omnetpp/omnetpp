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

#include "cmodule.h"
#include "cgate.h"
#include "cchannel.h"
#include "cbasicchannel.h"
#include "ccomponenttype.h"

#include "nedelements.h"
#include "nederror.h"

#include "nedparser.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedbasicvalidator.h"
#include "nedsemanticvalidator.h"
#include "xmlgenerator.h"  // for debugging

#include "cneddeclaration.h"
#include "cnednetworkbuilder.h"
#include "cnedloader.h"
#include "cexpressionbuilder.h"
#include "nedsupport.h"


inline bool strnull(const char *s)
{
    return !s || !s[0];
}

static void dump(NEDElement *node)
{
    generateXML(std::cout, node, false);
    std::cout.flush();
}

void cNEDNetworkBuilder::addParameters(cComponent *component, cNEDDeclaration *decl)
{
    printf("adding params of %s to %s\n", decl->name(), component->fullPath().c_str()); //XXX
    printf("DECL {\n%s\n}\n", decl->detailedInfo().c_str()); //XXX
    int n = decl->numPars();
    for (int i=0; i<n; i++)
    {
        const cNEDDeclaration::ParamDescription& desc = decl->paramDescription(i);
        ASSERT(desc.value);
//FIXME        component->addPar(desc.name.c_str(), desc.value->dup());
        cPar& par = component->par(desc.name.c_str());
        printf("  added param %s, isSet=%d, isExpr=%d, info: %s\n", desc.name.c_str(), par.isSet(), !par.isConstant(), par.info().c_str()); //XXX
    }
}

void cNEDNetworkBuilder::addGates(cModule *module, cNEDDeclaration *decl)
{
    int n = decl->numGates();
    for (int i=0; i<n; i++)
    {
        const cNEDDeclaration::GateDescription& desc = decl->gateDescription(i);
        const char *gateName = desc.name.c_str();
        module->addGate(gateName, desc.type, desc.isVector);
        if (desc.isVector && desc.gatesize!=NULL)
            module->setGateSize(gateName, desc.gatesize->longValue(module));
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

void cNEDNetworkBuilder::buildInside(cModule *modp, cNEDDeclaration *decl)
{
    printf("started buildinside of %s\n", modp->fullPath().c_str()); //XXX
    // set display string
//FIXME    setBackgroundDisplayString(modp, modulenode);

    // add submodules and connections. Submodules and connections are inherited:
    // we need to start start with the the base classes, and do this compound
    // module last.
    submodMap.clear();
    buildRecursively(modp, decl);

    // recursively build the submodules too (top-down)
    for (cSubModIterator submod(*modp); !submod.end(); submod++)
    {
       cModule *m = submod();
       m->buildInside();
    }
    printf("done buildinside of %s\n", modp->fullPath().c_str()); //XXX
}

void cNEDNetworkBuilder::buildRecursively(cModule *modp, cNEDDeclaration *decl)
{
    if (decl->numExtendsNames() > 0)
    {
        const char *superName = decl->extendsName(0);
        cNEDDeclaration *superDecl = cNEDLoader::instance()->lookup2(superName);
        ASSERT(superDecl!=NULL);
        buildRecursively(modp, superDecl);
    }

    addSubmodulesAndConnections(modp, decl);
}

void cNEDNetworkBuilder::addSubmodulesAndConnections(cModule *modp, cNEDDeclaration *decl)
{
    printf("  adding submodules and connections of decl %s to %s\n", decl->name(), modp->fullPath().c_str()); //XXX
    //dump(decl->getTree()); XXX

    SubmodulesNode *submods = decl->getSubmodules();
    if (submods)
    {
        for (SubmoduleNode *submod=submods->getFirstSubmoduleChild(); submod; submod=submod->getNextSubmoduleNodeSibling())
        {
            addSubmodule(modp, submod);
        }
    }

    // loop through connections and add them
    ConnectionsNode *conns = decl->getConnections();
    if (conns)
    {
        for (NEDElement *child=conns->getFirstChild(); child; child=child->getNextSibling())
        {
            if (child->getTagCode()==NED_CONNECTION || child->getTagCode()==NED_CONNECTION_GROUP)
                addConnectionOrConnectionGroup(modp, child);
        }
    }


    // check if there are unconnected gates left
    //FIXME not quite like this, BUT: if allowUnconnected=false, must check gates of submodules ADDED HERE (not all!)
//XXX    if (!conns || !conns->getAllowUnconnected())
//XXX        modp->checkInternalConnections();

    printf("  done adding submodules and connections of decl %s to %s\n", decl->name(), modp->fullPath().c_str()); //XXX
}

cModuleType *cNEDNetworkBuilder::findAndCheckModuleType(const char *modtypename, cModule *modp, const char *submodname)
{
    cModuleType *modtype = cModuleType::find(modtypename);
    if (!modtype)
        throw new cRuntimeError("dynamic module builder: module type definition `%s' for submodule %s "
                                "in (%s)%s not found (Define_Module() missing from C++ source?)",
                                modtypename, submodname, modp->className(), modp->fullPath().c_str());
    return modtype;
}

void cNEDNetworkBuilder::addSubmodule(cModule *modp, SubmoduleNode *submod)
{
    // create submodule
    const char *submodname = submod->getName();
    std::string submodtypename;
    if (strnull(submod->getLikeParam()))
    {
        submodtypename = submod->getType();
    }
    else
    {
        const char *parname = submod->getLikeParam();
        submodtypename = modp->par(parname).stringValue();
    }

    ExpressionNode *vectorsizeexpr = findExpression(submod, "vector-size");

    if (!vectorsizeexpr)
    {
        cModuleType *submodtype = findAndCheckModuleType(submodtypename.c_str(), modp, submodname);
        cModule *submodp = submodtype->create(submodname, modp);
        ModulePtrVector& v = submodMap[submodname];
        v.push_back(submodp);

        cContextSwitcher __ctx(submodp); // params need to be evaluated in the module's context
        setDisplayString(submodp, submod);
        assignComponentParams(submodp, submod);
        submodp->readParams();
        setupGateVectors(submodp, submod);
    }
    else
    {
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
            setDisplayString(submodp, submod);
            assignComponentParams(submodp, submod);
            submodp->readParams();
            setupGateVectors(submodp, submod);
        }
    }

    // Note: buildInside() will be called when connections have been built out
    // on this level too.
}


void cNEDNetworkBuilder::setDisplayString(cModule *submodp, SubmoduleNode *submod)
{
/*XXX
    DisplayStringNode *dispstrnode = submod->getFirstDisplayStringChild();
    if (dispstrnode)
    {
        const char *dispstr = dispstrnode->getValue();
        submodp->setDisplayString(dispstr);
    }
*/
}

void cNEDNetworkBuilder::setConnDisplayString(cGate *srcgatep, ConnectionNode *conn)
{
/*XXX
    DisplayStringNode *dispstrnode = conn->getFirstDisplayStringChild();
    if (dispstrnode)
    {
        const char *dispstr = dispstrnode->getValue();
        srcgatep->setDisplayString(dispstr);
    }
*/
}

void cNEDNetworkBuilder::setBackgroundDisplayString(cModule *modp, CompoundModuleNode *mod)
{
/*XXX
    DisplayStringNode *dispstrnode = mod->getFirstDisplayStringChild();
    if (dispstrnode)
    {
        const char *dispstr = dispstrnode->getValue();
        modp->setBackgroundDisplayString(dispstr);
    }
*/
}

void cNEDNetworkBuilder::assignComponentParams(cComponent *subcomponentp, NEDElement *subcomponent)
{
    ParametersNode *substparams = (ParametersNode *) subcomponent->getFirstChildWithTag(NED_PARAMETERS);
    if (!substparams)
        return;

    cModule *modp = subcomponentp->parentModule();
    ASSERT(modp);
    for (ParamNode *parnode=substparams->getFirstParamChild(); parnode; parnode=parnode->getNextParamNodeSibling())
    {
        // assign param value
        ExpressionNode *exprNode = findExpression(parnode, "value");
        if (exprNode)
        {
            const char *parname = parnode->getName();
            cPar *p = &(subcomponentp->par(parname));
            cDynamicExpression *e = cExpressionBuilder().process(exprNode, true);
            p->setExpression(e);
/*FIXME
            if (parnode->getIsDefault())
                p->markAsUnset();
*/
        }
    }
}

void cNEDNetworkBuilder::setupGateVectors(cModule *submodp, NEDElement *submod)
{
    GatesNode *gatesizes = (GatesNode *) submod->getFirstChildWithTag(NED_GATES);
    if (!gatesizes)
        return;

    cModule *modp = submodp->parentModule();
    for (GateNode *gate=gatesizes->getFirstGateChild(); gate; gate=gate->getNextGateNodeSibling())
    {
        // set gate vector size
        const char *gatename = gate->getName();
        int vectorsize = (int) evaluateAsLong(findExpression(gate, "vector-size"), submodp, true);
        submodp->setGateSize(gatename, vectorsize);
        //printf("DBG: gatesize: %s.%s[%d]\n", submodp->fullPath().c_str(), gatename, vectorsize);
    }
}

cGate *cNEDNetworkBuilder::getFirstUnusedParentModGate(cModule *modp, const char *gatename)
{
    // same code as generated by nedtool into _n.cc files
    int baseId = modp->findGate(gatename);
    if (baseId<0)
        throw new cRuntimeError("dynamic module builder: %s has no %s[] gate",modp->fullPath().c_str(), gatename);
    int n = modp->gate(baseId)->size();
    for (int i=0; i<n; i++)
        if (!modp->gate(baseId+i)->isConnectedInside())
            return modp->gate(baseId+i);
    throw new cRuntimeError("%s[] gates are all connected, no gate left for `++' operator",modp->fullPath().c_str(), gatename);
}

cGate *cNEDNetworkBuilder::getFirstUnusedSubmodGate(cModule *modp, const char *gatename)
{
    // same code as generated by nedtool into _n.cc files
    int baseId = modp->findGate(gatename);
    if (baseId<0)
        throw new cRuntimeError("dynamic module builder: %s has no %s[] gate",modp->fullPath().c_str(), gatename);
    int n = modp->gate(baseId)->size();
    for (int i=0; i<n; i++)
        if (!modp->gate(baseId+i)->isConnectedOutside())
            return modp->gate(baseId+i);
    int newBaseId = modp->setGateSize(gatename,n+1);
    return modp->gate(newBaseId+n);
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
            printf("---if: true, doing body\n"); //XXX
            doConnOrConnGroupBody(modp, loopOrCondition->getParent(), loopOrCondition->getNextSibling());
            printf("---endif\n"); //XXX
        }
        else printf("---if: FALSE\n"); //XXX
    }
    else if (loopOrCondition->getTagCode()==NED_LOOP)
    {
        LoopNode *loop = (LoopNode *)loopOrCondition;
        long start = evaluateAsLong(findExpression(loop, "from-value"), modp, false);
        long end = evaluateAsLong(findExpression(loop, "to-value"), modp, false);

        // do loop
        printf("---for %s=%ld..%ld\n", loop->getParamName(), start, end); //XXX
        long& i = NEDSupport::LoopVar::pushVar(loop->getParamName());
        for (i=start; i<=end; i++)
        {
            // do the body of the "if": either further "for"'s and "if"'s, or
            // the connection(group) itself that we are children of.
            doConnOrConnGroupBody(modp, loopOrCondition->getParent(), loopOrCondition->getNextSibling());
        }
        printf("---endfor %s\n", loop->getParamName()); //XXX
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
    // find gates and create connection
    cGate *srcg = resolveGate(modp, conn->getSrcModule(), findExpression(conn, "src-module-index"),
                                    conn->getSrcGate(), findExpression(conn, "src-gate-index"),
                                    conn->getSrcGatePlusplus());
    cGate *destg = resolveGate(modp, conn->getDestModule(), findExpression(conn, "dest-module-index"),
                                     conn->getDestGate(), findExpression(conn, "dest-gate-index"),
                                     conn->getDestGatePlusplus());

    printf("    creating connection: %s --> %s\n", srcg->fullPath().c_str(), destg->fullPath().c_str());

    // check directions
    cGate *errg = NULL;
    if (srcg->ownerModule()==modp ? srcg->type()!='I' : srcg->type()!='O')
        errg = srcg;
    if (destg->ownerModule()==modp ? destg->type()!='O' : destg->type()!='I')
        errg = destg;
    if (errg)
        throw new cRuntimeError("dynamic module builder: gate %s in (%s)%s is being "
                                "connected the wrong way: directions don't match",
                                errg->fullPath().c_str(), modp->className(), modp->fullPath().c_str());

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
        assignComponentParams(channel, channelspec);
        channel->readParams();

        //XXX display string
    }
}

cGate *cNEDNetworkBuilder::resolveGate(cModule *parentmodp,
                                       const char *modname, ExpressionNode *modindexp,
                                       const char *gatename, ExpressionNode *gateindexp,
                                       bool isplusplus)
{
    if (isplusplus && gateindexp)
        throw new cRuntimeError("dynamic module builder: \"++\" and gate index expression cannot exist together");

    cModule *modp;
    if (strnull(modname))
    {
        modp = parentmodp;
    }
    else
    {
        int modindex = !modindexp ? 0 : (int) evaluateAsLong(modindexp, parentmodp, false);
        modp = _submodule(parentmodp, modname,modindex);
        if (!modp)
        {
            if (!modindexp)
                throw new cRuntimeError("dynamic module builder: submodule `%s' in (%s)%s not found",
                                        modname, parentmodp->className(), parentmodp->fullPath().c_str());
            else
                throw new cRuntimeError("dynamic module builder: submodule `%s[%d]' in (%s)%s not found",
                                        modname, modindex, parentmodp->className(), parentmodp->fullPath().c_str());
        }
    }

    cGate *gatep = NULL;
    if (!gateindexp && !isplusplus)
    {
        gatep = modp->gate(gatename);
        if (!gatep)
            throw new cRuntimeError("dynamic module builder: module (%s)%s has no gate `%s'",
                                    modp->className(), modp->fullPath().c_str(), gatename);
    }
    else if (isplusplus)
    {
        if (modp == parentmodp)
            gatep = getFirstUnusedParentModGate(modp, gatename);
        else
            gatep = getFirstUnusedSubmodGate(modp, gatename);
    }
    else // (gateindexp)
    {
        int gateindex = (int) evaluateAsLong(gateindexp, parentmodp, false);
        gatep = modp->gate(gatename, gateindex);
        if (!gatep)
            throw new cRuntimeError("dynamic module builder: module (%s)%s has no gate `%s[%d]'",
                                    modp->className(), modp->fullPath().c_str(), gatename, gateindex);
    }
    return gatep;
}

cChannel *cNEDNetworkBuilder::createChannel(ChannelSpecNode *channelspec, cModule *parentmodp)
{
    // create channel object
    cChannel *channelp = NULL;
    std::string channeltypename;
    if (strnull(channelspec->getLikeParam()))
    {
        channeltypename = strnull(channelspec->getType()) ? "cBasicChannel" : channelspec->getType();
    }
    else
    {
        // "like"
        const char *parname = channelspec->getLikeParam();
        channeltypename = parentmodp->par(parname).stringValue();
    }

    cChannelType *channeltype = findAndCheckChannelType(channeltypename.c_str());
    channelp = channeltype->create("channel", parentmodp);

    return channelp;
}

cChannelType *cNEDNetworkBuilder::findAndCheckChannelType(const char *channeltypename)
{
    cChannelType *channeltype = cChannelType::find(channeltypename);
    if (!channeltype)
        throw new cRuntimeError("dynamic network builder: channel type definition `%s' not found "
                                "(Define_Channel() missing from C++ source?)", channeltypename);
    return channeltype;
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

long cNEDNetworkBuilder::evaluateAsLong(ExpressionNode *exprNode, cComponent *context, bool inSubcomponentScope)
{
    //FIXME this can be speeded up by caching cDynamicExpressions, and not recreating them every time. eg. use a NEDElement.id()-to-Expression map!
    cDynamicExpression *e = cExpressionBuilder().process(exprNode, inSubcomponentScope);
    return e->longValue(context);
}

bool cNEDNetworkBuilder::evaluateAsBool(ExpressionNode *exprNode, cComponent *context, bool inSubcomponentScope)
{
    //FIXME this can be speeded up by caching cDynamicExpressions, and not recreating them every time. eg. use a NEDElement.id()-to-Expression map!
    cDynamicExpression *e = cExpressionBuilder().process(exprNode, inSubcomponentScope);
    return e->boolValue(context);
}
