//==========================================================================
//  NEDGENERATOR.CC - part of
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


#include <string.h>
#include <string>
#include <sstream>
#include "ned2generator.h"
#include "stringutil.h"
#include "nederror.h"

NAMESPACE_BEGIN

using std::ostream;

#define DEFAULTINDENT "            "

void generateNED2(ostream& out, NEDElement *node, NEDErrorStore *e)
{
    NED2Generator nedgen(e);
    nedgen.generate(out, node, "");
}

//-----------------------------------------

#define OUT  (*outp)

NED2Generator::NED2Generator(NEDErrorStore *e)
{
    outp = NULL;
    indentsize = 4;
    errors = e;
}

NED2Generator::~NED2Generator()
{
}

void NED2Generator::setIndentSize(int indentsiz)
{
    indentsize = indentsiz;
}

void NED2Generator::generate(ostream& out, NEDElement *node, const char *indent)
{
    outp = &out;
    generateNedItem(node, indent, false);
    outp = NULL;
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
    if (ilen+indentsize <= (int)sizeof(spaces)-1)
        ilen+=indentsize;
    const char *newindent = spaces+sizeof(spaces)-1-ilen;
    return newindent;
}

const char *NED2Generator::decreaseIndent(const char *indent)
{
    return indent + indentsize;
}

//---------------------------------------------------------------------------

void NED2Generator::generateChildren(NEDElement *node, const char *indent, const char *arg)
{
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        generateNedItem(child, indent, child==node->getLastChild(), arg);
}

void NED2Generator::generateChildrenWithType(NEDElement *node, int tagcode, const char *indent, const char *arg)
{
    // find last
    NEDElement *lastWithTag = NULL;
    for (NEDElement *child1=node->getFirstChild(); child1; child1=child1->getNextSibling())
        if (child1->getTagCode()==tagcode)
             lastWithTag = child1;

    // now the recursive call
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        if (child->getTagCode()==tagcode)
            generateNedItem(child, indent, child==lastWithTag, arg);
}

static int isInVector(int a, int v[])
{
    for (int i=0; v[i]; i++)  // v[] is zero-terminated
        if (v[i]==a)
            return true;
    return false;
}

void NED2Generator::generateChildrenWithTypes(NEDElement *node, int tagcodes[], const char *indent, const char *arg)
{
    // find last
    NEDElement *lastWithTag = NULL;
    for (NEDElement *child1=node->getFirstChild(); child1; child1=child1->getNextSibling())
        if (isInVector(child1->getTagCode(), tagcodes))
             lastWithTag = child1;

    // now the recursive call
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        if (isInVector(child->getTagCode(), tagcodes))
            generateNedItem(child, indent, child==lastWithTag, arg);
}

//---------------------------------------------------------------------------

void NED2Generator::printInheritance(NEDElement *node, const char *indent)
{
    if (node->getFirstChildWithTag(NED_EXTENDS))
    {
        OUT << " extends ";
        generateChildrenWithType(node, NED_EXTENDS, increaseIndent(indent), ", ");
    }

    if (node->getFirstChildWithTag(NED_INTERFACE_NAME))
    {
        OUT << " like ";
        generateChildrenWithType(node, NED_INTERFACE_NAME, increaseIndent(indent), ", ");
    }
}

bool NED2Generator::hasExpression(NEDElement *node, const char *attr)
{
    if (!opp_isempty(node->getAttribute(attr)))
    {
        return true;
    }
    else
    {
        for (ExpressionNode *expr=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr=expr->getNextExpressionNodeSibling())
            if (!opp_isempty(expr->getTarget()) && !strcmp(expr->getTarget(),attr))
                return true;
        return false;
    }
}

void NED2Generator::printExpression(NEDElement *node, const char *attr, const char *indent)
{
    if (!opp_isempty(node->getAttribute(attr)))
    {
        OUT << node->getAttribute(attr);
    }
    else
    {
        for (ExpressionNode *expr=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr=expr->getNextExpressionNodeSibling())
            if (!opp_isempty(expr->getTarget()) && !strcmp(expr->getTarget(),attr))
                generateNedItem(expr, indent, false, NULL);
    }
}

