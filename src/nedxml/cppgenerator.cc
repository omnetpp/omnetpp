//==========================================================================
//   CPPGENERATOR.CC
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <stdio.h>
#include <time.h>

#include "nedelements.h"
#include "cppgenerator.h"
#include "nederror.h"

using std::ostream;

#define NEDC_VERSION "2.30" //FIXME
#define NEDC_VERSION_HEX "0x0230" //FIXME


void generateCpp(ostream& out, ostream& outh, NEDElement *node, NEDSymbolTable *symtab)
{
    NEDCppGenerator cppgen(out, outh, symtab);
    cppgen.generate(node);
}

inline bool strnotnull(const char *s)
{
    return s && s[0];
}

//-----------------------------------------------------------------------

NEDCppGenerator::NEDCppGenerator(ostream& _out, ostream& _outh, NEDSymbolTable *symtab) :
  out(_out), outh(_outh), exprgen(_out), symboltable(symtab)
{
    indentsize = 4;
    in_network = false;
}

NEDCppGenerator::~NEDCppGenerator()
{
}

void NEDCppGenerator::setIndentSize(int indentsiz)
{
    indentsize = indentsiz;
}


const char *NEDCppGenerator::increaseIndent(const char *indent)
{
    // biggest possible indent:
    static char spaces[] = "                                                                     ";

    // bump...
    unsigned ilen = strlen(indent);
    if (ilen+indentsize <= sizeof(spaces)-1)
        ilen+=indentsize;
    const char *newindent = spaces+sizeof(spaces)-1-ilen;
    return newindent;
}

const char *NEDCppGenerator::decreaseIndent(const char *indent)
{
    return indent + indentsize;
}

void NEDCppGenerator::generate(NEDElement *node)
{
    writeProlog(out);
    generateItem(node, "");
}

void NEDCppGenerator::generateItem(NEDElement *node, const char *indent, int mode, const char *arg)
{
    int tagcode = node->getTagCode();

    const char *newindent = indent;
    switch (mode)
    {
        case MODE_NORMAL:
            switch (tagcode)
            {
                case NED_NED_FILES:
                    doNedFiles((NedFilesNode *)node, newindent, mode, arg); break;
                case NED_NED_FILE:
                    doNedFile((NedFileNode *)node, newindent, mode, arg); break;
                case NED_IMPORT:
                    doImports((ImportNode *)node, newindent, mode, arg); break;
                case NED_IMPORTED_FILE:
                    doImport((ImportedFileNode *)node, newindent, mode, arg); break;
                case NED_CHANNEL:
                    doChannel((ChannelNode *)node, newindent, mode, arg); break;
                case NED_CHANNEL_ATTR:
                    doChannelAttr((ChannelAttrNode *)node, newindent, mode, arg); break;
                case NED_NETWORK:
                    doNetwork((NetworkNode *)node, newindent, mode, arg); break;
                case NED_SIMPLE_MODULE:
                    doSimple((SimpleModuleNode *)node, newindent, mode, arg); break;
                case NED_COMPOUND_MODULE:
                    doModule((CompoundModuleNode *)node, newindent, mode, arg); break;
                case NED_PARAMS:
                    doParams((ParamsNode *)node, newindent, mode, arg); break;
                case NED_PARAM:
                    doParam((ParamNode *)node, newindent, mode, arg); break;
                case NED_GATES:
                    doGates((GatesNode *)node, newindent, mode, arg); break;
                case NED_GATE:
                    doGate((GateNode *)node, newindent, mode, arg); break;
                case NED_MACHINES:
                    doMachines((MachinesNode *)node, newindent, mode, arg); break;
                case NED_MACHINE:
                    doMachine((MachineNode *)node, newindent, mode, arg); break;
                case NED_SUBMODULES:
                    doSubmodules((SubmodulesNode *)node, newindent, mode, arg); break;
                case NED_SUBMODULE:
                    doSubmodule((SubmoduleNode *)node, newindent, mode, arg); break;
                case NED_SUBSTPARAMS:
                    doSubstparams((SubstparamsNode *)node, newindent, mode, arg); break;
                case NED_SUBSTPARAM:
                    doSubstparam((SubstparamNode *)node, newindent, mode, arg); break;
                case NED_GATESIZES:
                    doGatesizes((GatesizesNode *)node, newindent, mode, arg); break;
                case NED_GATESIZE:
                    doGatesize((GatesizeNode *)node, newindent, mode, arg); break;
                case NED_SUBSTMACHINES:
                    doSubstmachines((SubstmachinesNode *)node, newindent, mode, arg); break;
                case NED_SUBSTMACHINE:
                    doSubstmachine((SubstmachineNode *)node, newindent, mode, arg); break;
                case NED_CONNECTIONS:
                    doConnections((ConnectionsNode *)node, newindent, mode, arg); break;
                case NED_CONNECTION:
                    doConnection((ConnectionNode *)node, newindent, mode, arg); break;
                case NED_CONN_ATTR:
                    doConnattr((ConnAttrNode *)node, newindent, mode, arg); break;
                case NED_FOR_LOOP:
                    doForloop((ForLoopNode *)node, newindent, mode, arg); break;
                case NED_LOOP_VAR:
                    doLoopvar((LoopVarNode *)node, newindent, mode, arg); break;
                case NED_DISPLAY_STRING:
                    doDisplayString((DisplayStringNode *)node, newindent, mode, arg); break;
                case NED_EXPRESSION:
                    doExpression((ExpressionNode *)node, newindent, mode, arg); break;
                case NED_CPLUSPLUS:
                    doCplusplus((CplusplusNode *)node, newindent, mode, arg); break;
                case NED_STRUCT_DECL:
                    doStructDecl((StructDeclNode *)node, newindent, mode, arg); break;
                case NED_CLASS_DECL:
                    doClassDecl((ClassDeclNode *)node, newindent, mode, arg); break;
                case NED_ENUM:
                    doEnum((EnumNode *)node, newindent, mode, arg); break;
                case NED_ENUM_FIELDS:
                    doEnumFields((EnumFieldsNode *)node, newindent, mode, arg); break;
                case NED_ENUM_FIELD:
                    doEnumField((EnumFieldNode *)node, newindent, mode, arg); break;
                case NED_MESSAGE:
                    doMessage((MessageNode *)node, newindent, mode, arg); break;
                case NED_CLASS:
                    doClass((ClassNode *)node, newindent, mode, arg); break;
                case NED_STRUCT:
                    doStruct((StructNode *)node, newindent, mode, arg); break;
                case NED_FIELDS:
                case NED_FIELD:
                case NED_PROPERTIES:
                case NED_PROPERTY:
                    INTERNAL_ERROR1(node,"generateItem(): element cannot be generated in itself: %s", node->getTagName());
                default:
                    INTERNAL_ERROR1(node,"generateItem(): unrecognized tag: %s", node->getTagName());
            }
            break;
        case MODE_CLEANUP:
            switch (tagcode)
            {
                case NED_SUBMODULE:
                    doSubmoduleCleanup((SubmoduleNode *)node, newindent, mode, arg); break;
                default:
                    generateChildren(node, newindent, mode, arg);
            }
            break;
        default:
            INTERNAL_ERROR1(node,"generateItem(): unrecognized mode %d", mode);
    }
}

