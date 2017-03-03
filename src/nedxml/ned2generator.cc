//==========================================================================
//  NED2GENERATOR.CC - part of
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
#include "ned2generator.h"
#include "nedutil.h"
#include "nederror.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

using std::ostream;

#define DEFAULTINDENT    "            "

void generateNED2(ostream& out, NEDElement *node, NEDErrorStore *e)
{
    NED2Generator nedgen(e);
    nedgen.generate(out, node, "");
}

//-----------------------------------------

#define OUT    (*outp)

NED2Generator::NED2Generator(NEDErrorStore *e)
{
    outp = nullptr;
    indentSize = 4;
    errors = e;
}

NED2Generator::~NED2Generator()
{
}

void NED2Generator::setIndentSize(int indentsiz)
{
    indentSize = indentsiz;
}

void NED2Generator::generate(ostream& out, NEDElement *node, const char *indent)
{
    outp = &out;
    generateNedItem(node, indent, false);
    outp = nullptr;
}

std::string NED2Generator::generate(NEDElement *node, const char *indent)
{
    std::stringstream ss;
    generate(ss, node, indent);
    return ss.str();
}

const char *NED2Generator::increaseIndent(const char *indent)
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

const char *NED2Generator::decreaseIndent(const char *indent)
{
    return indent + indentSize;
}

//---------------------------------------------------------------------------

static bool _isNetwork(NEDElement *node)
{
    Assert(node->getTagCode() == NED_COMPOUND_MODULE || node->getTagCode() == NED_SIMPLE_MODULE);
    return NEDElementUtil::getLocalBoolProperty(node, "isNetwork");
}

//---------------------------------------------------------------------------

void NED2Generator::generateChildren(NEDElement *node, const char *indent, const char *arg)
{
    for (NEDElement *child = node->getFirstChild(); child; child = child->getNextSibling())
        generateNedItem(child, indent, child == node->getLastChild(), arg);
}

