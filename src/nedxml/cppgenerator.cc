//==========================================================================
//   CPPGENERATOR.CC
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
#include <stdio.h>
#include <time.h>

#include "nedelements.h"
#include "cppgenerator.h"
#include "nederror.h"

using std::ostream;

#define NEDC_VERSION "3.4"
#define NEDC_VERSION_HEX "0x0304"


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
  out(_out), outh(_outh), exprgen(_out,symtab), symboltable(symtab)
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
                case NED_MESSAGE_DECL:
                    doMessageDecl((MessageDeclNode *)node, newindent, mode, arg); break;
                case NED_ENUM_DECL:
                    doEnumDecl((EnumDeclNode *)node, newindent, mode, arg); break;
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
        case MODE_FINALLY:
            switch (tagcode)
            {
                case NED_SUBMODULE:
                    doSubmoduleFinally((SubmoduleNode *)node, newindent, mode, arg); break;
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

//--------------------------------------

//
// NED-I: Modules, channels
//

// helpers
void NEDCppGenerator::printTemporaryVariables(const char *indent)
{
        out << indent << "// temporary variables:\n";
        out << indent << "cPar tmpval;\n";  // for compiled expressions
        out << indent << "const char *modtypename;\n";   // for submodule creation
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
    out << "// date: " << asctime(now); // no newline -- looks like asctime() contains one
    out << "//\n";
    out << "//-----------------------------------------\n\n\n";

    out << "#include <math.h>\n";
    out << "#include \"omnetpp.h\"\n\n";

    out << "// NEDC version check\n"
           "#define NEDC_VERSION " NEDC_VERSION_HEX "\n"
           "#if (NEDC_VERSION!=OMNETPP_VERSION)\n"
           "#    error Version mismatch! Probably this file was generated by an earlier version of nedc: 'make clean' should help.\n"
           "#endif\n\n";

    out << "// Disable warnings about unused variables. For MSVC and BC only:\n"
           "// GCC has no way to turn on its -Wunused option in a source file :(\n"
           "#ifdef _MSC_VER\n"
           "#  pragma warning(disable:4101)\n"
           "#endif\n"
           "#ifdef __BORLANDC__\n"
           "#  pragma warn -waus\n"
           "#  pragma warn -wuse\n"
           "#endif\n\n";

    out << "static cModuleType *_getModuleType(const char *modname)\n";
    out << "{\n";
    out << "    cModuleType *modtype = findModuleType(modname);\n";
    out << "    if (!modtype)\n";
    out << "        throw new cRuntimeError(\"Module type definition %s not found (Define_Module() missing from C++ code?)\", modname);\n";
    out << "    return modtype;\n";
    out << "}\n\n";

    out << "static void _checkModuleVectorSize(int vectorsize, const char *mod)\n";
    out << "{\n";
    out << "    if (vectorsize<0)\n";
    out << "        throw new cRuntimeError(\"Negative module vector size %s[%d]\", mod, vectorsize);\n";
    out << "}\n";
    out << "\n";

    out << "static void _readModuleParameters(cModule *mod)\n";
    out << "{\n";
    out << "    int n = mod->params();\n";
    out << "    for (int k=0; k<n; k++)\n";
    out << "        if (mod->par(k).isInput())\n";
    out << "            mod->par(k).read();\n";
    out << "}\n";
    out << "\n";

    out << "static int _checkModuleIndex(int index, int vectorsize, const char *modname)\n";
    out << "{\n";
    out << "    if (index<0 || index>=vectorsize)\n";
    out << "        throw new cRuntimeError(\"Submodule index %s[%d] out of range, sizeof(%s) is %d\", modname, index, modname, vectorsize);\n";
    out << "    return index;\n";
    out << "}\n";
    out << "\n";

    out << "static cGate *_checkGate(cModule *mod, const char *gatename)\n";
    out << "{\n";
    out << "    cGate *g = mod->gate(gatename);\n";
    out << "    if (!g)\n";
    out << "        throw new cRuntimeError(\"%s has no gate named %s\",mod->fullPath().c_str(), gatename);\n";
    out << "    return g;\n";
    out << "}\n";
    out << "\n";

    out << "static cGate *_checkGate(cModule *mod, const char *gatename, int gateindex)\n";
    out << "{\n";
    out << "    cGate *g = mod->gate(gatename, gateindex);\n";
    out << "    if (!g)\n";
    out << "        throw new cRuntimeError(\"%s has no gate %s[%d]\",mod->fullPath().c_str(), gatename, gateindex);\n";
    out << "    return g;\n";
    out << "}\n";
    out << "\n";

    out << "static cGate *_getFirstUnusedParentModGate(cModule *mod, const char *gatename)\n";
    out << "{\n";
    out << "    int baseId = mod->findGate(gatename);\n";
    out << "    if (baseId<0)\n";
    out << "        throw new cRuntimeError(\"%s has no %s[] gate\",mod->fullPath().c_str(), gatename);\n";
    out << "    int n = mod->gate(baseId)->size();\n";
    out << "    for (int i=0; i<n; i++)\n";
    out << "        if (!mod->gate(baseId+i)->isConnectedInside())\n";
    out << "            return mod->gate(baseId+i);\n";
    out << "    throw new cRuntimeError(\"%s[] gates are all connected, no gate left for `++' operator\",mod->fullPath().c_str(), gatename);\n";
    out << "}\n";
    out << "\n";

    out << "static cGate *_getFirstUnusedSubmodGate(cModule *mod, const char *gatename)\n";
    out << "{\n";
    out << "    int baseId = mod->findGate(gatename);\n";
    out << "    if (baseId<0)\n";
    out << "        throw new cRuntimeError(\"%s has no %s[] gate\",mod->fullPath().c_str(), gatename);\n";
    out << "    int n = mod->gate(baseId)->size();\n";
    out << "    for (int i=0; i<n; i++)\n";
    out << "        if (!mod->gate(baseId+i)->isConnectedOutside())\n";
    out << "            return mod->gate(baseId+i);\n";
    out << "    int newBaseId = mod->setGateSize(gatename,n+1);\n";
    out << "    return mod->gate(newBaseId+n);\n";
    out << "}\n";
    out << "\n";

    out << "static cFunctionType *_getFunction(const char *funcname, int argcount)\n";
    out << "{\n";
    out << "    cFunctionType *functype = findFunction(funcname,argcount);\n";
    out << "    if (!functype)\n";
    out << "        throw new cRuntimeError(\"Function %s with %d args not found\", funcname, argcount);\n";
    out << "    return functype;\n";
    out << "}\n";
    out << "\n";

    out << "static cChannel *_createChannel(const char *channeltypename)\n";
    out << "{\n";
    out << "    cChannelType *channeltype = findChannelType(channeltypename);\n";
    out << "    if (!channeltype)\n";
    out << "        throw new cRuntimeError(\"Channel type %s not found\", channeltypename);\n";
    out << "    cChannel *channel = channeltype->create(\"channel\");\n";
    out << "    return channel;\n";
    out << "}\n";
    out << "\n";

    out << "static cChannel *_createNonTypedBasicChannel(double delay, double error, double datarate)\n";
    out << "{\n";
    out << "    cBasicChannel *channel = new cBasicChannel(\"channel\");\n";
    out << "    if (delay!=0) channel->setDelay(delay);\n";
    out << "    if (error!=0) channel->setError(error);\n";
    out << "    if (datarate!=0) channel->setDatarate(datarate);\n";
    out << "    return channel;\n";
    out << "}\n";
    out << "\n";

    out << "static cXMLElement *_getXMLDocument(const char *fname, const char *pathexpr=NULL)\n";
    out << "{\n";
    out << "    cXMLElement *node = ev.getXMLDocument(fname, pathexpr);\n";
    out << "    if (!node)\n";
    out << "        throw new cRuntimeError(!pathexpr ? \"xmldoc(\\\"%s\\\"): element not found\" : \"xmldoc(\\\"%s\\\", \\\"%s\\\"): element not found\",fname,pathexpr);\n";
    out << "    return node;\n";
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

    // channeltype object definition
    const char *channelname = node->getName();
    out << "// channel definition " << channelname << "\n";
    out << "class " << channelname <<  " : public cChannelType\n";
    out << "{\n";
    out << "  public:\n";
    out << "    " << channelname << "(const char *name) : cChannelType(name) {}\n";
    out << "    " << channelname << "(const " << channelname << "& n) : cChannelType(n.name()) {operator=(n);}\n";
    out << "    virtual cChannel *create(const char *name);\n";
    out << "};\n\n";

    out << "Define_Channel(" << channelname << ");\n\n";

    // factory method
    out << "cChannel *" << channelname << "::create(const char *name)\n";
    out << "{\n";
    out << "    cChannel *chan = new cBasicChannel(name);\n";
    out << "    cPar tmpval, *p;\n\n";

    // generate channel attributes code
    generateChildren(node, increaseIndent(indent));

    out << "    return chan;\n";
    out << "};\n\n";
}

void NEDCppGenerator::doChannelAttr(ChannelAttrNode *node, const char *indent, int mode, const char *)
{
    const char *attrname = node->getName();    // attribute name

    out << "    p = new cPar(\"" << attrname << "\");\n";
    ExpressionNode *value = findExpression(node, "value");
    out << "    *p = ";
    generateItem(value, indent, mode);
    out << ";\n";
    out << "    chan->addPar(p);\n\n";
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
    out << "    " << networkname << "(const " << networkname << "& n) : cNetworkType(n.name()) {operator=(n);}\n";
    out << "    virtual void setupNetwork();\n";
    out << "};\n\n";

    out << "Define_Network(" << networkname << ");\n\n";

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
    out << indent << "modtype = _getModuleType(\"" << node->getTypeName() << "\");\n";

    // create module
    const char *submodule_name = node->getName();
    submodule_var = node->getName();
    submodule_var += "_p";
    out << indent << "cModule *" << submodule_var.c_str() << " = modtype->create(\"" << submodule_name << "\", NULL);\n\n";
    out << indent << "cContextSwitcher __ctx(" << submodule_var.c_str() << "); // do the rest in this module's context\n";

    // generate children (except expressions)
    generateChildrenWithTags(node, "substmachines,substparams,gatesizes", indent, mode);

    // FIXME convert const params to const

    // force all parameters to pick up a value before doing buildInside()
    out << indent << "_readModuleParameters(" << submodule_var.c_str() << ");\n";

    // add display string
    DisplayStringNode *dispstr = (DisplayStringNode *)node->getFirstChildWithTag(NED_DISPLAY_STRING);
    if (dispstr)
    {
        out << indent << submodule_var.c_str() << "->setBackgroundDisplayString(\""
            << dispstr->getValue() << "\");\n\n";
    }

    // build function call
    out << indent << "// build submodules recursively (if it has any):\n";
    out << indent << submodule_var.c_str() << "->buildInside();\n";

    // epilog
    out << "}\n\n";
    in_network = false;
}

void NEDCppGenerator::doSimple(SimpleModuleNode *node, const char *indent, int mode, const char *)
{
    // generate Interface() stuff
    const char *module_name = node->getName();

    out << "ModuleInterface(" << module_name << ")\n";
    generateChildrenWithTags(node, "params,gates,machines", increaseIndent(indent));
    out << "EndInterface\n\n";
    out << "Register_ModuleInterface(" << module_name << ")\n\n";

    // epilog
    out << "//// Sample code:\n";
    out << "// class " << module_name << " : public cSimpleModule\n";
    out << "// {\n";
    out << "//     Module_Class_Members(" << module_name << ",cSimpleModule,16384)\n";
    out << "//     virtual void activity();\n";
    out << "//     // Add you own member functions here!\n";
    out << "// };\n";
    out << "//\n";
    out << "// Define_Module(" << module_name << ");\n";
    out << "//\n";
    out << "// void " << module_name << "::activity()\n";
    out << "// {\n";
    out << "//     // Put code for simple module activity here!\n";
    out << "// }\n";
    out << "//\n\n";
}

void NEDCppGenerator::doModule(CompoundModuleNode *node, const char *indent, int mode, const char *)
{
    // generate Interface() stuff
    const char *module_name = node->getName();

    out << "ModuleInterface(" << module_name << ")\n";
    generateChildrenWithTags(node, "params,gates,machines", increaseIndent(indent));
    out << "EndInterface\n\n";

    out << "Register_ModuleInterface(" << module_name << ");\n\n";

    // class declaration
    out << "class " << module_name << " : public cCompoundModule\n";
    out << "{\n";
    out << "  public:\n";
    out << "    " << module_name << "() : cCompoundModule() {}\n";
    out << "  protected:\n";
    out << "    virtual void doBuildInside();\n";
    out << "};\n\n";

    out << "Define_Module(" << module_name << ");\n\n";

    // generate expression shells
    exprgen.collectExpressions(node);
    exprgen.generateExpressionClasses();

    // prolog
    out << "void " << module_name << "::doBuildInside()\n";
    out << "{\n";

    indent = increaseIndent(indent);
    out << indent << "cModule *mod = this;\n\n";
    printTemporaryVariables(indent);

    // add display string
    DisplayStringNode *dispstr = (DisplayStringNode *)node->getFirstChildWithTag(NED_DISPLAY_STRING);
    if (dispstr)
    {
        out << indent << "mod->setBackgroundDisplayString(\"" << dispstr->getValue() << "\");\n\n";
    }

    // generate submodules
    generateChildrenWithTags(node, "submodules", indent);

    // generate connections
    generateChildrenWithTags(node, "connections", indent);

    // check if there are unconnected gates left
    ConnectionsNode *conns = (ConnectionsNode *)node->getFirstChildWithTag(NED_CONNECTIONS);
    if (!conns || conns->getCheckUnconnected())
    {
       out << indent << "// check all gates are connected:\n";
       out << indent << "mod->checkInternalConnections();\n\n";
    }

    // generate buildInside() calls for submodules and to free up arrays of module pointers
    out << "\n";
    out << indent << "//\n";
    out << indent << "// this level is done -- recursively build submodules too\n";
    out << indent << "//\n";
    generateChildren(node, indent, MODE_FINALLY);
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
    if (!strcmp(datatype,"numeric"))
        typecode = "ParType_Numeric";
    else if (!strcmp(datatype,"numeric const"))
        typecode = "ParType_Numeric ParType_Const";
    else if (!strcmp(datatype,"string"))
        typecode = "ParType_String";
    else if (!strcmp(datatype,"bool"))
        typecode = "ParType_Bool";
    else if (!strcmp(datatype,"xml"))
        typecode = "ParType_XML";
    else if (!strcmp(datatype,"anytype"))
        typecode = "ParType_Any";
    else
        {INTERNAL_ERROR1(node, "doParam(): unexpected parameter datatype '%s'", datatype);typecode = "ParType_Any";}

    out << indent << "Parameter(" << node->getName() << ", " << typecode << ")\n";
}

void NEDCppGenerator::doGates(GatesNode *node, const char *indent, int mode, const char *)
{
    // generate children
    out << indent << "// gates:\n";
    generateChildren(node, indent, mode);
}

void NEDCppGenerator::doGate(GateNode *node, const char *indent, int mode, const char *)
{
    out << indent << "Gate(" << node->getName();
    out << (node->getIsVector() ? "[]" : "") << ", ";
    out << (node->getDirection()==NED_GATEDIR_INPUT ? "GateDir_Input" : "GateDir_Output") << ")\n";
}

void NEDCppGenerator::doMachines(MachinesNode *node, const char *indent, int mode, const char *)
{
    // ignore machines (obsolete)
}

void NEDCppGenerator::doMachine(MachineNode *node, const char *indent, int mode, const char *)
{
    // ignore machines (obsolete)
}

void NEDCppGenerator::doSubmodules(SubmodulesNode *node, const char *indent, int mode, const char *)
{
    // prolog
    out << indent << "// submodules:\n";
    out << indent << "cModuleType *modtype = NULL;\n";
    out << indent << "int submodindex;\n\n";

    // generate children
    generateChildren(node, indent, mode);
}

void NEDCppGenerator::resolveSubmoduleType(SubmoduleNode *node, const char *indent)
{
    // find module descriptor
    if (!strnotnull(node->getLikeParam()))
    {
        out << indent << "modtype = _getModuleType(\"" << node->getTypeName() << "\");\n";
    }
    else
    {
        out << indent << "modtypename = mod->par(\"" << node->getLikeParam() << "\");\n";
        out << indent << "modtype = _getModuleType(modtypename);\n";
    }
}

void NEDCppGenerator::doSubmodule(SubmoduleNode *node, const char *indent, int mode, const char *)
{
    out << indent << "//\n";
    out << indent << "// submodule '" << node->getName() << "':\n";
    out << indent << "//\n";

    // create module
    const char *submodule_name = node->getName();

    ExpressionNode *vectorsize = findExpression(node, "vector-size");
    if (!vectorsize)
    {
        out << indent << "int " << node->getName() << "_size = 1;\n";
        resolveSubmoduleType(node, indent);
        submodule_var = node->getName();
        submodule_var += "_p";
        out << indent << "cModule *" << submodule_var.c_str() << " = modtype->create(\"" << submodule_name << "\", mod);\n";
        out << indent << "{\n";
        indent = increaseIndent(indent);
        out << indent << "cContextSwitcher __ctx(" << submodule_var.c_str() << "); // do the rest in this module's context\n";
    }
    else
    {
        submodule_var = node->getName();
        submodule_var += "_p";
        std::string submodulesize_var = node->getName();
        submodulesize_var += "_size";

        out << indent << "int " << submodulesize_var.c_str() << " = (int)(";
        generateItem(vectorsize, indent, mode);
        out << ");\n";

        out << indent << "_checkModuleVectorSize(" << submodulesize_var.c_str() << ",\"" << submodule_name << "\");\n";
        out << indent << "cModule **" << submodule_var.c_str() << " = new cModule *[" << submodulesize_var.c_str() << "];\n";
        out << indent << "if (" << submodulesize_var.c_str() << ">0)\n";
        out << indent << "{\n";
        resolveSubmoduleType(node, increaseIndent(indent));
        out << indent << "}\n";
        out << indent << "for (submodindex=0; submodindex<" << submodulesize_var.c_str() << "; submodindex++)\n";
        out << indent << "{\n";
        submodule_var += "[submodindex]";
        indent = increaseIndent(indent);
        out << indent << submodule_var.c_str() << " = modtype->create(\"" << submodule_name << "\", mod, " << submodulesize_var.c_str() << ", submodindex);\n";
        out << indent << "cContextSwitcher __ctx(" << submodule_var.c_str() << "); // do the rest in this module's context\n";
    }
    out << "\n";

    // generate children (except expressions)
    generateChildrenWithTags(node, "substmachines,substparams,gatesizes", indent, mode);

    // FIXME convert const params to const

    // force all parameters to pick up a value before building connections and esp. calling buildInside()
    out << indent << "_readModuleParameters(" << submodule_var.c_str() << ");\n";

    // add display string
    DisplayStringNode *dispstr = (DisplayStringNode *)node->getFirstChildWithTag(NED_DISPLAY_STRING);
    if (dispstr)
    {
        out << indent << submodule_var.c_str() << "->setDisplayString(\"" <<  dispstr->getValue() << "\");\n";
    }

    // note: we'll call buildinside only if all connections on this level have been built as well

    indent = decreaseIndent(indent);
    out << indent << "}\n\n";
}

void NEDCppGenerator::doSubmoduleFinally(SubmoduleNode *node, const char *indent, int mode, const char *)
{
    // generate buildInside() call, and if necessary, free up array of cModule pointers
    const char *submodname = node->getName();
    if (!findExpression(node, "vector-size"))
    {
        out << indent << submodname << "_p->buildInside();\n";
    }
    else
    {
        out << indent << "for (submodindex=0; submodindex<" << submodname << "_size; submodindex++)\n";
        out << indent << "    " << submodname << "_p[submodindex]->buildInside();\n";
        out << indent << "delete [] " << submodname << "_p;\n";
    }
}

void NEDCppGenerator::doSubstparams(SubstparamsNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// parameter assignments:\n";
    beginConditionalBlock(node, indent, mode, arg);

    // generate children
    generateChildrenExceptTags(node, "expression", indent, mode);

    endConditionalBlock(node, indent, mode, arg);
    out << "\n";
}

void NEDCppGenerator::doSubstparam(SubstparamNode *node, const char *indent, int mode, const char *)
{
    // set parameter value
    out << indent << submodule_var.c_str() << "->par(\"" << node->getName() << "\") = ";
    ExpressionNode *paramvalue = findExpression(node, "value");
    if (paramvalue)  generateItem(paramvalue, indent, mode);
    out << ";\n";
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
    out << indent << submodule_var.c_str() << "->setGateSize(\"" << node->getName() << "\", (int)(";

    ExpressionNode *vectorsize = findExpression(node, "vector-size");
    if (vectorsize)  generateItem(vectorsize, indent, mode);

    out << "));\n";
}

void NEDCppGenerator::doSubstmachines(SubstmachinesNode *node, const char *indent, int mode, const char *arg)
{
    // ignore machines (obsolete)
}

void NEDCppGenerator::doSubstmachine(SubstmachineNode *node, const char *indent, int mode, const char *)
{
    // ignore machines (obsolete)
}

void NEDCppGenerator::doConnections(ConnectionsNode *node, const char *indent, int mode, const char *)
{
    // prolog
    out << indent << "//\n";
    out << indent << "// connections:\n";
    out << indent << "//\n";
    out << indent << "cGate *srcgate, *destgate;\n\n";
    out << indent << "cChannel *channel;\n";
    out << indent << "cPar *par;\n";

    // generate children
    generateChildren(node, indent, mode);

    // epilog: checkInternalConnections() is invoked from doModule(), because it also has to take place if there's no "connections:" at all
}

void NEDCppGenerator::resolveGate(const char *modname, ExpressionNode *modindex, const char *gatename, ExpressionNode *gateindex, bool isplusplus)
{
    if (isplusplus && gateindex)
        INTERNAL_ERROR0(NULL,"resolveGate(): \"++\" and gate index expression cannot exist together");

    // wrap
    if (isplusplus && !strnotnull(modname))
        out << "_getFirstUnusedParentModGate(";
    else if (isplusplus)
        out << "_getFirstUnusedSubmodGate(";
    else
        out << "_checkGate(";

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
             out << "[_checkModuleIndex((int)(";
             generateItem(modindex,  "        ");
             out << ")," << modname << "_size,\"" << modname << "\")]";
        }
    }

    // gate
    out << ", \"" << gatename << "\"";
    if (gateindex)
    {
        out << ", (int)(";
        generateItem(gateindex, "        ");
        out << ")";
    }
    out << ")";
}