void NEDCppGenerator::generateChildren(NEDElement *node, const char *indent, int mode, const char *arg)
{
    for (NEDElement *child=node->getFirstChild(); child; child = child->getNextSibling())
    {
        generateItem(child, indent, mode, arg);
    }
}

void NEDCppGenerator::generateChildrenWithTags(NEDElement *node, const char *tags, const char *indent, int mode, const char *arg)
{
    for (NEDElement *child=node->getFirstChild(); child; child = child->getNextSibling())
    {
        if (strstr(tags, child->getTagName()))
            generateItem(child, indent, mode, arg);
    }
}

void NEDCppGenerator::generateChildrenExceptTags(NEDElement *node, const char *tags, const char *indent, int mode, const char *arg)
{
    for (NEDElement *child=node->getFirstChild(); child; child = child->getNextSibling())
    {
        if (!strstr(tags, child->getTagName()))
            generateItem(child, indent, mode, arg);
    }
}

ExpressionNode *NEDCppGenerator::findExpression(NEDElement *parent, const char *target)
{
    if (!parent)
        return NULL;
    for (NEDElement *child=parent->getFirstChildWithTag(NED_EXPRESSION); child; child = child->getNextSiblingWithTag(NED_EXPRESSION))
    {
        ExpressionNode *expr = (ExpressionNode *)child;
        if (!strcmp(expr->getTarget(),target))
            return expr;
    }
    return NULL;
}

ConstNode *NEDCppGenerator::getConstantExpression(ExpressionNode *node)
{
    if (!node)
        return NULL;
    NEDElement *firstchild = node->getFirstChild();
    if (!firstchild || firstchild->getTagCode()!=NED_CONST || firstchild->getNextSibling())
        return NULL;
    return (ConstNode *)firstchild;
}

NEDElement *NEDCppGenerator::findFirstChildWithAttribute(NEDElement *node, int tagcode,
                                                         const char *attr, const char *attrvalue)
{
    if (!node)
        return NULL;
    for (NEDElement *child=node->getFirstChildWithTag(tagcode); child; child = child->getNextSiblingWithTag(tagcode))
    {
        const char *val = child->getAttribute(attr);
        if (val && !strcmp(val,attrvalue))
            return child;
    }
    return NULL;
}


//--------------------------------------

//
// NED-I: Modules, channels
//

// helpers
void NEDCppGenerator::printTemporaryVariables(const char *indent)
{
        out << indent << "// temporary variables:\n";
        out << indent << "cPar tmpval;\n";  // for compiled expressions
        out << indent << "cPar *machinename;\n";  // for 'on:'
        out << indent << "const char *type_name;\n";   // for submodule creation
        out << indent << "cArray machines;\n";  // for 'on:'
        out << indent << "bool islocal;\n";  // for 'on:'
        out << indent << "int size;\n";  // for gatesize
        out << "\n";
}


void NEDCppGenerator::beginConditionalBlock(NEDElement *node, const char *&indent, int mode, const char *)
{
    ExpressionNode *condition = findExpression(node, "condition");
    if (condition)
    {
        out << indent << "if (";
        generateItem(condition, indent, mode);
        out << ")\n";
        out << indent << "{\n";
        indent = increaseIndent(indent);
    }
}

void NEDCppGenerator::endConditionalBlock(NEDElement *node, const char *&indent, int mode, const char *)
{
    ExpressionNode *condition = findExpression(node, "condition");
    if (condition)
    {
        indent = decreaseIndent(indent);
        out << indent << "}\n";
    }
}

