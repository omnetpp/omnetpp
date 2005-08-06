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


void generateNed(ostream& out, NEDElement *node, bool newsyntax)
{
    NEDGenerator nedgen(out);
    nedgen.setNewSyntax(newsyntax);
    nedgen.generate(node, "");
}

//-----------------------------------------

NEDGenerator::NEDGenerator(ostream& _out) : out(_out)
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

void NEDGenerator::printExpression(NEDElement *node, const char *attr, const char *indent)
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
                generateNedItem(expr, indent, false, NULL);
            }
        }
    }
}

void NEDGenerator::printVector(NEDElement *node, const char *attr, const char *indent)
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
                generateNedItem(child, indent, false, NULL);
                out << "]";
            }
        }
    }
}

void NEDGenerator::printIfExpression(NEDElement *node, const char *attr, const char *indent)
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
                generateNedItem(child, indent, false, NULL);
            }
        }
    }
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

void NEDGenerator::doNedfiles(NedFilesNode *node, const char *indent, bool, const char *)
{
    generateChildren(node, indent);
}

void NEDGenerator::doNedfile(NedFileNode *node, const char *indent, bool, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    generateChildren(node, indent);
}

void NEDGenerator::doImports(ImportNode *node, const char *indent, bool islast, const char *)
{
    // things would probably get simpler if we allowed one filename per "import" directive only...
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "import ";

    if (node->getNumChildrenWithTag(NED_IMPORTED_FILE)==1)
    {
        ImportedFileNode *importedFile = dynamic_cast<ImportedFileNode *>(node->getFirstChildWithTag(NED_IMPORTED_FILE));
        out << indent << "\"" << importedFile->getFilename() << "\";";
        appendRightComment(importedFile->getRightComment(), indent);
    }
    else
    {
        appendRightComment(node->getRightComment(), indent);
        generateChildren(node, increaseIndent(indent));
    }
}

