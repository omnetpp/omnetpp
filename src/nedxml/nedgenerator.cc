//==========================================================================
//  NEDGENERATOR.CC - part of
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

#include "nedgenerator.h"

#include <cstring>
#include <string>
#include <sstream>
#include "common/stringutil.h"
#include "errorstore.h"
#include "nedutil.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

using std::ostream;

#define DEFAULTINDENT    "            "

void generateNed(ostream& out, ASTNode *node)
{
    NedGenerator nedgen;
    nedgen.generate(out, node, "");
}

//-----------------------------------------

#define OUT    (*outp)

void NedGenerator::setIndentSize(int indentsiz)
{
    indentSize = indentsiz;
}

void NedGenerator::generate(ostream& out, ASTNode *node, const char *indent)
{
    outp = &out;
    generateNedItem(node, indent, false);
    outp = nullptr;
}

std::string NedGenerator::generate(ASTNode *node, const char *indent)
{
    std::stringstream ss;
    generate(ss, node, indent);
    return ss.str();
}

const char *NedGenerator::increaseIndent(const char *indent)
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

const char *NedGenerator::decreaseIndent(const char *indent)
{
    return indent + indentSize;
}

//---------------------------------------------------------------------------

void NedGenerator::generateChildren(ASTNode *node, const char *indent, const char *arg)
{
    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling())
        generateNedItem(child, indent, child == node->getLastChild(), arg);
}

void NedGenerator::generateChildrenWithType(ASTNode *node, int tagcode, const char *indent, const char *arg)
{
    // find last
    ASTNode *lastWithTag = nullptr;
    for (ASTNode *child1 = node->getFirstChild(); child1; child1 = child1->getNextSibling())
        if (child1->getTagCode() == tagcode)
            lastWithTag = child1;


    // now the recursive call
    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling())
        if (child->getTagCode() == tagcode)
            generateNedItem(child, indent, child == lastWithTag, arg);

}

static int isInVector(int a, int v[])
{
    for (int i = 0; v[i]; i++)  // v[] is zero-terminated
        if (v[i] == a)
            return true;

    return false;
}

void NedGenerator::generateChildrenWithTypes(ASTNode *node, int tagcodes[], const char *indent, const char *arg)
{
    // find last
    ASTNode *lastWithTag = nullptr;
    for (ASTNode *child1 = node->getFirstChild(); child1; child1 = child1->getNextSibling())
        if (isInVector(child1->getTagCode(), tagcodes))
            lastWithTag = child1;


    // now the recursive call
    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling())
        if (isInVector(child->getTagCode(), tagcodes))
            generateNedItem(child, indent, child == lastWithTag, arg);

}

//---------------------------------------------------------------------------

void NedGenerator::printInheritance(ASTNode *node, const char *indent)
{
    if (node->getFirstChildWithTag(NED_EXTENDS)) {
        OUT << " extends ";
        generateChildrenWithType(node, NED_EXTENDS, increaseIndent(indent), ", ");
    }

    if (node->getFirstChildWithTag(NED_INTERFACE_NAME)) {
        OUT << " like ";
        generateChildrenWithType(node, NED_INTERFACE_NAME, increaseIndent(indent), ", ");
    }
}

bool NedGenerator::hasExpression(ASTNode *node, const char *attr)
{
    return !opp_isempty(node->getAttribute(attr));
}

void NedGenerator::printExpression(ASTNode *node, const char *attr, const char *indent)
{
    OUT << node->getAttribute(attr);
}

void NedGenerator::printOptVector(ASTNode *node, const char *attr, const char *indent)
{
    if (hasExpression(node, attr)) {
        OUT << "[";
        printExpression(node, attr, indent);
        OUT << "]";
    }
}

//---------------------------------------------------------------------------