void NEDCppGenerator::writeProlog(ostream& out)
{
    // cpp file prolog
    time_t nowt;
    struct tm *now;
    time(&nowt);
    now = localtime(&nowt);

    out << "//-----------------------------------------\n";
    out << "//\n";
    out << "// Generated by nedtool, version " NEDC_VERSION "\n";
    out << "// date:    " << asctime(now) << "\n";
    out << "//\n";
    out << "//-----------------------------------------\n\n\n";

    out << "#include <math.h>\n";
    out << "#include \"omnetpp.h\"\n\n";

    out << "// NEDC version check\n"
           "#define NEDC_VERSION " NEDC_VERSION_HEX "\n"
           "#if (NEDC_VERSION!=OMNETPP_VERSION)\n"
           "//#    error Version mismatch! Probably this file was generated by an earlier version of nedc: 'make clean' should help.\n"
           "#endif\n\n"; // FIXME add back version check...

    out << "// Disable warnings about unused variables. For MSVC and BC only:\n"
           "// GCC has no way to turn on its -Wunused option in a source file :(\n"
           "#ifdef _MSC_VER\n"
           "#  pragma warning(disable:4101)\n"
           "#endif\n"
           "#ifdef __BORLANDC__\n"
           "#  pragma warn -waus\n"
           "#  pragma warn -wuse\n"
           "#endif\n\n";

    out << "static void check_module_count(int num, const char *mod, const char *parentmod) {\n";
    out << "    if (num<0)\n";
    out << "        throw new cException(\"Negative module vector size %s[%d] in compound module %s\", mod,(int)num,parentmod);\n";
    out << "}\n";
    out << "static void check_gate_count(int num, const char *mod, const char *gate, const char *parentmod) {\n";
    out << "    if (num<0)\n";
    out << "        throw new cException(\"Negative gate vector size %s.%s[%d] in compound module %s\", mod,gate,(int)num,parentmod);\n";
    out << "}\n";
    out << "static void check_loop_bounds(long lower, long upper, const char *parentmod) {\n";
    out << "    if (lower<0)\n";
    out << "        throw new cException(\"Bad loop bounds (%d..%d) in compound module %s\", (int)lower,(int)upper,parentmod);\n";
    out << "}\n";
    out << "static void check_module_index(long index, cModule **modvar, const char *modname, const char *parentmod) {\n";
    out << "    if (index<0 || index>=modvar[0]->size())\n";
    out << "        throw new cException(\"Bad submodule index %s[%d] in compound module %s\", modname,(int)index,parentmod);\n";
    out << "}\n";
    out << "static void check_channel_params(double delay, double err, const char *channel) {\n";
    out << "    if (delay<0.0)\n";
    out << "        throw new cException(\"Negative delay value %lf in channel %s\",(double)delay,channel);\n";
    out << "    if (err<0.0 || err>1.0)\n";
    out << "        throw new cException(\"Incorrect error value %lf in channel %s\",(double)err,channel);\n";
    out << "}\n";
    out << "static void check_modtype(cModuleType *modtype, const char *modname) {\n";
    out << "    if (!modtype)\n";
    out << "        throw new cException(\"Simple module type definition %s not found\", modname);\n";
    out << "}\n";
    out << "static void check_gate(int gateindex, const char *modname, const char *gatename) {\n";
    out << "    if (gateindex==-1)\n";
    out << "        throw new cException(\"Gate %s.%s not found\",modname,gatename);\n";
    out << "}\n";
    out << "static void check_anc_param(cPar *ptr, const char *parname, const char *compoundmod) {\n";
    out << "    if (!ptr)\n";
    out << "        throw new cException(\"Unknown ancestor parameter named %s in compound module %s\", parname,compoundmod);\n";
    out << "}\n";
    out << "static void check_param(cModule *modulep, const char *parname) {\n";
    out << "    if (!modulep->hasPar(parname))\n";
    out << "        throw new cException(\"Unknown parameter named %s\",parname);\n";
    out << "}\n";
    out << "\n";

    out << "static void _readModuleParameters(cModule *mod)\n";
    out << "{\n";
    out << "    int n = mod->params();\n";
    out << "    for (int k=0; k<n; k++)\n";
    out << "    {\n";
    out << "        if (mod->par(k).isInput())\n";
    out << "        {\n";
    out << "            mod->par(k).read();\n";
    out << "        }\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";

    out << "static bool _isLocal(cArray& machines)\n";
    out << "{\n";
    out << "    return !simulation.netInterface() || simulation.netInterface()->isLocalMachineIn( machines );\n";
    out << "}\n";
    out << "\n";

    out << "static cFunctionType *_getFunction(const char *funcname, int argcount)\n";
    out << "{\n";
    out << "    cFunctionType *functype = findFunction(funcname,argcount);\n";
    out << "    if (!functype)\n";
    out << "        throw new cException(\"Function %s with %d args not found\", funcname, argcount);\n";
    out << "    return functype;\n";
    out << "}\n";
    out << "\n";

    out << "static cChannel *_createSimpleChannel(double delay, double error, double datarate)\n";
    out << "{\n";
    out << "    cChannel *channel = new cSimpleChannel(\"channel\");\n";
    out << "    if (delay!=0)\n";
    out << "    {\n";
    out << "        cPar *par = new cPar(\"delay\");\n";
    out << "        (*par) = delay;\n";
    out << "        channel->addPar(par);\n";
    out << "    }\n";
    out << "    if (error!=0)\n";
    out << "    {\n";
    out << "        cPar *par = new cPar(\"error\");\n";
    out << "        (*par) = error;\n";
    out << "        channel->addPar(par);\n";
    out << "    }\n";
    out << "    if (datarate!=0)\n";
    out << "    {\n";
    out << "        cPar *par = new cPar(\"datarate\");\n";
    out << "        (*par) = datarate;\n";
    out << "        channel->addPar(par);\n";
    out << "    }\n";
    out << "    return channel;\n";
    out << "}\n";
    out << "\n";

    out << "static cChannel *_createChannel(const char *name)\n";
    out << "{\n";
    out << "    cLinkType *link = findLink(name);\n";
    out << "    if (!link)\n";
    out << "        throw new cException(\"Channel type %s not found\", name);\n";
    out << "    cChannel *channel = new cSimpleChannel(\"channel\",link);\n";
    out << "    return channel;\n";
    out << "}\n";
    out << "\n";
}

