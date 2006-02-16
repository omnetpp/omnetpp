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
#include "nedgenerator.h"
#include "nederror.h"


void generateNed(ostream& out, NEDElement *node)
{
    NEDGenerator nedgen(out);
    nedgen.generate(node, "");
}

//-----------------------------------------

NEDGenerator::NEDGenerator(ostream& _out) : out(_out)
{
    indentsize = 4;
}

NEDGenerator::~NEDGenerator()
{
}

void NEDGenerator::setIndentSize(int indentsiz)
{
    indentsize = indentsiz;
}

void NEDGenerator::generate(NEDElement *node, const char *indent)
{
    generateNedItem(node, indent, false);
}

const char *NEDGenerator::increaseIndent(const char *indent)
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

const char *NEDGenerator::decreaseIndent(const char *indent)
{
    return indent + indentsize;
}

inline bool strnotnull(const char *s)
{
    return s && s[0];
}

//---------------------------------------------------------------------------

void NEDGenerator::generateChildren(NEDElement *node, const char *indent, const char *arg)
{
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        generateNedItem(child, indent, child==node->getLastChild(), arg);
    }
}

void NEDGenerator::generateChildrenWithType(NEDElement *node, int tagcode, const char *indent, const char *arg)
{
    // find last
    NEDElement *lastWithTag = NULL;
    for (NEDElement *child1=node->getFirstChild(); child1; child1=child1->getNextSibling())
    {
        if (child1->getTagCode()==tagcode)
             lastWithTag = child1;
    }

    // now the recursive call
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        if (child->getTagCode()==tagcode)
        {
            generateNedItem(child, indent, child==lastWithTag, arg);
        }
    }
}

//---------------------------------------------------------------------------

void NEDGenerator::printInheritance(NEDElement *node, const char *indent)
{
    if (node->getFirstChildWithTag(NED_EXTENDS))
    {
        out << " extends";
        generateChildrenWithType(node, NED_EXTENDS, increaseIndent(indent), ",");
    }
    if (node->getFirstChildWithTag(NED_INTERFACE_NAME))
    {
        out << " like";
        generateChildrenWithType(node, NED_INTERFACE_NAME, increaseIndent(indent), ",");
    }
}

bool NEDGenerator::hasExpression(NEDElement *node, const char *attr)
{
    if (strnotnull(node->getAttribute(attr)))
    {
        return true;
    }
    else
    {
        for (ExpressionNode *expr=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr=expr->getNextExpressionNodeSibling())
            if (strnotnull(expr->getTarget()) && !strcmp(expr->getTarget(),attr))
                return true;
        return false;
    }
}

void NEDGenerator::printExpression(NEDElement *node, const char *attr, const char *indent)
{
    if (strnotnull(node->getAttribute(attr)))
    {
        out << node->getAttribute(attr);
    }
    else
    {
        for (ExpressionNode *expr=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr=expr->getNextExpressionNodeSibling())
            if (strnotnull(expr->getTarget()) && !strcmp(expr->getTarget(),attr))
                generateNedItem(expr, indent, false, NULL);
    }
}

void NEDGenerator::printOptVector(NEDElement *node, const char *attr, const char *indent)
{
    if (hasExpression(node,attr))
    {
        out << "[";
        printExpression(node,attr,indent);
        out << "]";
    }
}

void NEDGenerator::printIfExpression(NEDElement *node, const char *attr, const char *indent)
{
    out << "ifXXX ";
    printExpression(node,attr,indent);
}


//---------------------------------------------------------------------------

void NEDGenerator::appendBannerComment(const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    out << comment;
}

void NEDGenerator::appendRightComment(const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    out << comment;
}

void NEDGenerator::appendInlineRightComment(const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    out << comment;
}

void NEDGenerator::appendTrailingComment(const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    out << comment;
}

//---------------------------------------------------------------------------

void NEDGenerator::doNedfiles(FilesNode *node, const char *indent, bool, const char *)
{
    generateChildren(node, indent);
}

void NEDGenerator::doNedfile(NedFileNode *node, const char *indent, bool, const char *)
{
    //XXX appendBannerComment(node->getBannerComment(), indent);
    generateChildren(node, indent);
}

void NEDGenerator::doImport(ImportNode *node, const char *indent, bool islast, const char *)
{
    //XXX appendBannerComment(node->getBannerComment(), indent);
    out << indent << "import \"" << node->getFilename() << "\";" << "\n";
}