void NEDCppGenerator::resolveConnectionAttributes(ConnectionNode *node, const char *indent, int mode)
{
    // emit code that creates channel and puts its ptr to "channel" variable

    // channel?
    ConnAttrNode *channelAttr = (ConnAttrNode *)node->getFirstChildWithAttribute(NED_CONN_ATTR,"name","channel");
    ConstNode *channel = getConstantExpression(findExpression(channelAttr,"value"));
    if (channel)
    {
        out << indent << "channel = _createChannel(\"" << channel->getValue() << "\");\n";
        return;
    }

    // optimization: assess if simplified code can be generated
    ConnAttrNode *delayAttr = (ConnAttrNode *)node->getFirstChildWithAttribute(NED_CONN_ATTR,"name","delay");
    ConstNode *delay = getConstantExpression(findExpression(delayAttr,"value"));
    bool isDelaySimple =  !delayAttr ? true : delay!=NULL;
    ConnAttrNode *errorAttr = (ConnAttrNode *)node->getFirstChildWithAttribute(NED_CONN_ATTR,"name","error");
    ConstNode *error = getConstantExpression(findExpression(errorAttr,"value"));
    bool isErrorSimple =  !errorAttr ? true : error!=NULL;
    ConnAttrNode *datarateAttr = (ConnAttrNode *)node->getFirstChildWithAttribute(NED_CONN_ATTR,"name","datarate");
    ConstNode *datarate = getConstantExpression(findExpression(datarateAttr,"value"));
    bool isDatarateSimple =  !datarateAttr ? true : datarate!=NULL;

    if (isDelaySimple && isErrorSimple && isDatarateSimple)
    {
        // generate optimized code: delay, error, datarate with a specialized function
        out << indent << "channel = _createNonTypedBasicChannel("
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
    }
    else
    {
        // fallback: general code
        out << "\n" << indent << "// add channel\n";
        out << indent << "channel = new cBasicChannel(\"channel\");\n";
        generateChildrenWithTags(node, "conn-attr", indent);
    }
}

