//==========================================================================
//   CNEDNETWORKBUILDER.CC
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

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
}

cNEDNetworkBuilder::~cNEDNetworkBuilder()
{
}

void cNEDNetworkBuilder::setupNetwork(NetworkNode *networknode)
{
    // this code is a simplified version of addSubmodule()
    const char *modname = networknode->getName();
    const char *modtypename = networknode->getTypeName();

    cModuleType *modtype = findModuleType(modtypename);
    if (!modtype)
        throw new cException("dynamic module builder: module type definition `%s' for network %s not found",
                             modtypename, modname);

    cModule *networkp = modtype->create(modname, NULL);
    assignSubmoduleParams(networkp, networknode);
    readInputParams(networkp);

    networkp->buildInside();
}

void cNEDNetworkBuilder::buildInside(cModule *modp, CompoundModuleNode *modulenode)
{
    // loop through submods and add them
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

    // FIXME move buildinside here!!!!!
}

cChannel *cNEDNetworkBuilder::createChannel(const char *name, ChannelNode *channelnode)
{
    cSimpleChannel *chanp = new cSimpleChannel(name);
    for (ChannelAttrNode *chattr=channelnode->getFirstChannelAttrChild(); chattr; chattr=chattr->getNextChannelAttrNodeSibling())
    {
        addChannelAttr(chanp, chattr);
    }
    return chanp;
}

void cNEDNetworkBuilder::addChannelAttr(cChannel *chanp, ChannelAttrNode *channelattr)
{
    // FIXME fill this in!
}

void cNEDNetworkBuilder::addSubmodule(cModule *modp, SubmoduleNode *submod)
{
    // create submodule
    const char *modname = submod->getName();
    const char *modtypename;
    if (strnull(submod->getLikeParam()))
    {
        modtypename = submod->getTypeName();
    }
    else
    {
        const char *parname = submod->getLikeParam();
        modtypename = modp->par(parname).stringValue();
    }

    cModuleType *modtype = findModuleType(modtypename);
    if (!modtype)
        throw new cException("dynamic module builder: module type definition `%s' for submodule %s in (%s)%s not found",
                             modtypename, modname, modp->className(), modp->fullPath());

    ExpressionNode *vectorsizeexpr = findExpression(submod, "vector-size");

    if (!vectorsizeexpr)
    {
        cModule *submodp = modtype->create(modname, modp);
        setDisplayString(submodp, submod);
        assignSubmoduleParams(submodp, submod);
        readInputParams(submodp);
        setupGateVectors(submodp, submod);

        submodp->buildInside();
    }
    else
    {
        int vectorsize = (int) evaluate(modp, vectorsizeexpr);
        for (int i=0; i<vectorsize; i++)
        {
            cModule *submodp = modtype->create(modname, modp, vectorsize, i);
            setDisplayString(submodp, submod);
            assignSubmoduleParams(submodp, submod);
            readInputParams(submodp);
            setupGateVectors(submodp, submod);

            submodp->buildInside();
        }
    }
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
                assignParamValue(p, findExpression(par,"value"),modp);
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
                printf("DBG: gatesize: %s.%s[%d]\n", submodp->fullPath(), gatename, vectorsize);
            }
        }
    }
}

void cNEDNetworkBuilder::readInputParams(cModule *submodp)
{
	int n = submodp->params();
	for (int i=0; i<n; i++)
	{
	   if (submodp->par(i).isInput())
	   {
	      submodp->par(i).read();
	   }
	}
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
        throw new cException("dynamic module builder: nesting of for loops is too deep, max %d is allowed", MAX_LOOP_NESTING);
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
                                    conn->getSrcGate(), findExpression(conn, "src-gate-index"));
    cGate *destg = resolveGate(modp, conn->getDestModule(), findExpression(conn, "dest-module-index"),
                                     conn->getDestGate(), findExpression(conn, "dest-gate-index"));
    cChannel *channel = createChannel(conn);

    // connect
    if (channel)
        srcg->connectTo(destg, channel);
    else
        srcg->connectTo(destg);
}

cGate *cNEDNetworkBuilder::resolveGate(cModule *parentmodp,
                                       const char *modname, ExpressionNode *modindexp,
                                       const char *gatename, ExpressionNode *gateindexp)
{
// FIXME plusplus!!!
    cModule *modp;
    if (strnull(modname))
    {
        modp = parentmodp;
    }
    else
    {
        int modindex = !modindexp ? 0 : (int) evaluate(parentmodp, modindexp);
        modp = parentmodp->submodule(modname,modindex);
        if (!modp)
        {
            if (!modindexp)
                throw new cException("dynamic module builder: submodule `%s' in (%s)%s not found",
                                     modname, parentmodp->className(), parentmodp->fullPath());
            else
                throw new cException("dynamic module builder: submodule `%s[%d]' in (%s)%s not found",
                                     modname, modindex, parentmodp->className(), parentmodp->fullPath());
        }
    }

    int gateindex = !gateindexp ? 0 : (int) evaluate(parentmodp, gateindexp);
    cGate *gatep = modp->gate(gatename, gateindex);
    if (!gatep)
    {
        if (!gateindexp)
            throw new cException("dynamic module builder: module (%s)%s has no gate `%s'",
                                 modp->className(), modp->fullPath(), gatename);
        else
            throw new cException("dynamic module builder: module (%s)%s has no gate `%s[%d]'",
                                 modp->className(), modp->fullPath(), gatename, gateindex);
    }

    return gatep;
}

