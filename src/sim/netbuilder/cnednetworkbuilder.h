//==========================================================================
//   CNEDNETWORKBUILDER.H -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CNETWORKBUILDER_H
#define __CNETWORKBUILDER_H

#include "nedelements.h"
#include "cpar.h"

class cModule;
class cGate;
class cChannel;
class cPar;


#define MAX_LOOP_NESTING 32


/**
 * Builds a network from the NED file.
 * Assumes object tree has already passed all validation stages (DTD, basic, semantic).
 *
 * See also loadNedFile()
 *
 * @ingroup NetworkBuilder
 */
class cNEDNetworkBuilder
{
  protected:
    // stack of loop variables
    struct {const char *varname; int value;} loopVarStack[MAX_LOOP_NESTING];
    int loopVarSP;

    cPar::ExprElem *xelemsBuf;

  protected:
    void addChannelAttr(cChannel *chanp, ChannelAttrNode *channelattr);
    void addSubmodule(cModule *modp, SubmoduleNode *submod);
    void setDisplayString(cModule *submodp, SubmoduleNode *submod);
    void assignSubmoduleParams(cModule *submodp, NEDElement *submod);
    void setupGateVectors(cModule *submodp, NEDElement *submod);
    void readInputParams(cModule *submodp);
    cGate *getFirstUnusedParentModGate(cModule *mod, const char *gatename);
    cGate *getFirstUnusedSubmodGate(cModule *mod, const char *gatename);
    void addLoopConnection(cModule *modp, ForLoopNode *forloop);
    void doLoopVar(cModule *modp, LoopVarNode *loopvar);
    void addConnection(cModule *modp, ConnectionNode *conn);
    cGate *resolveGate(cModule *modp, const char *modname, ExpressionNode *modindex,
                       const char *gatename, ExpressionNode *gateindex, bool isplusplus);
    cChannel *createChannelForConnection(ConnectionNode *conn, cModule *parentmodp);
    ExpressionNode *findExpression(NEDElement *node, const char *exprname);

    double evaluate(cModule *modp, ExpressionNode *expr, cModule *submodp=NULL);

    cPar *resolveParamRef(ParamRefNode *node, cModule *parentmodp, cModule *submodp);

    double evaluateNode(NEDElement *node, cModule *parentmodp, cModule *submodp);
    double evalOperator(OperatorNode *node, cModule *parentmodp, cModule *submodp);
    double evalFunction(FunctionNode *node, cModule *parentmodp, cModule *submodp);
    double evalParamref(ParamRefNode *node, cModule *parentmodp, cModule *submodp);
    double evalIdent(IdentNode *node, cModule *parentmodp, cModule *submodp);
    double evalConst(ConstNode *node, cModule *parentmodp, cModule *submodp);

    void assignParamValue(cPar& p, ExpressionNode *expr, cModule *parentmodp, cModule *submodp);

    bool needsDynamicExpression(ExpressionNode *expr);
    void addXElems(NEDElement *node, cPar::ExprElem *xelems, int& pos, cModule *submodp);
    void addXElemsOperator(OperatorNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp);
    void addXElemsFunction(FunctionNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp);
    void addXElemsParamref(ParamRefNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp);
    void addXElemsIdent(IdentNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp);
    void addXElemsConst(ConstNode *node, cPar::ExprElem *xelems, int& pos, cModule *submodp);

  public:
    /**
     * Constructor.
     */
    cNEDNetworkBuilder();

    /**
     * Destructor.
     */
    ~cNEDNetworkBuilder();

    /**
     * Creates the toplevel module, based on the info in the passed NEDElement tree.
     * Invoked from cDynamicNetwork.
     */
    void setupNetwork(NetworkNode *networknode);

    /**
     * Builds submodules and internal connections, based on the info in the
     * passed NEDElement tree. Invoked from cDynamicModule.
     */
    void buildInside(cModule *module, CompoundModuleNode *modulenode);

    /**
     * Creates a channel object, based on the info in the passed NEDElement tree.
     * Invoked from cDynamicChannel.
     */
    cChannel *createChannel(const char *name, ChannelNode *channelnode);

};

#endif