void NED2Generator::generateChildrenWithType(NEDElement *node, int tagcode, const char *indent, const char *arg)
{
    // find last
    NEDElement *lastWithTag = nullptr;
    for (NEDElement *child1 = node->getFirstChild(); child1; child1 = child1->getNextSibling())
        if (child1->getTagCode() == tagcode)
            lastWithTag = child1;


    // now the recursive call
    for (NEDElement *child = node->getFirstChild(); child; child = child->getNextSibling())
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

void NED2Generator::generateChildrenWithTypes(NEDElement *node, int tagcodes[], const char *indent, const char *arg)
{
    // find last
    NEDElement *lastWithTag = nullptr;
    for (NEDElement *child1 = node->getFirstChild(); child1; child1 = child1->getNextSibling())
        if (isInVector(child1->getTagCode(), tagcodes))
            lastWithTag = child1;


    // now the recursive call
    for (NEDElement *child = node->getFirstChild(); child; child = child->getNextSibling())
        if (isInVector(child->getTagCode(), tagcodes))
            generateNedItem(child, indent, child == lastWithTag, arg);

}

//---------------------------------------------------------------------------

void NED2Generator::printInheritance(NEDElement *node, const char *indent)
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

bool NED2Generator::hasExpression(NEDElement *node, const char *attr)
{
    if (!opp_isempty(node->getAttribute(attr))) {
        return true;
    }
    else {
        for (ExpressionElement *expr = (ExpressionElement *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr = expr->getNextExpressionSibling())
            if (!opp_isempty(expr->getTarget()) && !strcmp(expr->getTarget(), attr))
                return true;

        return false;
    }
}

void NED2Generator::printExpression(NEDElement *node, const char *attr, const char *indent)
{
    if (!opp_isempty(node->getAttribute(attr))) {
        OUT << node->getAttribute(attr);
    }
    else {
        for (ExpressionElement *expr = (ExpressionElement *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr = expr->getNextExpressionSibling())
            if (!opp_isempty(expr->getTarget()) && !strcmp(expr->getTarget(), attr))
                generateNedItem(expr, indent, false, nullptr);

    }
}

void NED2Generator::printOptVector(NEDElement *node, const char *attr, const char *indent)
{
    if (hasExpression(node, attr)) {
        OUT << "[";
        printExpression(node, attr, indent);
        OUT << "]";
    }
}

//---------------------------------------------------------------------------

static const char *getComment(NEDElement *node, const char *locId)
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

std::string NED2Generator::concatInnerComments(NEDElement *node)
{
    std::string ret;
    for (NEDElement *child = node->getFirstChildWithTag(NED_COMMENT); child; child = child->getNextSiblingWithTag(NED_COMMENT)) {
        CommentElement *comment = (CommentElement *)child;
        if (!strcmp(comment->getLocid(), "inner"))
            ret += comment->getContent();
    }
    return ret;
}

std::string NED2Generator::getBannerComment(NEDElement *node, const char *indent)
{
    const char *comment = getComment(node, "banner");
    std::string innerComments = concatInnerComments(node);
    return formatComment(comment, indent, "") + formatComment(innerComments.c_str(), indent, "");
}

std::string NED2Generator::getRightComment(NEDElement *node)
{
    const char *comment = getComment(node, "right");
    return formatComment(comment, nullptr, "\n");
}

std::string NED2Generator::getInlineRightComment(NEDElement *node)
{
    const char *comment = getComment(node, "right");
    return formatComment(comment, nullptr, " ");
}

std::string NED2Generator::getTrailingComment(NEDElement *node)
{
    const char *comment = getComment(node, "trailing");
    return formatComment(comment, nullptr, "\n");
}

//---------------------------------------------------------------------------

void NED2Generator::doFiles(FilesElement *node, const char *indent, bool, const char *)
{
    generateChildren(node, indent);
}

void NED2Generator::doNedFile(NedFileElement *node, const char *indent, bool, const char *)
{
    OUT << getBannerComment(node, indent);
    generateChildren(node, indent);
}

void NED2Generator::doPackage(PackageElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "package " << node->getName() << ";" << getRightComment(node);
}

void NED2Generator::doImport(ImportElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "import " << node->getImportSpec() << ";" << getRightComment(node);
}

void NED2Generator::doPropertyDecl(PropertyDeclElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doExtends(ExtendsElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << node->getName();
    if (!islast && sep)
        OUT << (sep ? sep : "");
}

void NED2Generator::doInterfaceName(InterfaceNameElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << node->getName();
    if (!islast && sep)
        OUT << (sep ? sep : "");
}

void NED2Generator::doSimpleModule(SimpleModuleElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doModuleInterface(ModuleInterfaceElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doCompoundModule(CompoundModuleElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << (_isNetwork(node) ? "network" : "module") << " " << node->getName();
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

void NED2Generator::doChannelInterface(ChannelInterfaceElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "channelinterface " << node->getName();
    printInheritance(node, indent);
    OUT << getRightComment(node);
    OUT << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));

    OUT << indent << "}" << getTrailingComment(node);
}

void NED2Generator::doChannel(ChannelElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doParameters(ParametersElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    if (!node->getIsImplicit())
        OUT << indent << "parameters:" << getRightComment(node);

    // inside a connection, everything has to be on one line except it'd be too long
    // (rule of thumb: if it contains a param group or "parameters:" keyword is explicit)
    bool inlineParams = node->getIsImplicit() && node->getParent() && node->getParent()->getTagCode() == NED_CONNECTION;
    generateChildren(node, inlineParams ? nullptr : node->getIsImplicit() ? indent : increaseIndent(indent));
}

void NED2Generator::doParam(ParamElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    if (indent)
        OUT << indent;
    else
        OUT << " ";  // inline params, used in connections

    if (node->getIsVolatile())
        OUT << "volatile ";
    switch (node->getType()) {
        case NED_PARTYPE_NONE:   break;
        case NED_PARTYPE_DOUBLE: OUT << "double "; break;
        case NED_PARTYPE_INT:    OUT << "int "; break;
        case NED_PARTYPE_STRING: OUT << "string "; break;
        case NED_PARTYPE_BOOL:   OUT << "bool "; break;
        case NED_PARTYPE_XML:    OUT << "xml "; break;
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

void NED2Generator::doProperty(PropertyElement *node, const char *indent, bool islast, const char *sep)
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

void NED2Generator::doPropertyKey(PropertyKeyElement *node, const char *indent, bool islast, const char *sep)
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

void NED2Generator::doGates(GatesElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "gates:" << getRightComment(node);
    generateChildren(node, increaseIndent(indent));
}

void NED2Generator::doGate(GateElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent;
    switch (node->getType()) {
        case NED_GATETYPE_NONE:   break;
        case NED_GATETYPE_INPUT:  OUT << "input "; break;
        case NED_GATETYPE_OUTPUT: OUT << "output "; break;
        case NED_GATETYPE_INOUT:  OUT << "inout "; break;
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

void NED2Generator::doTypes(TypesElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "types:" << getRightComment(node);
    generateChildren(node, increaseIndent(indent));
}

void NED2Generator::doSubmodules(SubmodulesElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "submodules:" << getRightComment(node);
    generateChildren(node, increaseIndent(indent));
}

void NED2Generator::doSubmodule(SubmoduleElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doConnections(ConnectionsElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doConnection(ConnectionElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doConnectionGroup(ConnectionGroupElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doLoop(LoopElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << "for " << node->getParamName() << "=";
    printExpression(node, "from-value", indent);
    OUT << "..";
    printExpression(node, "to-value", indent);

    if (!islast)
        OUT << (sep ? sep : "");
}

void NED2Generator::doCondition(ConditionElement *node, const char *indent, bool islast, const char *sep)
{
    OUT << "if ";
    printExpression(node, "condition", indent);
    if (!islast)
        OUT << (sep ? sep : "");
}

void NED2Generator::printConnectionGate(NEDElement *conn, const char *modname, const char *modindexattr,
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
        case NED_SUBGATE_NONE: break;
        case NED_SUBGATE_I: OUT << "$i"; break;
        case NED_SUBGATE_O: OUT << "$o"; break;
        default: INTERNAL_ERROR0(conn, "wrong subg type");
    }
    if (isplusplus)
        OUT << "++";
    else
        printOptVector(conn, gateindexattr, indent);
}

void NED2Generator::doExpression(ExpressionElement *node, const char *indent, bool islast, const char *)
{
    generateChildren(node, indent);
}

int NED2Generator::getOperatorPrecedence(const char *op, int args)
{
    //
    // this method should always contain the same precendence rules as ebnf.y
    //

    if (args == 3) {
        // %left '?' ':'
        if (!strcmp(op, "?:"))
            return 1;
        INTERNAL_ERROR1(nullptr, "getOperatorPrecedence(): unknown tertiary operator '%s'", op);
    }

    if (args == 2) {
        // %left OR
        if (!strcmp(op,"||")) return 2;
        // %left XOR
        if (!strcmp(op,"##")) return 3;
        // %left AND
        if (!strcmp(op,"&&")) return 4;

        // %left EQ NE GT GE LS LE
        if (!strcmp(op,"==")) return 5;
        if (!strcmp(op,"!=")) return 5;
        if (!strcmp(op,"<"))  return 5;
        if (!strcmp(op,">"))  return 5;
        if (!strcmp(op,"<=")) return 5;
        if (!strcmp(op,">=")) return 5;

        // %left BIN_OR
        if (!strcmp(op,"|"))  return 6;
        // %left BIN_XOR
        if (!strcmp(op,"#"))  return 7;
        // %left BIN_AND
        if (!strcmp(op,"&"))  return 8;

        // %left SHIFT_LEFT SHIFT_RIGHT
        if (!strcmp(op,"<<")) return 9;
        if (!strcmp(op,">>")) return 9;

        // %left PLUS MIN
        if (!strcmp(op,"+"))  return 10;
        if (!strcmp(op,"-"))  return 10;

        // %left MUL DIV MOD
        if (!strcmp(op,"*"))  return 11;
        if (!strcmp(op,"/"))  return 11;
        if (!strcmp(op,"%"))  return 11;

        // %right EXP
        if (!strcmp(op,"^"))  return 12;
        INTERNAL_ERROR1(nullptr, "getOperatorPrecedence(): unknown binary operator '%s'", op);
    }

    if (args == 1) {
        // %left UMIN NOT BIN_COMPL
        if (!strcmp(op,"-"))  return 13;
        if (!strcmp(op,"!"))  return 13;
        if (!strcmp(op,"~"))  return 13;
        INTERNAL_ERROR1(nullptr, "getOperatorPrecedence(): unknown unary operator '%s'", op);
    }

    INTERNAL_ERROR1(nullptr, "getOperatorPrecedence(): bad number of args: %d", args);
    return -1;
}

bool NED2Generator::isOperatorLeftAssoc(const char *op)
{
    // only exponentiation is right assoc, all others are left assoc
    if (!strcmp(op,"^")) return false;
    return true;
}

void NED2Generator::doOperator(OperatorElement *node, const char *indent, bool islast, const char *)
{
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : nullptr;
    NEDElement *op3 = op2 ? op2->getNextSibling() : nullptr;

    if (!op2) {
        // unary
        OUT << node->getName();
        generateNedItem(op1, indent, false, nullptr);
    }
    else if (!op3) {
        // binary
        int prec = getOperatorPrecedence(node->getName(), 2);
        // bool leftassoc = isOperatorLeftAssoc(node->getName());

        bool needsParen = false;
        bool spacious = (prec <= 2);  // we want spaces around &&, ||, ##

        NEDElement *parent = node->getParent();
        if (parent && parent->getTagCode() == NED_OPERATOR) {
            OperatorElement *parentop = (OperatorElement *)parent;
            int parentprec = getOperatorPrecedence(parentop->getName(), parentop->getNumChildren());
            if (parentprec > prec) {
                needsParen = true;
            }
            else if (parentprec == prec) {
                // TBD can be refined (make use of associativity & precedence rules)
                needsParen = true;
            }
        }

        if (needsParen)
            OUT << "(";
        generateNedItem(op1, indent, false, nullptr);
        if (spacious)
            OUT << " " << node->getName() << " ";
        else
            OUT << node->getName();
        generateNedItem(op2, indent, false, nullptr);
        if (needsParen)
            OUT << ")";
    }
    else {
        // tertiary
        bool needsParen = true;  // TBD can be refined
        bool spacious = true;  // TBD can be refined

        if (needsParen)
            OUT << "(";
        generateNedItem(op1, indent, false, nullptr);
        OUT << (spacious ? " ? " : "?");
        generateNedItem(op2, indent, false, nullptr);
        OUT << (spacious ? " : " : ":");
        generateNedItem(op3, indent, false, nullptr);
        if (needsParen)
            OUT << ")";
    }
}

void NED2Generator::doFunction(FunctionElement *node, const char *indent, bool islast, const char *)
{
    if (!strcmp(node->getName(), "index")) {
        OUT << node->getName();  // 'index' doesn't need parentheses
        return;
    }

    OUT << node->getName() << "(";
    for (NEDElement *child = node->getFirstChild(); child; child = child->getNextSibling()) {
        if (child != node->getFirstChild())
            OUT << ", ";
        generateNedItem(child, indent, false, nullptr);
    }
    OUT << ")";
}

void NED2Generator::doIdent(IdentElement *node, const char *indent, bool islast, const char *)
{
    if (!opp_isempty(node->getModule())) {
        OUT << node->getModule();
        if (node->getFirstChild()) {
            OUT << "[";
            generateChildren(node, indent, nullptr);
            OUT << "]";
        }
        OUT << ".";
    }

    OUT << node->getName();
}

void NED2Generator::doLiteral(LiteralElement *node, const char *indent, bool islast, const char *sep)
{
    if (!opp_isempty(node->getText())) {
        OUT << node->getText();
    }
    else {
        // fallback: when original text is not present, use value
        if (node->getType() == NED_CONST_STRING)
            OUT << opp_quotestr(node->getValue());
        else
            OUT << node->getValue();
    }
    if (!islast)
        OUT << (sep ? sep : "");
}

void NED2Generator::doMsgFile(MsgFileElement *node, const char *indent, bool, const char *)
{
    OUT << getBannerComment(node, indent);
    generateChildren(node, indent);
}

void NED2Generator::doNamespace(NamespaceElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "namespace " << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void NED2Generator::doCplusplus(CplusplusElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "cplusplus {{" << node->getBody() << "}}" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void NED2Generator::doStructDecl(StructDeclElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "struct " << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void NED2Generator::doClassDecl(ClassDeclElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doMessageDecl(MessageDeclElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "message " << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void NED2Generator::doPacketDecl(PacketDeclElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "packet " << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void NED2Generator::doEnumDecl(EnumDeclElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "enum ";
    OUT << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void NED2Generator::doEnum(EnumElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "enum " << node->getName();
    if (!opp_isempty(node->getExtendsName()))
        OUT << " extends " << node->getExtendsName();
    OUT << getRightComment(node);
    OUT << indent << "{\n";
    generateChildren(node, increaseIndent(indent));
    OUT << indent << "}";
    OUT << getTrailingComment(node);
}

void NED2Generator::doEnumFields(EnumFieldsElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << getRightComment(node);
    generateChildren(node, indent);
}

void NED2Generator::doEnumField(EnumFieldElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << node->getName();
    if (!opp_isempty(node->getValue()))
        OUT << " = " << node->getValue();
    OUT << ";" << getRightComment(node);
}

void NED2Generator::doMessage(MessageElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doPacket(PacketElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doClass(ClassElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doStruct(StructElement *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doMsgClassOrStructBody(NEDElement *node, const char *indent)
{
    // "node" must be a PacketElement, MessageElement, ClassElement or StructElement
    generateChildren(node, increaseIndent(indent));

    // if (msgfileVersion!=1)
    //    generateChildren(node, increaseIndent(indent)); // 4.x syntax
    // else
    // {
    //     // old (3.x) syntax
    //     if (node->getFirstChildWithTag(NED_PROPERTY))
    //     {
    //         OUT << increaseIndent(indent) << "properties:\n";
    //         generateChildrenWithType(node, NED_PROPERTY, increaseIndent(increaseIndent(indent)));
    //     }
    //     if (node->getFirstChildWithTag(NED_FIELD))
    //     {
    //         OUT << increaseIndent(indent) << "fields:\n";
    //         generateChildrenWithType(node, NED_FIELD, increaseIndent(increaseIndent(indent)));
    //     }
    // }
}

void NED2Generator::doField(FieldElement *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent;
    if (node->getIsAbstract())
        OUT << "abstract ";
    if (node->getIsReadonly())
        OUT << "readonly ";
    if (!opp_isempty(node->getDataType()))
        OUT << node->getDataType() << " ";
    OUT << node->getName();
    if (node->getIsVector() && !opp_isempty(node->getVectorSize()))
        OUT << "[" << node->getVectorSize() << "]";
    else if (node->getIsVector())
        OUT << "[]";
    const char *subindent = indent ? increaseIndent(indent) : DEFAULTINDENT;
    generateChildrenWithType(node, NED_PROPERTY, subindent, " ");
    if (!opp_isempty(node->getDefaultValue()))
        OUT << " = " << node->getDefaultValue();
    OUT << ";" << getRightComment(node);
}

void NED2Generator::doComment(CommentElement *node, const char *indent, bool islast, const char *)
{
    // ignore here: comments are taken care of individually where they occur
}

//---------------------------------------------------------------------------

void NED2Generator::generateNedItem(NEDElement *node, const char *indent, bool islast, const char *arg)
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
        case NED_EXPRESSION: doExpression(static_cast<ExpressionElement *>(node), indent, islast, arg); break;
        case NED_OPERATOR: doOperator(static_cast<OperatorElement *>(node), indent, islast, arg); break;
        case NED_FUNCTION: doFunction(static_cast<FunctionElement *>(node), indent, islast, arg); break;
        case NED_IDENT: doIdent(static_cast<IdentElement *>(node), indent, islast, arg); break;
        case NED_LITERAL: doLiteral(static_cast<LiteralElement *>(node), indent, islast, arg); break;

        case NED_MSG_FILE: doMsgFile(static_cast<MsgFileElement *>(node), indent, islast, arg); break;
        case NED_NAMESPACE: doNamespace(static_cast<NamespaceElement *>(node), indent, islast, arg); break;
        case NED_CPLUSPLUS: doCplusplus(static_cast<CplusplusElement *>(node), indent, islast, arg); break;
        case NED_STRUCT_DECL: doStructDecl(static_cast<StructDeclElement *>(node), indent, islast, arg); break;
        case NED_CLASS_DECL: doClassDecl(static_cast<ClassDeclElement *>(node), indent, islast, arg); break;
        case NED_MESSAGE_DECL: doMessageDecl(static_cast<MessageDeclElement *>(node), indent, islast, arg); break;
        case NED_PACKET_DECL: doPacketDecl(static_cast<PacketDeclElement *>(node), indent, islast, arg); break;
        case NED_ENUM_DECL: doEnumDecl(static_cast<EnumDeclElement *>(node), indent, islast, arg); break;
        case NED_ENUM: doEnum(static_cast<EnumElement *>(node), indent, islast, arg); break;
        case NED_ENUM_FIELDS: doEnumFields(static_cast<EnumFieldsElement *>(node), indent, islast, arg); break;
        case NED_ENUM_FIELD: doEnumField(static_cast<EnumFieldElement *>(node), indent, islast, arg); break;
        case NED_MESSAGE: doMessage(static_cast<MessageElement *>(node), indent, islast, arg); break;
        case NED_PACKET: doPacket(static_cast<PacketElement *>(node), indent, islast, arg); break;
        case NED_CLASS: doClass(static_cast<ClassElement *>(node), indent, islast, arg); break;
        case NED_STRUCT: doStruct(static_cast<StructElement *>(node), indent, islast, arg); break;
        case NED_FIELD: doField(static_cast<FieldElement *>(node), indent, islast, arg); break;
        case NED_COMMENT: doComment(static_cast<CommentElement *>(node), indent, islast, arg); break;
        default: INTERNAL_ERROR1(node, "generateNedItem(): unknown tag '%s'", node->getTagName());
    }
}

}  // namespace nedxml
}  // namespace omnetpp