void NEDGenerator::doPropertyDecl(PropertyDeclNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "property @" << node->getName();
    if (node->getFirstChildWithTag(NED_PROPERTY_KEY))
    {
        out << "(";
        generateChildrenWithType(node, NED_PROPERTY_KEY, increaseIndent(indent), ",");
        out << ")";
    }
    out << ";\n\n";
}

void NEDGenerator::doExtends(ExtendsNode *node, const char *indent, bool islast, const char *sep)
{
    out << " " << node->getName();
    if (!islast && sep)
        out << sep;
}

void NEDGenerator::doInterfaceName(InterfaceNameNode *node, const char *indent, bool islast, const char *sep)
{
    out << " " << node->getName();
    if (!islast && sep)
        out << sep;
}

void NEDGenerator::doSimpleModule(SimpleModuleNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "simple " << node->getName();
    printInheritance(node, indent);
    out << "\n" << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));

    out << indent << "}\n\n";
}

void NEDGenerator::doModuleInterface(ModuleInterfaceNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "interface " << node->getName();
    printInheritance(node, indent);
    out << "\n" << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));

    out << indent << "}\n\n";
}

void NEDGenerator::doCompoundModule(CompoundModuleNode *node, const char *indent, bool islast, const char *)
{
    out << indent << (node->getIsNetwork() ? "network" : "module") << " " << node->getName();
    printInheritance(node, indent);
    out << "\n" << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));
    generateChildrenWithType(node, NED_TYPES, increaseIndent(indent));
    generateChildrenWithType(node, NED_SUBMODULES, increaseIndent(indent));
    generateChildrenWithType(node, NED_CONNECTIONS, increaseIndent(indent));

    out << indent << "}\n\n";
}

void NEDGenerator::doChannelInterface(ChannelInterfaceNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "channelinterface " << node->getName();
    printInheritance(node, indent);
    out << "\n" << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));

    out << indent << "}\n\n";
}

void NEDGenerator::doChannel(ChannelNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "channel ";
    if (node->getIsWithcppclass())
        out << "withcppclass ";
    out << node->getName();
    printInheritance(node, indent);
    out << "\n" << indent << "{\n";

    generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));

    out << indent << "}\n\n";
}