cChannel *cNEDNetworkBuilder::createChannel(ConnectionNode *conn)
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
            throw new cException("dynamic module builder: channel type should be string constant");
        const char *channeltypename = cnode->getValue();

        // create channel
        cChannelType *channeltype = findChannelType(channeltypename);
        if (!channeltype)
            throw new cException("dynamic module builder: channel type %s not found", channeltypename);
        cChannel *channel = channeltype->create("channel");
        return channel;
    }

    // connection attributes
    cSimpleChannel *channel = new cSimpleChannel();
    for (ConnAttrNode *child=conn->getFirstConnAttrChild(); child; child = child->getNextConnAttrNodeSibling())
    {
        const char *name = child->getName();
        ExpressionNode *expr = findExpression(child,"value");
        cPar *par = new cPar(name);
        assignParamValue(*par, expr, NULL);
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
    // FIXME submodp may be network module, then parentmodp==NULL?
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
            modp = parentmodp->submodule(modname,modindex);
            if (!modp)
            {
                if (!modindexp)
                    throw new cException("dynamic module builder: submodule `%s' not found", modname);
                else
                    throw new cException("dynamic module builder: submodule `%s[%d]' not found", modname, modindex);
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
            throw new cException("dynamic module builder: evaluate: unexpected tag %s", node->getTagName());
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
            throw new cException("dynamic module builder: evaluate: unexpected operator %s", name);
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
            throw new cException("dynamic module builder: evaluate: unexpected operator %s", name);
    }
    else
    {
        // tertiary can only be "?:"
        if (!strcmp(name,"?:"))
            return val1 ? val2 : val3;
        else
            throw new cException("dynamic module builder: evaluate: unexpected operator %s", name);
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
            throw new cException("dynamic module builder: evaluate: index operator cannot be used outside module parameters");
        return submodp->index();
    }
    else if (!strcmp(funcname,"sizeof"))
    {
        IdentNode *op1 = node->getFirstIdentChild();
        ASSERT(op1);
        const char *name = op1->getName();

        if (parentmodp)  // in network params, parentmodp==NULL
        {
            // if not found, find among submodules
            cModule *m = parentmodp->submodule(name);
            if (m)
                return m->size();

            // if not found, find among gates of parent module
            cGate *g = parentmodp->gate(name);
            if (g)
                return g->size();
        }
        throw new cException("dynamic module builder: evaluate: sizeof(%s) failed -- no such gate or module", name);
    }
    else if (!strcmp(funcname,"input"))
    {
        // "input" is handled separately (where module parameters are assigned)
        throw new cException("dynamic module builder: evaluate: input operator cannot be used here");
    }

    // normal function: find it and evaluate
    cFunctionType *functype = findFunction(funcname,argcount);
    if (!functype)
        throw new cException("dynamic module builder: function %s with %d args not found", funcname, argcount);

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
        default: throw new cException("dynamic module builder: evaluate: only functions with up to 4 arguments are supported");
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

    throw new cException("dynamic module builder: unknown variable `%s'", varname);
}

double cNEDNetworkBuilder::evalConst(ConstNode *node, cModule *, cModule *)
{
    const char *val = node->getValue();
    switch (node->getType())
    {
        case NED_CONST_BOOL:
            return !strcmp(val,"true");
        case NED_CONST_INT:
            return atoi(node->getValue());
        case NED_CONST_REAL:
            return atof(node->getValue());
        case NED_CONST_TIME:
            return strToSimtime(node->getValue());
        case NED_CONST_STRING:
            throw new cException("dynamic module builder: evaluate: string literals not supported here");
        default:
            throw new cException("dynamic module builder: evaluate: internal error: wrong constant type");
    }
}

//----------------------------------------------------------------------------
// assign parameter value
// build reverse Polish expressions

// helper, for debugging
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