// generators
void NEDCppGenerator::doNedFiles(NedFilesNode *node, const char *indent, int mode, const char *)
{
    generateChildren(node, increaseIndent(indent));
}

void NEDCppGenerator::doNedFile(NedFileNode *node, const char *indent, int mode, const char *)
{
    // generate children
    generateChildren(node, indent);
}

void NEDCppGenerator::doImports(ImportNode *node, const char *indent, int mode, const char *)
{
    // no code to be generated
}

void NEDCppGenerator::doImport(ImportedFileNode *node, const char *indent, int mode, const char *)
{
    // no code to be generated
}

ChannelAttrNode *findChannelAttribute(ChannelNode *node, const char *attrname)
{
    for (NEDElement *child=node->getFirstChildWithTag(NED_CHANNEL_ATTR); child; child = child->getNextSiblingWithTag(NED_CHANNEL_ATTR))
    {
        ChannelAttrNode *attr = (ChannelAttrNode *)child;
        if (!strcmp(attr->getName(), attrname))
            return attr;
    }
    return 0;
}

void NEDCppGenerator::doChannel(ChannelNode *node, const char *indent, int mode, const char *)
{
    // generate expression shells
    exprgen.collectExpressions(node);
    exprgen.generateExpressionClasses();

    // generate channel attributes code
    generateChildren(node, increaseIndent(indent));

    // generate channel code itself
    const char *channelname = node->getName();
    bool delay = findChannelAttribute(node, "delay") != 0;
    bool error = findChannelAttribute(node, "error") != 0;
    bool datarate = findChannelAttribute(node, "datarate") != 0;

    out << "// channel definition " << channelname << "\n";
    out << "Define_Link( " << channelname << ", ";
    out << (delay ?    channelname:"NULL") << (delay    ? "__delay"   :"") << ", ";
    out << (error ?    channelname:"NULL") << (error    ? "__error"   :"") << ", ";
    out << (datarate ? channelname:"NULL") << (datarate ? "__datarate":"") << ");\n\n";
}

void NEDCppGenerator::doChannelAttr(ChannelAttrNode *node, const char *indent, int mode, const char *)
{
    const char *channelname = ((ChannelNode *)(node->getParent()))->getName();  // channel name
    const char *attrname = node->getName();    // attribute name

    out << "static cPar *" << channelname << "__" << attrname << "()\n";
    out << "{\n";
    out << "    cPar tmpval;\n";
    out << "    cPar *p = new cPar(\"" << channelname << "_datarate\");\n";
    ExpressionNode *value = findExpression(node, "value");
    out << "    *p = ";
    generateItem(value, indent, mode);
    out << ";\n";
    out << "    return p;\n";
    out << "}\n\n";
}

void NEDCppGenerator::doNetwork(NetworkNode *node, const char *indent, int mode, const char *)
{
    in_network = true;
    const char *networkname = node->getName();

    // class declaration
    out << "class " << networkname <<  " : public cNetworkType\n";
    out << "{\n";
    out << "  public:\n";
    out << "    " << networkname << "(const char *name) : cNetworkType(name) {}\n";
    out << "    " << networkname << "(const " << networkname << "& n)  {setName(n.name());operator=(n);}\n";
    out << "    virtual void setupNetwork();\n";
    out << "};\n\n";

    out << "Define_Network( " << networkname << " );\n\n";

    // generate expression shells
    exprgen.collectExpressions(node);
    exprgen.generateExpressionClasses();

    // prolog
    out << "void " << networkname << "::setupNetwork()\n";
    out << "{\n";
    indent = increaseIndent(indent);

    printTemporaryVariables(indent);
    out << indent << "cModuleType *modtype;\n\n";

    // what comes here is a simplified version of submodule creation:
    // - no 'like' keyword
    // - network cannot be a vector of modules

    // find module descriptor
    out << indent << "modtype = findModuleType( \"" << node->getTypeName() << "\" );\n";
    out << indent << "check_modtype( modtype, \"" << node->getTypeName() << "\" );\n";

    // create module
    submodule_name = node->getName();
    submodule_var = node->getName();
    submodule_var += "_p";
    out << indent << "cModule *" << submodule_var.c_str() << ";\n\n";

    out << indent << "// create module:\n";
    out << indent << "islocal = _isLocal( machines );\n";

    // check locality
    out << indent << "if (!islocal)\n";
    out << indent << "    throw new cException(\"Local machine `%s' is not among machines specified for this network\",\n";
    out << indent << "              simulation.netInterface()->localhost());\n";

    // do create module
    out << indent << submodule_var.c_str() << " = modtype->create( \"" << submodule_name.c_str() << "\", NULL, islocal);\n";

    // FIXME:
    //   check module type exists
    //   check machine count matches

#if 0
     // they are equal: set machine list if machines are given --LG
     if ( cmd.submod_machs.count ) // actual machine list is not empty
     {
       int i;

       print_remark(tmp, "set machine list:");
       for (i=0; i<cmd.submod_machs.count; i++)
           out << indent << submodule_var << "->setMachinePar( \"" << nl_retr_ith(machs,i+1)->namestr << "\", ev.getPhysicalMachineFor(\"" << nl_retr_ith( &cmd.submod_machs,i+1)->namestr << "\") );\n",
       out << "\n";
       nl_empty(&cmd.submod_machs);
     }
#endif

    // generate children (except expressions)
    generateChildrenWithTags(node, "substmachines,substparams,gatesizes", indent, mode);

    // add display string
    DisplayStringNode *dispstr = (DisplayStringNode *)node->getFirstChildWithTag(NED_DISPLAY_STRING);
    if (dispstr)
    {
        out << "\n" << indent << submodule_var.c_str() << "->setDisplayStringAsParent(\""
            <<  dispstr->getValue() << "\");\n";
    }

    // build function call
    out << indent << "// build submodules recursively (if it has any):\n";
    out << indent << submodule_var.c_str() << "->buildInside();\n";

    out << indent << "machines.clear();\n\n";

    // epilog
    out << "}\n\n";
    in_network = false;
}

