//==========================================================================
// nedsemanticvalidator.h - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NEDSemanticValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __NEDSEMANTICVALIDATOR_H
#define __NEDSEMANTICVALIDATOR_H

#include <string.h>
#include "nedvalidator.h"

class NEDSymbolTable;

/**
 * Performs semantic validation. Should be called after tree passed
 * DTD validation and basic validation.
 *
 * @ingroup Validation
 */
class NEDSemanticValidator : public NEDValidatorBase
{
  protected:
    bool parsedExpressions;
    NEDSymbolTable *symboltable;

    // temporary variables:
    NEDElement *moduletypedecl;

    NEDElement *findChildWithTagAndAttribute(NEDElement *node, int tag, const char *attr, const char *value);
    void checkGate(GateNode *gate, bool hasGateIndex, bool isInput, NEDElement *conn, bool isSrcGate);
    void validateConnGate(const char *submodName, bool hasSubmodIndex,
                          const char *gateName, bool hasGateIndex,
                          NEDElement *parent, NEDElement *conn, bool isSrc);

  public:
    /** Constructor */
    NEDSemanticValidator(bool parsedExpr, NEDSymbolTable *symbtab, NEDErrorStore *e);

    /** Destructor */
    virtual ~NEDSemanticValidator();

  protected:
    /** @name Validator methods */
    //@{
    virtual void validateElement(FilesNode *node);
    virtual void validateElement(NedFileNode *node);
    virtual void validateElement(CommentNode *node);
    virtual void validateElement(ImportNode *node);
    virtual void validateElement(PropertyDeclNode *node);
    virtual void validateElement(ExtendsNode *node);
    virtual void validateElement(InterfaceNameNode *node);
    virtual void validateElement(SimpleModuleNode *node);
    virtual void validateElement(ModuleInterfaceNode *node);
    virtual void validateElement(CompoundModuleNode *node);
    virtual void validateElement(ParametersNode *node);
    virtual void validateElement(ParamNode *node);
    virtual void validateElement(PatternNode *node);
    virtual void validateElement(PropertyNode *node);
    virtual void validateElement(PropertyKeyNode *node);
    virtual void validateElement(GatesNode *node);
    virtual void validateElement(GateNode *node);
    virtual void validateElement(SubmodulesNode *node);
    virtual void validateElement(SubmoduleNode *node);
    virtual void validateElement(ConnectionsNode *node);
    virtual void validateElement(ConnectionNode *node);
    virtual void validateElement(ChannelInterfaceNode *node);
    virtual void validateElement(ChannelNode *node);
    virtual void validateElement(ConnectionGroupNode *node);
    virtual void validateElement(LoopNode *node);
    virtual void validateElement(ConditionNode *node);
    virtual void validateElement(ExpressionNode *node);
    virtual void validateElement(OperatorNode *node);
    virtual void validateElement(FunctionNode *node);
    virtual void validateElement(IdentNode *node);
    virtual void validateElement(LiteralNode *node);
    virtual void validateElement(MsgFileNode *node);
    virtual void validateElement(CplusplusNode *node);
    virtual void validateElement(StructDeclNode *node);
    virtual void validateElement(ClassDeclNode *node);
    virtual void validateElement(MessageDeclNode *node);
    virtual void validateElement(EnumDeclNode *node);
    virtual void validateElement(EnumNode *node);
    virtual void validateElement(EnumFieldsNode *node);
    virtual void validateElement(EnumFieldNode *node);
    virtual void validateElement(MessageNode *node);
    virtual void validateElement(ClassNode *node);
    virtual void validateElement(StructNode *node);
    virtual void validateElement(FieldsNode *node);
    virtual void validateElement(FieldNode *node);
    virtual void validateElement(PropertiesNode *node);
    virtual void validateElement(MsgpropertyNode *node);
    virtual void validateElement(UnknownNode *node);
    //@}
};

#endif

