//==========================================================================
//   NEDGEN.H -
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

#ifndef __NEDGENERATOR_H
#define __NEDGENERATOR_H

#include "iostream.h"
#include "nedelements.h"

/**
 * Simple front-end to NEDGenerator.
 *
 * @ingroup NEDGenerator
 */
void generateNed(ostream& out, NEDElement *node, bool newsyntax);

/**
 * Generates NED code from a NED object tree.
 * Assumes object tree has already passed all validation stages (DTD, basic, semantic).
 *
 * @ingroup NEDGenerator
 */
class NEDGenerator
{
  protected:
    bool newsyntax;
    int indentsize;

  public:
    NEDGenerator();
    ~NEDGenerator();
    void setIndentSize(int indentsize);
    void setNewSyntax(bool value);
    void generate(ostream& out, NEDElement *node, const char *indent);

  protected:
    const char *increaseIndent(const char *indent);
    const char *decreaseIndent(const char *indent);
    void generateNedItem(ostream& out, NEDElement *node, const char *indent, bool islast, const char *arg=NULL);;
    void generateChildren(ostream& out, NEDElement *node, const char *indent, const char *arg=NULL);
    void generateChildrenWithType(ostream& out, NEDElement *node, int tagcode, const char *indent, const char *arg=0);
    void printExpression(ostream& out, NEDElement *node, const char *attr, const char *indent);
    void printVector(ostream& out, NEDElement *node, const char *attr, const char *indent);
    void printIfExpression(ostream& out, NEDElement *node, const char *attr, const char *indent);
    void appendBannerComment(ostream& out, const char *comment, const char *indent);
    void appendRightComment(ostream& out, const char *comment, const char *indent);
    void appendInlineRightComment(ostream& out, const char *comment, const char *indent);
    void appendTrailingComment(ostream& out, const char *comment, const char *indent);
    void doNedfiles(ostream& out, NedFilesNode *node, const char *indent, bool islast, const char *);
    void doNedfile(ostream& out, NedFileNode *node, const char *indent, bool islast, const char *);
    void doImports(ostream& out, ImportNode *node, const char *indent, bool islast, const char *);
    void doImport(ostream& out, ImportedFileNode *node, const char *indent, bool islast, const char *);
    void doChannel(ostream& out, ChannelNode *node, const char *indent, bool islast, const char *);
    void doChanattr(ostream& out, ChannelAttrNode *node, const char *indent, bool islast, const char *);
    void doNetwork(ostream& out, NetworkNode *node, const char *indent, bool islast, const char *);
    void doSimple(ostream& out, SimpleModuleNode *node, const char *indent, bool islast, const char *);
    void doModule(ostream& out, CompoundModuleNode *node, const char *indent, bool islast, const char *);
    void doParams(ostream& out, ParamsNode *node, const char *indent, bool islast, const char *);
    void doParam(ostream& out, ParamNode *node, const char *indent, bool islast, const char *);
    void doGates(ostream& out, GatesNode *node, const char *indent, bool islast, const char *);
    void doGate(ostream& out, GateNode *node, const char *indent, bool islast, const char *);
    void doMachines(ostream& out, MachinesNode *node, const char *indent, bool islast, const char *);
    void doMachine(ostream& out, MachineNode *node, const char *indent, bool islast, const char *);
    void doSubmodules(ostream& out, SubmodulesNode *node, const char *indent, bool islast, const char *);
    void doSubmodule(ostream& out, SubmoduleNode *node, const char *indent, bool islast, const char *);
    void doSubstparams(ostream& out, SubstparamsNode *node, const char *indent, bool islast, const char *);
    void doSubstparam(ostream& out, SubstparamNode *node, const char *indent, bool islast, const char *);
    void doGatesizes(ostream& out, GatesizesNode *node, const char *indent, bool islast, const char *);
    void doGatesize(ostream& out, GatesizeNode *node, const char *indent, bool islast, const char *);
    void doSubstmachines(ostream& out, SubstmachinesNode *node, const char *indent, bool islast, const char *);
    void doSubstmachine(ostream& out, SubstmachineNode *node, const char *indent, bool islast, const char *);
    void doConnections(ostream& out, ConnectionsNode *node, const char *indent, bool islast, const char *);
    void printGate(ostream& out, NEDElement *conn, const char *modname, const char *modindexattr,
                   const char *gatename, const char *gateindexattr, const char *indent);
    void doConnection(ostream& out, ConnectionNode *node, const char *indent, bool islast, const char *);
    void doConnattr(ostream& out, ConnAttrNode *node, const char *indent, bool islast, const char *arrow);
    void doForloop(ostream& out, ForLoopNode *node, const char *indent, bool islast, const char *);
    void doLoopvar(ostream& out, LoopVarNode *node, const char *indent, bool islast, const char *);
    void doDisplaystring(ostream& out, DisplayStringNode *node, const char *indent, bool islast, const char *);
    void doExpression(ostream& out, ExpressionNode *node, const char *indent, bool islast, const char *);
    int getOperatorPriority(const char *op, int args);
    bool isOperatorLeftAssoc(const char *op);
    void doOperator(ostream& out, OperatorNode *node, const char *indent, bool islast, const char *);
    void doFunction(ostream& out, FunctionNode *node, const char *indent, bool islast, const char *);
    void doParamref(ostream& out, ParamRefNode *node, const char *indent, bool islast, const char *);
    void doIdent(ostream& out, IdentNode *node, const char *indent, bool islast, const char *);
    void doConst(ostream& out, ConstNode *node, const char *indent, bool islast, const char *);
};

#endif


