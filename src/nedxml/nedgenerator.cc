//==========================================================================
//   NEDGENERATOR.CC -
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


#include <string.h>
#include "nedgenerator.h"
#include "nederror.h"


void generateNed(ostream& out, NEDElement *node, bool newsyntax)
{
    NEDGenerator nedgen;
    nedgen.setNewSyntax(newsyntax);
    nedgen.generate(out, node, "");
}

//-----------------------------------------

NEDGenerator::NEDGenerator()
{
    newsyntax = true;
    indentsize = 4;
}

NEDGenerator::~NEDGenerator()
{
}

void NEDGenerator::setNewSyntax(bool value)
{
    newsyntax = value;
}

void NEDGenerator::setIndentSize(int indentsiz)
{
    indentsize = indentsiz;
}

void NEDGenerator::generate(ostream& out, NEDElement *node, const char *indent)
{
    generateNedItem(out, node, indent, false);
}

const char *NEDGenerator::increaseIndent(const char *indent)
{
    // biggest possible indent: ~70 chars:
    static char spaces[] = "                                                                     ";

    // bump...
    int ilen = strlen(indent);
    if (ilen+indentsize <= sizeof(spaces)-1)
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

void NEDGenerator::generateChildren(ostream& out, NEDElement *node, const char *indent, const char *arg)
{
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        generateNedItem(out, child, indent, child==node->getLastChild(), arg);
    }
}

void NEDGenerator::generateChildrenWithType(ostream& out, NEDElement *node, int tagcode, const char *indent, const char *arg)
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
            generateNedItem(out, child, indent, child==lastWithTag, arg);
        }
    }
}

//---------------------------------------------------------------------------

void NEDGenerator::printExpression(ostream& out, NEDElement *node, const char *attr, const char *indent)
{
    if (strnotnull(node->getAttribute(attr)))
    {
        out << node->getAttribute(attr);
    }
    else
    {
        for (ExpressionNode *expr=(ExpressionNode *)node->getFirstChildWithTag(NED_EXPRESSION); expr; expr=expr->getNextExpressionNodeSibling())
        {
            if (strnotnull(expr->getTarget()) && !strcmp(expr->getTarget(),attr))
            {
                generateNedItem(out, expr, indent, false, NULL);
            }
        }
    }
}

void NEDGenerator::printVector(ostream& out, NEDElement *node, const char *attr, const char *indent)
{
    if (strnotnull(node->getAttribute(attr)))
    {
        out << "[" << node->getAttribute(attr) << "]";
    }
    else
    {
        for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        {
            if (child->getTagCode()==NED_EXPRESSION &&
                 strnotnull(((ExpressionNode *)child)->getTarget()) &&
                 !strcmp(((ExpressionNode *)child)->getTarget(),attr))
            {
                out << "[";
                generateNedItem(out, child, indent, false, NULL);
                out << "]";
            }
        }
    }
}

void NEDGenerator::printIfExpression(ostream& out, NEDElement *node, const char *attr, const char *indent)
{
    if (strnotnull(node->getAttribute(attr)))
    {
        out << " if " << node->getAttribute(attr);
    }
    else
    {
        for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        {
            if (child->getTagCode()==NED_EXPRESSION &&
                 strnotnull(((ExpressionNode *)child)->getTarget()) &&
                 !strcmp(((ExpressionNode *)child)->getTarget(),attr))
            {
                out << " if ";
                generateNedItem(out, child, indent, false, NULL);
            }
        }
    }
}


//---------------------------------------------------------------------------

void NEDGenerator::appendBannerComment(ostream& out, const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    out << comment;
}

void NEDGenerator::appendRightComment(ostream& out, const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    out << comment;
}

void NEDGenerator::appendInlineRightComment(ostream& out, const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    out << comment;
}

void NEDGenerator::appendTrailingComment(ostream& out, const char *comment, const char *indent)
{
    if (!strnotnull(comment))
        return;

    out << comment;
}

//---------------------------------------------------------------------------

void NEDGenerator::doNedfiles(ostream& out, NedFilesNode *node, const char *indent, bool, const char *)
{
    generateChildren(out, node, indent);
}

void NEDGenerator::doNedfile(ostream& out, NedFileNode *node, const char *indent, bool, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    generateChildren(out, node, indent);
}

void NEDGenerator::doImports(ostream& out, ImportNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "import ";
    appendRightComment(out, node->getRightComment(), indent);
    generateChildren(out, node, increaseIndent(indent));
    appendTrailingComment(out, node->getTrailingComment(), "");
}

