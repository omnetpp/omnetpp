//==========================================================================
//  MSGGENERATOR.CC - part of
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

#include <cstring>
#include <string>
#include <sstream>
#include "common/stringutil.h"
#include "msggenerator.h"
#include "errorstore.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

using std::ostream;

#define DEFAULTINDENT    "            "

void generateMsg(ostream& out, ASTNode *node)
{
    MsgGenerator gen;
    gen.generate(out, node, "");
}

//-----------------------------------------

#define OUT    (*outp)

void MsgGenerator::setIndentSize(int indentsiz)
{
    indentSize = indentsiz;
}

void MsgGenerator::generate(ostream& out, ASTNode *node, const char *indent)
{
    outp = &out;
    generateItem(node, indent, false);
    outp = nullptr;
}

std::string MsgGenerator::generate(ASTNode *node, const char *indent)
{
    std::stringstream ss;
    generate(ss, node, indent);
    return ss.str();
}

const char *MsgGenerator::increaseIndent(const char *indent)
{
    // biggest possible indent: ~70 chars:
    static char spaces[] = "                                                                     ";

    // bump...
    int ilen = strlen(indent);
    if (ilen+indentSize <= (int)sizeof(spaces)-1)
        ilen += indentSize;
    const char *newindent = spaces+sizeof(spaces)-1-ilen;
    return newindent;
}

const char *MsgGenerator::decreaseIndent(const char *indent)
{
    return indent + indentSize;
}

//---------------------------------------------------------------------------

void MsgGenerator::generateChildren(ASTNode *node, const char *indent, const char *arg)
{
    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling())
        generateItem(child, indent, child == node->getLastChild(), arg);
}

void MsgGenerator::generateChildrenWithType(ASTNode *node, int tagcode, const char *indent, const char *arg)
{
    // find last
    ASTNode *lastWithTag = nullptr;
    for (ASTNode *child1 = node->getFirstChild(); child1; child1 = child1->getNextSibling())
        if (child1->getTagCode() == tagcode)
            lastWithTag = child1;


    // now the recursive call
    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling())
        if (child->getTagCode() == tagcode)
            generateItem(child, indent, child == lastWithTag, arg);

}

static int isInVector(int a, int v[])
{
    for (int i = 0; v[i]; i++)  // v[] is zero-terminated
        if (v[i] == a)
            return true;

    return false;
}

void MsgGenerator::generateChildrenWithTypes(ASTNode *node, int tagcodes[], const char *indent, const char *arg)
{
    // find last
    ASTNode *lastWithTag = nullptr;
    for (ASTNode *child1 = node->getFirstChild(); child1; child1 = child1->getNextSibling())
        if (isInVector(child1->getTagCode(), tagcodes))
            lastWithTag = child1;


    // now the recursive call
    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling())
        if (isInVector(child->getTagCode(), tagcodes))
            generateItem(child, indent, child == lastWithTag, arg);

}

//---------------------------------------------------------------------------

static const char *getComment(ASTNode *node, const char *locId)
{
    CommentElement *comment = (CommentElement *)node->getFirstChildWithAttribute(MSG_COMMENT, "locid", locId);
    return (comment && !opp_isempty(comment->getContent())) ? comment->getContent() : nullptr;
}

static std::string formatComment(const char *comment, const char *indent, const char *defaultValue)
{
    if (!comment || !comment[0])
        return defaultValue;

    // indent each line of comment; also ensure that if last line contains
    // a comment (//), it gets terminated by newline. If indent==nullptr,
    // keep original indent
    std::string ret;
    const char *curLine = comment;
    while (curLine[0]) {
        const char *nextLine = strchr(curLine, '\n');
        if (!nextLine)
            nextLine = curLine + strlen(curLine);
        const char *commentStart = strstr(curLine, "//");
        if (!commentStart || commentStart >= nextLine)
            ret += "\n";  // no comment in that line -- just add newline
        else if (!indent)
            ret += std::string(curLine, nextLine) + "\n";  // use original indent
        else
            ret += indent + std::string(commentStart, nextLine) + "\n";  // indent the comment

        curLine = nextLine[0] ? nextLine+1 : nextLine;  // +1: skip newline
    }
    return ret;
}

std::string MsgGenerator::concatInnerComments(ASTNode *node)
{
    std::string ret;
    for (ASTNode *child = node->getFirstChildWithTag(MSG_COMMENT); child; child = child->getNextSiblingWithTag(MSG_COMMENT)) {
        CommentElement *comment = (CommentElement *)child;
        if (!strcmp(comment->getLocid(), "inner"))
            ret += comment->getContent();
    }
    return ret;
}

std::string MsgGenerator::getBannerComment(ASTNode *node, const char *indent)
{
    const char *comment = getComment(node, "banner");
    std::string innerComments = concatInnerComments(node);
    return formatComment(comment, indent, "") + formatComment(innerComments.c_str(), indent, "");
}

