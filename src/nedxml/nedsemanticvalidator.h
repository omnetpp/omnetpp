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
    NEDSemanticValidator(bool parsedExpr, NEDSymbolTable *symbtab);

    /** Destructor */
    virtual ~NEDSemanticValidator();

    /** @name Validator methods */
    //@{
    virtual void validateElement(FilesNode *node) = 0;
    virtual void validateElement(NedFileNode *node) = 0;
    virtual void validateElement(WhitespaceNode *node) = 0;
    virtual void validateElement(ImportNode *node) = 0;
    virtual void validateElement(PropertydefNode *node) = 0;
    virtual void validateElement(ExtendsNode *node) = 0;
    virtual void validateElement(InterfaceNameNode *node) = 0;
    virtual void validateElement(ChannelInterfaceNameNode *node) = 0;
    virtual void validateElement(SimpleNode *node) = 0;
    virtual void validateElement(InterfaceNode *node) = 0;
    virtual void validateElement(ModuleNode *node) = 0;
    virtual void validateElement(ParametersNode *node) = 0;
    virtual void validateElement(ParamGroupNode *node) = 0;
    virtual void validateElement(ParamNode *node) = 0;
    virtual void validateElement(PropertyNode *node) = 0;
    virtual void validateElement(KeyValueNode *node) = 0;
    virtual void validateElement(GatesNode *node) = 0;
    virtual void validateElement(GateGroupNode *node) = 0;
    virtual void validateElement(GateNode *node) = 0;
    virtual void validateElement(SubmodulesNode *node) = 0;
    virtual void validateElement(SubmoduleNode *node) = 0;
    virtual void validateElement(ConnectionsNode *node) = 0;
    virtual void validateElement(ConnectionNode *node) = 0;
    virtual void validateElement(ChannelInterfaceNode *node) = 0;
    virtual void validateElement(ChannelNode *node) = 0;
    virtual void validateElement(ConnectionGroupNode *node) = 0;
    virtual void validateElement(LoopNode *node) = 0;
    virtual void validateElement(ConditionNode *node) = 0;
    virtual void validateElement(ExpressionNode *node) = 0;
    virtual void validateElement(OperatorNode *node) = 0;
    virtual void validateElement(FunctionNode *node) = 0;
    virtual void validateElement(RefNode *node) = 0;
    virtual void validateElement(ConstNode *node) = 0;
    virtual void validateElement(MsgFileNode *node) = 0;
    virtual void validateElement(CplusplusNode *node) = 0;
    virtual void validateElement(StructDeclNode *node) = 0;
    virtual void validateElement(ClassDeclNode *node) = 0;
    virtual void validateElement(MessageDeclNode *node) = 0;
    virtual void validateElement(EnumDeclNode *node) = 0;
    virtual void validateElement(EnumNode *node) = 0;
    virtual void validateElement(EnumFieldsNode *node) = 0;
    virtual void validateElement(EnumFieldNode *node) = 0;
    virtual void validateElement(MessageNode *node) = 0;
    virtual void validateElement(ClassNode *node) = 0;
    virtual void validateElement(StructNode *node) = 0;
    virtual void validateElement(FieldsNode *node) = 0;
    virtual void validateElement(FieldNode *node) = 0;
    virtual void validateElement(PropertiesNode *node) = 0;
    virtual void validateElement(MsgpropertyNode *node) = 0;
    virtual void validateElement(UnknownNode *node) = 0;
    //@}
};

#endif