void NEDCppGenerator::doSimple(SimpleModuleNode *node, const char *indent, int mode, const char *)
{
    // generate Interface() stuff
    module_name = node->getName();

    out << "ModuleInterface( " << module_name.c_str() << " )\n";
    generateChildrenWithTags(node, "params,gates,machines", increaseIndent(indent));
    out << "EndInterface\n\n";
    out << "Register_ModuleInterface( " << module_name.c_str() << " )\n\n";

    // epilog
    out << "//// Sample code:\n";
    out << "// class " << module_name.c_str() << " : public cSimpleModule\n";
    out << "// {\n";
    out << "//     Module_Class_Members(" << module_name.c_str() << ",cSimpleModule,16384)\n";
    out << "//     virtual void activity();\n";
    out << "//     // Add you own member functions here!\n";
    out << "// };\n";
    out << "//\n";
    out << "// Define_Module( " << module_name.c_str() << " );\n";
    out << "//\n";
    out << "// void " << module_name.c_str() << "::activity()\n";
    out << "// {\n";
    out << "//     // Put code for simple module activity here!\n";
    out << "// }\n";
    out << "//\n\n";
}

void NEDCppGenerator::doModule(CompoundModuleNode *node, const char *indent, int mode, const char *)
{
    // generate Interface() stuff
    module_name = node->getName();

    out << "ModuleInterface( " << module_name.c_str() << " )\n";
    generateChildrenWithTags(node, "params,gates,machines", increaseIndent(indent));
    out << "EndInterface\n\n";

    out << "Register_ModuleInterface( " << module_name.c_str() << " )\n\n";

    // class declaration
    out << "class " << module_name.c_str() << " : public cCompoundModule\n";
    out << "{\n";
    out << "  public:\n";
    out << "    " << module_name.c_str() << "(const char *name, cModule *parent) :\n";
    out << "      cCompoundModule(name, parent) {}\n";
    out << "\n";
    out << "  protected:\n";
    out << "    virtual void doBuildInside();\n";
    out << "};\n\n";

    out << "Define_Module( " << module_name.c_str() << " );\n\n";

    // generate expression shells
    exprgen.collectExpressions(node);
    exprgen.generateExpressionClasses();

    // prolog
    out << "void " << module_name.c_str() << "::doBuildInside()\n";
    out << "{\n";

    indent = increaseIndent(indent);
    out << indent << "cModule *mod = this;\n\n";
    printTemporaryVariables(indent);

    // add display string
    DisplayStringNode *dispstr = (DisplayStringNode *)node->getFirstChildWithTag(NED_DISPLAY_STRING);
    if (dispstr)
    {
        out << "\n" << indent << "mod->setDisplayStringAsParent(\"" << dispstr->getValue() << "\");\n";
    }

    // generate submodules
    generateChildrenWithTags(node, "submodules", indent);

    // generate connections
    generateChildrenWithTags(node, "connections", indent);

    // generate code to free array of module pointers
    generateChildren(node, indent, MODE_CLEANUP);
    indent = decreaseIndent(indent);
    out << indent << "}\n\n";
}

void NEDCppGenerator::doParams(ParamsNode *node, const char *indent, int mode, const char *)
{
    // generate children
    out << indent << "// parameters:\n";
    generateChildren(node, indent, mode);
}

void NEDCppGenerator::doParam(ParamNode *node, const char *indent, int mode, const char *)
{
    const char *typecode;
    const char *datatype = node->getDataType();
    if (strcmp(datatype,"numeric"))
       typecode = "ParType_Numeric";
    if (strcmp(datatype,"numeric const"))
       typecode = "ParType_Numeric ParType_Const";
    if (strcmp(datatype,"string"))
       typecode = "ParType_String";
    if (strcmp(datatype,"bool"))
       typecode = "ParType_Bool";
    if (strcmp(datatype,"any"))
       typecode = "ParType_Any";

    out << indent << "Parameter( " << node->getName() << ", " << typecode << " )\n";
}

void NEDCppGenerator::doGates(GatesNode *node, const char *indent, int mode, const char *)
{
    // generate children
    out << indent << "// gates:\n";
    generateChildren(node, indent, mode);
}

