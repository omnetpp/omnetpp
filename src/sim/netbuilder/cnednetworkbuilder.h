//==========================================================================
//  CNEDNETWORKBUILDER.H - part of
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

#ifndef __CNETWORKBUILDER_H
#define __CNETWORKBUILDER_H

#include <string>
#include <map>
#include <vector>
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

    // submodule pointers. This is an optimization because cModule::submodule()
    // is slow if there are very large submodule vectors.
    typedef std::vector<cModule*> ModulePtrVector;
    typedef std::map<std::string,ModulePtrVector> SubmodMap;
    SubmodMap submodMap;

  protected:
    cModule *_submodule(cModule *parentmodp, const char *submodname, int idx=-1);
//XXX    void addChannelAttr(cChannel *chanp, ChannelAttrNode *channelattr);
    cModuleType *findAndCheckModuleType(const char *modtypename, cModule *modp, const char *submodname);
    void addSubmodule(cModule *modp, SubmoduleNode *submod);
    void setDisplayString(cModule *submodp, SubmoduleNode *submod);
    void setConnDisplayString(cGate *srcgatep, ConnectionNode *conn);
    void setBackgroundDisplayString(cModule *modp, CompoundModuleNode *mod);
    void assignSubmoduleParams(cModule *submodp, NEDElement *submod);
    void setupGateVectors(cModule *submodp, NEDElement *submod);
    cGate *getFirstUnusedParentModGate(cModule *mod, const char *gatename);
    cGate *getFirstUnusedSubmodGate(cModule *mod, const char *gatename);
//XXX    void addLoopConnection(cModule *modp, ForLoopNode *forloop);
//XXX    void doLoopVar(cModule *modp, LoopVarNode *loopvar);
    void addConnection(cModule *modp, ConnectionNode *conn);
    cGate *resolveGate(cModule *modp, const char *modname, ExpressionNode *modindex,
                       const char *gatename, ExpressionNode *gateindex, bool isplusplus);
    cChannel *createChannelForConnection(ConnectionNode *conn, cModule *parentmodp);
    ExpressionNode *findExpression(NEDElement *node, const char *exprname);


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
//XXX    void setupNetwork(NetworkNode *networknode);

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