std::string MsgGenerator::getRightComment(ASTNode *node)
{
    const char *comment = getComment(node, "right");
    return formatComment(comment, nullptr, "\n");
}

std::string MsgGenerator::getInlineRightComment(ASTNode *node)
{
    const char *comment = getComment(node, "right");
    return formatComment(comment, nullptr, " ");
}

std::string MsgGenerator::getTrailingComment(ASTNode *node)
{
    const char *comment = getComment(node, "trailing");
    return formatComment(comment, nullptr, "\n");
}

//---------------------------------------------------------------------------

void MsgGenerator::doFiles(FilesElement *node, const char *indent, bool, const char *)
{
    generateChildren(node, indent);
}

void MsgGenerator::doImport(ImportElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "import " << node->getImportSpec() << ";" << getRightComment(node);
}

void MsgGenerator::doProperty(PropertyElement *node, const char *indent, bool islast, const char *sep)
{
    if (!node->getIsImplicit()) {
        // if sep==nullptr, print as standalone property (with indent and ";"), otherwise as inline property
        OUT << getBannerComment(node, indent);
        if (!sep && indent)
            OUT << indent;
        if (sep)
            OUT << " ";
        OUT << "@" << node->getName();
        if (!opp_isempty(node->getIndex()))
            OUT << "[" << node->getIndex() << "]";
        const char *subindent = indent ? increaseIndent(indent) : DEFAULTINDENT;
        if (node->getFirstChildWithTag(MSG_PROPERTY_KEY)) {
            OUT << "(";
            generateChildrenWithType(node, MSG_PROPERTY_KEY, subindent, "; ");
            OUT << ")";
        }
        if (!sep && !indent)
            OUT << ";";
        else if (!sep)
            OUT << ";" << getRightComment(node);
    }
}

void MsgGenerator::doPropertyKey(PropertyKeyElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << node->getName();
    if (node->getFirstChildWithTag(MSG_LITERAL)) {
        if (!opp_isempty(node->getName()))
            OUT << "=";
        generateChildrenWithType(node, MSG_LITERAL, increaseIndent(indent), ",");
    }
    if (!islast && sep)
        OUT << (sep ? sep : "");
}

void MsgGenerator::doLiteral(LiteralElement *node, const char *indent, bool islast, const char *sep)
{
    if (!opp_isempty(node->getText())) {
        OUT << node->getText();
    }
    else {
        // fallback: when original text is not present, use value
        if (node->getType() == LIT_STRING)
            OUT << opp_quotestr(node->getValue());
        else
            OUT << node->getValue();
    }
    if (!islast)
        OUT << (sep ? sep : "");
}

void MsgGenerator::doMsgFile(MsgFileElement *node, const char *indent, bool, const char *)
{
    OUT << getBannerComment(node, indent);
    generateChildren(node, indent);
}