void NEDGenerator::doParameters(ParametersNode *node, const char *indent, bool islast, const char *)
{
    if (!node->getIsImplicit())
        out << indent << "parameters:\n";
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doParamGroup(ParamGroupNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "{\n";
    generateChildren(node, increaseIndent(indent));
    out << indent << "}\n";
}

void NEDGenerator::doParam(ParamNode *node, const char *indent, bool islast, const char *)
{
    out << indent;
    switch (node->getType())
    {
        case NED_PARTYPE_NONE:   break;
        case NED_PARTYPE_DOUBLE: out << "double "; break;
        case NED_PARTYPE_INT:    out << "int "; break;
        case NED_PARTYPE_STRING: out << "string "; break;
        case NED_PARTYPE_BOOL:   out << "bool "; break;
        case NED_PARTYPE_XML:    out << "xml "; break;
        default: INTERNAL_ERROR0(node, "wrong type");
    }
    if (node->getIsFunction())
        out << "function ";
    out << node->getName();
    if (hasExpression(node,"value"))
    {
        out << " = ";
        printExpression(node, "value",indent);
    }

    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");
    generateChildrenWithType(node, NED_CONDITION, increaseIndent(indent));
    out << ";\n";
}

void NEDGenerator::doPattern(PatternNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "/" << node->getPattern() << "/ = ";
    printExpression(node, "value",indent);

    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");
    generateChildrenWithType(node, NED_CONDITION, increaseIndent(indent));
    out << ";\n";
}

void NEDGenerator::doProperty(PropertyNode *node, const char *indent, bool islast, const char *sep)
{
    if (!node->getIsImplicit())
    {
        // if sep==NULL, print as standalone property (with indent and ";"), otherwise as inline property
        if (!sep)
            out << indent;
        out << "@" << node->getName();
        if (node->getFirstChildWithTag(NED_PROPERTY_KEY))
        {
            out << "(";
            generateChildrenWithType(node, NED_PROPERTY_KEY, increaseIndent(indent), ",");
            out << ")";
        }
        if (!sep)
            out << ";\n";
        else if (!islast)
            out << sep;
    }
}

void NEDGenerator::doPropertyKey(PropertyKeyNode *node, const char *indent, bool islast, const char *sep)
{
    out << node->getKey();
    if (node->getFirstChildWithTag(NED_LITERAL))
    {
        if (strnotnull(node->getKey()))
            out << "=";
        generateChildrenWithType(node, NED_LITERAL, increaseIndent(indent),",");
    }
    if (!islast && sep)
        out << sep;
}

void NEDGenerator::doGates(GatesNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "gates:\n";
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doGateGroup(GateGroupNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "{\n";
    generateChildren(node, increaseIndent(indent));
    out << indent << "}\n";
}

void NEDGenerator::doGate(GateNode *node, const char *indent, bool islast, const char *)
{
    out << indent;
    switch (node->getType())
    {
        case NED_GATETYPE_NONE:   break;
        case NED_GATETYPE_INPUT:  out << "input "; break;
        case NED_GATETYPE_OUTPUT: out << "output "; break;
        case NED_GATETYPE_INOUT:  out << "inout "; break;
        default: INTERNAL_ERROR0(node, "wrong type");
    }
    out << node->getName();
    if (node->getIsVector())
        out << "[]";
    printOptVector(node, "vector-size",indent);

    generateChildrenWithType(node, NED_PROPERTY, increaseIndent(indent), " ");
    generateChildrenWithType(node, NED_CONDITION, increaseIndent(indent));
    out << ";\n";
}

void NEDGenerator::doTypes(TypesNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "types:\n";
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doSubmodules(SubmodulesNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "submodules:\n";
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doSubmodule(SubmoduleNode *node, const char *indent, bool islast, const char *)
{
    out << indent << node->getName();
    printOptVector(node, "vector-size",indent);
    out << ": ";

    if (node->getLikeAny() || strnotnull(node->getLikeType()))
    {
        // "like" version
        out << "<";
        if (strnotnull(node->getLikeParam()))
            printExpression(node, "like-param", indent); // this (incidentally) also works if like-param contains a property (ie. starts with "@")
        out << ">";

        if (strnotnull(node->getLikeType()))
            out << " like " << node->getLikeType();
        if (node->getLikeAny())
            out << " like *";
    }
    else
    {
        // "like"-less
        out << node->getType();
    }

    if (!node->getFirstChildWithTag(NED_PARAMETERS) && !node->getFirstChildWithTag(NED_GATES))
    {
        out << ";\n";
    }
    else
    {
        out << " {\n";
        generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
        generateChildrenWithType(node, NED_GATES, increaseIndent(indent));
        out << indent << "}\n";
    }
}

void NEDGenerator::doConnections(ConnectionsNode *node, const char *indent, bool islast, const char *)
{
    if (node->getAllowUnconnected()) {
        out << indent << "connections allowunconnected:\n";
    } else {
        out << indent << "connections:\n";
    }
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doConnection(ConnectionNode *node, const char *indent, bool islast, const char *)
{
    //  direction
    const char *arrow;
    bool srcfirst;
    switch (node->getArrowDirection())
    {
        case NED_ARROWDIR_L2R:   arrow = " -->"; srcfirst = true; break;
        case NED_ARROWDIR_R2L:   arrow = " <--"; srcfirst = false; break;
        case NED_ARROWDIR_BIDIR: arrow = " <-->"; srcfirst = true; break;
        default: INTERNAL_ERROR0(node, "wrong arrow-dir");
    }

    // print src
    out << indent;
    if (srcfirst) {
        printGate(node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", node->getSrcGatePlusplus(), indent);
    } else {
        printGate(node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", node->getDestGatePlusplus(), indent);
    }

    // arrow
    out << arrow;

    // print channel attributes
    if (node->getFirstChildWithTag(NED_CHANNEL_SPEC))
    {
        generateChildrenWithType(node, NED_CHANNEL_SPEC, indent);
        out << arrow;
    }

    // print dest
    out << " ";
    if (srcfirst) {
        printGate(node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", node->getDestGatePlusplus(), indent);
    } else {
        printGate(node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", node->getSrcGatePlusplus(), indent);
    }

    out << ";\n";
}

void NEDGenerator::doChannelSpec(ChannelSpecNode *node, const char *indent, bool islast, const char *)
{
    if (node->getLikeAny() || strnotnull(node->getLikeType()))
    {
        // "like" version
        out << " <";
        if (strnotnull(node->getLikeParam()))
            printExpression(node, "like-param", indent); // this (incidentally) also works if like-param contains a property (ie. starts with "@")
        out << ">";

        if (strnotnull(node->getLikeType()))
            out << " like " << node->getLikeType();
        if (node->getLikeAny())
            out << " like *";
    }
    else if (strnotnull(node->getType()))
    {
        // concrete channel type
        out << " " << node->getType();
    }

    if (node->getFirstChildWithTag(NED_PARAMETERS))
    {
        out << " { ";
        generateChildrenWithType(node, NED_PARAMETERS, increaseIndent(indent));
        out << indent << " }";
    }
}

void NEDGenerator::doConnectionGroup(ConnectionGroupNode *node, const char *indent, bool islast, const char *)
{
    out << indent << "{\n";
    generateChildren(node, increaseIndent(indent));
    out << indent << "}\n";
    generateChildrenWithType(node, NED_WHERE, increaseIndent(indent));
}

void NEDGenerator::doWhere(WhereNode *node, const char *indent, bool islast, const char *)
{
    out << "where ";
    generateChildren(node, indent);
}

void NEDGenerator::doLoop(LoopNode *node, const char *indent, bool islast, const char *)
{
    out << node->getParamName() << "=";
    printExpression(node, "from-value",indent);
    out << "..";
    printExpression(node, "to-value",indent);
}

void NEDGenerator::doCondition(ConditionNode *node, const char *indent, bool islast, const char *)
{
    out << "if ";
    printExpression(node, "condition",indent);
}

void NEDGenerator::printGate(NEDElement *conn, const char *modname, const char *modindexattr,
                             const char *gatename, const char *gateindexattr, bool isplusplus,
                             const char *indent)
{
    if (strnotnull(modname)) {
        out << modname;
        printOptVector(conn, modindexattr,indent);
        out << ".";
    }

    out << gatename;
    if (isplusplus)
        out << "++";
    else
        printOptVector(conn, gateindexattr,indent);
}

/*XXX
void NEDGenerator::doGate(GateNode *node, const char *indent, bool islast, const char *)
{
    //XXX appendBannerComment(node->getBannerComment(), indent);
    out << indent << (node->getType()==NED_GATETYPE_INPUT ? "in: " : "out: ") << node->getName();
    if (node->getIsVector()) {
        out << "[]";
    }
    out << ";";
    //XXX appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doSubstparams(SubstparamsNode *node, const char *indent, bool islast, const char *)
{
    //XXX appendBannerComment(node->getBannerComment(), indent);
    out << indent << "parameters";
    printIfExpression(node, "condition",indent);
    out << ":";

    //XXX appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doSubstparam(SubstparamNode *node, const char *indent, bool islast, const char *)
{
    //XXX appendBannerComment(node->getBannerComment(), indent);
    out << indent << node->getName() << " = ";
    printExpression(node, "value",indent);
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    //XXX appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doGatesizes(GatesizesNode *node, const char *indent, bool islast, const char *)
{
    //XXX appendBannerComment(node->getBannerComment(), indent);
    out << indent << "gatesizes";
    printIfExpression(node, "condition",indent);
    out << ":";

    //XXX appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doGatesize(GatesizeNode *node, const char *indent, bool islast, const char *)
{
    //XXX appendBannerComment(node->getBannerComment(), indent);
    out << indent << node->getName();
    printOptVector(node, "vector-size",indent);
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    //XXX appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doConnections(ConnectionsNode *node, const char *indent, bool islast, const char *)
{
    //XXX appendBannerComment(node->getBannerComment(), indent);
    if (!node->getCheckUnconnected()) {
        out << indent << "connections nocheck:";
    } else {
        out << indent << "connections:";
    }
    //XXX appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::printGate(NEDElement *conn, const char *modname, const char *modindexattr,
                             const char *gatename, const char *gateindexattr, bool isplusplus,
                             const char *indent)
{
    if (strnotnull(modname)) {
        out << modname;
        printOptVector(conn, modindexattr,indent);
        out << ".";
    }

    out << gatename;
    if (isplusplus)
        out << "++";
    else
        printOptVector(conn, gateindexattr,indent);
}

void NEDGenerator::doConnattr(ConnAttrNode *node, const char *indent, bool islast, const char *arrow)
{
    if (!strcmp(node->getName(),"channel")) {
        // must look like this:
        //     <conn-attr name="channel" value="mychanneltype">
        // or like this:
        //     <conn-attr name="channel">
        //         <expression target="value">
        //             <const type="string" value="mychanneltype"/>
        //         </expression>
        //     </conn-attr>
        NEDElement *expr=node->getFirstChildWithTag(NED_EXPRESSION);
        LiteralNode *aconst = (LiteralNode *)(expr ? expr->getFirstChildWithTag(NED_LITERAL) : NULL);
        bool exprOk = aconst && aconst->getType()==NED_CONST_STRING;
        const char *channelname = exprOk ? aconst->getValue() : node->getValue();
        out << " " << channelname << " ";
    } else {
        out << " " << node->getName() << " ";
        printExpression(node, "value",indent);
    }
    appendInlineRightComment(node->getRightComment(), "");
    if (islast) {
        out << arrow;
    }
}
*/

void NEDGenerator::doExpression(ExpressionNode *node, const char *indent, bool islast, const char *)
{
    generateChildren(node,indent);
}

int NEDGenerator::getOperatorPriority(const char *op, int args)
{
    //
    // this method should always contain the same precendence rules as ebnf.y
    //

    if (args==3)
    {
        // %left '?' ':'
        if (!strcmp(op,"?:")) return 1;
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
    }

    if (args==1)
    {
        // %left UMIN NOT BIN_COMPL
        if (!strcmp(op,"-"))  return 9;
        if (!strcmp(op,"!"))  return 9;
        if (!strcmp(op,"~"))  return 9;
    }
    INTERNAL_ERROR1(NULL, "getOperatorPriority(): unknown operator '%s'", op);
    return -1;
}

bool NEDGenerator::isOperatorLeftAssoc(const char *op)
{
    // only exponentiation is right assoc, all others are left assoc
    if (!strcmp(op,"^")) return false;
    return true;
}

void NEDGenerator::doOperator(OperatorNode *node, const char *indent, bool islast, const char *)
{
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    if (!op2)
    {
        // unary
        out << node->getName();
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
        if (parent && parent->getTagCode()==NED_OPERATOR) {
            OperatorNode *parentop = (OperatorNode *)parent;
            int parentprio = getOperatorPriority(parentop->getName(),2 ); //FIXME 2 ???
            if (parentprio>prio) {
                needsParen = true;
            } else if (parentprio==prio) {
                // TBD can be refined (make use of associativity & precedence rules)
                needsParen = true;
            }
        }

        if (needsParen) out << "(";
        generateNedItem(op1,indent,false,NULL);
        if (spacious)
            out << " " << node->getName() << " ";
        else
            out << node->getName();
        generateNedItem(op2,indent,false,NULL);
        if (needsParen) out << ")";
    }
    else
    {
        // tertiary
        bool needsParen = true; // TBD can be refined
        bool spacious = true; // TBD can be refined

        if (needsParen) out << "(";
        generateNedItem(op1,indent,false,NULL);
        out << (spacious ? " ? " : "?");
        generateNedItem(op1,indent,false,NULL);
        out << (spacious ? " : " : ":");
        generateNedItem(op1,indent,false,NULL);
        if (needsParen) out << ")";
    }
}

void NEDGenerator::doFunction(FunctionNode *node, const char *indent, bool islast, const char *)
{
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    if (!strcmp(node->getName(), "index")) {
        out << node->getName();  // 'index' doesn't need parentheses
        return;
    }

    out << node->getName() << "(";
    if (op1) {
        generateNedItem(op1,indent,false,NULL);
    }
    if (op2) {
        out << ", ";
        generateNedItem(op2,indent,false,NULL);
    }
    if (op3) {
        out << ", ";
        generateNedItem(op3,indent,false,NULL);
    }
    out << ")";
}

void NEDGenerator::doIdent(IdentNode *node, const char *indent, bool islast, const char *)
{
    if (strnotnull(node->getModule())) {
        out << node->getModule();
        printOptVector(node, "module-index", indent);
        out << ".";
    }

    out << node->getName();
}

void NEDGenerator::doLiteral(LiteralNode *node, const char *indent, bool islast, const char *)
{
    if (strnotnull(node->getText()))
    {
        out << node->getText();
    }
    else
    {
        // fallback: when original text is not present, use value
        bool isstring = (node->getType()==NED_CONST_STRING);
        if (isstring) out << "\"";
        out << node->getValue();
        if (isstring) out << "\"";
    }
}

void NEDGenerator::doCplusplus(CplusplusNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "cplusplus {{" << node->getBody() << "}}";
    appendRightComment(node->getRightComment(), indent);
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doStructDecl(StructDeclNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "struct " << node->getName() << ";";
    appendRightComment(node->getRightComment(), indent);
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doClassDecl(ClassDeclNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "class ";
    if (!node->getIsCobject()) out << "noncobject ";
    out << node->getName() << ";";
    appendRightComment(node->getRightComment(), indent);
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doMessageDecl(MessageDeclNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "message " << node->getName() << ";";
    appendRightComment(node->getRightComment(), indent);
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doEnumDecl(EnumDeclNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "enum ";
    out << node->getName() << ";";
    appendRightComment(node->getRightComment(), indent);
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doEnum(EnumNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "enum " << node->getName();
    if (strnotnull(node->getExtendsName()))
        out << " extends " << node->getExtendsName();
    appendRightComment(node->getRightComment(), indent);
    out << indent << "{\n";
    generateChildren(node, increaseIndent(indent));
    out << indent << "};";
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doEnumFields(EnumFieldsNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    appendRightComment(node->getRightComment(), indent);
    generateChildren(node, indent);
}

void NEDGenerator::doEnumField(EnumFieldNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << node->getName();
    if (strnotnull(node->getValue()))
        out << " = " << node->getValue();
    out << ";";
    appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doMessage(MessageNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "message " << node->getName();
    if (strnotnull(node->getExtendsName()))
        out << " extends " << node->getExtendsName();
    appendRightComment(node->getRightComment(), indent);
    out << indent << "{\n";
    generateChildren(node, increaseIndent(indent));
    out << indent << "};";
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doClass(ClassNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "class " << node->getName();
    if (strnotnull(node->getExtendsName()))
        out << " extends " << node->getExtendsName();
    appendRightComment(node->getRightComment(), indent);
    out << indent << "{\n";
    generateChildren(node, increaseIndent(indent));
    out << indent << "};";
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doStruct(StructNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "struct " << node->getName();
    if (strnotnull(node->getExtendsName()))
        out << " extends " << node->getExtendsName();
    appendRightComment(node->getRightComment(), indent);
    out << indent << "{\n";
    generateChildren(node, increaseIndent(indent));
    out << indent << "};";
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doFields(FieldsNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "fields:";
    appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doField(FieldNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent;
    if (node->getIsAbstract())
        out << "abstract ";
    if (node->getIsReadonly())
        out << "readonly ";
    if (strnotnull(node->getDataType()))
        out << node->getDataType() << " ";
    out << node->getName();
    if (node->getIsVector() && strnotnull(node->getVectorSize()))
        out << "[" << node->getVectorSize() << "]";
    else if (node->getIsVector())
        out << "[]";
    if (strnotnull(node->getEnumName()))
        out << " enum(" << node->getEnumName() << ")";
    if (strnotnull(node->getDefaultValue()))
        out << " = " << node->getDefaultValue();
    out << ";";
    appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doProperties(PropertiesNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "properties:";
    appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doMsgproperty(MsgpropertyNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << node->getName() << " = " << node->getValue() << ";";
    appendRightComment(node->getRightComment(), indent);
}


//---------------------------------------------------------------------------

void NEDGenerator::generateNedItem(NEDElement *node, const char *indent, bool islast, const char *arg)
{
    // dispatch
    int tagcode = node->getTagCode();
    switch (tagcode)
    {
        case NED_FILES:
            doNedfiles((FilesNode *)node, indent, islast, arg); break;
        case NED_NED_FILE:
            doNedfile((NedFileNode *)node, indent, islast, arg); break;
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
        case NED_PARAM_GROUP:
            doParamGroup((ParamGroupNode *)node, indent, islast, arg); break;
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
        case NED_GATE_GROUP:
            doGateGroup((GateGroupNode *)node, indent, islast, arg); break;
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
        case NED_WHERE:
            doWhere((WhereNode *)node, indent, islast, arg); break;
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
        case NED_FIELDS:
            doFields((FieldsNode *)node, indent, islast, arg); break;
        case NED_FIELD:
            doField((FieldNode *)node, indent, islast, arg); break;
        case NED_PROPERTIES:
            doProperties((PropertiesNode *)node, indent, islast, arg); break;
        case NED_MSGPROPERTY:
            doMsgproperty((MsgpropertyNode *)node, indent, islast, arg); break;
        default:
            ;//XXX: add back this line: INTERNAL_ERROR1(node, "generateNedItem(): unknown tag '%s'", node->getTagName());
    }
}


