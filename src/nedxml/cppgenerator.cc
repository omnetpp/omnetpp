//==========================================================================
//   CPPGENERATOR.CC
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


#include <iostream.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "nedelements.h"
#include "cppgenerator.h"
#include "nederror.h"

#define NEDC_VERSION "2.90" //FIXME
#define NEDC_VERSION_HEX "0x0290" //FIXME


void generateCpp(ostream& out, NEDElement *node)
{
    NEDCppGenerator cppgen;
    cppgen.generate(out, node);
}

inline bool strnotnull(const char *s)
{
    return s && s[0];
}

//-----------------------------------------------------------------------

NEDCppGenerator::NEDCppGenerator()
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
    int ilen = strlen(indent);
    if (ilen+indentsize <= sizeof(spaces)-1)
        ilen+=indentsize;
    const char *newindent = spaces+sizeof(spaces)-1-ilen;
    return newindent;
}

const char *NEDCppGenerator::decreaseIndent(const char *indent)
{
    return indent + indentsize;
}

void NEDCppGenerator::generate(ostream& out, NEDElement *node)
{
    writeProlog(out);
    generateItem(out, node, "");
}

void NEDCppGenerator::generateItem(ostream& out, NEDElement *node, const char *indent, int mode, const char *arg)
{
    int tagcode = node->getTagCode();

    const char *newindent = indent;
    switch (mode)
    {
        case MODE_NORMAL:
            switch (tagcode)
            {
                case NED_NED_FILES:
                    doNedFiles(out, (NedFilesNode *)node, newindent, mode, arg); break;
                case NED_NED_FILE:
                    doNedFile(out, (NedFileNode *)node, newindent, mode, arg); break;
                case NED_IMPORT:
                    doImports(out, (ImportNode *)node, newindent, mode, arg); break;
                case NED_IMPORTED_FILE:
                    doImport(out, (ImportedFileNode *)node, newindent, mode, arg); break;
                case NED_CHANNEL:
                    doChannel(out, (ChannelNode *)node, newindent, mode, arg); break;
                case NED_CHANNEL_ATTR:
                    doChannelAttr(out, (ChannelAttrNode *)node, newindent, mode, arg); break;
                case NED_NETWORK:
                    doNetwork(out, (NetworkNode *)node, newindent, mode, arg); break;
                case NED_SIMPLE_MODULE:
                    doSimple(out, (SimpleModuleNode *)node, newindent, mode, arg); break;
                case NED_COMPOUND_MODULE:
                    doModule(out, (CompoundModuleNode *)node, newindent, mode, arg); break;
                case NED_PARAMS:
                    doParams(out, (ParamsNode *)node, newindent, mode, arg); break;
                case NED_PARAM:
                    doParam(out, (ParamNode *)node, newindent, mode, arg); break;
                case NED_GATES:
                    doGates(out, (GatesNode *)node, newindent, mode, arg); break;
                case NED_GATE:
                    doGate(out, (GateNode *)node, newindent, mode, arg); break;
                case NED_MACHINES:
                    doMachines(out, (MachinesNode *)node, newindent, mode, arg); break;
                case NED_MACHINE:
                    doMachine(out, (MachineNode *)node, newindent, mode, arg); break;
                case NED_SUBMODULES:
                    doSubmodules(out, (SubmodulesNode *)node, newindent, mode, arg); break;
                case NED_SUBMODULE:
                    doSubmodule(out, (SubmoduleNode *)node, newindent, mode, arg); break;
                case NED_SUBSTPARAMS:
                    doSubstparams(out, (SubstparamsNode *)node, newindent, mode, arg); break;
                case NED_SUBSTPARAM:
                    doSubstparam(out, (SubstparamNode *)node, newindent, mode, arg); break;
                case NED_GATESIZES:
                    doGatesizes(out, (GatesizesNode *)node, newindent, mode, arg); break;
                case NED_GATESIZE:
                    doGatesize(out, (GatesizeNode *)node, newindent, mode, arg); break;
                case NED_SUBSTMACHINES:
                    doSubstmachines(out, (SubstmachinesNode *)node, newindent, mode, arg); break;
                case NED_SUBSTMACHINE:
                    doSubstmachine(out, (SubstmachineNode *)node, newindent, mode, arg); break;
                case NED_CONNECTIONS:
                    doConnections(out, (ConnectionsNode *)node, newindent, mode, arg); break;
                case NED_CONNECTION:
                    doConnection(out, (ConnectionNode *)node, newindent, mode, arg); break;
                case NED_CONN_ATTR:
                    doConnattr(out, (ConnAttrNode *)node, newindent, mode, arg); break;
                case NED_FOR_LOOP:
                    doForloop(out, (ForLoopNode *)node, newindent, mode, arg); break;
                case NED_LOOP_VAR:
                    doLoopvar(out, (LoopVarNode *)node, newindent, mode, arg); break;
                case NED_DISPLAY_STRING:
                    doDisplayString(out, (DisplayStringNode *)node, newindent, mode, arg); break;
                case NED_EXPRESSION:
                    doExpression(out, (ExpressionNode *)node, newindent, mode, arg); break;
                default:
                    INTERNAL_ERROR1(node,"generateItem(): unrecognized tag: %s", node->getTagName());
            }
            break;
        case MODE_CLEANUP:
            switch (tagcode)
            {
                case NED_SUBMODULE:
                    doSubmoduleCleanup(out, (SubmoduleNode *)node, newindent, mode, arg); break;
                default:
                    generateChildren(out, node, newindent, mode, arg);
            }
            break;
        default:
            INTERNAL_ERROR1(node,"generateItem(): unrecognized mode %d", mode);
    }
}