void NEDGenerator::doImport(ostream& out, ImportedFileNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "\"" << node->getFilename() << "\"";
    if (islast) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(out, node->getRightComment(), indent);
}

void NEDGenerator::doChannel(ostream& out, ChannelNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "channel " << node->getName() << "\n";
    if (newsyntax) out << indent << "{";
    appendRightComment(out, node->getRightComment(), indent);
    generateChildren(out, node, increaseIndent(indent));
    out << indent << (newsyntax?"}":"endchannel");
    appendTrailingComment(out, node->getTrailingComment(), "");
}

void NEDGenerator::doChanattr(ostream& out, ChannelAttrNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "" << node->getName() << " ";
    printExpression(out, node, "value",indent);
    out << ";";
    appendRightComment(out, node->getRightComment(), indent);
}

void NEDGenerator::doNetwork(ostream& out, NetworkNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "network " << node->getName() << " : " << node->getTypeName() << "\n";
    if (newsyntax) out << indent << "{";
    appendRightComment(out, node->getRightComment(), indent);

    generateChildrenWithType(out, node, NED_SUBSTMACHINES, increaseIndent(indent));
    generateChildrenWithType(out, node, NED_SUBSTPARAMS, increaseIndent(indent));

    out << indent << (newsyntax?"}":"endnetwork");
    appendTrailingComment(out, node->getTrailingComment(), "");
}

void NEDGenerator::doSimple(ostream& out, SimpleModuleNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "simple " << node->getName() << "\n";
    if (newsyntax) out << indent << "{";
    appendRightComment(out, node->getRightComment(), indent);

    generateChildrenWithType(out, node, NED_MACHINES, increaseIndent(indent));
    generateChildrenWithType(out, node, NED_PARAMS, increaseIndent(indent));
    generateChildrenWithType(out, node, NED_GATES, increaseIndent(indent));

    out << indent << (newsyntax?"}":"endsimple");
    appendTrailingComment(out, node->getTrailingComment(), "");
}

void NEDGenerator::doModule(ostream& out, CompoundModuleNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "module " << node->getName() << "\n";
    if (newsyntax) out << indent << "{";
    appendRightComment(out, node->getRightComment(), indent);

    generateChildrenWithType(out, node, NED_MACHINES, increaseIndent(indent));
    generateChildrenWithType(out, node, NED_PARAMS, increaseIndent(indent));
    generateChildrenWithType(out, node, NED_GATES, increaseIndent(indent));
    generateChildrenWithType(out, node, NED_SUBMODULES, increaseIndent(indent));
    generateChildrenWithType(out, node, NED_CONNECTIONS, increaseIndent(indent));
    generateChildrenWithType(out, node, NED_DISPLAY_STRING, increaseIndent(indent));

    out << indent << (newsyntax?"}":"endmodule");
    appendTrailingComment(out, node->getTrailingComment(), "");
}

void NEDGenerator::doParams(ostream& out, ParamsNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "parameters:";
    appendRightComment(out, node->getRightComment(), indent);
    generateChildren(out, node, increaseIndent(indent));
}

void NEDGenerator::doParam(ostream& out, ParamNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "" << node->getName() << " : " << node->getDataType();
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(out, node->getRightComment(), indent);
}

void NEDGenerator::doGates(ostream& out, GatesNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "gates:";
    appendRightComment(out, node->getRightComment(), indent);
    generateChildren(out, node, increaseIndent(indent));
}

void NEDGenerator::doGate(ostream& out, GateNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << (node->getDirection()==NED_GATEDIR_INPUT ? "in: " : "out: ") << node->getName();
    if (node->getIsVector()) {
        out << "[]";
    }
    out << ";";
    appendRightComment(out, node->getRightComment(), indent);
}

void NEDGenerator::doMachines(ostream& out, MachinesNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "machines:";
    appendRightComment(out, node->getRightComment(), indent);
    generateChildren(out, node, increaseIndent(indent));
}

void NEDGenerator::doMachine(ostream& out, MachineNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "" << node->getName();
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(out, node->getRightComment(), indent);
}

void NEDGenerator::doSubmodules(ostream& out, SubmodulesNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "submodules:";
    appendRightComment(out, node->getRightComment(), indent);
    generateChildren(out, node, increaseIndent(indent));
}

