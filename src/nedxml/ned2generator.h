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
#include "nederror.h"


/**
 * Simple front-end to NED2Generator.
 *
 * @ingroup NED2Generator
 */
NEDXML_API void generateNED2(std::ostream& out, NEDElement *node, NEDErrorStore *e);

/**
 * Generates NED code from a NED object tree.
 * Assumes object tree has already passed all validation stages (DTD, basic, semantic).
 *
 * @ingroup NED2Generator
 */
class NEDXML_API NED2Generator
{
  protected:
    int indentsize;
    std::ostream *outp;
    NEDErrorStore *errors;

  public:
    /**
     * Constructor.
     */
    NED2Generator(NEDErrorStore *errors);

    /**
     * Destructor.
     */
    ~NED2Generator();

    /**
     * Sets the indent size in the generated NED code. Default is 4 spaces.
     */
    void setIndentSize(int indentsize);

    /**
     * Generates NED code. Takes an output stream where the generated NED code
     * will be written, the object tree and the base indentation.
     */
    void generate(std::ostream& out, NEDElement *node, const char *indent);

    /**
     * Generates NED code and returns it as a string.
     */
    std::string generate(NEDElement *node, const char *indent);

  protected:
    /** @name Change indentation level */
    //@{
    const char *increaseIndent(const char *indent);
    const char *decreaseIndent(const char *indent);
    //@}

    /** Dispatch to various doXXX() methods according to node type */
    void generateNedItem(NEDElement *node, const char *indent, bool islast, const char *arg=NULL);;

    /** Invoke generateNedItem() on all children */
    void generateChildren(NEDElement *node, const char *indent, const char *arg=NULL);

    /** Invoke generateNedItem() on all children of the given tagcode */
    void generateChildrenWithType(NEDElement *node, int tagcode, const char *indent, const char *arg=0);

    /** Invoke generateNedItem() on children of the given tagcodes (NED_NULL-terminated array) */
    void generateChildrenWithTypes(NEDElement *node, int tagcodes[], const char *indent, const char *arg=0);

    /** @name Utilities */
    //@{
    void printInheritance(NEDElement *node, const char *indent);
    bool hasExpression(NEDElement *node, const char *attr);
    void printExpression(NEDElement *node, const char *attr, const char *indent);
    void printOptVector(NEDElement *node, const char *attr, const char *indent);
    bool isEmptyChannelSpec(ChannelSpecNode *node);
    //@}

    /** @name Getters for comments */
    //@{
    std::string concatInnerComments(NEDElement *node);
    std::string getBannerComment(NEDElement *node, const char *indent);
    std::string getRightComment(NEDElement *node);
    std::string getInlineRightComment(NEDElement *node);
    std::string getTrailingComment(NEDElement *node);
    //@}

    /** @name Generate NED code from the given element */
    //@{
    void doFiles(FilesNode *node, const char *indent, bool islast, const char *);
    void doNedFile(NedFileNode *node, const char *indent, bool islast, const char *);
    void doImport(ImportNode *node, const char *indent, bool islast, const char *);
    void doPropertyDecl(PropertyDeclNode *node, const char *indent, bool islast, const char *);
    void doExtends(ExtendsNode *node, const char *indent, bool islast, const char *);
    void doInterfaceName(InterfaceNameNode *node, const char *indent, bool islast, const char *);
    void doSimpleModule(SimpleModuleNode *node, const char *indent, bool islast, const char *);
    void doModuleInterface(ModuleInterfaceNode *node, const char *indent, bool islast, const char *);
    void doCompoundModule(CompoundModuleNode *node, const char *indent, bool islast, const char *);
    void doChannelInterface(ChannelInterfaceNode *node, const char *indent, bool islast, const char *);
    void doChannel(ChannelNode *node, const char *indent, bool islast, const char *);
    void doParameters(ParametersNode *node, const char *indent, bool islast, const char *);
    void doParam(ParamNode *node, const char *indent, bool islast, const char *);
    void doPattern(PatternNode *node, const char *indent, bool islast, const char *);
    void doProperty(PropertyNode *node, const char *indent, bool islast, const char *);
    void doPropertyKey(PropertyKeyNode *node, const char *indent, bool islast, const char *);
    void doGates(GatesNode *node, const char *indent, bool islast, const char *);
    void doGate(GateNode *node, const char *indent, bool islast, const char *);
    void doTypes(TypesNode *node, const char *indent, bool islast, const char *);
    void doSubmodules(SubmodulesNode *node, const char *indent, bool islast, const char *);
    void doSubmodule(SubmoduleNode *node, const char *indent, bool islast, const char *);
    void doConnections(ConnectionsNode *node, const char *indent, bool islast, const char *);
    void doConnection(ConnectionNode *node, const char *indent, bool islast, const char *);
    void doChannelSpec(ChannelSpecNode *node, const char *indent, bool islast, const char *);
    void doConnectionGroup(ConnectionGroupNode *node, const char *indent, bool islast, const char *);
    void doLoop(LoopNode *node, const char *indent, bool islast, const char *);
    void doCondition(ConditionNode *node, const char *indent, bool islast, const char *);
    void printConnectionGate(NEDElement *conn, const char *modname, const char *modindexattr,
                             const char *gatename, const char *gateindexattr, bool isplusplus,
                             int gatesubg, const char *indent);

    // expressions
    void doExpression(ExpressionNode *node, const char *indent, bool islast, const char *);
    int getOperatorPriority(const char *op, int args);
    bool isOperatorLeftAssoc(const char *op);
    void doOperator(OperatorNode *node, const char *indent, bool islast, const char *);
    void doFunction(FunctionNode *node, const char *indent, bool islast, const char *);
    void doIdent(IdentNode *node, const char *indent, bool islast, const char *);
    void doLiteral(LiteralNode *node, const char *indent, bool islast, const char *);

    // msg stuff
    void doMsgFile(MsgFileNode *node, const char *indent, bool islast, const char *);
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
    void doMsgproperty(MsgpropertyNode *node, const char *indent, bool islast, const char *);
    void doComment(CommentNode *node, const char *indent, bool islast, const char *);
    //@}
};

#endif