void cNEDNetworkBuilder::assignParamValue(cPar& p, ExpressionNode *expr, cModule *parentmodp)
{
    cModule *submodp = check_and_cast<cModulePar *>(&p)->ownerModule();

    // handle "input" here (it must be single item in expression)
    FunctionNode *fnode = expr->getFirstFunctionChild();
    if (fnode && !strcmp(fnode->getName(),"input"))
    {
        NEDElement *op1 = fnode->getFirstChild();
        NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;

        // op1 is default value
        if (op1)
        {
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
        char buf[512]; // FIXME dbg code only (2 lines)
        par->info(buf);
        printf("DBG: param assignment: %s = %s = %s\n", p.fullPath(), par->fullName(), buf);
        if (pnode->getIsRef())
            p.setRedirection(par);
        else
            p = *par;
        return;
    }

    // FIXME if param is const (non-volatile), no need for reverse polish stuff!
    //  ^^ we need to check moduleinterface for that!

    // check if we really need an expression (ie. not if expr is simple or param is const)
    if (!needsDynamicExpression(expr))
    {
        // static evaluation will do
        double d = evaluate(parentmodp, expr, submodp);
        p.setDoubleValue(d);
    }
    else
    {
        // create dynamically evaluated expression (reverse Polish)
        cPar::ExprElem *xelems = new cPar::ExprElem[1000]; // FIXME count needed size
        int n = 0;
        addXElems(expr->getFirstChild(), xelems, n, submodp);
        p.setDoubleValue(xelems, n);

        char buf[1000];
        printXElems(buf, xelems, n);
        printf("dbg: compiled expression: %s = %s\n", p.fullPath(), buf);
    }
}

bool cNEDNetworkBuilder::needsDynamicExpression(ExpressionNode *expr)
{
    // FIXME this has to be revised!

    // if simple expr, no expression class necessary
    NEDElement *node = expr->getFirstChild();
    if (!node)  {INTERNAL_ERROR0(expr, "needsExpressionClass(): empty expression");return false;}
    int tag = node->getTagCode();

    // only non-const parameter assignments and channel attrs need expression classes (?)
    int parenttag = expr->getParent()->getTagCode();
    if (parenttag!=NED_SUBSTPARAM && parenttag!=NED_CHANNEL_ATTR && parenttag!=NED_CONN_ATTR)
        return false;

    // also: if the parameter is non-volatile (that is, const), we don't need expr class
    if (parenttag==NED_SUBSTPARAM && false /*FIXME ...and param is const*/)
        return false;

    // identifiers and constants never need expression classes
    if (tag==NED_IDENT || tag==NED_CONST)
        return false;

    // special functions (INPUT, INDEX, SIZEOF) may also go without expression classes
    if (tag==NED_FUNCTION)
    {
        const char *funcname = ((FunctionNode *)node)->getName();
        if (!strcmp(funcname,"index"))
            return false;
        if (!strcmp(funcname,"sizeof"))
            return false;
        if (!strcmp(funcname,"input"))
        {
            NEDElement *op1 = node->getFirstChild();
            NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
            if ((!op1 || op1->getTagCode()==NED_CONST) && (!op2 || op2->getTagCode()==NED_CONST))
                return false;
        }
    }

    // FIXME: some paramrefs and functions may also qualify
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
            throw new cException("dynamic module builder: evaluate: unexpected tag %s", node->getTagName());
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
            throw new cException("dynamic module builder: evaluate: unexpected operator %s", name);
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
            throw new cException("dynamic module builder: evaluate: unexpected operator %s", name);
    }
    else
    {
        // tertiary can only be "?:"
        if (!strcmp(name,"?:"))
            xelems[pos++] = '?';
        else
            throw new cException("dynamic module builder: evaluate: unexpected operator %s", name);
    }
}

void cNEDNetworkBuilder::addXElemsFunction(FunctionNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp)
{
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

        // find among local module gates
        cGate *g = submodp->gate(name);
        if (g)
        {
            xelems[pos++] = g->size();
            return;
        }

        // if not found, find among submodules
        cModule *parentmodp = submodp->parentModule();
        // FIXME submodp may be network module, then parentmodp==NULL?
        cModule *m = parentmodp->submodule(name);
        if (!m) m = parentmodp->submodule(name,0);
        if (m)
        {
            xelems[pos++] = m->size();
            return;
        }

        throw new cException("dynamic module builder: evaluate: sizeof(%s) failed -- no such gate or module", name);
    }
    else if (!strcmp(funcname,"input"))
    {
        // "input" is handled separately (where module parameters are assigned)
        throw new cException("dynamic module builder: evaluate: input operator cannot be used here");
    }

    // normal function: find it and add to reverse Polish expression
    cFunctionType *functype = findFunction(funcname,argcount);
    if (!functype)
        throw new cException("dynamic module builder: function %s with %d args not found", funcname, argcount);

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
        default: throw new cException("dynamic module builder: internal error: function with %d args???", funcname, argcount);
    }
}

void cNEDNetworkBuilder::addXElemsParamref(ParamRefNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp)
{
    cModule *parentmodp = submodp->parentModule(); // FIXME what if submodp==NULL?????
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
    throw new cException("dynamic module builder: internal error: variable found in dynamic (reverse Polish) expression (`%s')", varname);
}

void cNEDNetworkBuilder::addXElemsConst(ConstNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp)
{
    const char *val = node->getValue();
    switch (node->getType())
    {
        case NED_CONST_BOOL: xelems[pos++] = !strcmp(val,"true"); break;
        case NED_CONST_INT:  xelems[pos++] = atoi(node->getValue()); break;
        case NED_CONST_REAL: xelems[pos++] = atof(node->getValue()); break;
        case NED_CONST_TIME: xelems[pos++] = strToSimtime(node->getValue()); break;
        case NED_CONST_STRING: throw new cException("dynamic module builder: evaluate: string literals not supported here");
        default: throw new cException("dynamic module builder: evaluate: internal error: wrong constant type");
    }
}


