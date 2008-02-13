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
#include "nedcompiler.h" // for NEDTypeResolver


//XXX *** CURRENTLY NOT IN USE ***


NAMESPACE_BEGIN

/**
 * Simple front-end to NEDCppGenerator.
 *
 * @ingroup CppGenerator
 */
void generateCpp(std::ostream& out, std::ostream& outh, NEDElement *node, NEDTypeResolver *resolver, NEDErrorStore *errors);


/**
 * NEDC functionality: generates C++ code from a NED object tree.
 * Assumes object tree has already passed all validation stages (DTD, syntax, semantic).
 *
 * Uses CppExpressionGenerator.
 *
 * @ingroup CppGenerator
 */
class NEDXML_API NEDCppGenerator
{
  protected:
    ostream& out;  // stream for writing .cc file
    ostream& outh; // stream for writing .h file
    NEDErrorStore *errors;

    bool in_network;
    std::string submodule_var;
    int indentsize;

    enum {
      MODE_NORMAL,
      MODE_FINALLY
    };

    CppExpressionGenerator exprgen;

    NEDTypeResolver *resolver;

  public:
    /**
     * Constructor. It expects two streams on which it will write the generated code
     * (stream for the C++ source code and stream for the C++ header),
     * and the symbol table.
     */
    NEDCppGenerator(std::ostream& out, std::ostream& outh, NEDTypeResolver *resolver, NEDErrorStore *e);

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
    ExpressionElement *findExpression(NEDElement *parent, const char *target);
    // if expression consists of a constant, return its pointer, NULL otherwise.
    LiteralElement *getConstantExpression(ExpressionElement *node);

    void writeProlog(ostream& out);
    void printTemporaryVariables(const char *indent);
    void beginConditionalBlock(NEDElement *node, const char *&indent, int mode, const char *);
    void endConditionalBlock(NEDElement *node, const char *&indent, int mode, const char *);

    void doNedFile(NedFileElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doNedFiles(FilesElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doImports(ImportElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doImport(ImportedFileElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doChannel(ChannelElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doChannelAttr(ChannelAttrElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doNetwork(NetworkElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSimple(SimpleModuleElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doModule(CompoundModuleElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doParams(ParamsElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doParam(ParamElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGates(GatesElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGate(GateElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doMachines(MachinesElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doMachine(MachineElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubmodules(SubmodulesElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void resolveSubmoduleType(SubmoduleElement *node, const char *indent);
    void doSubmodule(SubmoduleElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubmoduleFinally(SubmoduleElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstparams(SubstparamsElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstparam(SubstparamElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGatesizes(GatesizesElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doGatesize(GatesizeElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstmachines(SubstmachinesElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doSubstmachine(SubstmachineElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doConnections(ConnectionsElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void resolveGate(const char *modname, ExpressionElement *modindex, const char *gatename, ExpressionElement *gateindex, bool isplusplus);
    void resolveConnectionAttributes(ConnectionElement *node, const char *indent, int mode);
    void doConnection(ConnectionElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doConnattr(ConnAttrElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doForloop(ForLoopElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doLoopvar(LoopVarElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doDisplayString(DisplayStringElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doExpression(ExpressionElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);

    struct ClassDesc;
    struct FieldDesc;

    void doNamespace(NamespaceElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doCplusplus(CplusplusElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doStructDecl(StructDeclElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doClassDecl(ClassDeclElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doMessageDecl(MessageDeclElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doEnumDecl(EnumDeclElement *node, const char *indent, int mode=MODE_NORMAL, const char *arg=NULL);
    void doEnum(EnumElement *node, const char *indent, int mode, const char *);
    void doEnumFields(EnumFieldsElement *node, const char *indent, int mode, const char *);
    void doEnumField(EnumFieldElement *node, const char *indent, int mode, const char *);
    void doMessage(MessageElement *node, const char *, int, const char *);
    void doClass(ClassElement *node, const char *, int, const char *);
    void doStruct(StructElement *node, const char *, int, const char *);
    void prepareForCodeGeneration(NEDElement *node, ClassDesc& cld, FieldDesc *&fld, int& numfields);
    void generateClass(ClassDesc& cld, FieldDesc *&fld, int numfields);
    void generateStruct(ClassDesc& cld, FieldDesc *&fld, int numfields);
    void generateDescriptorClass(ClassDesc& cld, FieldDesc *&fld, int numfields);
};

NAMESPACE_END


#endif