void NEDCppGenerator::doGate(GateNode *node, const char *indent, int mode, const char *)
{
    out << indent << "Gate( " << node->getName();
    out << (node->getIsVector() ? "[]" : "") << ", ";
    out << (node->getDirection()==NED_GATEDIR_INPUT ? "GateDir_Input" : "GateDir_Output") << " )\n";
}

void NEDCppGenerator::doMachines(MachinesNode *node, const char *indent, int mode, const char *)
{
    // generate children
    out << indent << "// machines:\n";
    generateChildren(node, indent, mode);
}

void NEDCppGenerator::doMachine(MachineNode *node, const char *indent, int mode, const char *)
{
    out << indent << "Machine( " << node->getName() << " )\n";
}

void NEDCppGenerator::doSubmodules(SubmodulesNode *node, const char *indent, int mode, const char *)
{
    // prolog
    out << indent << "// submodules:\n";
    out << indent << "cModuleType *modtype = NULL;\n";
    out << indent << "long submod_count = 0;\n";
    out << indent << "long submod_i = 0;\n\n";

    // generate children
    generateChildren(node, indent, mode);
}

void NEDCppGenerator::doSubmodule(SubmoduleNode *node, const char *indent, int mode, const char *)
{
    out << indent << "//\n";
    out << indent << "// submodule '" << node->getName() << "':\n";
    out << indent << "//\n";

    // find module descriptor
    if (!strnotnull(node->getLikeParam()))
    {
        out << indent << "modtype = findModuleType( \"" << node->getTypeName() << "\" );\n";
        out << indent << "check_modtype( modtype, \"" << node->getTypeName() << "\" );\n";
    }
    else
    {
        out << indent << "type_name = mod->par(\"" << node->getLikeParam() << "\");\n";
        out << indent << "modtype = findModuleType( type_name );\n";
        out << indent << "check_modtype( modtype, type_name );\n";
    }

    // create module
    submodule_name = node->getName();

    ExpressionNode *vectorsize = findExpression(node, "vector-size");
    if (!vectorsize)
    {
        submodule_var = node->getName();
        submodule_var += "_p";
        out << indent << "cModule *" << submodule_var.c_str() << ";\n\n";
    }
    else
    {
        out << indent << "submod_count = ";
        generateItem(vectorsize, indent, mode);
        out << ";\n";
        submodule_var = node->getName();
        submodule_var += "_p";
        out << indent << "check_module_count(submod_count,\"" << submodule_name.c_str() << "\",\"" << module_name.c_str() << "\");\n";
        out << indent << "cModule **" << submodule_var.c_str() << " = new cModule *[submod_count];\n\n";
        out << indent << "for (submod_i=0; submod_i<submod_count; submod_i++)\n";
        out << indent << "{\n";
        submodule_var += "[submod_i]";
        indent = increaseIndent(indent);
    }

    out << indent << "// create module:\n";
    out << indent << "islocal = _isLocal( machines );\n";

    // create submodule
    out << indent << submodule_var.c_str() << " = modtype->create( \"" << submodule_name.c_str() << "\", mod, islocal);\n";

    if (vectorsize)
    {
        out << indent << submodule_var.c_str() << "->setIndex(submod_i, submod_count);\n\n";
    }

    // FIXME:
    //   check module type exists
    //   check machine count matches

#if 0
     // they are equal: set machine list if machines are given --LG
     if ( cmd.submod_machs.count ) // actual machine list is not empty
     {
       int i;

       print_remark(tmp, "set machine list:");
       for (i=0; i<cmd.submod_machs.count; i++)
           out << indent << submodule_var << "->setMachinePar( \"" << nl_retr_ith(machs,i+1)->namestr << "\", ((cPar *)machines[" << i << "])->stringValue() );\n",
       out << "\n";
       nl_empty(&cmd.submod_machs);
     }
#endif

    // generate children (except expressions)
    generateChildrenWithTags(node, "substmachines,substparams,gatesizes", indent, mode);

    // add display string
    DisplayStringNode *dispstr = (DisplayStringNode *)node->getFirstChildWithTag(NED_DISPLAY_STRING);
    if (dispstr)
    {
        out << "\n" << indent << submodule_var.c_str() << "->setDisplayString(\"" <<  dispstr->getValue() << "\");\n";
    }

    // build function call
    out << indent << "// build submodules recursively (if it has any):\n";
    out << indent << submodule_var.c_str() << "->buildInside();\n";

    if (vectorsize)
    {
        indent = decreaseIndent(indent);
        out << indent << "}\n";
    }
    out << indent << "machines.clear();\n\n";
}

void NEDCppGenerator::doSubmoduleCleanup(SubmoduleNode *node, const char *indent, int mode, const char *)
{
    if (findExpression(node, "vector-size")!=NULL)
        out << indent << "delete [] " << node->getName() << "_p;\n";
}

void NEDCppGenerator::doSubstparams(SubstparamsNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// parameter assignments:\n";
    beginConditionalBlock(node, indent, mode, arg);

    // generate children
    generateChildrenExceptTags(node, "expression", indent, mode);

    // epilog: read all parameters
    out << indent << "_readModuleParameters(" << submodule_var.c_str() << ");\n";

    endConditionalBlock(node, indent, mode, arg);
    out << "\n";
}