void NEDGenerator::doSubmodule(ostream& out, SubmoduleNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << node->getName() << ": " << node->getTypeName();
    printVector(out, node, "vector-size",indent);
    if (strnotnull(node->getLikeName())) {
        out << " like " << node->getLikeName();
    }
    out << ";";
    appendRightComment(out, node->getRightComment(), indent);

    generateChildrenWithType(out, node, NED_SUBSTMACHINES, increaseIndent(indent));
    generateChildrenWithType(out, node, NED_SUBSTPARAMS, increaseIndent(indent));
    generateChildrenWithType(out, node, NED_GATESIZES, increaseIndent(indent));
    generateChildrenWithType(out, node, NED_DISPLAY_STRING, increaseIndent(indent));
}

void NEDGenerator::doSubstparams(ostream& out, SubstparamsNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "parameters";
    printIfExpression(out, node, "condition",indent);
    out << ":";

    appendRightComment(out, node->getRightComment(), indent);
    generateChildren(out, node, increaseIndent(indent));
}

void NEDGenerator::doSubstparam(ostream& out, SubstparamNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "" << node->getName() << " = ";
    printExpression(out, node, "value",indent);
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(out, node->getRightComment(), indent);
}

void NEDGenerator::doGatesizes(ostream& out, GatesizesNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "gatesizes";
    printIfExpression(out, node, "condition",indent);
    out << ":";

    appendRightComment(out, node->getRightComment(), indent);
    generateChildren(out, node, increaseIndent(indent));
}

void NEDGenerator::doGatesize(ostream& out, GatesizeNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "" << node->getName();
    printVector(out, node, "vector-size",indent);
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(out, node->getRightComment(), indent);
}

void NEDGenerator::doSubstmachines(ostream& out, SubstmachinesNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "on";
    printIfExpression(out, node, "condition",indent);
    out << ":";

    appendRightComment(out, node->getRightComment(), indent);
    generateChildren(out, node, increaseIndent(indent));
}

void NEDGenerator::doSubstmachine(ostream& out, SubstmachineNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "" << node->getName();
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(out, node->getRightComment(), indent);
}

void NEDGenerator::doConnections(ostream& out, ConnectionsNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    if (!node->getCheckUnconnected()) {
        out << indent << "connections nocheck:";
    } else {
        out << indent << "connections:";
    }
    appendRightComment(out, node->getRightComment(), indent);
    generateChildren(out, node, increaseIndent(indent));
}

void NEDGenerator::printGate(ostream& out, NEDElement *conn, const char *modname, const char *modindexattr,
                             const char *gatename, const char *gateindexattr, const char *indent)
{
    if (strnotnull(modname)) {
        out << modname;
        printVector(out, conn, modindexattr,indent);
        out << ".";
    }

    out << gatename;
    printVector(out, conn, gateindexattr,indent);
}