void NEDCppGenerator::generateChildren(ostream& out, NEDElement *node, const char *indent, int mode, const char *arg)
{
    for (NEDElement *child=node->getFirstChild(); child; child = child->getNextSibling())
    {
        generateItem(out, child, indent, mode, arg);
    }
}

void NEDCppGenerator::generateChildrenWithTags(ostream& out, NEDElement *node, const char *tags, const char *indent, int mode, const char *arg)
{
    for (NEDElement *child=node->getFirstChild(); child; child = child->getNextSibling())
    {
        if (strstr(tags, child->getTagName()))
            generateItem(out, child, indent, mode, arg);
    }
}

void NEDCppGenerator::generateChildrenExceptTags(ostream& out, NEDElement *node, const char *tags, const char *indent, int mode, const char *arg)
{
    for (NEDElement *child=node->getFirstChild(); child; child = child->getNextSibling())
    {
        if (!strstr(tags, child->getTagName()))
            generateItem(out, child, indent, mode, arg);
    }
}

// --------------------------------------

// helper
void NEDCppGenerator::printTemporaryVariables(ostream& out, const char *indent)
{
        out << indent << "// temporary variables:\n";
        out << indent << "cPar tmpval;\n";  // for compiled expressions
        out << indent << "const char *type_name;\n";   // for submodule creation
        out << indent << "cArray machines;\n";  // for 'on:'
        out << indent << "bool islocal;\n";  // for 'on:'
        out << indent << "int size;\n";  // for gatesize
        out << "\n";
}


void NEDCppGenerator::beginConditionalBlock(ostream& out, NEDElement *node, const char *&indent, int mode, const char *)
{
    ExpressionNode *condition = findExpression(node, "condition");
    if (condition)
    {
        out << indent << "if (";
        generateItem(out, condition, indent, mode);
        out << ")\n";
        out << indent << "{\n";
        indent = increaseIndent(indent);
    }
}

