//==========================================================================
//   CPPEXPRGENERATOR.CC
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


#include "cppexprgenerator.h"
#include "nederror.h"


inline bool strnotnull(const char *s)
{
    return s && s[0];
}

static struct { char *fname; int args; } known_funcs[] =
{
   /* <math.h> */
   {"fabs", 1},    {"fmod", 2},
   {"acos", 1},    {"asin", 1},    {"atan", 1},   {"atan2", 1},
   {"sin", 1},     {"cos", 1},     {"tan", 1},    {"hypot", 2},
   {"ceil", 1},    {"floor", 1},
   {"exp", 1},     {"pow", 2},     {"sqrt", 1},
   {"log",  1},    {"log10", 1},

   /* OMNeT++ */
   {"uniform", 2},      {"intuniform", 2},       {"exponential", 1},
   {"normal", 2},       {"truncnormal", 2},
   {"genk_uniform", 3}, {"genk_intuniform",  3}, {"genk_exponential", 2},
   {"genk_normal", 3},  {"genk_truncnormal", 3},
   {"min", 2},          {"max", 2},

   /* OMNeT++, to support expressions */
   {"bool_and",2}, {"bool_or",2}, {"bool_xor",2}, {"bool_not",1},
   {"bin_and",2},  {"bin_or",2},  {"bin_xor",2},  {"bin_compl",1},
   {"shift_left",2}, {"shift_right",2},
   {NULL,0}
};


int CppExpressionGenerator::count = 0;

void CppExpressionGenerator::collectExpressions(NEDElement *node)
{
    exprMap.clear();
    doCollectExpressions(node);
}

void CppExpressionGenerator::doCollectExpressions(NEDElement *node)
{
    for (NEDElement *child=node->getFirstChild(); child; child = child->getNextSibling())
    {
        if (child->getTagCode()==NED_EXPRESSION)
        {
            if (needsExpressionClass((ExpressionNode *)child))
                collectExpressionInfo((ExpressionNode *)child);
        }
        else // stop at expressions: embedded expressions are merged into their parents
        {
            doCollectExpressions(child);
        }
    }
}

bool CppExpressionGenerator::needsExpressionClass(ExpressionNode *expr)
{
    // if simple expr, no expression class necessary
    NEDElement *node = expr->getFirstChild();
    if (!node)  {INTERNAL_ERROR0(expr, "needsExpressionClass(): empty expression");return false;}
    int tag = node->getTagCode();
    if (tag==NED_IDENT || tag==NED_CONST) // FIXME: some paramrefs and functions may also qualify
        return false;

    // only non-const parameter assignments and channel attrs need expression classes (?)
    int parenttag = expr->getParent()->getTagCode();
    if (parenttag!=NED_SUBSTPARAM && parenttag!=NED_CHANNEL_ATTR)
        return false;
    return true;
}

void CppExpressionGenerator::collectExpressionInfo(ExpressionNode *expr)
{
    char buf[32];
    sprintf(buf,"Expr%d",count++);
    NEDElement *ctx = expr;
    while (ctx->getTagCode()!=NED_CHANNEL &&
           ctx->getTagCode()!=NED_SIMPLE_MODULE &&
           ctx->getTagCode()!=NED_COMPOUND_MODULE &&
           ctx->getTagCode()!=NED_NETWORK &&
           ctx->getParent())
    {
        ctx = ctx->getParent();
    }

    ExpressionInfo info;
    info.expr = expr;
    info.name = buf;
    info.ctxtype = ctx ? ctx->getTagCode() : 0;
    doExtractArgs(info, expr);
    exprMap[expr] = info;
}

void CppExpressionGenerator::doExtractArgs(ExpressionInfo& info, NEDElement *node)
{
    for (NEDElement *child=node->getFirstChild(); child; child = child->getNextSibling())
    {
        bool pushit = false;
        int tag = child->getTagCode();
        if (tag==NED_IDENT)
            pushit = true;
        else if (node->getTagCode()==NED_FUNCTION && !strcmp(((FunctionNode *)node)->getName(),"index"))
            pushit = true;
        else if (node->getTagCode()==NED_FUNCTION && !strcmp(((FunctionNode *)node)->getName(),"sizeof"))
            pushit = true;
        if (pushit)
            info.args.push_back(child);
        doExtractArgs(info, child);
    }
}