void NEDGenerator::doImport(ImportedFileNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "\"" << node->getFilename() << "\"";
    if (islast) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doChannel(ChannelNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "channel " << node->getName();
    appendRightComment(node->getRightComment(), indent);
    if (newsyntax) out << indent << "{";
    generateChildren(node, increaseIndent(indent));
    out << indent << (newsyntax?"}":"endchannel");
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doChanattr(ChannelAttrNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << node->getName() << " ";
    printExpression(node, "value",indent);
    out << ";";
    appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doNetwork(NetworkNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "network " << node->getName() << " : " << node->getTypeName();
    appendRightComment(node->getRightComment(), indent);
    if (newsyntax) out << indent << "{";

    generateChildrenWithType(node, NED_SUBSTMACHINES, increaseIndent(indent));
    generateChildrenWithType(node, NED_SUBSTPARAMS, increaseIndent(indent));

    out << indent << (newsyntax?"}":"endnetwork");
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doSimple(SimpleModuleNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "simple " << node->getName();
    appendRightComment(node->getRightComment(), indent);
    if (newsyntax) out << indent << "{";

    generateChildrenWithType(node, NED_MACHINES, increaseIndent(indent));
    generateChildrenWithType(node, NED_PARAMS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));

    out << indent << (newsyntax?"}":"endsimple");
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doModule(CompoundModuleNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "module " << node->getName();
    appendRightComment(node->getRightComment(), indent);
    if (newsyntax) out << indent << "{";

    generateChildrenWithType(node, NED_MACHINES, increaseIndent(indent));
    generateChildrenWithType(node, NED_PARAMS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATES, increaseIndent(indent));
    generateChildrenWithType(node, NED_SUBMODULES, increaseIndent(indent));
    generateChildrenWithType(node, NED_CONNECTIONS, increaseIndent(indent));
    generateChildrenWithType(node, NED_DISPLAY_STRING, increaseIndent(indent));

    out << indent << (newsyntax?"}":"endmodule");
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doParams(ParamsNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "parameters:";
    appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doParam(ParamNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << node->getName() << ": " << node->getDataType();
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doGates(GatesNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "gates:";
    appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doGate(GateNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << (node->getDirection()==NED_GATEDIR_INPUT ? "in: " : "out: ") << node->getName();
    if (node->getIsVector()) {
        out << "[]";
    }
    out << ";";
    appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doMachines(MachinesNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "machines:";
    appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doMachine(MachineNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << node->getName();
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doSubmodules(SubmodulesNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "submodules:";
    appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doSubmodule(SubmoduleNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << node->getName() << ": ";
    if (strnotnull(node->getLikeParam())) {
        out << node->getLikeParam();
        printVector(node, "vector-size",indent);
        out << " like " << node->getTypeName();
    }
    else
    {
        out << node->getTypeName();
        printVector(node, "vector-size",indent);
    }
    out << ";";
    appendRightComment(node->getRightComment(), indent);

    generateChildrenWithType(node, NED_SUBSTMACHINES, increaseIndent(indent));
    generateChildrenWithType(node, NED_SUBSTPARAMS, increaseIndent(indent));
    generateChildrenWithType(node, NED_GATESIZES, increaseIndent(indent));
    generateChildrenWithType(node, NED_DISPLAY_STRING, increaseIndent(indent));
}

void NEDGenerator::doSubstparams(SubstparamsNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "parameters";
    printIfExpression(node, "condition",indent);
    out << ":";

    appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doSubstparam(SubstparamNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << node->getName() << " = ";
    printExpression(node, "value",indent);
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doGatesizes(GatesizesNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "gatesizes";
    printIfExpression(node, "condition",indent);
    out << ":";

    appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doGatesize(GatesizeNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << node->getName();
    printVector(node, "vector-size",indent);
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doSubstmachines(SubstmachinesNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "on";
    printIfExpression(node, "condition",indent);
    out << ":";

    appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::doSubstmachine(SubstmachineNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << node->getName();
    if (islast || newsyntax) {
        out << ";";
    } else {
        out << ",";
    }
    appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doConnections(ConnectionsNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    if (!node->getCheckUnconnected()) {
        out << indent << "connections nocheck:";
    } else {
        out << indent << "connections:";
    }
    appendRightComment(node->getRightComment(), indent);
    generateChildren(node, increaseIndent(indent));
}

void NEDGenerator::printGate(NEDElement *conn, const char *modname, const char *modindexattr,
                             const char *gatename, const char *gateindexattr, bool isplusplus,
                             const char *indent)
{
    if (strnotnull(modname)) {
        out << modname;
        printVector(conn, modindexattr,indent);
        out << ".";
    }

    out << gatename;
    if (isplusplus)
        out << "++";
    else
        printVector(conn, gateindexattr,indent);
}

void NEDGenerator::doConnection(ConnectionNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);

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
        printGate(node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", node->getSrcGatePlusplus(), indent);
    } else {
        printGate(node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", node->getDestGatePlusplus(), indent);
    }

    // arrow
    out << arrow;

    // print channel attributes
    generateChildrenWithType(node, NED_CONN_ATTR, indent, arrow);

    // print dest
    out << " ";
    if (srcfirst) {
        printGate(node, node->getDestModule(), "dest-module-index", node->getDestGate(), "dest-gate-index", node->getDestGatePlusplus(), indent);
    } else {
        printGate(node, node->getSrcModule(), "src-module-index", node->getSrcGate(), "src-gate-index", node->getSrcGatePlusplus(), indent);
    }

    // print 'if' clause
    printIfExpression(node, "condition",indent);

    // print display string
    DisplayStringNode *dispstr = (DisplayStringNode *) node->getFirstChildWithTag(NED_DISPLAY_STRING);
    if (dispstr)
    {
        out << " display \"" << dispstr->getValue() << "\"";
    }

    out << ";";

    // print comments
    appendRightComment(node->getRightComment(), indent);
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
        ConstNode *aconst = (ConstNode *)(expr ? expr->getFirstChildWithTag(NED_CONST) : NULL);
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

void NEDGenerator::doForloop(ForLoopNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "for ";
    generateChildrenWithType(node, NED_LOOP_VAR, increaseIndent(indent));
    out << " do";
    appendRightComment(node->getRightComment(), indent);
    generateChildrenWithType(node, NED_CONNECTION, increaseIndent(indent));
    out << indent << "endfor;";
    appendTrailingComment(node->getTrailingComment(), "");
}

void NEDGenerator::doLoopvar(LoopVarNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << node->getParamName() << "=";
    printExpression(node, "from-value",indent);
    out << "..";
    printExpression(node, "to-value",indent);
    if (!islast) out << ", ";
    appendRightComment(node->getRightComment(), indent);
}

void NEDGenerator::doDisplaystring(DisplayStringNode *node, const char *indent, bool islast, const char *)
{
    appendBannerComment(node->getBannerComment(), indent);
    out << indent << "display: \"" << node->getValue() << "\";";
    appendInlineRightComment(node->getRightComment(), "");
}

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
            int parentprio = getOperatorPriority(parentop->getName(),2 /*FIXME!!!*/);
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

void NEDGenerator::doParamref(ParamRefNode *node, const char *indent, bool islast, const char *)
{
    if (node->getIsAncestor())
        out << "ancestor ";

    if (node->getIsRef())
        out << "ref ";

    if (strnotnull(node->getModule())) {
        out << node->getModule();
        printVector(node, "module-index", indent);
        out << ".";
    }

    out << node->getParamName();
}

void NEDGenerator::doIdent(IdentNode *node, const char *indent, bool islast, const char *)
{
    out << node->getName();
}

void NEDGenerator::doConst(ConstNode *node, const char *indent, bool islast, const char *)
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

void NEDGenerator::doProperty(PropertyNode *node, const char *indent, bool islast, const char *)
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
        case NED_NED_FILES:
            doNedfiles((NedFilesNode *)node, indent, islast, arg); break;
        case NED_NED_FILE:
            doNedfile((NedFileNode *)node, indent, islast, arg); break;
        case NED_IMPORT:
            doImports((ImportNode *)node, indent, islast, arg); break;
        case NED_IMPORTED_FILE:
            doImport((ImportedFileNode *)node, indent, islast, arg); break;
        case NED_CHANNEL:
            doChannel((ChannelNode *)node, indent, islast, arg); break;
        case NED_CHANNEL_ATTR:
            doChanattr((ChannelAttrNode *)node, indent, islast, arg); break;
        case NED_NETWORK:
            doNetwork((NetworkNode *)node, indent, islast, arg); break;
        case NED_SIMPLE_MODULE:
            doSimple((SimpleModuleNode *)node, indent, islast, arg); break;
        case NED_COMPOUND_MODULE:
            doModule((CompoundModuleNode *)node, indent, islast, arg); break;
        case NED_PARAMS:
            doParams((ParamsNode *)node, indent, islast, arg); break;
        case NED_PARAM:
            doParam((ParamNode *)node, indent, islast, arg); break;
        case NED_GATES:
            doGates((GatesNode *)node, indent, islast, arg); break;
        case NED_GATE:
            doGate((GateNode *)node, indent, islast, arg); break;
        case NED_MACHINES:
            doMachines((MachinesNode *)node, indent, islast, arg); break;
        case NED_MACHINE:
            doMachine((MachineNode *)node, indent, islast, arg); break;
        case NED_SUBMODULES:
            doSubmodules((SubmodulesNode *)node, indent, islast, arg); break;
        case NED_SUBMODULE:
            doSubmodule((SubmoduleNode *)node, indent, islast, arg); break;
        case NED_SUBSTPARAMS:
            doSubstparams((SubstparamsNode *)node, indent, islast, arg); break;
        case NED_SUBSTPARAM:
            doSubstparam((SubstparamNode *)node, indent, islast, arg); break;
        case NED_GATESIZES:
            doGatesizes((GatesizesNode *)node, indent, islast, arg); break;
        case NED_GATESIZE:
            doGatesize((GatesizeNode *)node, indent, islast, arg); break;
        case NED_SUBSTMACHINES:
            doSubstmachines((SubstmachinesNode *)node, indent, islast, arg); break;
        case NED_SUBSTMACHINE:
            doSubstmachine((SubstmachineNode *)node, indent, islast, arg); break;
        case NED_CONNECTIONS:
            doConnections((ConnectionsNode *)node, indent, islast, arg); break;
        case NED_CONNECTION:
            doConnection((ConnectionNode *)node, indent, islast, arg); break;
        case NED_CONN_ATTR:
            doConnattr((ConnAttrNode *)node, indent, islast, arg); break;
        case NED_FOR_LOOP:
            doForloop((ForLoopNode *)node, indent, islast, arg); break;
        case NED_LOOP_VAR:
            doLoopvar((LoopVarNode *)node, indent, islast, arg); break;
        case NED_DISPLAY_STRING:
            doDisplaystring((DisplayStringNode *)node, indent, islast, arg); break;
        case NED_EXPRESSION:
            doExpression((ExpressionNode *)node, indent, islast, arg); break;
        case NED_OPERATOR:
            doOperator((OperatorNode *)node, indent, islast, arg); break;
        case NED_FUNCTION:
            doFunction((FunctionNode *)node, indent, islast, arg); break;
        case NED_PARAM_REF:
            doParamref((ParamRefNode *)node, indent, islast, arg); break;
        case NED_IDENT:
            doIdent((IdentNode *)node, indent, islast, arg); break;
        case NED_CONST:
            doConst((ConstNode *)node, indent, islast, arg); break;
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
        case NED_PROPERTY:
            doProperty((PropertyNode *)node, indent, islast, arg); break;
        default:
            INTERNAL_ERROR1(node, "generateNedItem(): unknown tag '%s'", node->getTagName());
    }
}