void NEDCppGenerator::endConditionalBlock(ostream& out, NEDElement *node, const char *&indent, int mode, const char *)
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
    out << "// Generated by NEDC version " NEDC_VERSION "\n";
    out << "// date:    " << asctime(now) << "\n";
    out << "//\n";
    out << "//-----------------------------------------\n\n\n";

    out << "#include <math.h>\n";
    out << "#include \"omnetpp.h\"\n\n";

    out << "#define check_error() \\\n"
                "    {if (!simulation.ok()) return;}\n";
    out << "#define check_memory() \\\n"
                "    {if (memoryIsLow()) {opp_error(eNOMEM); return;}}\n";
    out << "#define check_module_count(num, mod, parentmod) \\\n"
                "    {if ((int)num<0) {opp_error(\"Negative module vector size %s[%d] in compound module %s\", \\\n"
                "                          mod,(int)num,parentmod);return;}}\n";
    out << "#define check_gate_count(num, mod, gate, parentmod) \\\n"
                "    {if ((int)num<0) {opp_error(\"Negative gate vector size %s.%s[%d] in compound module %s\", \\\n"
                "                          mod,gate,(int)num,parentmod);return;}}\n";
    out << "#define check_loop_bounds(lower, upper, parentmod) \\\n"
                "    {if ((int)lower<0) \\\n"
                "        {opp_error(\"Bad loop bounds (%d..%d) in compound module %s\", \\\n"
                "                 (int)lower,(int)upper,parentmod);return;}}\n";
    out << "#define check_module_index(index,modvar,modname,parentmod) \\\n"
                "    {if (index<0 || index>=modvar[0]->size()) {opp_error(\"Bad submodule index %s[%d] in compound module %s\", \\\n"
                "          modname,(int)index,parentmod);return;}}\n";
    out << "#define check_channel_params(delay, err, channel) \\\n"
                "    {if ((double)delay<0.0) \\\n"
                "        {opp_error(\"Negative delay value %%lf in channel %s\",(double)delay,channel);return;} \\\n"
                "     if ((double)err<0.0 || (double)err>1.0) \\\n"
                "        {opp_error(\"Incorrect error value %%lf in channel %s\",(double)err,channel);return;}}\n";
    out << "#define check_modtype(modtype, modname) \\\n"
                "    {if ((modtype)==NULL) {opp_error(\"Simple module type definition %s not found\", \\\n"
                "                                     modname);return;}}\n";
    out << "#define check_function(funcptr, funcname) \\\n"
                "    {if ((funcptr)==NULL) {opp_error(\"Function %s not found\", \\\n"
                "                                     funcname);return;}}\n";
    out << "#define check_gate(gateindex, modname, gatename) \\\n"
                "    {if ((int)gateindex==-1) {opp_error(\"Gate %s.%s not found\",modname,gatename);return;}}\n";
    out << "#define check_anc_param(ptr,parname,compoundmod) \\\n"
                "    {if ((ptr)==NULL) {opp_error(\"Unknown ancestor parameter named %s in compound module %s\", \\\n"
                "                                parname,compoundmod);return;}}\n";
    out << "#define check_param(modulep,parname) \\\n"
                "    {if (!modulep->hasPar(parname)) {opp_error(\"Unknown parameter named %s\",parname);return;}}\n";
    out << "#ifndef __cplusplus\n"
                "#  error Compile as C++!\n"
                "#endif\n"
                "#ifdef __BORLANDC__\n"
                "#  if !defined(__FLAT__) && !defined(__LARGE__)\n"
                "#    error Compile as 16-bit LARGE model or 32-bit DPMI!\n"
                "#  endif\n"
                "#endif\n\n";
    out << "// Disable warnings about unused variables:\n"
                "#ifdef _MSC_VER\n"
                "#  pragma warning(disable:4101)\n"
                "#endif\n"
                "#ifdef __BORLANDC__\n"
                "#  pragma warn -waus\n"
                "#  pragma warn -wuse\n"
                "#endif\n"
                "// for GCC, seemingly there's no way to emulate the -Wunused command-line\n"
                "// flag from a source file...\n\n";
    out << "// Version check\n"
                "#define NEDC_VERSION " NEDC_VERSION_HEX "\n"
                "#if (NEDC_VERSION!=OMNETPP_VERSION)\n"
                "//#    error Version mismatch! Probably this file was generated by an earlier version of nedc: 'make clean' should help.\n"
                "#endif\n"; // FIXME add back version check...
    out << "\n";

    out << "static void readModuleParameters(cModule *mod)\n";
    out << "{\n";
    out << "    int n = mod->params();\n";
    out << "    for (int k=0; k<n; k++)\n";
    out << "    {\n";
    out << "        if (mod->par(k).isInput())\n";
    out << "        {\n";
    out << "            mod->par(k).read();check_error();\n";
    out << "        }\n";
    out << "    }\n";
    out << "}\n";
    out << "\n";
}