static const char *getComment(ASTNode *node, const char *locId)
{
    CommentElement *comment = (CommentElement *)node->getFirstChildWithAttribute(NED_COMMENT, "locid", locId);
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

std::string NedGenerator::concatInnerComments(ASTNode *node)
{
    std::string ret;
    for (ASTNode *child = node->getFirstChildWithTag(NED_COMMENT); child; child = child->getNextSiblingWithTag(NED_COMMENT)) {
        CommentElement *comment = (CommentElement *)child;
        if (!strcmp(comment->getLocid(), "inner"))
            ret += comment->getContent();
    }
    return ret;
}

std::string NedGenerator::getBannerComment(ASTNode *node, const char *indent)
{
    const char *comment = getComment(node, "banner");
    std::string innerComments = concatInnerComments(node);
    return formatComment(comment, indent, "") + formatComment(innerComments.c_str(), indent, "");
}

std::string NedGenerator::getRightComment(ASTNode *node)
{
    const char *comment = getComment(node, "right");
    return formatComment(comment, nullptr, "\n");
}

std::string NedGenerator::getInlineRightComment(ASTNode *node)
{
    const char *comment = getComment(node, "right");
    return formatComment(comment, nullptr, " ");
}

std::string NedGenerator::getTrailingComment(ASTNode *node)
{
    const char *comment = getComment(node, "trailing");
    return formatComment(comment, nullptr, "\n");
}

//---------------------------------------------------------------------------

void NedGenerator::doFiles(FilesElement *node, const char *indent, bool, const char *)
{
    generateChildren(node, indent);
}

void NedGenerator::doNedFile(NedFileElement *node, const char *indent, bool, const char *)
{
    OUT << getBannerComment(node, indent);
    generateChildren(node, indent);
}

void NedGenerator::doPackage(PackageElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "package " << node->getName() << ";" << getRightComment(node);
}

void NedGenerator::doImport(ImportElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "import " << node->getImportSpec() << ";" << getRightComment(node);
}

void NedGenerator::doPropertyDecl(PropertyDeclElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "property @" << node->getName();
    if (node->getIsArray())
        OUT << "[]";
    if (node->getFirstChildWithTag(NED_PROPERTY_KEY)) {
        OUT << "(";
        generateChildrenWithType(node, NED_PROPERTY_KEY, increaseIndent(indent), "; ");
        OUT << ")";
    }
    OUT << ";" << getRightComment(node);
}

void NedGenerator::doExtends(ExtendsElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << node->getName();
    if (!islast && sep)
        OUT << (sep ? sep : "");
}

void NedGenerator::doInterfaceName(InterfaceNameElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << node->getName();
    if (!islast && sep)
        OUT << (sep ? sep : "");
}

void NedGenerator::doSimpleModule(SimpleModuleElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "simple " << node->getName();
    printInheritance(node, indent);
    OUT << getRightComment(node);
    OUT << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));

    OUT << indent << "}" << getTrailingComment(node);
}

void NedGenerator::doModuleInterface(ModuleInterfaceElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "moduleinterface " << node->getName();
    printInheritance(node, indent);
    OUT << getRightComment(node);
    OUT << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));

    OUT << indent << "}" << getTrailingComment(node);
}

void NedGenerator::doCompoundModule(CompoundModuleElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    bool isNetwork = ASTNodeUtil::getLocalBoolProperty(node, "isNetwork");
    OUT << indent << (isNetwork ? "network" : "module") << " " << node->getName();
    printInheritance(node, indent);
    OUT << getRightComment(node);
    OUT << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));
    generateChildrenWithType(node, NED_TYPES, increaseIndent(indent));
    generateChildrenWithType(node, NED_SUBMODULES, increaseIndent(indent));
    generateChildrenWithType(node, NED_CONNECTIONS, increaseIndent(indent));

    OUT << indent << "}" << getTrailingComment(node);
}

void NedGenerator::doChannelInterface(ChannelInterfaceElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "channelinterface " << node->getName();
    printInheritance(node, indent);
    OUT << getRightComment(node);
    OUT << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));

    OUT << indent << "}" << getTrailingComment(node);
}

void NedGenerator::doChannel(ChannelElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "channel ";
    OUT << node->getName();
    printInheritance(node, indent);
    OUT << getRightComment(node);
    OUT << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));

    OUT << indent << "}" << getTrailingComment(node);
}

void NedGenerator::doParameters(ParametersElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    if (!node->getIsImplicit())
        OUT << indent << "parameters:" << getRightComment(node);

    // inside a connection, everything has to be on one line except it'd be too long
    // (rule of thumb: if it contains a param group or "parameters:" keyword is explicit)
    bool inlineParams = node->getIsImplicit() && node->getParent() && node->getParent()->getTagCode() == NED_CONNECTION;
    generateChildren(node, inlineParams ? nullptr : node->getIsImplicit() ? indent : increaseIndent(indent));
}