void MsgGenerator::doNamespace(NamespaceElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "namespace " << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void MsgGenerator::doCplusplus(CplusplusElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "cplusplus {{" << node->getBody() << "}}" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void MsgGenerator::doStructDecl(StructDeclElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "struct " << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void MsgGenerator::doClassDecl(ClassDeclElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "class ";
    if (!node->getIsCobject())
        OUT << "noncobject ";
    OUT << node->getName();
    if (!opp_isempty(node->getExtendsName()))
        OUT << " extends " << node->getExtendsName();
    OUT << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void MsgGenerator::doMessageDecl(MessageDeclElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "message " << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void MsgGenerator::doPacketDecl(PacketDeclElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "packet " << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void MsgGenerator::doEnumDecl(EnumDeclElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "enum ";
    OUT << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void MsgGenerator::doEnum(EnumElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "enum " << node->getName();
    OUT << getRightComment(node);
    OUT << indent << "{\n";
    generateChildren(node, increaseIndent(indent));
    OUT << indent << "}";
    OUT << getTrailingComment(node);
}

void MsgGenerator::doEnumField(EnumFieldElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << node->getName();
    if (!opp_isempty(node->getValue()))
        OUT << " = " << node->getValue();
    OUT << ";" << getRightComment(node);
}

void MsgGenerator::doMessage(MessageElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "message " << node->getName();
    if (!opp_isempty(node->getExtendsName()))
        OUT << " extends " << node->getExtendsName();
    OUT << getRightComment(node);
    OUT << indent << "{\n";
    doMsgClassOrStructBody(node, indent);
    OUT << indent << "}";
    OUT << getTrailingComment(node);
}

void MsgGenerator::doPacket(PacketElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "packet " << node->getName();
    if (!opp_isempty(node->getExtendsName()))
        OUT << " extends " << node->getExtendsName();
    OUT << getRightComment(node);
    OUT << indent << "{\n";
    doMsgClassOrStructBody(node, indent);
    OUT << indent << "}";
    OUT << getTrailingComment(node);
}

void MsgGenerator::doClass(ClassElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "class " << node->getName();
    if (!opp_isempty(node->getExtendsName()))
        OUT << " extends " << node->getExtendsName();
    OUT << getRightComment(node);
    OUT << indent << "{\n";
    doMsgClassOrStructBody(node, indent);
    OUT << indent << "}";
    OUT << getTrailingComment(node);
}

void MsgGenerator::doStruct(StructElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "struct " << node->getName();
    if (!opp_isempty(node->getExtendsName()))
        OUT << " extends " << node->getExtendsName();
    OUT << getRightComment(node);
    OUT << indent << "{\n";
    doMsgClassOrStructBody(node, indent);
    OUT << indent << "}" << getTrailingComment(node);
}

void MsgGenerator::doMsgClassOrStructBody(ASTNode *node, const char *indent)
{
    // "node" must be a PacketElement, MessageElement, ClassElement or StructElement
    generateChildren(node, increaseIndent(indent));
}

void MsgGenerator::doField(FieldElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent;
    if (node->getIsAbstract())
        OUT << "abstract ";
    if (node->getIsConst())
        OUT << "const ";
    if (!opp_isempty(node->getDataType()))
        OUT << node->getDataType() << " ";
    if (node->getIsPointer())
        OUT << "*";
    OUT << node->getName();
    if (node->getIsVector() && !opp_isempty(node->getVectorSize()))
        OUT << "[" << node->getVectorSize() << "]";
    else if (node->getIsVector())
        OUT << "[]";
    const char *subindent = indent ? increaseIndent(indent) : DEFAULTINDENT;
    generateChildrenWithType(node, MSG_PROPERTY, subindent, " ");
    if (!opp_isempty(node->getDefaultValue()))
        OUT << " = " << node->getDefaultValue();
    OUT << ";" << getRightComment(node);
}

void MsgGenerator::doComment(CommentElement *node, const char *indent, bool islast, const char *)
{
    // ignore here: comments are taken care of individually where they occur
}

//---------------------------------------------------------------------------

void MsgGenerator::generateItem(ASTNode *node, const char *indent, bool islast, const char *arg)
{
    // dispatch
    int tagcode = node->getTagCode();
    switch (tagcode) {
        case MSG_FILES: doFiles(static_cast<FilesElement *>(node), indent, islast, arg); break;
        case MSG_MSG_FILE: doMsgFile(static_cast<MsgFileElement *>(node), indent, islast, arg); break;
        case MSG_IMPORT: doImport(static_cast<ImportElement *>(node), indent, islast, arg); break;
        case MSG_PROPERTY: doProperty(static_cast<PropertyElement *>(node), indent, islast, arg); break;
        case MSG_PROPERTY_KEY: doPropertyKey(static_cast<PropertyKeyElement *>(node), indent, islast, arg); break;
        case MSG_LITERAL: doLiteral(static_cast<LiteralElement *>(node), indent, islast, arg); break;
        case MSG_NAMESPACE: doNamespace(static_cast<NamespaceElement *>(node), indent, islast, arg); break;
        case MSG_CPLUSPLUS: doCplusplus(static_cast<CplusplusElement *>(node), indent, islast, arg); break;
        case MSG_STRUCT_DECL: doStructDecl(static_cast<StructDeclElement *>(node), indent, islast, arg); break;
        case MSG_CLASS_DECL: doClassDecl(static_cast<ClassDeclElement *>(node), indent, islast, arg); break;
        case MSG_MESSAGE_DECL: doMessageDecl(static_cast<MessageDeclElement *>(node), indent, islast, arg); break;
        case MSG_PACKET_DECL: doPacketDecl(static_cast<PacketDeclElement *>(node), indent, islast, arg); break;
        case MSG_ENUM_DECL: doEnumDecl(static_cast<EnumDeclElement *>(node), indent, islast, arg); break;
        case MSG_ENUM: doEnum(static_cast<EnumElement *>(node), indent, islast, arg); break;
        case MSG_ENUM_FIELD: doEnumField(static_cast<EnumFieldElement *>(node), indent, islast, arg); break;
        case MSG_MESSAGE: doMessage(static_cast<MessageElement *>(node), indent, islast, arg); break;
        case MSG_PACKET: doPacket(static_cast<PacketElement *>(node), indent, islast, arg); break;
        case MSG_CLASS: doClass(static_cast<ClassElement *>(node), indent, islast, arg); break;
        case MSG_STRUCT: doStruct(static_cast<StructElement *>(node), indent, islast, arg); break;
        case MSG_FIELD: doField(static_cast<FieldElement *>(node), indent, islast, arg); break;
        case MSG_COMMENT: doComment(static_cast<CommentElement *>(node), indent, islast, arg); break;
        default: INTERNAL_ERROR1(node, "generateItem(): unknown tag '%s'", node->getTagName());
    }
}

}  // namespace nedxml
}  // namespace omnetpp