void NED2Generator::printOptVector(NEDElement *node, const char *attr, const char *indent)
{
    if (hasExpression(node,attr))
    {
        OUT << "[";
        printExpression(node,attr,indent);
        OUT << "]";
    }
}

//---------------------------------------------------------------------------

static const char *getComment(NEDElement *node, const char *locId)
{
    CommentNode *comment = (CommentNode *)node->getFirstChildWithAttribute(NED_COMMENT, "locid", locId);
    return (comment && !opp_isempty(comment->getContent())) ? comment->getContent() : NULL;
}

static std::string formatComment(const char *comment, const char *indent, const char *defaultValue)
{
    if (!comment || !comment[0])
        return defaultValue;

    // indent each line of comment; also ensure that if last line contains
    // a comment (//), it gets terminated by newline. If indent==NULL,
    // keep original indent
    std::string ret;
    const char *curLine = comment;
    while (curLine[0])
    {
        const char *nextLine = strchr(curLine,'\n');
        if (!nextLine)
            nextLine = curLine + strlen(curLine);
        const char *commentStart = strstr(curLine, "//");
        if (!commentStart || commentStart>=nextLine)
            ret += "\n"; // no comment in that line -- just add newline
        else if (!indent)
            ret += std::string(curLine, nextLine) + "\n"; // use original indent
        else
            ret += indent + std::string(commentStart, nextLine) + "\n"; // indent the comment

        curLine = nextLine[0] ? nextLine+1 : nextLine; // +1: skip newline
    }
    return ret;
}

