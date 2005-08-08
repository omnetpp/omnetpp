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

#ifdef _MSC_VER
// disable "identifier was truncated to '255' characters in the debug information" warnings
#pragma warning(disable:4786)
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "cmodule.h"
#include "cgate.h"
#include "cchannel.h"
#include "ctypes.h"

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

cNEDNetworkBuilder::cNEDNetworkBuilder()
{
    loopVarSP = 0;
    xelemsBuf = new cPar::ExprElem[1000]; // we'll use it as buffer
}

cNEDNetworkBuilder::~cNEDNetworkBuilder()
{
    delete [] xelemsBuf;
}

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
    readInputParams(networkp);

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
        readInputParams(submodp);
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
            readInputParams(submodp);
            setupGateVectors(submodp, submod);
        }
    }

    // Note: buildInside() will be called when connections have been built out
    // on this level too.
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

void cNEDNetworkBuilder::readInputParams(cModule *submodp)
{
    // same code as generated by nedtool into _n.cc files
    int n = submodp->params();
    for (int i=0; i<n; i++)
        if (submodp->par(i).isInput())
            submodp->par(i).read();
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

double cNEDNetworkBuilder::evaluate(cModule *modp, ExpressionNode *expr, cModule *submodp)
{
    // uses registered loopvars and module/submodule parameters
    return evaluateNode(expr->getFirstChild(), modp, submodp);
}

cPar *cNEDNetworkBuilder::resolveParamRef(ParamRefNode *node, cModule *parentmodp, cModule *submodp)
{
    // submodp may be network module, then parentmodp==NULL
    if (!parentmodp)
        throw new cRuntimeError("dynamic module builder: parameter reference occurs in wrong context");

    const char *paramname = node->getParamName();
    cPar *par;
    if (node->getIsAncestor())
    {
        // find ancestor parameter
        par = &(parentmodp->ancestorPar(paramname)); // this throws exception if not found
    }
    else
    {
        // find module
        cModule *modp;
        if (strnull(node->getModule()))
        {
            modp = parentmodp;
        }
        else
        {
            const char *modname = node->getModule();
            ExpressionNode *modindexp = findExpression(node, "module-index");
            int modindex = !modindexp ? 0 : (int) evaluate(parentmodp, modindexp,submodp);
            modp = _submodule(parentmodp, modname,modindex);
            if (!modp)
            {
                if (!modindexp)
                    throw new cRuntimeError("dynamic module builder: submodule `%s' not found", modname);
                else
                    throw new cRuntimeError("dynamic module builder: submodule `%s[%d]' not found", modname, modindex);
            }
        }
        par = &(modp->par(paramname)); // this throws exception if not found
    }
    return par;
}

//----------------------------------------------------------------------------
// direct expression evaluation

double cNEDNetworkBuilder::evaluateNode(NEDElement *node, cModule *parentmodp, cModule *submodp)
{
    int tagcode = node->getTagCode();
    switch (tagcode)
    {
        case NED_OPERATOR:
            return evalOperator((OperatorNode *)node, parentmodp, submodp);
        case NED_FUNCTION:
            return evalFunction((FunctionNode *)node, parentmodp, submodp);
        case NED_PARAM_REF:
            return evalParamref((ParamRefNode *)node, parentmodp, submodp);
        case NED_IDENT:
            return evalIdent((IdentNode *)node, parentmodp, submodp);
        case NED_CONST:
            return evalConst((ConstNode *)node, parentmodp, submodp);
        default:
            throw new cRuntimeError("dynamic module builder: evaluate: unexpected tag %s", node->getTagName());
    }
}

double cNEDNetworkBuilder::evalOperator(OperatorNode *node, cModule *parentmodp, cModule *submodp)
{
    const char *name = node->getName();
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    double val1 = op1 ? evaluateNode(op1,parentmodp, submodp) : 0;
    double val2 = op2 ? evaluateNode(op2,parentmodp, submodp) : 0;
    double val3 = op3 ? evaluateNode(op3,parentmodp, submodp) : 0;

    if (!op2)
    {
        // unary:
        if (!strcmp(name,"-"))
            return -1 * val1;
        else if (!strcmp(name,"!"))
            return val1==0.0;
        else if (!strcmp(name,"~"))
            return ~(unsigned long)val1;
        else
            throw new cRuntimeError("dynamic module builder: evaluate: unexpected operator %s", name);
    }
    else if (!op3)
    {
        // binary:

        // arithmetic
        if (!strcmp(name,"+"))
            return val1 + val2;
        else if (!strcmp(name,"-"))
            return val1 - val2;
        else if (!strcmp(name,"*"))
            return val1 * val2;
        else if (!strcmp(name,"/"))
            return val1 / val2;
        else if (!strcmp(name,"%"))
            return (long)val1 % (long)val2;
        else if (!strcmp(name,"^"))
            return pow(val1, val2);

        // logical
        else if (!strcmp(name,"=="))
            return val1 == val2;
        else if (!strcmp(name,"!="))
            return val1 != val2;
        else if (!strcmp(name,"<"))
            return val1 < val2;
        else if (!strcmp(name,"<="))
            return val1 <= val2;
        else if (!strcmp(name,">"))
            return val1 > val2;
        else if (!strcmp(name,">="))
            return val1 >= val2;
        else if (!strcmp(name,"&&"))
            return (bool)val1 && (bool)val2;
        else if (!strcmp(name,"||"))
            return (bool)val1 || (bool)val2;
        else if (!strcmp(name,"##"))
            return (bool)val1 != (bool)val2;

        // bitwise
        else if (!strcmp(name,"&"))
            return (unsigned long)val1 & (unsigned long)val2;
        else if (!strcmp(name,"|"))
            return (unsigned long)val1 | (unsigned long)val2;
        else if (!strcmp(name,"#"))
            return (unsigned long)val1 ^ (unsigned long)val2;
        else if (!strcmp(name,"<<"))
            return (unsigned long)val1 << (unsigned long)val2;
        else if (!strcmp(name,">>"))
            return (unsigned long)val1 >> (unsigned long)val2;
        else
            throw new cRuntimeError("dynamic module builder: evaluate: unexpected operator %s", name);
    }
    else
    {
        // tertiary can only be "?:"
        if (!strcmp(name,"?:"))
            return val1 ? val2 : val3;
        else
            throw new cRuntimeError("dynamic module builder: evaluate: unexpected operator %s", name);
    }
}

double cNEDNetworkBuilder::evalFunction(FunctionNode *node, cModule *parentmodp, cModule *submodp)
{
    // get function name, arg count, args
    const char *funcname = node->getName();
    int argcount = node->getNumChildren();

    // operators should be handled specially
    if (!strcmp(funcname,"index"))
    {
        if (!submodp)
            throw new cRuntimeError("dynamic module builder: evaluate: index operator cannot be used outside a submodule");
        return submodp->index();
    }
    else if (!strcmp(funcname,"sizeof"))
    {
        IdentNode *op1 = node->getFirstIdentChild();
        ASSERT(op1);
        const char *name = op1->getName();

        // TBD this is duplicate code -- same occurs below, in reverse Polish stuff
        if (!parentmodp)
            throw new cRuntimeError("dynamic module builder: evaluate: sizeof() occurs in wrong context", name);

        // find among gates of parent module
        cGate *g = parentmodp->gate(name);
        if (g)
            return g->size();

        // if not found, find among submodules. If there's no such submodule, it may
        // be that such submodule vector never existed, or can be that it's zero
        // size -- we cannot tell, so we have to return 0.
        cModule *m = _submodule(parentmodp, name,0);
        if (!m && _submodule(parentmodp, name))
            throw new cRuntimeError("dynamic module builder: evaluate: sizeof(): %s is not a vector submodule", name);
        return m ? m->size() : 0;
    }
    else if (!strcmp(funcname,"input") || !strcmp(funcname,"xmldoc"))
    {
        // input() and xmldoc() are handled separately (where module parameters are assigned)
        throw new cRuntimeError("dynamic module builder: evaluate: `%s' cannot be used here", funcname);
    }

    // normal function: find it and evaluate
    cFunctionType *functype = findFunction(funcname,argcount);
    if (!functype)
        throw new cRuntimeError("dynamic module builder: function %s with %d args not found", funcname, argcount);

    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;
    NEDElement *op4 = op3 ? op3->getNextSibling() : NULL;

    double val1 = op1 ? evaluateNode(op1,parentmodp, submodp) : 0;
    double val2 = op2 ? evaluateNode(op2,parentmodp, submodp) : 0;
    double val3 = op3 ? evaluateNode(op3,parentmodp, submodp) : 0;
    double val4 = op4 ? evaluateNode(op4,parentmodp, submodp) : 0;

    switch (argcount)
    {
        case 0: return functype->mathFuncNoArg()();
        case 1: return functype->mathFunc1Arg()(val1);
        case 2: return functype->mathFunc2Args()(val1,val2);
        case 3: return functype->mathFunc3Args()(val1,val2,val3);
        case 4: return functype->mathFunc4Args()(val1,val2,val3,val4);
        default: throw new cRuntimeError("dynamic module builder: evaluate: only functions with up to 4 arguments are supported");
    }
}

double cNEDNetworkBuilder::evalParamref(ParamRefNode *node, cModule *parentmodp, cModule *submodp)
{
    // Note: the getIsRef() modifier can be ignored, because this expression
    // is evaluated once (and not stored)
    cPar *par = resolveParamRef(node,parentmodp,submodp);
    ASSERT(par);
    double val = par->doubleValue();
    return val;
}

double cNEDNetworkBuilder::evalIdent(IdentNode *node, cModule *, cModule *)
{
    const char *varname = node->getName();

    // find variable in loop variables
    for (int i=0; i<loopVarSP; i++)
        if (!strcmp(varname, loopVarStack[i].varname))
            return loopVarStack[i].value;

    throw new cRuntimeError("dynamic module builder: unknown variable `%s'", varname);
}

double cNEDNetworkBuilder::evalConst(ConstNode *node, cModule *, cModule *)
{
    const char *val = node->getValue();
    switch (node->getType())
    {
        case NED_CONST_BOOL:
            return !strcmp(val,"true");
        case NED_CONST_INT:
            return strtol(node->getValue(), NULL, 0); // this handles hex as well
        case NED_CONST_REAL:
            return atof(node->getValue());
        case NED_CONST_TIME:
            return strToSimtime(node->getValue());
        case NED_CONST_STRING:
            throw new cRuntimeError("dynamic module builder: evaluate: string literals not supported here");
        default:
            throw new cRuntimeError("dynamic module builder: evaluate: internal error: wrong constant type");
    }
}

//----------------------------------------------------------------------------
// assign parameter value
// build reverse Polish expressions

// helper, for debugging
#if 0
static void printXElems(char *buf, cPar::ExprElem *xelems, int n)
{
    for (int i=0; i<n; i++)
    {
        cPar::ExprElem& x = xelems[i];
        switch (x.type)
        {
            case 'D': sprintf(buf, "%lg ", x.d); break;
            case 'P': sprintf(buf, "P:%s ", x.p->fullName()); break;
            case 'R': sprintf(buf, "R:%s ", x.p->fullName()); break;
            case '0': sprintf(buf, "%s(void) ", findfunctionbyptr(MathFunc(x.f0))->name()); break;
            case '1': sprintf(buf, "%s() ", findfunctionbyptr(MathFunc(x.f1))->name()); break;
            case '2': sprintf(buf, "%s(,) ", findfunctionbyptr(MathFunc(x.f2))->name()); break;
            case '3': sprintf(buf, "%s(,,) ", findfunctionbyptr(MathFunc(x.f3))->name()); break;
            case '4': sprintf(buf, "%s(,,,) ", findfunctionbyptr(MathFunc(x.f4))->name()); break;
            case '@': sprintf(buf, "%c ", x.op); break;
            default:  sprintf(buf, "unknown(type=%c) ", x.type);
        }
        buf += strlen(buf);
    }
}
#endif

void cNEDNetworkBuilder::assignParamValue(cPar& p, ExpressionNode *expr, cModule *parentmodp, cModule *submodp)
{
    // when p is a connection or channel attribute, submodp==NULL (with channel attr, even parentmodp!)

    // handle "input" here (it must be single item in expression)
    FunctionNode *fnode = expr->getFirstFunctionChild();
    if (fnode && !strcmp(fnode->getName(),"input"))
    {
        NEDElement *op1 = fnode->getFirstChild();
        NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;

        // op1 is default value
        if (op1 && op1->getTagCode()==NED_CONST && ((ConstNode *)op1)->getType()==NED_CONST_STRING)
        {
            // string literal
            p.setStringValue(((ConstNode *)op1)->getValue());
        }
        else if (op1)
        {
            // numeric expression
            double d = evaluateNode(op1, parentmodp, submodp);
            p.setDoubleValue(d);
        }
        if (op2 && op2->getTagCode()==NED_CONST)
        {
            p.setPrompt(((ConstNode *)op2)->getValue());
        }
        p.setInput(true);
        return;
    }

    // handle "xmldoc" here (it must be single item in expression)
    if (fnode && !strcmp(fnode->getName(),"xmldoc"))
    {
        NEDElement *op1 = fnode->getFirstChild();
        NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;

        // op1 is xml file name, op2 is optional pathexpr (must be NED_CONST_STRING)
        if (!op1 || op1->getTagCode()!=NED_CONST || ((ConstNode *)op1)->getType()!=NED_CONST_STRING ||
            (op2 && (op2->getTagCode()!=NED_CONST || ((ConstNode *)op2)->getType()!=NED_CONST_STRING)))
            {INTERNAL_ERROR0(fnode, "assignParamValue(): invalid or missing arg for xmldoc()");return;}

        const char *fname = ((ConstNode *)op1)->getValue();
        const char *pathexpr = !op2 ? NULL : ((ConstNode *)op2)->getValue();
        cXMLElement *node = ev.getXMLDocument(fname,pathexpr);
        if (!node)
            throw new cRuntimeError(&p, !op2 ? "xmldoc(\"%s\"): element not found" :
                                            "xmldoc(\"%s\", \"%s\"): element not found",
                                            fname, pathexpr);
        p.setXMLValue(node);
        return;
    }

    // handle string literals here (expressions are all double for now)
    ConstNode *cnode = expr->getFirstConstChild();
    if (cnode && cnode->getType()==NED_CONST_STRING)
    {
        p.setStringValue(cnode->getValue());
        return;
    }

    // handle direct parameter assignments
    ParamRefNode *pnode = expr->getFirstParamRefChild();
    if (pnode)
    {
        cPar *par = resolveParamRef(pnode, parentmodp, submodp);

        //char buf[512];
        //par->info(buf);
        //printf("DBG: param assignment: %s = %s = %s\n", p.fullPath().c_str(), par->fullName(), buf);

        if (pnode->getIsRef())
            p.setRedirection(par);
        else
            p = *par;
        return;
    }

    // check if p should be constant, according to the module decl.
    bool isConst = false;
    if (submodp)
    {
        cModuleInterface *modif = submodp->moduleType()->moduleInterface();
        isConst = modif->isParamConst(modif->findParam(p.name()));
    }

    // check if we really need an expression (ie. not if expr is simple or param is const)
    if (isConst || !needsDynamicExpression(expr))
    {
        // static evaluation will do
        double d = evaluate(parentmodp, expr, submodp);
        p.setDoubleValue(d);
    }
    else
    {
        // create dynamically evaluated expression (reverse Polish).
        // we don't know the size in advance, so first collect it in xelemsBuf[1000],
        // then make a copy
        int n = 0;
        addXElems(expr->getFirstChild(), xelemsBuf, n, submodp);
        cPar::ExprElem *xelems = new cPar::ExprElem[n];
        for (int i=0; i<n; i++)
            xelems[i] = xelemsBuf[i];
        p.setDoubleValue(xelems, n);

        //debug:
        //char buf[1000];
        //printXElems(buf, xelems, n);
        //printf("dbg: compiled expression: %s = %s\n", p.fullPath().c_str(), buf);
    }
}

bool cNEDNetworkBuilder::needsDynamicExpression(ExpressionNode *expr)
{
    // TBD revise

    // if simple expr, no reverse Polish expression necessary
    NEDElement *node = expr->getFirstChild();
    if (!node)  {INTERNAL_ERROR0(expr, "needsExpressionClass(): empty expression");return false;}
    int tag = node->getTagCode();

    // only non-const parameter assignments and channel attrs need reverse Polish expression(?)
    int parenttag = expr->getParent()->getTagCode();
    if (parenttag!=NED_SUBSTPARAM && parenttag!=NED_CHANNEL_ATTR && parenttag!=NED_CONN_ATTR)
        return false;

    // identifiers and constants never need reverse Polish expression
    if (tag==NED_IDENT || tag==NED_CONST)
        return false;

    // also, non-ref bare parameter references never need reverse Polish expression
    if (tag==NED_PARAM_REF && !((ParamRefNode *)node)->getIsRef())
        return false;

    // special functions (INPUT, INDEX, SIZEOF) may also go without reverse Polish expression
    if (tag==NED_FUNCTION)
    {
        const char *funcname = ((FunctionNode *)node)->getName();
        if (!strcmp(funcname,"index"))
            return false;
        if (!strcmp(funcname,"sizeof"))
            return false;
        if (!strcmp(funcname,"input"))
            return false;
    }

    // TBD some paramrefs and functions may also qualify
    return true;
}

void cNEDNetworkBuilder::addXElems(NEDElement *node, cPar::ExprElem *xelems, int& pos, cModule *submodp)
{
    int tagcode = node->getTagCode();
    switch (tagcode)
    {
        case NED_OPERATOR:
            addXElemsOperator((OperatorNode *)node, xelems, pos, submodp); break;
        case NED_FUNCTION:
            addXElemsFunction((FunctionNode *)node, xelems, pos, submodp); break;
        case NED_PARAM_REF:
            addXElemsParamref((ParamRefNode *)node, xelems, pos, submodp); break;
        case NED_IDENT:
            addXElemsIdent((IdentNode *)node, xelems, pos, submodp); break;
        case NED_CONST:
            addXElemsConst((ConstNode *)node, xelems, pos, submodp); break;
        default:
            throw new cRuntimeError("dynamic module builder: evaluate: unexpected tag %s", node->getTagName());
    }
}

void cNEDNetworkBuilder::addXElemsOperator(OperatorNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp)
{
    // push args first
    for (NEDElement *op=node->getFirstChild(); op; op=op->getNextSibling())
    {
        addXElems(op, xelems, pos, submodp);
    }

    // determine name and arg count
    const char *name = node->getName();
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    if (!op2)
    {
        // unary:
        if (!strcmp(name,"-"))
        {
            xelems[pos++] = -1;
            xelems[pos++] = '*';
        }
        else if (!strcmp(name,"!"))
            xelems[pos++] = bool_not;
        else if (!strcmp(name,"~"))
            xelems[pos++] = bin_compl;
        else
            throw new cRuntimeError("dynamic module builder: evaluate: unexpected operator %s", name);
    }
    else if (!op3)
    {
        // binary:

        // arithmetic
        if (!strcmp(name,"+"))
            xelems[pos++] = '+';
        else if (!strcmp(name,"-"))
            xelems[pos++] = '-';
        else if (!strcmp(name,"*"))
            xelems[pos++] = '*';
        else if (!strcmp(name,"/"))
            xelems[pos++] = '/';
        else if (!strcmp(name,"%"))
            xelems[pos++] = '%';
        else if (!strcmp(name,"^"))
            xelems[pos++] = '^';

        // logical
        else if (!strcmp(name,"=="))
            xelems[pos++] = '=';
        else if (!strcmp(name,"!="))
            xelems[pos++] = '!';
        else if (!strcmp(name,"<"))
            xelems[pos++] = '<';
        else if (!strcmp(name,"<="))
            xelems[pos++] = '{';
        else if (!strcmp(name,">"))
            xelems[pos++] = '>';
        else if (!strcmp(name,">="))
            xelems[pos++] = '}';
        else if (!strcmp(name,"&&"))
            xelems[pos++] = bool_and;
        else if (!strcmp(name,"||"))
            xelems[pos++] = bool_or;
        else if (!strcmp(name,"##"))
            xelems[pos++] = bool_xor;

        // bitwise
        else if (!strcmp(name,"&"))
            xelems[pos++] = bin_and;
        else if (!strcmp(name,"|"))
            xelems[pos++] = bin_or;
        else if (!strcmp(name,"#"))
            xelems[pos++] = bin_xor;
        else if (!strcmp(name,"<<"))
            xelems[pos++] = shift_left;
        else if (!strcmp(name,">>"))
            xelems[pos++] = shift_right;
        else
            throw new cRuntimeError("dynamic module builder: evaluate: unexpected operator %s", name);
    }
    else
    {
        // tertiary can only be "?:"
        if (!strcmp(name,"?:"))
            xelems[pos++] = '?';
        else
            throw new cRuntimeError("dynamic module builder: evaluate: unexpected operator %s", name);
    }
}

void cNEDNetworkBuilder::addXElemsFunction(FunctionNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp)
{
    ASSERT(submodp); // FIXME we're in trouble here: for conn parameters, where do we get a parentmodp???

    // get function name, arg count, args
    const char *funcname = node->getName();
    int argcount = node->getNumChildren();

    // operators should be handled specially
    if (!strcmp(funcname,"index"))
    {
        xelems[pos++] = submodp->index();
        return;
    }
    else if (!strcmp(funcname,"sizeof"))
    {
        IdentNode *op1 = node->getFirstIdentChild();
        ASSERT(op1);
        const char *name = op1->getName();

        // TBD this is duplicate code -- same occurs in evaluated expressions as well
        cModule *parentmodp = submodp->parentModule();
        if (!parentmodp)
            throw new cRuntimeError("dynamic module builder: evaluate: sizeof() occurs in wrong context", name);

        // find among parent module gates
        cGate *g = parentmodp->gate(name);
        if (g)
        {
            xelems[pos++] = g->size();
            return;
        }

        // if not found, find among submodules. If there's no such submodule, it may
        // be that such submodule vector never existed, or can be that it's zero
        // size -- we cannot tell, so we have to return 0.
        cModule *m = _submodule(parentmodp, name,0);
        if (!m && _submodule(parentmodp, name))
            throw new cRuntimeError("dynamic module builder: evaluate: sizeof(): %s is not a vector submodule", name);
        xelems[pos++] = m ? m->size() : 0;
        return;
    }
    else if (!strcmp(funcname,"input"))
    {
        // "input" is handled separately (where module parameters are assigned)
        throw new cRuntimeError("dynamic module builder: evaluate: input operator cannot be used here");
    }

    // normal function: find it and add to reverse Polish expression
    cFunctionType *functype = findFunction(funcname,argcount);
    if (!functype)
        throw new cRuntimeError("dynamic module builder: function %s with %d args not found", funcname, argcount);

    // push args first
    for (NEDElement *op=node->getFirstChild(); op; op=op->getNextSibling())
    {
        addXElems(op, xelems, pos, submodp);
    }

    // then function
    switch (argcount)
    {
        case 0: xelems[pos++] = functype->mathFuncNoArg(); break;
        case 1: xelems[pos++] = functype->mathFunc1Arg(); break;
        case 2: xelems[pos++] = functype->mathFunc2Args(); break;
        case 3: xelems[pos++] = functype->mathFunc3Args(); break;
        case 4: xelems[pos++] = functype->mathFunc4Args(); break;
        default: throw new cRuntimeError("dynamic module builder: internal error: function with %d args???", funcname, argcount);
    }
}

void cNEDNetworkBuilder::addXElemsParamref(ParamRefNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp)
{
    ASSERT(submodp); // FIXME we're in trouble here: for conn parameters, where do we get a parentmodp???

    cModule *parentmodp = submodp->parentModule();
    cPar *par = resolveParamRef(node,parentmodp,submodp);

    // store either the object itself or a copy
    ASSERT(par);
    if (node->getIsRef())
        xelems[pos++] = par;  // store pointer ('P' type)
    else
        xelems[pos++] = *par;  // op=() makes a copy of the cPar ('R' type)
}

void cNEDNetworkBuilder::addXElemsIdent(IdentNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp)
{
    const char *varname = node->getName();
    throw new cRuntimeError("dynamic module builder: internal error: variable found in dynamic (reverse Polish) expression (`%s')", varname);
}

void cNEDNetworkBuilder::addXElemsConst(ConstNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp)
{
    const char *val = node->getValue();
    switch (node->getType())
    {
        case NED_CONST_BOOL: xelems[pos++] = !strcmp(val,"true"); break;
        case NED_CONST_INT:  xelems[pos++] = strtol(node->getValue(), NULL, 0); break; // this handles hex as well
        case NED_CONST_REAL: xelems[pos++] = atof(node->getValue()); break;
        case NED_CONST_TIME: xelems[pos++] = strToSimtime(node->getValue()); break;
        case NED_CONST_STRING: throw new cRuntimeError("dynamic module builder: evaluate: string literals not supported here");
        default: throw new cRuntimeError("dynamic module builder: evaluate: internal error: wrong constant type");
    }
}