void NedGenerator::doParam(ParamElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    if (indent)
        OUT << indent;
    else
        OUT << " ";  // inline params, used in connections

    if (node->getIsVolatile())
        OUT << "volatile ";
    switch (node->getType()) {
        case PARTYPE_NONE:   break;
        case PARTYPE_DOUBLE: OUT << "double "; break;
        case PARTYPE_INT:    OUT << "int "; break;
        case PARTYPE_STRING: OUT << "string "; break;
        case PARTYPE_BOOL:   OUT << "bool "; break;
        case PARTYPE_OBJECT: OUT << "object "; break;
        case PARTYPE_XML:    OUT << "xml "; break;
        default: INTERNAL_ERROR0(node, "wrong type");
    }
    OUT << node->getName();

    const char *subindent = indent ? increaseIndent(indent) : DEFAULTINDENT;
    generateChildrenWithType(node, NED_PROPERTY, subindent, " ");

    if (hasExpression(node, "value")) {
        OUT << " = ";
        if (node->getIsDefault())
            OUT << "default(";
        printExpression(node, "value", indent);
        if (node->getIsDefault())
            OUT << ")";
    }
    else if (node->getIsDefault()) {
        OUT << " = default";
    }

    if (indent)
        OUT << ";" << getRightComment(node);
    else
        OUT << ";";
}

void NedGenerator::doProperty(PropertyElement *node, const char *indent, bool islast, const char *sep)
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
        if (node->getFirstChildWithTag(NED_PROPERTY_KEY)) {
            OUT << "(";
            generateChildrenWithType(node, NED_PROPERTY_KEY, subindent, "; ");
            OUT << ")";
        }
        if (!sep && !indent)
            OUT << ";";
        else if (!sep)
            OUT << ";" << getRightComment(node);
    }
}

void NedGenerator::doPropertyKey(PropertyKeyElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << node->getName();
    if (node->getFirstChildWithTag(NED_LITERAL)) {
        if (!opp_isempty(node->getName()))
            OUT << "=";
        generateChildrenWithType(node, NED_LITERAL, increaseIndent(indent), ",");
    }
    if (!islast && sep)
        OUT << (sep ? sep : "");
}

void NedGenerator::doGates(GatesElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "gates:" << getRightComment(node);
    generateChildren(node, increaseIndent(indent));
}

void NedGenerator::doGate(GateElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent;
    switch (node->getType()) {
        case GATETYPE_NONE:   break;
        case GATETYPE_INPUT:  OUT << "input "; break;
        case GATETYPE_OUTPUT: OUT << "output "; break;
        case GATETYPE_INOUT:  OUT << "inout "; break;
        default: INTERNAL_ERROR0(node, "wrong type");
    }
    OUT << node->getName();
    if (node->getIsVector() && !hasExpression(node, "vector-size"))
        OUT << "[]";
    printOptVector(node, "vector-size", indent);

    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");
    OUT << ";" << getRightComment(node);
    ;
}

void NedGenerator::doTypes(TypesElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "types:" << getRightComment(node);
    generateChildren(node, increaseIndent(indent));
}

void NedGenerator::doSubmodules(SubmodulesElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "submodules:" << getRightComment(node);
    generateChildren(node, increaseIndent(indent));
}

void NedGenerator::doSubmodule(SubmoduleElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << node->getName();
    printOptVector(node, "vector-size", indent);
    OUT << ": ";

    if (!opp_isempty(node->getLikeType())) {
        // "like" version
        OUT << "<";
        if (node->getIsDefault())
            OUT << "default(";
        printExpression(node, "like-expr", indent);  // this (incidentally) also works if like-expr contains a property (ie. starts with "@")
        if (node->getIsDefault())
            OUT << ")";
        OUT << ">";
        OUT << " like " << node->getLikeType();
    }
    else {
        // "like"-less
        OUT << node->getType();
    }

    ConditionElement *condition = node->getFirstConditionChild();
    if (condition) {
        OUT << " ";
        doCondition(condition, nullptr, true, nullptr);
    }

    if (!node->getFirstParametersChild() && !node->getFirstGatesChild()) {
        OUT << ";" << getTrailingComment(node);
    }
    else {
        OUT << " {" << getRightComment(node);
        generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
        generateChildrenWithType(node, NED_GATES, increaseIndent(indent));
        OUT << indent << "}" << getTrailingComment(node);
    }
}

