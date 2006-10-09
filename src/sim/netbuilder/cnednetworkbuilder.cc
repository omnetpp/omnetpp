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
#include "ccomponenttype.h"

#include "nedelements.h"
#include "nederror.h"

#include "nedparser.h"
#include "nedxmlparser.h"
#include "neddtdvalidator.h"
#include "nedbasicvalidator.h"
#include "nedsemanticvalidator.h"

#include "cnednetworkbuilder.h"


inline bool strnull(const char *s)
{
    return !s || !s[0];
}

void cNEDNetworkBuilder::addParameters(cComponent *component, cNEDDeclaration *decl)
{
}

void cNEDNetworkBuilder::addGates(cModule *module, cNEDDeclaration *decl)
{
}

void cNEDNetworkBuilder::buildInside(cModule *module, cNEDDeclaration *decl)
{
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


/*
void cNEDNetworkBuilder::setupNetwork(NetworkNode *networknode)
{
    // this code is a simplified version of addSubmodule()
    const char *modname = networknode->getName();
    const char *modtypename = networknode->getTypeName();

    cModuleType *modtype = findModuleType(modtypename);
    if (!modtype)
        throw new cRuntimeError("dynamic module builder: module type definition `%s' "
                                "for network %s not found (Define_Module() missing from C++ source?)",
                                modtypename, modname);

    cModule *networkp = modtype->create(modname, NULL);

    cContextSwitcher __ctx(networkp); // params need to be evaluated in the module's context
    assignSubmoduleParams(networkp, networknode);
    networkp->readInputParams();

    networkp->buildInside();
}

void cNEDNetworkBuilder::buildInside(cModule *modp, CompoundModuleNode *modulenode)
{
    // set display string
    setBackgroundDisplayString(modp, modulenode);

    // loop through submods and add them
    submodMap.clear();
    SubmodulesNode *submods = modulenode->getFirstSubmodulesChild();
    if (submods)
    {
        for (SubmoduleNode *submod=submods->getFirstSubmoduleChild(); submod; submod=submod->getNextSubmoduleNodeSibling())
        {
            addSubmodule(modp, submod);
        }
    }

    // loop through connections and add them
    ConnectionsNode *conns = modulenode->getFirstConnectionsChild();
    if (conns)
    {
        for (NEDElement *child=conns->getFirstChild(); child; child=child->getNextSibling())
        {
            if (child->getTagCode()==NED_CONNECTION)
                addConnection(modp, (ConnectionNode *)child);
            else if (child->getTagCode()==NED_FOR_LOOP)
                addLoopConnection(modp, (ForLoopNode *)child);
        }
    }

    // check if there are unconnected gates left
    if (!conns || conns->getCheckUnconnected())
        modp->checkInternalConnections();

    // recursively build the submodules too (top-down)
    for (cSubModIterator submod(*modp); !submod.end(); submod++)
    {
       cModule *m = submod();
       m->buildInside();
    }
}

cChannel *cNEDNetworkBuilder::createChannel(const char *name, ChannelNode *channelnode)
{
    cBasicChannel *chanp = new cBasicChannel(name);
    for (ChannelAttrNode *chattr=channelnode->getFirstChannelAttrChild(); chattr; chattr=chattr->getNextChannelAttrNodeSibling())
    {
        addChannelAttr(chanp, chattr);
    }
    return chanp;
}

void cNEDNetworkBuilder::addChannelAttr(cChannel *chanp, ChannelAttrNode *channelattr)
{
    const char *attrname = channelattr->getName();
    cPar *p = new cPar(attrname);
    ExpressionNode *valueexpr = findExpression(channelattr, "value");
    *p = evaluate(NULL,valueexpr); // note: this doesn't allow strings or "volatile" values
    chanp->addPar(p);
}

cModuleType *cNEDNetworkBuilder::findAndCheckModuleType(const char *modtypename, cModule *modp, const char *submodname)
{
    cModuleType *modtype = findModuleType(modtypename);
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
    const char *submodtypename;
    if (strnull(submod->getLikeParam()))
    {
        submodtypename = submod->getTypeName();
    }
    else
    {
        const char *parname = submod->getLikeParam();
        submodtypename = modp->par(parname).stringValue();
    }

    ExpressionNode *vectorsizeexpr = findExpression(submod, "vector-size");

    if (!vectorsizeexpr)
    {
        cModuleType *submodtype = findAndCheckModuleType(submodtypename, modp, submodname);
        cModule *submodp = submodtype->create(submodname, modp);
        ModulePtrVector& v = submodMap[submodname];
        v.push_back(submodp);

        cContextSwitcher __ctx(submodp); // params need to be evaluated in the module's context
        setDisplayString(submodp, submod);
        assignSubmoduleParams(submodp, submod);
        submodp->readInputParams();
        setupGateVectors(submodp, submod);
    }
    else
    {
        int vectorsize = (int) evaluate(modp, vectorsizeexpr);
        ModulePtrVector& v = submodMap[submodname];
        cModuleType *submodtype = NULL;
        for (int i=0; i<vectorsize; i++)
        {
            if (!submodtype)
                submodtype = findAndCheckModuleType(submodtypename, modp, submodname);
            cModule *submodp = submodtype->create(submodname, modp, vectorsize, i);
            v.push_back(submodp);

            cContextSwitcher __ctx(submodp); // params need to be evaluated in the module's context
            setDisplayString(submodp, submod);
            assignSubmoduleParams(submodp, submod);
            submodp->readInputParams();
            setupGateVectors(submodp, submod);
        }
    }

    // Note: buildInside() will be called when connections have been built out
    // on this level too.
}


void cNEDNetworkBuilder::setDisplayString(cModule *submodp, SubmoduleNode *submod)
{
    DisplayStringNode *dispstrnode = submod->getFirstDisplayStringChild();
    if (dispstrnode)
    {
        const char *dispstr = dispstrnode->getValue();
        submodp->setDisplayString(dispstr);
    }
}

void cNEDNetworkBuilder::setConnDisplayString(cGate *srcgatep, ConnectionNode *conn)
{
    DisplayStringNode *dispstrnode = conn->getFirstDisplayStringChild();
    if (dispstrnode)
    {
        const char *dispstr = dispstrnode->getValue();
        srcgatep->setDisplayString(dispstr);
    }
}

void cNEDNetworkBuilder::setBackgroundDisplayString(cModule *modp, CompoundModuleNode *mod)
{
    DisplayStringNode *dispstrnode = mod->getFirstDisplayStringChild();
    if (dispstrnode)
    {
        const char *dispstr = dispstrnode->getValue();
        modp->setBackgroundDisplayString(dispstr);
    }
}

void cNEDNetworkBuilder::assignSubmoduleParams(cModule *submodp, NEDElement *submod)
{
    SubstparamsNode *substparams = (SubstparamsNode *) submod->getFirstChildWithTag(NED_SUBSTPARAMS);
    cModule *modp = submodp->parentModule();
    for (; substparams; substparams = substparams->getNextSubstparamsNodeSibling())
    {
        // evaluate condition
        ExpressionNode *condexpr = findExpression(substparams, "condition");
        bool cond = !condexpr || evaluate(modp, condexpr, submodp)!=0;

        // process section
        if (cond)
        {
            for (SubstparamNode *par=substparams->getFirstSubstparamChild(); par; par=par->getNextSubstparamNodeSibling())
            {
                // assign param value
                const char *parname = par->getName();
                cPar& p = submodp->par(parname);
                assignParamValue(p, findExpression(par,"value"),modp,submodp);
            }
        }
    }
}

void cNEDNetworkBuilder::setupGateVectors(cModule *submodp, NEDElement *submod)
{
    GatesizesNode *gatesizes = (GatesizesNode *) submod->getFirstChildWithTag(NED_GATESIZES);
    cModule *modp = submodp->parentModule();
    for (; gatesizes; gatesizes = gatesizes->getNextGatesizesNodeSibling())
    {
        // evaluate condition
        ExpressionNode *condexpr = findExpression(gatesizes, "condition");
        bool cond = !condexpr || evaluate(modp, condexpr, submodp)!=0;

        // process section
        if (cond)
        {
            for (GatesizeNode *gate=gatesizes->getFirstGatesizeChild(); gate; gate=gate->getNextGatesizeNodeSibling())
            {
                // set gate vector size
                const char *gatename = gate->getName();
                int vectorsize = (int) evaluate(modp, findExpression(gate, "vector-size"), submodp);
                submodp->setGateSize(gatename, vectorsize);
                //printf("DBG: gatesize: %s.%s[%d]\n", submodp->fullPath().c_str(), gatename, vectorsize);
            }
        }
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

void cNEDNetworkBuilder::addLoopConnection(cModule *modp, ForLoopNode *forloop)
{
    loopVarSP = 0;
    doLoopVar(modp, forloop->getFirstLoopVarChild());
}

void cNEDNetworkBuilder::doLoopVar(cModule *modp, LoopVarNode *loopvar)
{
    ForLoopNode *forloop = (ForLoopNode *) loopvar->getParent();

    int start = (int) evaluate(modp, findExpression(loopvar, "from-value"));
    int end = (int) evaluate(modp, findExpression(loopvar, "to-value"));
    LoopVarNode *nextloopvar = loopvar->getNextLoopVarNodeSibling();

    // register loop var
    if (loopVarSP==MAX_LOOP_NESTING)
        throw new cRuntimeError("dynamic module builder: nesting of for loops is too deep, max %d is allowed", MAX_LOOP_NESTING);
    loopVarSP++;
    loopVarStack[loopVarSP-1].varname = loopvar->getParamName();
    int& i = loopVarStack[loopVarSP-1].value;

    // do for loop
    if (nextloopvar)
    {
        for (i=start; i<=end; i++)
        {
            // do nested loops
            doLoopVar(modp, nextloopvar);
        }
    }
    else
    {
        for (i=start; i<=end; i++)
        {
            // do connections
            for (ConnectionNode *conn=forloop->getFirstConnectionChild(); conn; conn=conn->getNextConnectionNodeSibling())
            {
                addConnection(modp, conn);
            }
        }
    }

    // deregister loop var
    loopVarSP--;
}

void cNEDNetworkBuilder::addConnection(cModule *modp, ConnectionNode *conn)
{
    // check condition first
    ExpressionNode *condexpr = findExpression(conn, "condition");
    if (condexpr)
    {
        bool cond = evaluate(modp, condexpr)!=0;
        if (cond == false)
            return;
    }

    // find gates and create connection
    cGate *srcg = resolveGate(modp, conn->getSrcModule(), findExpression(conn, "src-module-index"),
                                    conn->getSrcGate(), findExpression(conn, "src-gate-index"),
                                    conn->getSrcGatePlusplus());
    cGate *destg = resolveGate(modp, conn->getDestModule(), findExpression(conn, "dest-module-index"),
                                     conn->getDestGate(), findExpression(conn, "dest-gate-index"),
                                     conn->getDestGatePlusplus());
    cChannel *channel = createChannelForConnection(conn,modp);

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
    if (channel)
        srcg->connectTo(destg, channel);
    else
        srcg->connectTo(destg);

    // display string
    setConnDisplayString(srcg, conn);
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
        int modindex = !modindexp ? 0 : (int) evaluate(parentmodp, modindexp);
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
        int gateindex = (int) evaluate(parentmodp, gateindexp);
        gatep = modp->gate(gatename, gateindex);
        if (!gatep)
            throw new cRuntimeError("dynamic module builder: module (%s)%s has no gate `%s[%d]'",
                                    modp->className(), modp->fullPath().c_str(), gatename, gateindex);
    }
    return gatep;
}

cChannel *cNEDNetworkBuilder::createChannelForConnection(ConnectionNode *conn, cModule *parentmodp)
{
    ConnAttrNode *connattr = conn->getFirstConnAttrChild();
    if (!connattr)
        return NULL;

    if (!strcmp(connattr->getName(),"channel"))
    {
        // find channel name
        ExpressionNode *expr = findExpression(connattr,"value");
        ConstNode *cnode = expr->getFirstConstChild();
        if (!cnode || cnode->getType()!=NED_CONST_STRING)
            throw new cRuntimeError("dynamic module builder: channel type should be string constant");
        const char *channeltypename = cnode->getValue();

        // create channel
        cChannelType *channeltype = findChannelType(channeltypename);
        if (!channeltype)
            throw new cRuntimeError("dynamic module builder: channel type %s not found", channeltypename);
        cChannel *channel = channeltype->create("channel");
        return channel;
    }

    // connection attributes
    cBasicChannel *channel = new cBasicChannel();
    for (ConnAttrNode *child=conn->getFirstConnAttrChild(); child; child = child->getNextConnAttrNodeSibling())
    {
        const char *name = child->getName();
        ExpressionNode *expr = findExpression(child,"value");
        cPar *par = new cPar(name);
        assignParamValue(*par, expr, parentmodp,NULL);
        channel->addPar(par);
    }
    return channel;
}

*/
