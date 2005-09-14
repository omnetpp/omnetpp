//==========================================================================
//  CPPGENERATOR.H - part of
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

#ifndef __CPPGENERATOR_H
#define __CPPGENERATOR_H

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "nedelements.h"
#include "cppexprgenerator.h"
#include "nedcompiler.h" // for NEDSymbolTable

using std::ostream;

/**
 * Simple front-end to NEDCppGenerator.
 *
 * @ingroup CppGenerator
 */
void generateCpp(ostream& out, ostream& outh, NEDElement *node, NEDSymbolTable *symtab);


/**
 * NEDC functionality: generates C++ code from a NED object tree.
 * Assumes object tree has already passed all validation stages (DTD, basic, semantic).
 *
 * Uses CppExpressionGenerator.
 *
 * @ingroup CppGenerator
 */
class NEDCppGenerator
{
  protected:
    ostream& out;  // stream for writing .cc file
    ostream& outh; // stream for writing .h file

    bool in_network;
    std::string submodule_var;
    int indentsize;

    enum {
      MODE_NORMAL,
      MODE_FINALLY
    };

    CppExpressionGenerator exprgen;

    NEDSymbolTable *symboltable;

  public:
    /**
     * Constructor. It expects two streams on which it will write the generated code
     * (stream for the C++ source code and stream for the C++ header),
     * and the symbol table.
     */
    NEDCppGenerator(ostream& out, ostream& outh, NEDSymbolTable *symtab);

    /**
     * Destructor.
     */
    ~NEDCppGenerator();

    /**
     * Set indentation in generated C++ source. Default is 4 spaces.
     */
    void setIndentSize(int indentsize);

    /**
     * Generate C++ source code. Code will be written to the streams given
     * to the constructor.
     */
    void generate(NEDElement *node);

  protected:
    const char *increaseIndent(const char *indent);
    const char *decreaseIndent(const char *indent);

    void generateItem(NEDElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void generateChildren(NEDElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void generateChildrenWithTags(NEDElement *node, const char *tags, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void generateChildrenExceptTags(NEDElement *node, const char *tags, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);

    // find given expression node in parent.
    ExpressionNode *findExpression(NEDElement *parent, const char *target);
    // if expression consists of a constant, return its pointer, NULL otherwise.
    ConstNode *getConstantExpression(ExpressionNode *node);

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
    void resolveSubmoduleType(SubmoduleNode *node, const char *indent);
    void doSubmodule(SubmoduleNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubmoduleFinally(SubmoduleNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstparams(SubstparamsNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstparam(SubstparamNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGatesizes(GatesizesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGatesize(GatesizeNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstmachines(SubstmachinesNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstmachine(SubstmachineNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doConnections(ConnectionsNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void resolveGate(const char *modname, ExpressionNode *modindex, const char *gatename, ExpressionNode *gateindex, bool isplusplus);
    void resolveConnectionAttributes(ConnectionNode *node, const char *indent, int mode);
    void doConnection(ConnectionNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doConnattr(ConnAttrNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doForloop(ForLoopNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doLoopvar(LoopVarNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doDisplayString(DisplayStringNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doExpression(ExpressionNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);

    struct ClassDesc;
    struct FieldDesc;

    void doCplusplus(CplusplusNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doStructDecl(StructDeclNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doClassDecl(ClassDeclNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doMessageDecl(MessageDeclNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doEnumDecl(EnumDeclNode *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
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