void NEDGenerator::doConnection(ostream& out, ConnectionNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);

    //  direction
    const char *arrow;
    bool srcfirst;
    if (node->getArrowDirection()==NED_ARROWDIR_RIGHT) {
        arrow = " -->";
        srcfirst = true;
    } else {
        arrow = " <--";
        srcfirst = false;
    }

    // print src
    out << indent;
    if (srcfirst) {
        printGate(out, node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", indent);
    } else {
        printGate(out, node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", indent);
    }

    // arrow
    out << arrow;

    // print channel attributes
    generateChildrenWithType(out, node, NED_CONN_ATTR, indent, arrow);

    // print dest
    out << " ";
    if (srcfirst) {
        printGate(out, node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", indent);
    } else {
        printGate(out, node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", indent);
    }

    // print 'if' clause
    printIfExpression(out, node, "condition",indent);

    // print display string
    DisplayStringNode *dispstr = (DisplayStringNode *) node->getFirstChildWithTag(NED_DISPLAY_STRING);
    if (dispstr)
    {
        out << " display \"" << dispstr->getValue() << "\"";
    }

    out << ";";

    // print comments
    appendRightComment(out, node->getRightComment(), indent);
}

void NEDGenerator::doConnattr(ostream& out, ConnAttrNode *node, const char *indent, bool islast, const char *arrow)
{
    if (!strcmp(node->getName(),"channel")) {
        //  sorry for the special case :-(
        printExpression(out, node, "value",indent);
    } else {
        out << " " << node->getName() << " ";
        printExpression(out, node, "value",indent);
    }
    appendInlineRightComment(out, node->getRightComment(), "");
    if (islast) {
        out << arrow;
    }
}

void NEDGenerator::doForloop(ostream& out, ForLoopNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "for ";
    generateChildrenWithType(out, node, NED_LOOP_VAR, increaseIndent(indent));
    out << " do";
    appendRightComment(out, node->getRightComment(), indent);
    generateChildrenWithType(out, node, NED_CONNECTION, increaseIndent(indent));
    out << indent << "endfor;";
    appendTrailingComment(out, node->getTrailingComment(), "");
}

void NEDGenerator::doLoopvar(ostream& out, LoopVarNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << node->getParamName() << "=";
    printExpression(out, node, "from-value",indent);
    out << "..";
    printExpression(out, node, "to-value",indent);
    if (!islast) out << ", ";
    appendRightComment(out, node->getRightComment(), indent);
}

void NEDGenerator::doDisplaystring(ostream& out, DisplayStringNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(out, node->getBannerComment(), indent);
    out << indent << "display: \"" << node->getValue() << "\";";
    appendInlineRightComment(out, node->getRightComment(), "");
}

void NEDGenerator::doExpression(ostream& out, ExpressionNode *node, const char *indent, bool islast, const char *)
{
    generateChildren(out,node,indent);
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

void NEDGenerator::doOperator(ostream& out, OperatorNode *node, const char *indent, bool islast, const char *)
{
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    if (!op2)
    {
        // unary
        out << node->getName();
        generateNedItem(out,op1,indent,false,NULL);
    }
    else if (!op3)
    {
        // binary
        int prio = getOperatorPriority(node->getName(), 2);
        bool leftassoc = isOperatorLeftAssoc(node->getName());

        bool needsParen = false;
        bool spacious = false; // FIXME something better here!!!

        NEDElement *parent = node->getParent();
        if (parent && parent->getTagCode()==NED_OPERATOR) {
            OperatorNode *parentop = (OperatorNode *)parent;
            int parentprio = getOperatorPriority(parentop->getName(),2 /*FIXME!!!*/);
            if (parentprio>prio) {
                needsParen = true;
            } else if (parentprio==prio) {
                // FIXME refine here.... depending on assoc, etc
                needsParen = true;
            }
        }

        if (needsParen) out << "(";
        generateNedItem(out,op1,indent,false,NULL);
        if (spacious)
            out << " " << node->getName() << " ";
        else
            out << node->getName();
        generateNedItem(out,op2,indent,false,NULL);
        if (needsParen) out << ")";
    }
    else
    {
        // tertiary
        bool needsParen = true; // FIXME something better here!!!
        bool spacious = true; // FIXME something better here!!!

        if (needsParen) out << "(";
        generateNedItem(out,op1,indent,false,NULL);
        out << (spacious ? " ? " : "?");
        generateNedItem(out,op1,indent,false,NULL);
        out << (spacious ? " : " : ":");
        generateNedItem(out,op1,indent,false,NULL);
        if (needsParen) out << ")";
    }
}

void NEDGenerator::doFunction(ostream& out, FunctionNode *node, const char *indent, bool islast, const char *)
{
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    out << node->getName() << "(";
    if (op1) {
        generateNedItem(out,op1,indent,false,NULL);
    }
    if (op2) {
        out << ", ";
        generateNedItem(out,op2,indent,false,NULL);
    }
    if (op3) {
        out << ", ";
        generateNedItem(out,op3,indent,false,NULL);
    }
    out << ")";
}

void NEDGenerator::doParamref(ostream& out, ParamRefNode *node, const char *indent, bool islast, const char *)
{
    if (node->getIsAncestor())
        out << "ancestor ";

    if (node->getIsRef())
        out << "ref ";

    if (strnotnull(node->getModule())) {
        out << node->getModule();
        printVector(out, node, "module-index", indent);
        out << ".";
    }

    out << node->getParamName();
}

void NEDGenerator::doIdent(ostream& out, IdentNode *node, const char *indent, bool islast, const char *)
{
    out << node->getName();
}

void NEDGenerator::doConst(ostream& out, ConstNode *node, const char *indent, bool islast, const char *)
{
    bool isstring = (node->getType()==NED_CONST_STRING);

    if (isstring) out << "\"";
    if (strnotnull(node->getText())) {
        out << node->getText();
    } else {
        out << node->getValue();
    }
    if (isstring) out << "\"";
}

//---------------------------------------------------------------------------

void NEDGenerator::generateNedItem(ostream& out, NEDElement *node, const char *indent, bool islast, const char *arg)
{
    // dispatch
    int tagcode = node->getTagCode();
    switch (tagcode)
    {
        case NED_NED_FILES:
            doNedfiles(out, (NedFilesNode *)node, indent, islast, arg); break;
        case NED_NED_FILE:
            doNedfile(out, (NedFileNode *)node, indent, islast, arg); break;
        case NED_IMPORT:
            doImports(out, (ImportNode *)node, indent, islast, arg); break;
        case NED_IMPORTED_FILE:
            doImport(out, (ImportedFileNode *)node, indent, islast, arg); break;
        case NED_CHANNEL:
            doChannel(out, (ChannelNode *)node, indent, islast, arg); break;
        case NED_CHANNEL_ATTR:
            doChanattr(out, (ChannelAttrNode *)node, indent, islast, arg); break;
        case NED_NETWORK:
            doNetwork(out, (NetworkNode *)node, indent, islast, arg); break;
        case NED_SIMPLE_MODULE:
            doSimple(out, (SimpleModuleNode *)node, indent, islast, arg); break;
        case NED_COMPOUND_MODULE:
            doModule(out, (CompoundModuleNode *)node, indent, islast, arg); break;
        case NED_PARAMS:
            doParams(out, (ParamsNode *)node, indent, islast, arg); break;
        case NED_PARAM:
            doParam(out, (ParamNode *)node, indent, islast, arg); break;
        case NED_GATES:
            doGates(out, (GatesNode *)node, indent, islast, arg); break;
        case NED_GATE:
            doGate(out, (GateNode *)node, indent, islast, arg); break;
        case NED_MACHINES:
            doMachines(out, (MachinesNode *)node, indent, islast, arg); break;
        case NED_MACHINE:
            doMachine(out, (MachineNode *)node, indent, islast, arg); break;
        case NED_SUBMODULES:
            doSubmodules(out, (SubmodulesNode *)node, indent, islast, arg); break;
        case NED_SUBMODULE:
            doSubmodule(out, (SubmoduleNode *)node, indent, islast, arg); break;
        case NED_SUBSTPARAMS:
            doSubstparams(out, (SubstparamsNode *)node, indent, islast, arg); break;
        case NED_SUBSTPARAM:
            doSubstparam(out, (SubstparamNode *)node, indent, islast, arg); break;
        case NED_GATESIZES:
            doGatesizes(out, (GatesizesNode *)node, indent, islast, arg); break;
        case NED_GATESIZE:
            doGatesize(out, (GatesizeNode *)node, indent, islast, arg); break;
        case NED_SUBSTMACHINES:
            doSubstmachines(out, (SubstmachinesNode *)node, indent, islast, arg); break;
        case NED_SUBSTMACHINE:
            doSubstmachine(out, (SubstmachineNode *)node, indent, islast, arg); break;
        case NED_CONNECTIONS:
            doConnections(out, (ConnectionsNode *)node, indent, islast, arg); break;
        case NED_CONNECTION:
            doConnection(out, (ConnectionNode *)node, indent, islast, arg); break;
        case NED_CONN_ATTR:
            doConnattr(out, (ConnAttrNode *)node, indent, islast, arg); break;
        case NED_FOR_LOOP:
            doForloop(out, (ForLoopNode *)node, indent, islast, arg); break;
        case NED_LOOP_VAR:
            doLoopvar(out, (LoopVarNode *)node, indent, islast, arg); break;
        case NED_DISPLAY_STRING:
            doDisplaystring(out, (DisplayStringNode *)node, indent, islast, arg); break;
        case NED_EXPRESSION:
            doExpression(out, (ExpressionNode *)node, indent, islast, arg); break;
        case NED_OPERATOR:
            doOperator(out, (OperatorNode *)node, indent, islast, arg); break;
        case NED_FUNCTION:
            doFunction(out, (FunctionNode *)node, indent, islast, arg); break;
        case NED_PARAM_REF:
            doParamref(out, (ParamRefNode *)node, indent, islast, arg); break;
        case NED_IDENT:
            doIdent(out, (IdentNode *)node, indent, islast, arg); break;
        case NED_CONST:
            doConst(out, (ConstNode *)node, indent, islast, arg); break;
        default:
            INTERNAL_ERROR1(node, "generateNedItem(): unknown tag '%s'", node->getTagName());
    }
}