void NEDCppGenerator::doNedFiles(ostream& out, NedFilesNode *node, const char *indent, int mode, const char *)
{
    generateChildren(out, node, increaseIndent(indent));
}

void NEDCppGenerator::doNedFile(ostream& out, NedFileNode *node, const char *indent, int mode, const char *)
{
    // generate children
    generateChildren(out, node, indent);
}

void NEDCppGenerator::doImports(ostream& out, ImportNode *node, const char *indent, int mode, const char *)
{
    // no code to be generated
}

void NEDCppGenerator::doImport(ostream& out, ImportedFileNode *node, const char *indent, int mode, const char *)
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

void NEDCppGenerator::doChannel(ostream& out, ChannelNode *node, const char *indent, int mode, const char *)
{
    // generate expression shells
    exprgen.collectExpressions(node);
    exprgen.generateExpressionClasses(out);

    // generate channel attributes code
    generateChildren(out, node, increaseIndent(indent));

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

void NEDCppGenerator::doChannelAttr(ostream& out, ChannelAttrNode *node, const char *indent, int mode, const char *)
{
    const char *channelname = ((ChannelNode *)(node->getParent()))->getName();  // channel name
    const char *attrname = node->getName();    // attribute name

    out << "static cPar *" << channelname << "__" << attrname << "()\n";
    out << "{\n";
    out << "    cPar tmpval;\n";
    out << "    cPar *p = new cPar(\"" << channelname << "_datarate\");\n";
    ExpressionNode *value = findExpression(node, "value");
    out << "    *p = ";
    generateItem(out, value, indent, mode);
    out << ";\n";
    out << "    return p;\n";
    out << "}\n\n";
}

void NEDCppGenerator::doNetwork(ostream& out, NetworkNode *node, const char *indent, int mode, const char *)
{
    in_network = true;

    // prolog
    const char *networkname = node->getName();
    out << "Define_Network( " << networkname << ", " << networkname << "_setup);\n\n";

    out << "void " << networkname << "_setup()\n";
    out << "{\n\n";
    printTemporaryVariables(out, indent);
    out << indent << "cModuleType *modtype;\n";

#if 0
    strcpy (submodule_name, networkname);
    sprintf(submodule_var, "%s_mod", networkname);

    cmd_new (NULL,0);  // treat system module as a submodule

    // generate submodule code
    do_sub_or_sys ( networkname, NULL, 1, sttype, stlike); //FIXME
#endif

    generateChildren(out, node, indent);

    out << indent << "check_error(); check_memory();\n";
    out << "}\n\n";
    in_network = false;

}

void NEDCppGenerator::doSimple(ostream& out, SimpleModuleNode *node, const char *indent, int mode, const char *)
{
    // generate Interface() stuff
    module_name = node->getName();

    out << "Interface( " << module_name.c_str() << " )\n";
    generateChildrenWithTags(out, node, "params,gates,machines", increaseIndent(indent));
    out << "EndInterface\n\n";
    out << "Register_Interface( " << module_name.c_str() << " )\n\n";

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

void NEDCppGenerator::doModule(ostream& out, CompoundModuleNode *node, const char *indent, int mode, const char *)
{
    // generate Interface() stuff
    module_name = node->getName();

    out << "Interface( " << module_name.c_str() << " )\n";
    generateChildrenWithTags(out, node, "params,gates,machines", increaseIndent(indent));
    out << "EndInterface\n\n";

    out << "Register_Interface( " << module_name.c_str() << " )\n\n";

    // class declaration
    out << "class " << module_name.c_str() << " : public cCompoundModule\n";
    out << "{\n";
    out << "  public:\n";
    out << "    " << module_name.c_str() << "(const char *name, cModule *parent) :\n";
    out << "      cCompoundModule(name, parent) {}\n";
    out << "    virtual const char *className()  {return \"" << module_name.c_str() << "\";}\n";
    out << "\n";
    out << "    // function to build submodules\n";
    out << "    virtual void buildInside();\n";
    out << "};\n\n";

    out << "Define_Module( " << module_name.c_str() << " );\n\n";

    // generate expression shells
    exprgen.collectExpressions(node);
    exprgen.generateExpressionClasses(out);

    // prolog
    out << "void " << module_name.c_str() << "::buildInside()\n";
    out << "{\n";

    indent = increaseIndent(indent);
    out << indent << "cModule *mod = this;\n\n";
    printTemporaryVariables(out, indent);

    // generate submodules
    generateChildrenWithTags(out, node, "submodules", indent);

    // generate connections
    generateChildrenWithTags(out, node, "connections", indent);

    // generate code to free array of module pointers
    generateChildren(out, node, increaseIndent(indent), MODE_CLEANUP);
    out << indent << "check_error(); check_memory();\n";
    indent = decreaseIndent(indent);
    out << indent << "}\n\n";
}

void NEDCppGenerator::doParams(ostream& out, ParamsNode *node, const char *indent, int mode, const char *)
{
    // generate children
    out << indent << "// parameters:\n";
    generateChildren(out, node, indent, mode);
}

void NEDCppGenerator::doParam(ostream& out, ParamNode *node, const char *indent, int mode, const char *)
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

void NEDCppGenerator::doGates(ostream& out, GatesNode *node, const char *indent, int mode, const char *)
{
    // generate children
    out << indent << "// gates:\n";
    generateChildren(out, node, indent, mode);
}

void NEDCppGenerator::doGate(ostream& out, GateNode *node, const char *indent, int mode, const char *)
{
    out << indent << "Gate( " << node->getName();
    out << (node->getIsVector() ? "[]" : "") << ", ";
    out << (node->getDirection()==NED_GATEDIR_INPUT ? "GateDir_Input" : "GateDir_Output") << " )\n";
}

void NEDCppGenerator::doMachines(ostream& out, MachinesNode *node, const char *indent, int mode, const char *)
{
    // generate children
    out << indent << "// machines:\n";
    generateChildren(out, node, indent, mode);
}

void NEDCppGenerator::doMachine(ostream& out, MachineNode *node, const char *indent, int mode, const char *)
{
    out << indent << "Machine( " << node->getName() << " )\n";
}

void NEDCppGenerator::doSubmodules(ostream& out, SubmodulesNode *node, const char *indent, int mode, const char *)
{
    // prolog
    out << indent << "// submodules:\n";
    out << indent << "cModuleType *modtype = NULL;\n";
    out << indent << "long submod_count = 0;\n";
    out << indent << "long submod_i = 0;\n\n";

    // generate children
    generateChildren(out, node, indent, mode);
}

void NEDCppGenerator::doSubmodule(ostream& out, SubmoduleNode *node, const char *indent, int mode, const char *)
{
    out << indent << "//\n";
    out << indent << "// submodule '" << node->getName() << "':\n";
    out << indent << "//\n";

    // find module descriptor
    if (!strnotnull(node->getLikeName()))
    {
        out << indent << "modtype = findModuleType( \"" << node->getTypeName() << "\" );\n";
        out << indent << "check_modtype( modtype, \"" << node->getTypeName() << "\" );\n";
    }
    else
    {
        out << indent << "type_name = mod->par(\"" << node->getTypeName() << "\");\n";
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
        generateItem(out, vectorsize, indent, mode);
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
    out << indent << "islocal = !simulation.netInterface() || simulation.netInterface()->isLocalMachineIn( machines );\n";

    bool issys = false;

//        if (issys)
//        {
//          out << indent << "if (!islocal)\n";
//          out << indent << "{\n";
//          out << indent << "    opp_error(\"Local machine `%s' is not among machines specified for this network\",\n";
//          out << indent << "              simulation.netInterface()->localhost());\n";
//          out << indent << "    return;\n";
//          out << indent << "}\n";
//        }

    // create submodule
    out << indent << submodule_var.c_str() << " = modtype->create( \"" << submodule_name.c_str() << "\", " << (issys?"NULL":"mod") << ", islocal);\n";
    out << indent << "check_error(); check_memory();\n\n";

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
       {
         if (issys)
           out << indent << submodule_var << "->setMachinePar( \"" << nl_retr_ith(machs,i+1)->namestr << "\", ev.getPhysicalMachineFor(\"" << nl_retr_ith( &cmd.submod_machs,i+1)->namestr << "\") );\n",
         else
           out << indent << submodule_var << "->setMachinePar( \"" << nl_retr_ith(machs,i+1)->namestr << "\", ((cPar *)machines[" << i << "])->stringValue() );\n",
       }
       out << indent << "check_error(); check_memory();\n\n";
       nl_empty(&cmd.submod_machs);
     }
#endif

    // generate children (except expressions)
    generateChildrenWithTags(out, node, "substmachines,substparams,gatesizes,display-string", indent, mode);

    // build function call
    out << indent << "// build submodules recursively (if it has any):\n";
    out << indent << "modtype->buildInside( " << submodule_var.c_str() << " );\n";

    if (vectorsize)
    {
        indent = decreaseIndent(indent);
        out << indent << "}\n";
    }
    out << indent << "machines.clear();\n\n";
}

void NEDCppGenerator::doSubmoduleCleanup(ostream& out, SubmoduleNode *node, const char *indent, int mode, const char *)
{
    if (findExpression(node, "vector-size")!=NULL)
        out << indent << "delete [] " << node->getName() << "_p;\n";
}

void NEDCppGenerator::doSubstparams(ostream& out, SubstparamsNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// parameter assignments:\n";
    beginConditionalBlock(out, node, indent, mode, arg);

    // generate children
    generateChildrenExceptTags(out, node, "expression", indent, mode);

    // epilog: read all parameters
    out << indent << "readModuleParameters(" << submodule_var.c_str() << ");\n";
    out << indent << "check_error();\n";

    endConditionalBlock(out, node, indent, mode, arg);
    out << "\n";
}

void NEDCppGenerator::doSubstparam(ostream& out, SubstparamNode *node, const char *indent, int mode, const char *)
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
    if (paramvalue)  generateItem(out, paramvalue, indent, mode);
    out << ";\n\n";
}

void NEDCppGenerator::doGatesizes(ostream& out, GatesizesNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// gatesizes:\n";
    beginConditionalBlock(out, node, indent, mode, arg);

    // generate children
    generateChildrenExceptTags(out, node, "expression", indent, mode);

    // epilog
    endConditionalBlock(out, node, indent, mode, arg);
    out << "\n";
}

void NEDCppGenerator::doGatesize(ostream& out, GatesizeNode *node, const char *indent, int mode, const char *)
{
    out << indent << "size = ";
    ExpressionNode *vectorsize = findExpression(node, "vector-size");
    if (vectorsize)  generateItem(out, vectorsize, indent, mode);
    out << ";\n";

    out << indent << "check_gate_count( size, \"" << submodule_name.c_str() << "\",\"" << node->getName() << "\",\"" << module_name.c_str() << "\");\n";
    out << indent << submodule_var.c_str() << "->setGateSize(\"" << node->getName() << "\", size);\n\n";
}

void NEDCppGenerator::doSubstmachines(ostream& out, SubstmachinesNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// 'on' section:\n";
    beginConditionalBlock(out, node, indent, mode, arg);

    // generate children
    generateChildrenExceptTags(out, node, "expression", indent, mode);

    // epilog
    endConditionalBlock(out, node, indent, mode, arg);
    out << "\n";
}

void NEDCppGenerator::doSubstmachine(ostream& out, SubstmachineNode *node, const char *indent, int mode, const char *)
{
    if (!in_network)
    {
        out << indent << "par = new cPar();\n";
        out << indent << "*par = mod->machinePar(\"" << node->getName() << "\");\n";
        out << indent << "machines.add( par );\n";
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
    out << indent << "check_error(); check_memory();\n\n";
}

void NEDCppGenerator::doConnections(ostream& out, ConnectionsNode *node, const char *indent, int mode, const char *)
{
    // prolog
    out << indent << "//\n";
    out << indent << "// connections:\n";
    out << indent << "//\n";
    out << indent << "cLinkType *link_p;\n";
    out << indent << "cModule *tmpmod;\n";
    out << indent << "cPar *delay_p, *error_p, *datarate_p;\n";
    out << indent << "cGate *srcgate, *destgate;\n\n";

    // generate children
    generateChildren(out, node, indent, mode);

    // epilog
    if (node->getCheckUnconnected())
    {
       // check connections
       // FIXME compile-time check as far as possible
       out << indent << "// check all gates are connected:\n";
       out << indent << "mod->checkInternalConnections();\n\n";
    }
}

// helper
char *do_channeldescr(char *link_name, char *delay_expr, char *error_expr, char *datarate_expr)
{
#if 0
        // returns a string that can be used as args to a connect() call
        char buf[64];
        expr_type value;

        // channel type
        if (link_name!=NULL)
        {
            // we must have seen the declaration
            if (!nl_find (channel_list, link_name))
            {
                sprintf (errstr, "Channel \"" << %s << "\" unknown\n", link_name);
                adderr;
            }
            out << indent << "link_p = findLink( \"" << %s << "\" );\n",
                     indent, link_name);

            jar_free(link_name);
            // no delay, error, datarate was given, no more jar_free()
            return jar_strdup("link_p");
        }

        // delay, error and data rate expressions
        if (delay_expr!=NULL)
        {
            get_expression(delay_expr,tmp,value);
            out << %s << "delay_p = new cPar(); *delay_p = " << %s << ";\n",
                     indent,value);
        }
        if (error_expr!=NULL)
        {
            get_expression(error_expr,tmp,value);
            out << indent << "error_p = new cPar(); *error_p = " << %s << ";\n",
                     indent,value);
        }
        if (datarate_expr!=NULL)
        {
            get_expression(datarate_expr,tmp,value);
            out << indent << "datarate_p = new cPar(); *datarate_p = " << %s << ";\n",
                     indent,value);
        }

        sprintf(buf, "" << %s << ", " << %s << ", " << %s << "",
                  (delay_expr!=NULL) ?    "delay_p" :    "NO(cPar)",
                  (error_expr!=NULL) ?    "error_p" :    "NO(cPar)",
                  (datarate_expr!=NULL) ? "datarate_p" : "NO(cPar)"
               );

        //jar_free (link_name);
        jar_free (delay_expr);
        jar_free (error_expr);
        jar_free (datarate_expr);
        return jar_strdup( buf );