void NedGenerator::doConnections(ConnectionsElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    if (node->getAllowUnconnected()) {
        OUT << indent << "connections allowunconnected:" << getRightComment(node);
    }
    else {
        OUT << indent << "connections:" << getRightComment(node);
    }
    generateChildren(node, increaseIndent(indent));
}

void NedGenerator::doConnection(ConnectionElement *node, const char *indent, bool islast, const char *)
{
    // direction
    const char *arrow = node->getIsBidirectional() ? " <-->" : node->getIsForwardArrow() ? " -->" : " <--";
    bool srcfirst = node->getIsForwardArrow();

    OUT << getBannerComment(node, indent);

    // print src
    OUT << indent;
    if (srcfirst)
        printConnectionGate(node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", node->getSrcGatePlusplus(), node->getSrcGateSubg(), indent);
    else
        printConnectionGate(node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", node->getDestGatePlusplus(), node->getDestGateSubg(), indent);

    // arrow
    OUT << arrow;

    // print channel spec
    if (!opp_isempty(node->getName()) || !opp_isempty(node->getType()) || !opp_isempty(node->getLikeType()) || node->getFirstChildWithTag(NED_PARAMETERS)) {
        if (!opp_isempty(node->getName())) {
            OUT << " " << node->getName() << ":";
        }

        if (!opp_isempty(node->getLikeType())) {
            // "like" version
            OUT << " <";
            if (node->getIsDefault())
                OUT << "default(";
            printExpression(node, "like-expr", indent);  // this (incidentally) also works if like-expr contains a property (ie. starts with "@")
            if (node->getIsDefault())
                OUT << ")";
            OUT << ">";
            OUT << " like " << node->getLikeType();
        }
        else if (!opp_isempty(node->getType())) {
            // concrete channel type
            OUT << " " << node->getType();
        }

        if (node->getFirstChildWithTag(NED_PARAMETERS)) {
            OUT << " { ";
            generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
            OUT << " }";
        }

        OUT << arrow;
    }

    // print dest
    OUT << " ";
    if (srcfirst)
        printConnectionGate(node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", node->getDestGatePlusplus(), node->getDestGateSubg(), indent);
    else
        printConnectionGate(node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", node->getSrcGatePlusplus(), node->getSrcGateSubg(), indent);

    // print loops and conditions
    if (node->getFirstChildWithTag(NED_LOOP) || node->getFirstChildWithTag(NED_CONDITION)) {
        OUT << " ";
        int tags[] = {
            NED_LOOP, NED_CONDITION, NED_NULL
        };
        generateChildrenWithTypes(node, tags, increaseIndent(indent), ", ");
    }
    OUT << ";" << getRightComment(node);
}

void NedGenerator::doConnectionGroup(ConnectionGroupElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    if (node->getFirstChildWithTag(NED_LOOP) || node->getFirstChildWithTag(NED_CONDITION)) {
        OUT << indent;
        int tags[] = {
            NED_LOOP, NED_CONDITION, NED_NULL
        };
        generateChildrenWithTypes(node, tags, increaseIndent(indent), ", ");
    }

    OUT << " {" << getRightComment(node);
    generateChildrenWithType(node, NED_CONNECTION, increaseIndent(indent));
    OUT << indent << "}" << getTrailingComment(node);
}

void NedGenerator::doLoop(LoopElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << "for " << node->getParamName() << "=";
    printExpression(node, "from-value", indent);
    OUT << "..";
    printExpression(node, "to-value", indent);

    if (!islast)
        OUT << (sep ? sep : "");
}

void NedGenerator::doCondition(ConditionElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << "if ";
    printExpression(node, "condition", indent);
    if (!islast)
        OUT << (sep ? sep : "");
}

void NedGenerator::printConnectionGate(ASTNode *conn, const char *modname, const char *modindexattr,
        const char *gatename, const char *gateindexattr, bool isplusplus,
        int gatesubg, const char *indent)
{
    if (!opp_isempty(modname)) {
        OUT << modname;
        printOptVector(conn, modindexattr, indent);
        OUT << ".";
    }

    OUT << gatename;
    switch (gatesubg) {
        case SUBGATE_NONE: break;
        case SUBGATE_I: OUT << "$i"; break;
        case SUBGATE_O: OUT << "$o"; break;
        default: INTERNAL_ERROR0(conn, "wrong subg type");
    }
    if (isplusplus)
        OUT << "++";
    else
        printOptVector(conn, gateindexattr, indent);
}

void NedGenerator::doLiteral(LiteralElement *node, const char *indent, bool islast, const char *sep)
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

void NedGenerator::doComment(CommentElement *node, const char *indent, bool islast, const char *)
{
    // ignore here: comments are taken care of individually where they occur
}

//---------------------------------------------------------------------------

void NedGenerator::generateNedItem(ASTNode *node, const char *indent, bool islast, const char *arg)
{
    // dispatch
    int tagcode = node->getTagCode();
    switch (tagcode) {
        case NED_FILES: doFiles(static_cast<FilesElement *>(node), indent, islast, arg); break;
        case NED_NED_FILE: doNedFile(static_cast<NedFileElement *>(node), indent, islast, arg); break;
        case NED_PACKAGE: doPackage(static_cast<PackageElement *>(node), indent, islast, arg); break;
        case NED_IMPORT: doImport(static_cast<ImportElement *>(node), indent, islast, arg); break;
        case NED_PROPERTY_DECL: doPropertyDecl(static_cast<PropertyDeclElement *>(node), indent, islast, arg); break;
        case NED_EXTENDS: doExtends(static_cast<ExtendsElement *>(node), indent, islast, arg); break;
        case NED_INTERFACE_NAME: doInterfaceName(static_cast<InterfaceNameElement *>(node), indent, islast, arg); break;
        case NED_SIMPLE_MODULE: doSimpleModule(static_cast<SimpleModuleElement *>(node), indent, islast, arg); break;
        case NED_MODULE_INTERFACE: doModuleInterface(static_cast<ModuleInterfaceElement *>(node), indent, islast, arg); break;
        case NED_COMPOUND_MODULE: doCompoundModule(static_cast<CompoundModuleElement *>(node), indent, islast, arg); break;
        case NED_CHANNEL_INTERFACE: doChannelInterface(static_cast<ChannelInterfaceElement *>(node), indent, islast, arg); break;
        case NED_CHANNEL: doChannel(static_cast<ChannelElement *>(node), indent, islast, arg); break;
        case NED_PARAMETERS: doParameters(static_cast<ParametersElement *>(node), indent, islast, arg); break;
        case NED_PARAM: doParam(static_cast<ParamElement *>(node), indent, islast, arg); break;
        case NED_PROPERTY: doProperty(static_cast<PropertyElement *>(node), indent, islast, arg); break;
        case NED_PROPERTY_KEY: doPropertyKey(static_cast<PropertyKeyElement *>(node), indent, islast, arg); break;
        case NED_GATES: doGates(static_cast<GatesElement *>(node), indent, islast, arg); break;
        case NED_GATE: doGate(static_cast<GateElement *>(node), indent, islast, arg); break;
        case NED_TYPES: doTypes(static_cast<TypesElement *>(node), indent, islast, arg); break;
        case NED_SUBMODULES: doSubmodules(static_cast<SubmodulesElement *>(node), indent, islast, arg); break;
        case NED_SUBMODULE: doSubmodule(static_cast<SubmoduleElement *>(node), indent, islast, arg); break;
        case NED_CONNECTIONS: doConnections(static_cast<ConnectionsElement *>(node), indent, islast, arg); break;
        case NED_CONNECTION: doConnection(static_cast<ConnectionElement *>(node), indent, islast, arg); break;
        case NED_CONNECTION_GROUP: doConnectionGroup(static_cast<ConnectionGroupElement *>(node), indent, islast, arg); break;
        case NED_LOOP: doLoop(static_cast<LoopElement *>(node), indent, islast, arg); break;
        case NED_CONDITION: doCondition(static_cast<ConditionElement *>(node), indent, islast, arg); break;
        case NED_LITERAL: doLiteral(static_cast<LiteralElement *>(node), indent, islast, arg); break;
        case NED_COMMENT: doComment(static_cast<CommentElement *>(node), indent, islast, arg); break;
        default: INTERNAL_ERROR1(node, "generateNedItem(): unknown tag '%s'", node->getTagName());
    }
}

}  // namespace nedxml
}  // namespace omnetpp