void NEDCppGenerator::doSubstparam(SubstparamNode *node, const char *indent, int mode, const char *)
{
    // FIXME check type mismatch:

//    // bool will also be treated as numeric
//    if (decl_type==TYPE_CONST_NUM || decl_type==TYPE_BOOL)
//        decl_type = TYPE_NUMERIC;
//    if (expr_type==TYPE_CONST_NUM || expr_type==TYPE_BOOL)
//        expr_type = TYPE_NUMERIC;
//
//    if (decl_type!=expr_type &&
//        decl_type!=TYPE_ANYTYPE &&
//        expr_type!=TYPE_ANYTYPE)
//    {
//        sprintf (errstr,"Type mismatch for parameter \"%s\"\n",pname);
//        adderr;
//    }

    // set parameter value
    out << indent << "check_param(" << submodule_var.c_str() << ", \"" << node->getName() << "\");\n";
    out << indent << submodule_var.c_str() << "->par(\"" << node->getName() << "\") = ";
    ExpressionNode *paramvalue = findExpression(node, "value");
    if (paramvalue)  generateItem(paramvalue, indent, mode);
    out << ";\n\n";
}

void NEDCppGenerator::doGatesizes(GatesizesNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// gatesizes:\n";
    beginConditionalBlock(node, indent, mode, arg);

    // generate children
    generateChildrenExceptTags(node, "expression", indent, mode);

    // epilog
    endConditionalBlock(node, indent, mode, arg);
    out << "\n";
}

void NEDCppGenerator::doGatesize(GatesizeNode *node, const char *indent, int mode, const char *)
{
    out << indent << "size = ";
    ExpressionNode *vectorsize = findExpression(node, "vector-size");
    if (vectorsize)  generateItem(vectorsize, indent, mode);
    out << ";\n";

    out << indent << "check_gate_count( size, \"" << submodule_name.c_str() << "\",\"" << node->getName() << "\",\"" << module_name.c_str() << "\");\n";
    out << indent << submodule_var.c_str() << "->setGateSize(\"" << node->getName() << "\", size);\n\n";
}

void NEDCppGenerator::doSubstmachines(SubstmachinesNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// 'on' section:\n";
    beginConditionalBlock(node, indent, mode, arg);

    // generate children
    generateChildrenExceptTags(node, "expression", indent, mode);

    // epilog
    endConditionalBlock(node, indent, mode, arg);
    out << "\n";
}

void NEDCppGenerator::doSubstmachine(SubstmachineNode *node, const char *indent, int mode, const char *)
{
    if (!in_network)
    {
        out << indent << "machinename = new cPar();\n";
        out << indent << "*machinename = mod->machinePar(\"" << node->getName() << "\");\n";
        out << indent << "machines.add( machinename );\n";
    }
    else
    {
#if 0
        int idx;
        char *phys_mach;
        idx = nl_find (machine_list, maname);
        if (idx==0)
           phys_mach = maname; // no substitution!
        else
           phys_mach = nl_retr_ith( &machine_list, idx )->parstr;

        out << %s << "par = new cPar();\n"
                      %s << "*par = ev.getPhysicalMachineFor(\"" << %s << "\");\n"
                      %s << "machines.add( par );\n",
                       indent, indent, phys_mach, indent );
#endif
    }
    out << "\n";
}

void NEDCppGenerator::doConnections(ConnectionsNode *node, const char *indent, int mode, const char *)
{
    // prolog
    out << indent << "//\n";
    out << indent << "// connections:\n";
    out << indent << "//\n";
    out << indent << "cLinkType *channeltype;\n";
    out << indent << "cModule *tmpmod;\n";
    out << indent << "cPar *par;\n";
    out << indent << "cChannel *channel;\n";
    out << indent << "cGate *srcgate, *destgate;\n\n";

    // generate children
    generateChildren(node, indent, mode);

    // epilog
    if (node->getCheckUnconnected())
    {
       // check connections
       // FIXME compile-time check as far as possible
       out << indent << "// check all gates are connected:\n";
       out << indent << "mod->checkInternalConnections();\n\n";
    }
}

void NEDCppGenerator::resolveGate(const char *modname, ExpressionNode *modindex, const char *gatename, ExpressionNode *gateindex)
{
   // module
   if (!strnotnull(modname))
   {
       out << "mod";
   }
   else
   {
       out << modname << "_p";
       if (modindex)
       {
            out << "[(long)("; // FIXME bounds check needed!
            generateItem(modindex,  "        ");
            out << ")]";
       }
   }

   // gate
   out << "->gate(\"" << gatename << "\"";
   if (gateindex)
   {
       out << ", ";
       generateItem(gateindex, "        ");
   }
   out << ")";

   //   out << indent << "check_gate( gateL, \"" << %s << "\", "
   //      "indexedname(b1,\"" << %s << "\",gate_nr_L) );\n\n",
   //      indent, mod_L, gate_L );
}