#endif
   return 0;
}

void NEDCppGenerator::resolveGate(ostream& out, const char *var, const char *indent, const char *modname, ExpressionNode *modindex, const char *gatename, ExpressionNode *gateindex)
{
   // module
   const char *modvar = "mod";
   if (modname)
   {
       modvar = "tmpmod";
       out << indent << "tmpmod = " << modname << "_p";
       if (modindex)
       {
            out << "[(long)("; // FIXME bounds check needed!
            generateItem(out, modindex, indent);
            out << ")]";
       }
       out << ";\n";
   }

   // gate
   out << indent << var << " = " << modvar << "->gate(\"" << gatename << "\"";
   if (gateindex)
   {
       out << ", ";
       generateItem(out, gateindex, indent);
   }
   out << ");\n";

   //   out << indent << "check_gate( gateL, \"" << %s << "\", "
   //      "indexedname(b1,\"" << %s << "\",gate_nr_L) );\n\n",
   //      indent, mod_L, gate_L );
}

void NEDCppGenerator::doConnection(ostream& out, ConnectionNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// connection\n";
    beginConditionalBlock(out, node, indent, mode, arg);

    // create connection
    resolveGate(out, "srcgate", indent,
           node->getSrcModule(), findExpression(node,"src-module-index"),
           node->getSrcGate(), findExpression(node,"src-gate-index"));

    resolveGate(out, "destgate", indent,
           node->getDestModule(), findExpression(node,"dest-module-index"),
           node->getDestGate(), findExpression(node,"dest-gate-index"));

    out << indent << "srcgate->connect(destgate);\n";  // FIXME channel!

    generateChildrenWithTags(out, node, "conn-attr", indent);

    DisplayStringNode *dispstr = (DisplayStringNode *)node->getFirstChildWithTag(NED_DISPLAY_STRING);
    if (dispstr)
    {
        out << indent << "srcgate->setDisplayString(\"" <<  dispstr->getValue() << "\");\n";
    }
    out << indent << "check_error(); check_memory();\n";

    // epilog
    endConditionalBlock(out, node, indent, mode, arg);
    out << "\n";
}

