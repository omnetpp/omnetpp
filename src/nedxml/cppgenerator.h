//==========================================================================
//   CPPGENERATOR.H -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPPGENERATOR_H
#define __CPPGENERATOR_H

#include <string>
#include <map>
#include <vector>
#include "iostream.h"
#include "nedelements.h"
#include "cppexprgenerator.h"

/**
 * Simple front-end to NEDCppGenerator.
 *
 * @ingroup CppGenerator
 */
void generateCpp(ostream& out, NEDElement *node);


/**
 * Nedc functionality: generates C++ code from a NED object tree.
 * Assumes object tree has already passed all validation stages (DTD, basic, semantic).
 *
 * Uses CppExpressionGenerator.
 *
 * @ingroup CppGenerator
 */
class NEDCppGenerator
{
  protected:
    bool in_network;
    std::string module_name;
    std::string submodule_name;
    std::string submodule_var;
    int indentsize;

    enum {
      MODE_NORMAL,
      MODE_CLEANUP,
    };

    CppExpressionGenerator exprgen;

  public:
    NEDCppGenerator();
    ~NEDCppGenerator();
    void setIndentSize(int indentsize);
    void generate(ostream& out, NEDElement *node);

  protected:
    const char *increaseIndent(const char *indent);
    const char *decreaseIndent(const char *indent);

    void generateItem(ostream& out, NEDElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void generateChildren(ostream& out, NEDElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void generateChildrenWithTags(ostream& out, NEDElement *node, const char *tags, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void generateChildrenExceptTags(ostream& out, NEDElement *node, const char *tags, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);

    void writeProlog(ostream& out);
    void printTemporaryVariables(ostream& out, const char *indent);
    void beginConditionalBlock(ostream& out, NEDElement *node, const char *&indent, int mode, const char *);
    void endConditionalBlock(ostream& out, NEDElement *node, const char *&indent, int mode, const char *);

    void doNedFile(ostream& out, NedFileNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doNedFiles(ostream& out, NedFilesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doImports(ostream& out, ImportNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doImport(ostream& out, ImportedFileNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doChannel(ostream& out, ChannelNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doChannelAttr(ostream& out, ChannelAttrNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doNetwork(ostream& out, NetworkNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSimple(ostream& out, SimpleModuleNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doModule(ostream& out, CompoundModuleNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doParams(ostream& out, ParamsNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doParam(ostream& out, ParamNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGates(ostream& out, GatesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGate(ostream& out, GateNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doMachines(ostream& out, MachinesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doMachine(ostream& out, MachineNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubmodules(ostream& out, SubmodulesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubmodule(ostream& out, SubmoduleNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubmoduleCleanup(ostream& out, SubmoduleNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstparams(ostream& out, SubstparamsNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstparam(ostream& out, SubstparamNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGatesizes(ostream& out, GatesizesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGatesize(ostream& out, GatesizeNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstmachines(ostream& out, SubstmachinesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstmachine(ostream& out, SubstmachineNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doConnections(ostream& out, ConnectionsNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doConnection(ostream& out, ConnectionNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doConnattr(ostream& out, ConnAttrNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doForloop(ostream& out, ForLoopNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doLoopvar(ostream& out, LoopVarNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doDisplayString(ostream& out, DisplayStringNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doExpression(ostream& out, ExpressionNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);

    ExpressionNode *findExpression(NEDElement *parent, const char *target);
    void resolveGate(ostream& out, const char *var, const char *indent, const char *modname, ExpressionNode *modindex, const char *gatename, ExpressionNode *gateindex);
};

#endif