void NEDCppGenerator::doConnection(ConnectionNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// connection\n";
    beginConditionalBlock(node, indent, mode, arg);

    // create connection
    out << indent << "srcgate = ";
    resolveGate(node->getSrcModule(), findExpression(node,"src-module-index"),
                node->getSrcGate(), findExpression(node,"src-gate-index"),
                node->getSrcGatePlusplus());
    out << ";\n";

    out << indent << "destgate = ";
    resolveGate(node->getDestModule(), findExpression(node,"dest-module-index"),
                node->getDestGate(), findExpression(node,"dest-gate-index"),
                node->getDestGatePlusplus());
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
        out << indent << "srcgate->setDisplayString(\"" <<  dispstr->getValue() << "\");\n\n";
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

    out << indent << "long start = (long)(";
    generateItem(fromvalue, indent, mode);
    out << ");\n";
    out << indent << "long end = (long)(";
    generateItem(tovalue, indent, mode);
    out << ");\n";
    out << indent << "for (long " << node->getParamName() << "_var=start; " << node->getParamName() << "_var<=end; " << node->getParamName() << "_var++)\n";
}

void NEDCppGenerator::doDisplayString(DisplayStringNode *node, const char *indent, int mode, const char *)
{
    // unused
}


void NEDCppGenerator::doExpression(ExpressionNode *node, const char *indent, int mode, const char *arg)
{
    exprgen.generateExpressionUsage((ExpressionNode *)node,indent);
}