void NEDCppGenerator::resolveConnectionAttributes(ConnectionNode *node, const char *indent, int mode)
{
    // emit code that creates channel and puts its ptr to "channel" variable

    // channel?
    ConnAttrNode *channelAttr = (ConnAttrNode *)findFirstChildWithAttribute(node,NED_CONN_ATTR,"name","channel");
    ConstNode *channel = getConstantExpression(findExpression(channelAttr,"value"));
    if (channel)
    {
        out << indent << "channel = _createChannel(\"" << channel->getValue() << "\");\n";
        return;
    }

    // optimization: assess if simplified code can be generated
    ConnAttrNode *delayAttr = (ConnAttrNode *)findFirstChildWithAttribute(node,NED_CONN_ATTR,"name","delay");
    ConstNode *delay = getConstantExpression(findExpression(delayAttr,"value"));
    bool isDelaySimple =  !delayAttr ? true : delay!=NULL;
    ConnAttrNode *errorAttr = (ConnAttrNode *)findFirstChildWithAttribute(node,NED_CONN_ATTR,"name","error");
    ConstNode *error = getConstantExpression(findExpression(errorAttr,"value"));
    bool isErrorSimple =  !errorAttr ? true : error!=NULL;
    ConnAttrNode *datarateAttr = (ConnAttrNode *)findFirstChildWithAttribute(node,NED_CONN_ATTR,"name","datarate");
    ConstNode *datarate = getConstantExpression(findExpression(datarateAttr,"value"));
    bool isDatarateSimple =  !datarateAttr ? true : datarate!=NULL;

    if (isDelaySimple && isErrorSimple && isDatarateSimple)
    {
        // generate optimized code: delay, error, datarate with a specialized function
        out << indent << "channel = _createSimpleChannel("
            << (delay ? delay->getValue() : "0") << ", "
            << (error ? error->getValue() : "0") << ", "
            << (datarate ? datarate->getValue() : "0") << ");\n";

        // add possible other attributes in the normal way
        for (NEDElement *child=node->getFirstChildWithTag(NED_CONN_ATTR); child; child = child->getNextSiblingWithTag(NED_CONN_ATTR))
        {
            ConnAttrNode *connattr = (ConnAttrNode *)child;
            if (strcmp(connattr->getName(),"delay")!=0 &&
                strcmp(connattr->getName(),"error")!=0 &&
                strcmp(connattr->getName(),"datarate")!=0)
            {
                generateItem(child, indent, mode);
            }
        }
        return;
    }

    // fallback: general code
    out << "\n" << indent << "// add channel\n";
    out << indent << "channel = new cSimpleChannel(\"channel\");\n";
    generateChildrenWithTags(node, "conn-attr", indent);
}

void NEDCppGenerator::doConnection(ConnectionNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// connection\n";
    beginConditionalBlock(node, indent, mode, arg);

    // create connection
    out << indent << "srcgate = ";
    resolveGate(node->getSrcModule(), findExpression(node,"src-module-index"),
                node->getSrcGate(), findExpression(node,"src-gate-index"));
    out << ";\n";

    out << indent << "destgate = ";
    resolveGate(node->getDestModule(), findExpression(node,"dest-module-index"),
                node->getDestGate(), findExpression(node,"dest-gate-index"));
    out << ";\n";


    // add channel if needed
    if (!node->getFirstConnAttrChild())
    {
        // connect without channel
        out << indent << "srcgate->connectTo(destgate);\n";
    }
    else
    {
        // fill "channel" variable, then connect
        resolveConnectionAttributes(node, indent, mode);
        out << indent << "srcgate->connectTo(destgate,channel);\n";
    }

    // display string
    DisplayStringNode *dispstr = (DisplayStringNode *)node->getFirstChildWithTag(NED_DISPLAY_STRING);
    if (dispstr)
    {
        out << "\n" << indent << "srcgate->setDisplayString(\"" <<  dispstr->getValue() << "\");\n";
    }

    // epilog
    endConditionalBlock(node, indent, mode, arg);
    out << "\n";
}

void NEDCppGenerator::doConnattr(ConnAttrNode *node, const char *indent, int mode, const char *arrow)
{
    ExpressionNode *value = findExpression(node, "value");
    out << indent << "par = new cPar(\"" << node->getName() << "\");\n";
    out << indent << "(*par) = ";
    generateItem(value, indent, mode);
    out << indent << ";\n";
    out << indent << "channel->addPar(par);\n";
}

void NEDCppGenerator::doForloop(ForLoopNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// for loop:\n";

    // open new block to avoid C++ redeclaration error when several loops with same variables exist
    out << indent << "{\n";
    indent = increaseIndent(indent);

    // open loops
    for (NEDElement *child=node->getFirstChildWithTag(NED_LOOP_VAR); child; child = child->getNextSiblingWithTag(NED_LOOP_VAR))
    {
        generateItem(child, indent, mode);
        out << indent << "{\n";
        indent = increaseIndent(indent);
    }

    // generate children: then connections
    generateChildrenWithTags(node, "connection", indent, mode, arg);

    // close loops
    for (NEDElement *child1=node->getFirstChildWithTag(NED_LOOP_VAR); child1; child1 = child1->getNextSiblingWithTag(NED_LOOP_VAR))
    {
        indent = decreaseIndent(indent);
        out << indent << "}\n";
    }

    // close block
    indent = decreaseIndent(indent);
    out << indent << "}\n";
}

void NEDCppGenerator::doLoopvar(LoopVarNode *node, const char *indent, int mode, const char *)
{
    ExpressionNode *fromvalue = findExpression(node, "from-value");
    ExpressionNode *tovalue = findExpression(node, "to-value");

    out << indent << "long start = ";
    generateItem(fromvalue, indent, mode);
    out << ";\n";
    out << indent << "long end = ";
    generateItem(tovalue, indent, mode);
    out << ";\n";
    out << indent << "long " << node->getParamName() << "_var;\n";
    out << indent << "for (" << node->getParamName() << "_var=start; " << node->getParamName() << "_var<=end; " << node->getParamName() << "_var++)\n";
}

void NEDCppGenerator::doDisplayString(DisplayStringNode *node, const char *indent, int mode, const char *)
{
    // FIXME!!!
}


void NEDCppGenerator::doExpression(ExpressionNode *node, const char *indent, int mode, const char *arg)
{
    exprgen.generateExpressionUsage((ExpressionNode *)node,indent);
}

