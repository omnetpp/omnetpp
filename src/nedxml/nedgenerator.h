//==========================================================================
//  NEDGENERATOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDGENERATOR_H
#define __NEDGENERATOR_H

#include <iostream>
#include "nedelements.h"

using std::ostream;

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
    ostream& out;

  public:
    /**
     * Constructor. Takes an output stream where the generated NED code
     * will be written.
     */
    NEDGenerator(ostream& out);

    /**
     * Destructor.
     */
    ~NEDGenerator();

    /**
     * Sets the indent size in the generated NED code. Default is 4 spaces.
     */
    void setIndentSize(int indentsize);

    /**
     * Sets if new NED syntax (e.g. using curly braces like module { ... }
     * instead of module ... endmodule) should be used. Default is true.
     */
    void setNewSyntax(bool value);

    /**
     * Generates NED code. Takes object tree and base indentation.
     */
    void generate(NEDElement *node, const char *indent);

  protected:
    /** @name Change indentation level */
    //@{
    const char *increaseIndent(const char *indent);
    const char *decreaseIndent(const char *indent);
    //@}

    /** Dispatch to various doXXX() methods according to node type */
    void generateNedItem(NEDElement *node, const char *indent, bool islast, const char *arg=NULL);;

    /** Invoke generateNedItem() with all children */
    void generateChildren(NEDElement *node, const char *indent, const char *arg=NULL);

    /** Invoke generateNedItem() with all children of the given tagcode */
    void generateChildrenWithType(NEDElement *node, int tagcode, const char *indent, const char *arg=0);

    /** @name Utilities */
    //@{
    void printExpression(NEDElement *node, const char *attr, const char *indent);
    void printVector(NEDElement *node, const char *attr, const char *indent);
    void printIfExpression(NEDElement *node, const char *attr, const char *indent);
    //@}

    /** @name Format and write comments to the output */
    //@{
    void appendBannerComment(const char *comment, const char *indent);
    void appendRightComment(const char *comment, const char *indent);
    void appendInlineRightComment(const char *comment, const char *indent);
    void appendTrailingComment(const char *comment, const char *indent);
    //@}

    /** @name Generate NED code from the given element */
    //@{
    void doNedfiles(NedFilesNode *node, const char *indent, bool islast, const char *);
    void doNedfile(NedFileNode *node, const char *indent, bool islast, const char *);
    void doImports(ImportNode *node, const char *indent, bool islast, const char *);
    void doImport(ImportedFileNode *node, const char *indent, bool islast, const char *);
    void doChannel(ChannelNode *node, const char *indent, bool islast, const char *);
    void doChanattr(ChannelAttrNode *node, const char *indent, bool islast, const char *);
    void doNetwork(NetworkNode *node, const char *indent, bool islast, const char *);
    void doSimple(SimpleModuleNode *node, const char *indent, bool islast, const char *);
    void doModule(CompoundModuleNode *node, const char *indent, bool islast, const char *);
    void doParams(ParamsNode *node, const char *indent, bool islast, const char *);
    void doParam(ParamNode *node, const char *indent, bool islast, const char *);
    void doGates(GatesNode *node, const char *indent, bool islast, const char *);
    void doGate(GateNode *node, const char *indent, bool islast, const char *);
    void doMachines(MachinesNode *node, const char *indent, bool islast, const char *);
    void doMachine(MachineNode *node, const char *indent, bool islast, const char *);
    void doSubmodules(SubmodulesNode *node, const char *indent, bool islast, const char *);
    void doSubmodule(SubmoduleNode *node, const char *indent, bool islast, const char *);
    void doSubstparams(SubstparamsNode *node, const char *indent, bool islast, const char *);
    void doSubstparam(SubstparamNode *node, const char *indent, bool islast, const char *);
    void doGatesizes(GatesizesNode *node, const char *indent, bool islast, const char *);
    void doGatesize(GatesizeNode *node, const char *indent, bool islast, const char *);
    void doSubstmachines(SubstmachinesNode *node, const char *indent, bool islast, const char *);
    void doSubstmachine(SubstmachineNode *node, const char *indent, bool islast, const char *);
    void doConnections(ConnectionsNode *node, const char *indent, bool islast, const char *);
    void printGate(NEDElement *conn, const char *modname, const char *modindexattr,
                   const char *gatename, const char *gateindexattr, bool isplusplus,
                   const char *indent);
    void doConnection(ConnectionNode *node, const char *indent, bool islast, const char *);
    void doConnattr(ConnAttrNode *node, const char *indent, bool islast, const char *arrow);
    void doForloop(ForLoopNode *node, const char *indent, bool islast, const char *);
    void doLoopvar(LoopVarNode *node, const char *indent, bool islast, const char *);
    void doDisplaystring(DisplayStringNode *node, const char *indent, bool islast, const char *);
    void doExpression(ExpressionNode *node, const char *indent, bool islast, const char *);
    int getOperatorPriority(const char *op, int args);
    bool isOperatorLeftAssoc(const char *op);
    void doOperator(OperatorNode *node, const char *indent, bool islast, const char *);
    void doFunction(FunctionNode *node, const char *indent, bool islast, const char *);
    void doParamref(ParamRefNode *node, const char *indent, bool islast, const char *);
    void doIdent(IdentNode *node, const char *indent, bool islast, const char *);
    void doConst(ConstNode *node, const char *indent, bool islast, const char *);

    void doCplusplus(CplusplusNode *node, const char *indent, bool islast, const char *);
    void doStructDecl(StructDeclNode *node, const char *indent, bool islast, const char *);
    void doClassDecl(ClassDeclNode *node, const char *indent, bool islast, const char *);
    void doMessageDecl(MessageDeclNode *node, const char *indent, bool islast, const char *);
    void doEnumDecl(EnumDeclNode *node, const char *indent, bool islast, const char *);
    void doEnum(EnumNode *node, const char *indent, bool islast, const char *);
    void doEnumFields(EnumFieldsNode *node, const char *indent, bool islast, const char *);
    void doEnumField(EnumFieldNode *node, const char *indent, bool islast, const char *);
    void doMessage(MessageNode *node, const char *indent, bool islast, const char *);
    void doClass(ClassNode *node, const char *indent, bool islast, const char *);
    void doStruct(StructNode *node, const char *indent, bool islast, const char *);
    void doFields(FieldsNode *node, const char *indent, bool islast, const char *);
    void doField(FieldNode *node, const char *indent, bool islast, const char *);
    void doProperties(PropertiesNode *node, const char *indent, bool islast, const char *);
    void doProperty(PropertyNode *node, const char *indent, bool islast, const char *);
    //@}
};

#endif


