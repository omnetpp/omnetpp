//==========================================================================
//  NEDGENERATOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDGENERATOR_H
#define __NEDGENERATOR_H

#include <iostream>
#include "nedelements.h"
#include "nederror.h"

NAMESPACE_BEGIN


/**
 * Simple front-end to NED2Generator.
 *
 * @ingroup NED2Generator
 */
NEDXML_API void generateNED2(std::ostream& out, NEDElement *node, NEDErrorStore *e);

/**
 * Generates NED code from a NED object tree.
 * Assumes object tree has already passed all validation stages (DTD, syntax, semantic).
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
    bool isEmptyChannelSpec(ChannelSpecElement *node);
    void doMsgClassOrStructBody(NEDElement *msgclassorstruct, const char *indent);
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
    void doFiles(FilesElement *node, const char *indent, bool islast, const char *);
    void doNedFile(NedFileElement *node, const char *indent, bool islast, const char *);
    void doPackage(PackageElement *node, const char *indent, bool islast, const char *);
    void doImport(ImportElement *node, const char *indent, bool islast, const char *);
    void doPropertyDecl(PropertyDeclElement *node, const char *indent, bool islast, const char *);
    void doExtends(ExtendsElement *node, const char *indent, bool islast, const char *);
    void doInterfaceName(InterfaceNameElement *node, const char *indent, bool islast, const char *);
    void doSimpleModule(SimpleModuleElement *node, const char *indent, bool islast, const char *);
    void doModuleInterface(ModuleInterfaceElement *node, const char *indent, bool islast, const char *);
    void doCompoundModule(CompoundModuleElement *node, const char *indent, bool islast, const char *);
    void doChannelInterface(ChannelInterfaceElement *node, const char *indent, bool islast, const char *);
    void doChannel(ChannelElement *node, const char *indent, bool islast, const char *);
    void doParameters(ParametersElement *node, const char *indent, bool islast, const char *);
    void doParam(ParamElement *node, const char *indent, bool islast, const char *);
    void doPattern(PatternElement *node, const char *indent, bool islast, const char *);
    void doProperty(PropertyElement *node, const char *indent, bool islast, const char *);
    void doPropertyKey(PropertyKeyElement *node, const char *indent, bool islast, const char *);
    void doGates(GatesElement *node, const char *indent, bool islast, const char *);
    void doGate(GateElement *node, const char *indent, bool islast, const char *);
    void doTypes(TypesElement *node, const char *indent, bool islast, const char *);
    void doSubmodules(SubmodulesElement *node, const char *indent, bool islast, const char *);
    void doSubmodule(SubmoduleElement *node, const char *indent, bool islast, const char *);
    void doConnections(ConnectionsElement *node, const char *indent, bool islast, const char *);
    void doConnection(ConnectionElement *node, const char *indent, bool islast, const char *);
    void doChannelSpec(ChannelSpecElement *node, const char *indent, bool islast, const char *);
    void doConnectionGroup(ConnectionGroupElement *node, const char *indent, bool islast, const char *);
    void doLoop(LoopElement *node, const char *indent, bool islast, const char *);
    void doCondition(ConditionElement *node, const char *indent, bool islast, const char *);
    void printConnectionGate(NEDElement *conn, const char *modname, const char *modindexattr,
                             const char *gatename, const char *gateindexattr, bool isplusplus,
                             int gatesubg, const char *indent);

    // expressions
    void doExpression(ExpressionElement *node, const char *indent, bool islast, const char *);
    int getOperatorPrecedence(const char *op, int args);
    bool isOperatorLeftAssoc(const char *op);
    void doOperator(OperatorElement *node, const char *indent, bool islast, const char *);
    void doFunction(FunctionElement *node, const char *indent, bool islast, const char *);
    void doIdent(IdentElement *node, const char *indent, bool islast, const char *);
    void doLiteral(LiteralElement *node, const char *indent, bool islast, const char *);

    // msg stuff
    void doMsgFile(MsgFileElement *node, const char *indent, bool islast, const char *);
    void doNamespace(NamespaceElement *node, const char *indent, bool islast, const char *);
    void doCplusplus(CplusplusElement *node, const char *indent, bool islast, const char *);
    void doStructDecl(StructDeclElement *node, const char *indent, bool islast, const char *);
    void doClassDecl(ClassDeclElement *node, const char *indent, bool islast, const char *);
    void doMessageDecl(MessageDeclElement *node, const char *indent, bool islast, const char *);
    void doPacketDecl(PacketDeclElement *node, const char *indent, bool islast, const char *);
    void doEnumDecl(EnumDeclElement *node, const char *indent, bool islast, const char *);
    void doEnum(EnumElement *node, const char *indent, bool islast, const char *);
    void doEnumFields(EnumFieldsElement *node, const char *indent, bool islast, const char *);
    void doEnumField(EnumFieldElement *node, const char *indent, bool islast, const char *);
    void doMessage(MessageElement *node, const char *indent, bool islast, const char *);
    void doPacket(PacketElement *node, const char *indent, bool islast, const char *);
    void doClass(ClassElement *node, const char *indent, bool islast, const char *);
    void doStruct(StructElement *node, const char *indent, bool islast, const char *);
    void doField(FieldElement *node, const char *indent, bool islast, const char *);
    void doComment(CommentElement *node, const char *indent, bool islast, const char *);
    //@}
};

NAMESPACE_END


#endif