void CppExpressionGenerator::generateExpressionClass(ostream& out, ExpressionInfo& info)
{
    const char *classname = info.name.c_str();
    NEDElementVector::iterator i;

    // generate expression class
    const char *srcloc = info.expr->getSourceLocation();
    out << "// evaluator for expression at " << (srcloc ? srcloc : "(no source info)") << "\n";
    out << "class " << classname << " : public cDoubleExpression\n";
    out << "{\n";
    out << "  private:\n";
    if (info.ctxtype==NED_SIMPLE_MODULE || info.ctxtype==NED_COMPOUND_MODULE)
        out << "    cModule *mod;\n";
    else if (info.ctxtype==NED_CHANNEL)
        out << "    // cChannel *channel;\n";
    // variables to hold arguments (external references) for the expression
    for (i=info.args.begin(); i!=info.args.end(); ++i)
        out << "    " << getTypeForArg(*i) << getNameForArg(*i) << ";\n";
    out << "  public:\n";

    // constructor:
    out << "    " << classname << "(";
    if (info.ctxtype==NED_SIMPLE_MODULE || info.ctxtype==NED_COMPOUND_MODULE)
        out << "cModule *mod";
    else
        out << "void *";
    for (i=info.args.begin(); i!=info.args.end(); ++i)
        out << ", " << getTypeForArg(*i) << getNameForArg(*i);
    out << ")  {";
    if (info.ctxtype==NED_SIMPLE_MODULE || info.ctxtype==NED_COMPOUND_MODULE)
        out << "this->mod=mod; ";
    for (i=info.args.begin(); i!=info.args.end(); ++i)
        out << "this->" << getNameForArg(*i) << "=" << getNameForArg(*i) << "; ";
    out << "}\n";

    // dup()
    out << "    cExpression *dup()  {return new " << classname << "(";
    if (info.ctxtype==NED_SIMPLE_MODULE || info.ctxtype==NED_COMPOUND_MODULE)
        out << "mod";
    else
        out << "NULL";
    for (i=info.args.begin(); i!=info.args.end(); ++i)
        out << ", " << getNameForArg(*i);
    out << ");}\n";

    // expression method:
    out << "    double evaluate()  {return ";
    generateChildren(out, info.expr, "               ", MODE_EXPRESSION_CLASS);
    out << ";}\n";
    out << "};\n\n";
}

const char *CppExpressionGenerator::getTypeForArg(NEDElement *node)
{
    if (node->getTagCode()==NED_IDENT)
        return "long ";
    else if (node->getTagCode()==NED_FUNCTION && !strcmp(((FunctionNode *)node)->getName(),"index"))
        return "long ";
    else if (node->getTagCode()==NED_FUNCTION && !strcmp(((FunctionNode *)node)->getName(),"sizeof"))
        return "???? "; // FIXME
    else
        {INTERNAL_ERROR1(node, "getTypeForArg(): unexpected tag '%s'", node->getTagName());return NULL;}
}

const char *CppExpressionGenerator::getNameForArg(NEDElement *node)
{
    if (node->getTagCode()==NED_IDENT)
        return ((IdentNode *)node)->getName();
    else if (node->getTagCode()==NED_FUNCTION)
        return ((FunctionNode *)node)->getName(); //FIXME!!!
    else
        {INTERNAL_ERROR1(node, "getNameForArg(): unexpected tag '%s'", node->getTagName());return NULL;}
}

void CppExpressionGenerator::doValueForArg(ostream& out, NEDElement *node)
{
    if (node->getTagCode()==NED_IDENT)
        out << ((IdentNode *)node)->getName() << "_var";
    else if (node->getTagCode()==NED_FUNCTION)
        out << "fixme:" << ((FunctionNode *)node)->getName(); //FIXME!!!
    else
        {INTERNAL_ERROR1(node, "doValueForArg(): unexpected tag '%s'", node->getTagName());}
}

void CppExpressionGenerator::generateExpressionClasses(ostream& out)
{
   if (!exprMap.empty())
   {
       out << "namespace {  // unnamed namespace to avoid name clashes\n\n";
       for (NEDExpressionMap::iterator i=exprMap.begin(); i!=exprMap.end(); ++i)
           generateExpressionClass(out, (*i).second);
       out << "}  // end unnamed namespace\n\n";
   }
}

void CppExpressionGenerator::generateExpressionUsage(ostream& out, ExpressionNode *expr, const char *indent)
{
    // find name in hash table
    NEDExpressionMap::iterator e = exprMap.find(expr);
    if (e==exprMap.end())
    {
        // inline expression
        generateChildren(out, expr, indent, MODE_INLINE_EXPRESSION);
    }
    else
    {
        // use generated expression class
        ExpressionInfo& info = (*e).second;
        const char *classname = info.name.c_str();
        out << "tmpval.setDoubleValue(new " << classname << "(";
        if (info.ctxtype==NED_SIMPLE_MODULE || info.ctxtype==NED_COMPOUND_MODULE)
            out << "mod";
        else
            out << "NULL";
        for (NEDElementVector::iterator i=info.args.begin(); i!=info.args.end(); ++i)
        {
            out << ", ";
            doValueForArg(out, (*i));
        }
        out << "))";
    }
}

void CppExpressionGenerator::generateChildren(ostream& out, NEDElement *node, const char *indent, int mode)
{
    for (NEDElement *child=node->getFirstChild(); child; child = child->getNextSibling())
    {
        generateItem(out, child, indent, mode);
    }
}

