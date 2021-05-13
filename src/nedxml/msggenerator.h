//==========================================================================
//  MSGGENERATOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDXML_MSGGENERATOR_H
#define __OMNETPP_NEDXML_MSGGENERATOR_H

#include <iostream>

#include "errorstore.h"
#include "msgelements.h"

namespace omnetpp {
namespace nedxml {


/**
 * @brief Simple front-end to MsgGenerator.
 *
 * @ingroup MsgGenerator
 */
NEDXML_API void generateMsg(std::ostream& out, ASTNode *node);

/**
 * @brief Generates MSG source from an AST.
 *
 * Assumes that the object tree has already passed all validation stages (DTD,
 * syntax, semantic).
 *
 * @ingroup MsgGenerator
 */
class NEDXML_API MsgGenerator
{
  protected:
    int indentSize = 4;
    std::ostream *outp = nullptr;

  public:
    /**
     * Constructor.
     */
    MsgGenerator() {}

    /**
     * Destructor.
     */
    ~MsgGenerator() {}

    /**
     * Sets the indent size in the generated MSG source code. Default is 4 spaces.
     */
    void setIndentSize(int indentsize);

    /**
     * Generates MSG source code. Takes an output stream where the generated code
     * will be written, the object tree and the base indentation.
     */
    void generate(std::ostream& out, ASTNode *node, const char *indent);

    /**
     * Generates MSG source code and returns it as a string.
     */
    std::string generate(ASTNode *node, const char *indent);

  protected:
    /** @name Change indentation level */
    //@{
    const char *increaseIndent(const char *indent);
    const char *decreaseIndent(const char *indent);
    //@}

    /** Dispatch to various doXXX() methods according to node type */
    void generateItem(ASTNode *node, const char *indent, bool islast, const char *arg=nullptr);;

    /** Invoke generateItem() on all children */
    void generateChildren(ASTNode *node, const char *indent, const char *arg=nullptr);

    /** Invoke generateItem() on all children of the given tagcode */
    void generateChildrenWithType(ASTNode *node, int tagcode, const char *indent, const char *arg=nullptr);

    /** Invoke generateItem() on children of the given tagcodes (NED_NULL-terminated array) */
    void generateChildrenWithTypes(ASTNode *node, int tagcodes[], const char *indent, const char *arg=nullptr);

    void doMsgClassOrStructBody(ASTNode *msgclassorstruct, const char *indent);

    /** @name Getters for comments */
    //@{
    std::string concatInnerComments(ASTNode *node);
    std::string getBannerComment(ASTNode *node, const char *indent);
    std::string getRightComment(ASTNode *node);
    std::string getInlineRightComment(ASTNode *node);
    std::string getTrailingComment(ASTNode *node);
    //@}

    /** @name Generate MSG source code from the given element */
    //@{
    void doFiles(FilesElement *node, const char *indent, bool islast, const char *);
    void doMsgFile(MsgFileElement *node, const char *indent, bool islast, const char *);
    void doImport(ImportElement *node, const char *indent, bool islast, const char *);
    void doProperty(PropertyElement *node, const char *indent, bool islast, const char *sep);
    void doPropertyKey(PropertyKeyElement *node, const char *indent, bool islast, const char *sep);
    void doLiteral(LiteralElement *node, const char *indent, bool islast, const char *);
    void doNamespace(NamespaceElement *node, const char *indent, bool islast, const char *);
    void doCplusplus(CplusplusElement *node, const char *indent, bool islast, const char *);
    void doStructDecl(StructDeclElement *node, const char *indent, bool islast, const char *);
    void doClassDecl(ClassDeclElement *node, const char *indent, bool islast, const char *);
    void doMessageDecl(MessageDeclElement *node, const char *indent, bool islast, const char *);
    void doPacketDecl(PacketDeclElement *node, const char *indent, bool islast, const char *);
    void doEnumDecl(EnumDeclElement *node, const char *indent, bool islast, const char *);
    void doEnum(EnumElement *node, const char *indent, bool islast, const char *);
    void doEnumField(EnumFieldElement *node, const char *indent, bool islast, const char *);
    void doMessage(MessageElement *node, const char *indent, bool islast, const char *);
    void doPacket(PacketElement *node, const char *indent, bool islast, const char *);
    void doClass(ClassElement *node, const char *indent, bool islast, const char *);
    void doStruct(StructElement *node, const char *indent, bool islast, const char *);
    void doField(FieldElement *node, const char *indent, bool islast, const char *);
    void doComment(CommentElement *node, const char *indent, bool islast, const char *);
    //@}
};

}  // namespace nedxml
}  // namespace omnetpp


#endif