std::string NED2Generator::concatInnerComments(NEDElement *node)
{
    std::string ret;
    for (NEDElement *child=node->getFirstChildWithTag(NED_COMMENT); child; child = child->getNextSiblingWithTag(NED_COMMENT))
    {
        CommentNode *comment = (CommentNode *)child;
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
    return formatComment(comment, NULL, "\n");
}

std::string NED2Generator::getInlineRightComment(NEDElement *node)
{
    const char *comment = getComment(node, "right");
    return formatComment(comment, NULL, " ");
}

std::string NED2Generator::getTrailingComment(NEDElement *node)
{
    const char *comment = getComment(node, "trailing");
    return formatComment(comment, NULL, "\n");
}

//---------------------------------------------------------------------------

void NED2Generator::doFiles(FilesNode *node, const char *indent, bool, const char *)
{
    generateChildren(node, indent);
}

void NED2Generator::doNedFile(NedFileNode *node, const char *indent, bool, const char *)
{
    OUT << getBannerComment(node, indent);
    generateChildren(node, indent);
}

void NED2Generator::doPackage(PackageNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "package " << node->getName() << ";" << getRightComment(node);
}

void NED2Generator::doImport(ImportNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "import " << node->getImportSpec() << ";" << getRightComment(node);
}

void NED2Generator::doPropertyDecl(PropertyDeclNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "property @" << node->getName();
    if (node->getIsArray())
        OUT << "[]";
    if (node->getFirstChildWithTag(NED_PROPERTY_KEY))
    {
        OUT << "(";
        generateChildrenWithType(node, NED_PROPERTY_KEY, increaseIndent(indent), "; ");
        OUT << ")";
    }
    OUT << ";" << getRightComment(node);
}

void NED2Generator::doExtends(ExtendsNode *node, const char *indent, bool islast, const char *sep)
{
    OUT << node->getName();
    if (!islast && sep)
        OUT << (sep ? sep : "");
}

void NED2Generator::doInterfaceName(InterfaceNameNode *node, const char *indent, bool islast, const char *sep)
{
    OUT << node->getName();
    if (!islast && sep)
        OUT << (sep ? sep : "");
}

void NED2Generator::doSimpleModule(SimpleModuleNode *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doModuleInterface(ModuleInterfaceNode *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doCompoundModule(CompoundModuleNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << (node->getIsNetwork() ? "network" : "module") << " " << node->getName();
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

void NED2Generator::doChannelInterface(ChannelInterfaceNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "channelinterface " << node->getName();
    printInheritance(node, indent);
    OUT << getRightComment(node);
    OUT << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));

    OUT << indent << "}" << getTrailingComment(node);
}

void NED2Generator::doChannel(ChannelNode *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doParameters(ParametersNode *node, const char *indent, bool islast, const char *)
{
    // inside channel-spec, everything has to be on one line except it'd be too long
    // (rule of thumb: if it contains a param group or "parameters:" keyword is explicit)
    bool inlineParams = node->getParent()->getTagCode()==NED_CHANNEL_SPEC && node->getIsImplicit();

    OUT << getBannerComment(node, indent);
    if (!node->getIsImplicit())
        OUT << indent << "parameters:" << getRightComment(node);

    generateChildren(node, inlineParams ? NULL : node->getIsImplicit() ? indent : increaseIndent(indent));
}

void NED2Generator::doParam(ParamNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    if (indent)
        OUT << indent;
    else
        OUT << " ";  // inline params, used for channel-spec in connections

    if (node->getIsVolatile())
        OUT << "volatile ";
    switch (node->getType())
    {
        case NED_PARTYPE_NONE:   break;
        case NED_PARTYPE_DOUBLE: OUT << "double "; break;
        case NED_PARTYPE_INT:    OUT << "int "; break;
        case NED_PARTYPE_STRING: OUT << "string "; break;
        case NED_PARTYPE_BOOL:   OUT << "bool "; break;
        case NED_PARTYPE_XML:    OUT << "xml "; break;
        default: INTERNAL_ERROR0(node, "wrong type");
    }
    OUT << node->getName();
    if (hasExpression(node,"value"))
    {
        OUT << " = ";
        if (node->getIsDefault())
            OUT << "default(";
        printExpression(node, "value",indent);
        if (node->getIsDefault())
            OUT << ")";
    }

    const char *subindent = indent ? increaseIndent(indent) : DEFAULTINDENT;
    generateChildrenWithType(node, NED_PROPERTY, subindent, " ");

    if (indent)
        OUT << ";" << getRightComment(node);
    else
        OUT << ";";
}

void NED2Generator::doPattern(PatternNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "/" << node->getPattern() << "/ = ";
    printExpression(node, "value",indent);

    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");
    OUT << ";" << getRightComment(node);
}

void NED2Generator::doProperty(PropertyNode *node, const char *indent, bool islast, const char *sep)
{
    if (!node->getIsImplicit())
    {
        // if sep==NULL, print as standalone property (with indent and ";"), otherwise as inline property
        OUT << getBannerComment(node, indent);
        if (!sep && indent)
            OUT << indent;
        if (sep)
            OUT << " ";
        OUT << "@" << node->getName();
        if (!opp_isempty(node->getIndex()))
            OUT << "[" << node->getIndex() << "]";
        const char *subindent = indent ? increaseIndent(indent) : DEFAULTINDENT;
        if (node->getFirstChildWithTag(NED_PROPERTY_KEY))
        {
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

void NED2Generator::doPropertyKey(PropertyKeyNode *node, const char *indent, bool islast, const char *sep)
{
    OUT << node->getName();
    if (node->getFirstChildWithTag(NED_LITERAL))
    {
        if (!opp_isempty(node->getName()))
            OUT << "=";
        generateChildrenWithType(node, NED_LITERAL, increaseIndent(indent),",");
    }
    if (!islast && sep)
        OUT << (sep ? sep : "");
}

void NED2Generator::doGates(GatesNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "gates:" << getRightComment(node);
    generateChildren(node, increaseIndent(indent));
}

void NED2Generator::doGate(GateNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent;
    switch (node->getType())
    {
        case NED_GATETYPE_NONE:   break;
        case NED_GATETYPE_INPUT:  OUT << "input "; break;
        case NED_GATETYPE_OUTPUT: OUT << "output "; break;
        case NED_GATETYPE_INOUT:  OUT << "inout "; break;
        default: INTERNAL_ERROR0(node, "wrong type");
    }
    OUT << node->getName();
    if (node->getIsVector() && !hasExpression(node,"vector-size"))
        OUT << "[]";
    printOptVector(node, "vector-size",indent);

    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");
    OUT << ";" << getRightComment(node);;
}

void NED2Generator::doTypes(TypesNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "types:" << getRightComment(node);
    generateChildren(node, increaseIndent(indent));
}

void NED2Generator::doSubmodules(SubmodulesNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "submodules:" << getRightComment(node);
    generateChildren(node, increaseIndent(indent));
}

void NED2Generator::doSubmodule(SubmoduleNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << node->getName();
    printOptVector(node, "vector-size",indent);
    OUT << ": ";

    if (!opp_isempty(node->getLikeType()))
    {
        // "like" version
        OUT << "<";
        printExpression(node, "like-param", indent); // this (incidentally) also works if like-param contains a property (ie. starts with "@")
        OUT << ">";
        OUT << " like " << node->getLikeType();
    }
    else
    {
        // "like"-less
        OUT << node->getType();
    }

    if (!node->getFirstChildWithTag(NED_PARAMETERS) && !node->getFirstChildWithTag(NED_GATES))
    {
        OUT << ";" << getTrailingComment(node);
    }
    else
    {
        OUT << " {" << getRightComment(node);
        generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
        generateChildrenWithType(node, NED_GATES, increaseIndent(indent));
        OUT << indent << "}" << getTrailingComment(node);
    }
}

void NED2Generator::doConnections(ConnectionsNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    if (node->getAllowUnconnected()) {
        OUT << indent << "connections allowunconnected:" << getRightComment(node);
    } else {
        OUT << indent << "connections:" << getRightComment(node);
    }
    generateChildren(node, increaseIndent(indent));
}

void NED2Generator::doConnection(ConnectionNode *node, const char *indent, bool islast, const char *)
{
    // direction
    const char *arrow;
    bool srcfirst;
    switch (node->getArrowDirection())
    {
        case NED_ARROWDIR_L2R:   arrow = " -->"; srcfirst = true; break;
        case NED_ARROWDIR_R2L:   arrow = " <--"; srcfirst = false; break;
        case NED_ARROWDIR_BIDIR: arrow = " <-->"; srcfirst = true; break;
        default: INTERNAL_ERROR0(node, "wrong arrow-dir");
    }

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
    ChannelSpecNode *channelSpecNode = (ChannelSpecNode *)node->getFirstChildWithTag(NED_CHANNEL_SPEC);
    if (channelSpecNode && !isEmptyChannelSpec(channelSpecNode))
    {
        generateChildrenWithType(node, NED_CHANNEL_SPEC, indent);
        OUT << arrow;
    }

    // print dest
    OUT << " ";
    if (srcfirst)
        printConnectionGate(node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", node->getDestGatePlusplus(), node->getDestGateSubg(), indent);
    else
        printConnectionGate(node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", node->getSrcGatePlusplus(), node->getSrcGateSubg(), indent);

    // print loops and conditions
    if (node->getFirstChildWithTag(NED_LOOP) || node->getFirstChildWithTag(NED_CONDITION))
    {
        OUT << " ";
        int tags[] = {NED_LOOP, NED_CONDITION, NED_NULL};
        generateChildrenWithTypes(node, tags, increaseIndent(indent), ", ");
    }
    OUT << ";" << getRightComment(node);
}

bool NED2Generator::isEmptyChannelSpec(ChannelSpecNode *node)
{
    if (!opp_isempty(node->getType()) || !opp_isempty(node->getLikeType()) || !opp_isempty(node->getLikeParam()))
        return false;
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        if (child->getTagCode() != NED_COMMENT)
            return false;
    return true;
}

void NED2Generator::doChannelSpec(ChannelSpecNode *node, const char *indent, bool islast, const char *)
{
    if (!opp_isempty(node->getLikeType()))
    {
        // "like" version
        OUT << " <";
        printExpression(node, "like-param", indent); // this (incidentally) also works if like-param contains a property (ie. starts with "@")
        OUT << ">";
        OUT << " like " << node->getLikeType();
    }
    else if (!opp_isempty(node->getType()))
    {
        // concrete channel type
        OUT << " " << node->getType();
    }

    if (node->getFirstChildWithTag(NED_PARAMETERS))
    {
        OUT << " { ";
        generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
        OUT << " }";
    }
}

void NED2Generator::doConnectionGroup(ConnectionGroupNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    if (node->getFirstChildWithTag(NED_LOOP) || node->getFirstChildWithTag(NED_CONDITION))
    {
        OUT << indent;
        int tags[] = {NED_LOOP, NED_CONDITION, NED_NULL};
        generateChildrenWithTypes(node, tags, increaseIndent(indent), ", ");
    }

    OUT << " {" << getRightComment(node);
    generateChildrenWithType(node, NED_CONNECTION, increaseIndent(indent));
    OUT << indent << "}" << getTrailingComment(node);
}

void NED2Generator::doLoop(LoopNode *node, const char *indent, bool islast, const char *sep)
{
    OUT << "for " << node->getParamName() << "=";
    printExpression(node, "from-value",indent);
    OUT << "..";
    printExpression(node, "to-value",indent);

    if (!islast)
        OUT << (sep ? sep : "");
}

void NED2Generator::doCondition(ConditionNode *node, const char *indent, bool islast, const char *sep)
{
    OUT << "if ";
    printExpression(node, "condition",indent);
    if (!islast)
        OUT << (sep ? sep : "");
}

void NED2Generator::printConnectionGate(NEDElement *conn, const char *modname, const char *modindexattr,
                                        const char *gatename, const char *gateindexattr, bool isplusplus,
                                        int gatesubg, const char *indent)
{
    if (!opp_isempty(modname)) {
        OUT << modname;
        printOptVector(conn, modindexattr,indent);
        OUT << ".";
    }

    OUT << gatename;
    switch (gatesubg)
    {
        case NED_SUBGATE_NONE: break;
        case NED_SUBGATE_I: OUT << "$i"; break;
        case NED_SUBGATE_O: OUT << "$o"; break;
        default: INTERNAL_ERROR0(conn, "wrong subg type");
    }
    if (isplusplus)
        OUT << "++";
    else
        printOptVector(conn, gateindexattr,indent);
}

void NED2Generator::doExpression(ExpressionNode *node, const char *indent, bool islast, const char *)
{
    generateChildren(node,indent);
}

int NED2Generator::getOperatorPriority(const char *op, int args)
{
    //
    // this method should always contain the same precendence rules as ebnf.y
    //

    if (args==3)
    {
        // %left '?' ':'
        if (!strcmp(op,"?:")) return 1;
        INTERNAL_ERROR1(NULL, "getOperatorPriority(): unknown tertiary operator '%s'", op);
    }

    if (args==2)
    {
        // %left AND OR XOR
        if (!strcmp(op,"&&")) return 2;
        if (!strcmp(op,"||")) return 2;
        if (!strcmp(op,"##")) return 2;

        // %left EQ NE GT GE LS LE
        if (!strcmp(op,"==")) return 3;
        if (!strcmp(op,"!=")) return 3;
        if (!strcmp(op,"<"))  return 3;
        if (!strcmp(op,">"))  return 3;
        if (!strcmp(op,"<=")) return 3;
        if (!strcmp(op,">=")) return 3;

        // %left BIN_AND BIN_OR BIN_XOR
        if (!strcmp(op,"&"))  return 4;
        if (!strcmp(op,"|"))  return 4;
        if (!strcmp(op,"#"))  return 4;

        // %left SHIFT_LEFT SHIFT_RIGHT
        if (!strcmp(op,"<<")) return 5;
        if (!strcmp(op,">>")) return 5;

        // %left PLUS MIN
        if (!strcmp(op,"+"))  return 6;
        if (!strcmp(op,"-"))  return 6;

        // %left MUL DIV MOD
        if (!strcmp(op,"*"))  return 7;
        if (!strcmp(op,"/"))  return 7;
        if (!strcmp(op,"%"))  return 7;

        // %right EXP
        if (!strcmp(op,"^"))  return 8;
        INTERNAL_ERROR1(NULL, "getOperatorPriority(): unknown binary operator '%s'", op);
    }

    if (args==1)
    {
        // %left UMIN NOT BIN_COMPL
        if (!strcmp(op,"-"))  return 9;
        if (!strcmp(op,"!"))  return 9;
        if (!strcmp(op,"~"))  return 9;
        INTERNAL_ERROR1(NULL, "getOperatorPriority(): unknown unary operator '%s'", op);
    }

    INTERNAL_ERROR1(NULL, "getOperatorPriority(): bad number of args: %d", args);
    return -1;
}

bool NED2Generator::isOperatorLeftAssoc(const char *op)
{
    // only exponentiation is right assoc, all others are left assoc
    if (!strcmp(op,"^")) return false;
    return true;
}

void NED2Generator::doOperator(OperatorNode *node, const char *indent, bool islast, const char *)
{
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    if (!op2)
    {
        // unary
        OUT << node->getName();
        generateNedItem(op1,indent,false,NULL);
    }
    else if (!op3)
    {
        // binary
        int prio = getOperatorPriority(node->getName(), 2);
        //bool leftassoc = isOperatorLeftAssoc(node->getName());

        bool needsParen = false;
        bool spacious = (prio<=2);  // we want spaces around &&, ||, ##

        NEDElement *parent = node->getParent();
        if (parent && parent->getTagCode()==NED_OPERATOR)
        {
            OperatorNode *parentop = (OperatorNode *)parent;
            int parentprio = getOperatorPriority(parentop->getName(), parentop->getNumChildren());
            if (parentprio>prio)
            {
                needsParen = true;
            }
            else if (parentprio==prio)
            {
                // TBD can be refined (make use of associativity & precedence rules)
                needsParen = true;
            }
        }

        if (needsParen) OUT << "(";
        generateNedItem(op1,indent,false,NULL);
        if (spacious)
            OUT << " " << node->getName() << " ";
        else
            OUT << node->getName();
        generateNedItem(op2,indent,false,NULL);
        if (needsParen) OUT << ")";
    }
    else
    {
        // tertiary
        bool needsParen = true; // TBD can be refined
        bool spacious = true; // TBD can be refined

        if (needsParen) OUT << "(";
        generateNedItem(op1,indent,false,NULL);
        OUT << (spacious ? " ? " : "?");
        generateNedItem(op2,indent,false,NULL);
        OUT << (spacious ? " : " : ":");
        generateNedItem(op3,indent,false,NULL);
        if (needsParen) OUT << ")";
    }
}

void NED2Generator::doFunction(FunctionNode *node, const char *indent, bool islast, const char *)
{
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    if (!strcmp(node->getName(), "index")) {
        OUT << node->getName();  // 'index' doesn't need parentheses
        return;
    }

    OUT << node->getName() << "(";
    if (op1) {
        generateNedItem(op1,indent,false,NULL);
    }
    if (op2) {
        OUT << ", ";
        generateNedItem(op2,indent,false,NULL);
    }
    if (op3) {
        OUT << ", ";
        generateNedItem(op3,indent,false,NULL);
    }
    OUT << ")";
}

void NED2Generator::doIdent(IdentNode *node, const char *indent, bool islast, const char *)
{
    if (!opp_isempty(node->getModule())) {
        OUT << node->getModule();
        if (node->getFirstChild()) {
            OUT << "[";
            generateChildren(node,indent,NULL);
            OUT << "]";
        }
        OUT << ".";
    }

    OUT << node->getName();
}

void NED2Generator::doLiteral(LiteralNode *node, const char *indent, bool islast, const char *sep)
{
    if (!opp_isempty(node->getText()))
    {
        OUT << node->getText();
    }
    else
    {
        // fallback: when original text is not present, use value
        if (node->getType()==NED_CONST_STRING)
            OUT << opp_quotestr(node->getValue());
        else
            OUT << node->getValue();
    }
    if (!islast)
        OUT << (sep ? sep : "");
}

void NED2Generator::doMsgFile(MsgFileNode *node, const char *indent, bool, const char *)
{
    OUT << getBannerComment(node, indent);
    generateChildren(node, indent);
}

void NED2Generator::doCplusplus(CplusplusNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "cplusplus {{" << node->getBody() << "}}" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void NED2Generator::doStructDecl(StructDeclNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "struct " << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void NED2Generator::doClassDecl(ClassDeclNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "class ";
    if (!node->getIsCobject()) OUT << "noncobject ";
    OUT << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void NED2Generator::doMessageDecl(MessageDeclNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "message " << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void NED2Generator::doEnumDecl(EnumDeclNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << "enum ";
    OUT << node->getName() << ";" << getRightComment(node);
    OUT << getTrailingComment(node);
}

void NED2Generator::doEnum(EnumNode *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doEnumFields(EnumFieldsNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << getRightComment(node);
    generateChildren(node, indent);
}

void NED2Generator::doEnumField(EnumFieldNode *node, const char *indent, bool islast, const char *)
{
    OUT << getBannerComment(node, indent);
    OUT << indent << node->getName();
    if (!opp_isempty(node->getValue()))
        OUT << " = " << node->getValue();
    OUT << ";" << getRightComment(node);
}

void NED2Generator::doMessage(MessageNode *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doClass(ClassNode *node, const char *indent, bool islast, const char *)
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

void NED2Generator::doStruct(StructNode *node, const char *indent, bool islast, const char *)
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
    // "node" must be a MessageNode, ClassNode or StructNode
    generateChildren(node, increaseIndent(indent));

    //if (msgfileVersion!=1)
    //    generateChildren(node, increaseIndent(indent)); // 4.x syntax
    //else
    //{
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
    //}
}

void NED2Generator::doField(FieldNode *node, const char *indent, bool islast, const char *)
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
    if (!opp_isempty(node->getEnumName()))
        OUT << " enum(" << node->getEnumName() << ")";
    if (!opp_isempty(node->getDefaultValue()))
        OUT << " = " << node->getDefaultValue();
    OUT << ";" << getRightComment(node);
}

void NED2Generator::doComment(CommentNode *node, const char *indent, bool islast, const char *)
{
    // ignore here: comments are taken care of individually where they occur
}

//---------------------------------------------------------------------------

void NED2Generator::generateNedItem(NEDElement *node, const char *indent, bool islast, const char *arg)
{
    // dispatch
    int tagcode = node->getTagCode();
    switch (tagcode)
    {
        case NED_FILES:
            doFiles((FilesNode *)node, indent, islast, arg); break;
        case NED_NED_FILE:
            doNedFile((NedFileNode *)node, indent, islast, arg); break;
        case NED_PACKAGE:
            doPackage((PackageNode *)node, indent, islast, arg); break;
        case NED_IMPORT:
            doImport((ImportNode *)node, indent, islast, arg); break;
        case NED_PROPERTY_DECL:
            doPropertyDecl((PropertyDeclNode *)node, indent, islast, arg); break;
        case NED_EXTENDS:
            doExtends((ExtendsNode *)node, indent, islast, arg); break;
        case NED_INTERFACE_NAME:
            doInterfaceName((InterfaceNameNode *)node, indent, islast, arg); break;
        case NED_SIMPLE_MODULE:
            doSimpleModule((SimpleModuleNode *)node, indent, islast, arg); break;
        case NED_MODULE_INTERFACE:
            doModuleInterface((ModuleInterfaceNode *)node, indent, islast, arg); break;
        case NED_COMPOUND_MODULE:
            doCompoundModule((CompoundModuleNode *)node, indent, islast, arg); break;
        case NED_CHANNEL_INTERFACE:
            doChannelInterface((ChannelInterfaceNode *)node, indent, islast, arg); break;
        case NED_CHANNEL:
            doChannel((ChannelNode *)node, indent, islast, arg); break;
        case NED_PARAMETERS:
            doParameters((ParametersNode *)node, indent, islast, arg); break;
        case NED_PARAM:
            doParam((ParamNode *)node, indent, islast, arg); break;
        case NED_PATTERN:
            doPattern((PatternNode *)node, indent, islast, arg); break;
        case NED_PROPERTY:
            doProperty((PropertyNode *)node, indent, islast, arg); break;
        case NED_PROPERTY_KEY:
            doPropertyKey((PropertyKeyNode *)node, indent, islast, arg); break;
        case NED_GATES:
            doGates((GatesNode *)node, indent, islast, arg); break;
        case NED_GATE:
            doGate((GateNode *)node, indent, islast, arg); break;
        case NED_TYPES:
            doTypes((TypesNode *)node, indent, islast, arg); break;
        case NED_SUBMODULES:
            doSubmodules((SubmodulesNode *)node, indent, islast, arg); break;
        case NED_SUBMODULE:
            doSubmodule((SubmoduleNode *)node, indent, islast, arg); break;
        case NED_CONNECTIONS:
            doConnections((ConnectionsNode *)node, indent, islast, arg); break;
        case NED_CONNECTION:
            doConnection((ConnectionNode *)node, indent, islast, arg); break;
        case NED_CHANNEL_SPEC:
            doChannelSpec((ChannelSpecNode *)node, indent, islast, arg); break;
        case NED_CONNECTION_GROUP:
            doConnectionGroup((ConnectionGroupNode *)node, indent, islast, arg); break;
        case NED_LOOP:
            doLoop((LoopNode *)node, indent, islast, arg); break;
        case NED_CONDITION:
            doCondition((ConditionNode *)node, indent, islast, arg); break;
        case NED_EXPRESSION:
            doExpression((ExpressionNode *)node, indent, islast, arg); break;
        case NED_OPERATOR:
            doOperator((OperatorNode *)node, indent, islast, arg); break;
        case NED_FUNCTION:
            doFunction((FunctionNode *)node, indent, islast, arg); break;
        case NED_IDENT:
            doIdent((IdentNode *)node, indent, islast, arg); break;
        case NED_LITERAL:
            doLiteral((LiteralNode *)node, indent, islast, arg); break;

        case NED_MSG_FILE:
            doMsgFile((MsgFileNode *)node, indent, islast, arg); break;
        case NED_CPLUSPLUS:
            doCplusplus((CplusplusNode *)node, indent, islast, arg); break;
        case NED_STRUCT_DECL:
            doStructDecl((StructDeclNode *)node, indent, islast, arg); break;
        case NED_CLASS_DECL:
            doClassDecl((ClassDeclNode *)node, indent, islast, arg); break;
        case NED_MESSAGE_DECL:
            doMessageDecl((MessageDeclNode *)node, indent, islast, arg); break;
        case NED_ENUM_DECL:
            doEnumDecl((EnumDeclNode *)node, indent, islast, arg); break;
        case NED_ENUM:
            doEnum((EnumNode *)node, indent, islast, arg); break;
        case NED_ENUM_FIELDS:
            doEnumFields((EnumFieldsNode *)node, indent, islast, arg); break;
        case NED_ENUM_FIELD:
            doEnumField((EnumFieldNode *)node, indent, islast, arg); break;
        case NED_MESSAGE:
            doMessage((MessageNode *)node, indent, islast, arg); break;
        case NED_CLASS:
            doClass((ClassNode *)node, indent, islast, arg); break;
        case NED_STRUCT:
            doStruct((StructNode *)node, indent, islast, arg); break;
        case NED_FIELD:
            doField((FieldNode *)node, indent, islast, arg); break;
        case NED_COMMENT:
            doComment((CommentNode *)node, indent, islast, arg); break;
        default:
            INTERNAL_ERROR1(node, "generateNedItem(): unknown tag '%s'", node->getTagName());
    }
}

NAMESPACE_END