void CppExpressionGenerator::generateItem(ostream& out, NEDElement *node, const char *indent, int mode)
{
    int tagcode = node->getTagCode();
    switch (tagcode)
    {
        case NED_OPERATOR:
            doOperator(out, (OperatorNode *)node, indent, mode); break;
        case NED_FUNCTION:
            doFunction(out, (FunctionNode *)node, indent, mode); break;
        case NED_PARAM_REF:
            doParamref(out, (ParamRefNode *)node, indent, mode); break;
        case NED_IDENT:
            doIdent(out, (IdentNode *)node, indent, mode); break;
        case NED_CONST:
            doConst(out, (ConstNode *)node, indent, mode); break;
        case NED_EXPRESSION:
            doExpression(out, (ExpressionNode *)node, indent, mode); break;
        default:
            INTERNAL_ERROR1(node,"generateItem(): unexpected tag %s", node->getTagName());
    }
}

void CppExpressionGenerator::doOperator(ostream& out, OperatorNode *node, const char *indent, int mode)
{
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    if (!op2)
    {
        // unary
        out << node->getName();
        generateItem(out,op1,indent,mode);
    }
    else if (!op3)
    {
        // binary. Always put parens -- C++ operator precendence may be different
        // from NED's, and anyway who cares in generated C++ code...
        out << "(";
        generateItem(out,op1,indent,mode);
        out << node->getName();
        generateItem(out,op2,indent,mode);
        out << ")";
    }
    else
    {
        // tertiary can only be "?:"
        out << "(";
        generateItem(out,op1,indent,mode);
        out << " ? ";
        generateItem(out,op2,indent,mode);
        out << " : ";
        generateItem(out,op3,indent,mode);
        out << ")";
    }
}

void CppExpressionGenerator::doFunction(ostream& out, FunctionNode *node, const char *indent, int mode)
{
    const char *funcname = node->getName();
    NEDElement *op1 = node->getFirstChild();
    NEDElement *op2 = op1 ? op1->getNextSibling() : NULL;
    NEDElement *op3 = op2 ? op2->getNextSibling() : NULL;

    // both modes seem to be the same...

    // operators should be handled specially
    if (!strcmp(funcname,"index"))
    {
        out << "submod_i";
        return;
    }
    else if (!strcmp(funcname,"sizeof"))
    {
        // FIXME handle...
        return;
    }
    else if (!strcmp(funcname,"input"))
    {
        // FIXME handle...
        return;
    }

    // do we know this function?
    int i;
    for (i=0; known_funcs[i].fname!=NULL;i++)
        if (!strcmp(funcname,known_funcs[i].fname))
            break;
    if (known_funcs[i].fname!=NULL) // found
    {
        out << funcname;
    }
    else // unknown
    {
        out << "(findFunction(\"" << funcname << "\")->f)";
    }

    out << "(";
    if (op1) {
        generateItem(out,op1,indent,mode);
    }
    if (op2) {
        out << ", ";
        generateItem(out,op2,indent,mode);
    }
    if (op3) {
        out << ", ";
        generateItem(out,op3,indent,mode);
    }
    out << ")";
}

void CppExpressionGenerator::doParamref(ostream& out, ParamRefNode *node, const char *indent, int mode)
{
    if (mode==MODE_EXPRESSION_CLASS)
    {
        // arg holds pointer of current module
        out << "(double)mod";
        if (strnotnull(node->getModule())) {
            out << "->getSubmodule(\"" << node->getModule() << "\")";
            // FIXME index here!
        }
        out << "->par(\"" << node->getParamName() << "\")";
    }
    else // MODE_INLINE_EXPRESSION
    {
        out << "(double)";
        if (node->getIsAncestor())
            out << "ancestor "; // FIXME

        if (node->getIsRef())
            out << "ref "; // FIXME

        if (strnotnull(node->getModule())) {
            out << node->getModule() << "_p";
            // FIXME index here!
            out << "->";
        } else {
            out << "mod->";
        }

        out << "par(\"" << node->getParamName() << "\")";
    }
}

void CppExpressionGenerator::doIdent(ostream& out, IdentNode *node, const char *indent, int mode)
{
    if (mode==MODE_EXPRESSION_CLASS)
    {
        out << getNameForArg(node) << "_var";
    }
    else // MODE_INLINE_EXPRESSION
    {
        out << node->getName() << "_var";
    }
}

void CppExpressionGenerator::doConst(ostream& out, ConstNode *node, const char *indent, int mode)
{
    bool isstring = (node->getType()==NED_CONST_STRING);

    if (isstring) out << "\"";
    out << node->getValue();
    if (isstring) out << "\"";
}

void CppExpressionGenerator::doExpression(ostream& out, ExpressionNode *node, const char *indent, int mode)
{
    generateItem(out, node->getFirstChild(), indent, mode);
}

