//==========================================================================
//   CPPGENERATOR.H -
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

#ifndef __CPPGENERATOR_H
#define __CPPGENERATOR_H

#include <string>
#include <map>
#include <vector>
#include "iostream.h"
#include "nedelements.h"
#include "cppexprgenerator.h"

/**
 * Simple front-end to NEDCppGenerator.
 *
 * @ingroup CppGenerator
 */
void generateCpp(ostream& out, ostream& outh, NEDElement *node);


/**
 * Nedc functionality: generates C++ code from a NED object tree.
 * Assumes object tree has already passed all validation stages (DTD, basic, semantic).
 *
 * Uses CppExpressionGenerator.
 *
 * @ingroup CppGenerator
 */
class NEDCppGenerator
{
  protected:
    ostream& out;
    ostream& outh;
    bool in_network;
    std::string module_name;
    std::string submodule_name;
    std::string submodule_var;
    int indentsize;

    enum {
      MODE_NORMAL,
      MODE_CLEANUP,
    };

    CppExpressionGenerator exprgen;

  public:
    NEDCppGenerator(ostream& out, ostream& outh);
    ~NEDCppGenerator();
    void setIndentSize(int indentsize);
    void generate(NEDElement *node);

  protected:
    const char *increaseIndent(const char *indent);
    const char *decreaseIndent(const char *indent);

    void generateItem(NEDElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void generateChildren(NEDElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void generateChildrenWithTags(NEDElement *node, const char *tags, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void generateChildrenExceptTags(NEDElement *node, const char *tags, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    ExpressionNode *findExpression(NEDElement *parent, const char *target);

    void writeProlog(ostream& out);
    void printTemporaryVariables(const char *indent);
    void beginConditionalBlock(NEDElement *node, const char *&indent, int mode, const char *);
    void endConditionalBlock(NEDElement *node, const char *&indent, int mode, const char *);

    void doNedFile(NedFileNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doNedFiles(NedFilesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doImports(ImportNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doImport(ImportedFileNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doChannel(ChannelNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doChannelAttr(ChannelAttrNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doNetwork(NetworkNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSimple(SimpleModuleNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doModule(CompoundModuleNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doParams(ParamsNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doParam(ParamNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGates(GatesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGate(GateNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doMachines(MachinesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doMachine(MachineNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubmodules(SubmodulesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubmodule(SubmoduleNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubmoduleCleanup(SubmoduleNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstparams(SubstparamsNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstparam(SubstparamNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGatesizes(GatesizesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGatesize(GatesizeNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstmachines(SubstmachinesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstmachine(SubstmachineNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doConnections(ConnectionsNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void resolveGate(const char *var, const char *indent, const char *modname, ExpressionNode *modindex, const char *gatename, ExpressionNode *gateindex);
    void doConnection(ConnectionNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doConnattr(ConnAttrNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doForloop(ForLoopNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doLoopvar(LoopVarNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doDisplayString(DisplayStringNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doExpression(ExpressionNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);

    struct ClassDesc;
    struct FieldDesc;

    void doCppinclude(CppincludeNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doCppStruct(CppStructNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doCppCobject(CppCobjectNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doCppNoncobject(CppNoncobjectNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doEnum(EnumNode *node, const char *indent, int mode, const char *);
    void doEnumFields(EnumFieldsNode *node, const char *indent, int mode, const char *);
    void doEnumField(EnumFieldNode *node, const char *indent, int mode, const char *);
    void doMessage(MessageNode *node, const char *, int, const char *);
    void doClass(ClassNode *node, const char *, int, const char *);
    void doStruct(StructNode *node, const char *, int, const char *);
    void prepareForCodeGeneration(NEDElement *node, ClassDesc& cld, FieldDesc *&fld, int& numfields);
    void generateClass(ClassDesc& cld, FieldDesc *&fld, int numfields);
    void generateStruct(ClassDesc& cld, FieldDesc *&fld, int numfields);
    void generateDescriptorClass(ClassDesc& cld, FieldDesc *&fld, int numfields);
};

#endif