void NEDCppGenerator::doConnattr(ostream& out, ConnAttrNode *node, const char *indent, int mode, const char *arrow)
{
    ExpressionNode *value = findExpression(node, "value");
    out << indent << "double " << node->getName() << "_var = "; // FIXME should be cPar!!!
    generateItem(out, value, indent, mode);
    out << ";\n";
}

void NEDCppGenerator::doForloop(ostream& out, ForLoopNode *node, const char *indent, int mode, const char *arg)
{
    // prolog
    out << indent << "// for loop:\n";

    // open loops
    for (NEDElement *child=node->getFirstChildWithTag(NED_LOOP_VAR); child; child = child->getNextSiblingWithTag(NED_LOOP_VAR))
    {
        generateItem(out, child, indent, mode);
        out << indent << "{\n";
        indent = increaseIndent(indent);
    }

    // generate children: then connections
    generateChildrenWithTags(out, node, "connection", indent, mode, arg);

    // close loops
    for (NEDElement *child1=node->getFirstChildWithTag(NED_LOOP_VAR); child1; child1 = child1->getNextSiblingWithTag(NED_LOOP_VAR))
    {
        indent = decreaseIndent(indent);
        out << indent << "}\n";
    }
}

void NEDCppGenerator::doLoopvar(ostream& out, LoopVarNode *node, const char *indent, int mode, const char *)
{
    ExpressionNode *fromvalue = findExpression(node, "from-value");
    ExpressionNode *tovalue = findExpression(node, "to-value");

    out << indent << "long start = ";
    generateItem(out, fromvalue, indent, mode);
    out << ";\n";
    out << indent << "long end = ";
    generateItem(out, tovalue, indent, mode);
    out << ";\n";

    out << indent << "for (" << node->getParamName() << "_var=start; " << node->getParamName() << "_var<end; " << node->getParamName() << "_var++)\n";
}

void NEDCppGenerator::doDisplayString(ostream& out, DisplayStringNode *node, const char *indent, int mode, const char *)
{
    // FIXME!!!
}

ExpressionNode *NEDCppGenerator::findExpression(NEDElement *parent, const char *target)
{
    for (NEDElement *child=parent->getFirstChildWithTag(NED_EXPRESSION); child; child = child->getNextSiblingWithTag(NED_EXPRESSION))
    {
        ExpressionNode *expr = (ExpressionNode *)child;
        if (!strcmp(expr->getTarget(),target))
            return expr;
    }
    return NULL;
}

void NEDCppGenerator::doExpression(ostream& out, ExpressionNode *node, const char *indent, int mode, const char *arg)
{
    exprgen.generateExpressionUsage(out, (ExpressionNode *)node,indent);
}

