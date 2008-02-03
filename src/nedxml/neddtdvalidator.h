//==========================================================================
// Part of the OMNeT++/OMNEST Discrete Event Simulation System
//
// Generated from ned.dtd by dtdclassgen.pl
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//
// THIS IS A GENERATED FILE, DO NOT EDIT!
//


#ifndef __DTDVALIDATOR_H
#define __DTDVALIDATOR_H

#include "nedelements.h"
#include "neddtdvalidatorbase.h"

NAMESPACE_BEGIN

/**
 * GENERATED CLASS. Validates a NEDElement tree by the DTD.
 *
 * @ingroup Validation
 */
class NEDXML_API NEDDTDValidator : public NEDDTDValidatorBase
{
  public:
    NEDDTDValidator(NEDErrorStore *e) : NEDDTDValidatorBase(e) {}
    virtual ~NEDDTDValidator() {}

  protected:
    /** @name Validation functions */
    //@{
    virtual void validateElement(FilesNode *node);
    virtual void validateElement(NedFileNode *node);
    virtual void validateElement(CommentNode *node);
    virtual void validateElement(PackageNode *node);
    virtual void validateElement(ImportNode *node);
    virtual void validateElement(PropertyDeclNode *node);
    virtual void validateElement(ExtendsNode *node);
    virtual void validateElement(InterfaceNameNode *node);
    virtual void validateElement(SimpleModuleNode *node);
    virtual void validateElement(ModuleInterfaceNode *node);
    virtual void validateElement(CompoundModuleNode *node);
    virtual void validateElement(ChannelInterfaceNode *node);
    virtual void validateElement(ChannelNode *node);
    virtual void validateElement(ParametersNode *node);
    virtual void validateElement(ParamNode *node);
    virtual void validateElement(PatternNode *node);
    virtual void validateElement(PropertyNode *node);
    virtual void validateElement(PropertyKeyNode *node);
    virtual void validateElement(GatesNode *node);
    virtual void validateElement(GateNode *node);
    virtual void validateElement(TypesNode *node);
    virtual void validateElement(SubmodulesNode *node);
    virtual void validateElement(SubmoduleNode *node);
    virtual void validateElement(ConnectionsNode *node);
    virtual void validateElement(ConnectionNode *node);
    virtual void validateElement(ChannelSpecNode *node);
    virtual void validateElement(ConnectionGroupNode *node);
    virtual void validateElement(LoopNode *node);
    virtual void validateElement(ConditionNode *node);
    virtual void validateElement(ExpressionNode *node);
    virtual void validateElement(OperatorNode *node);
    virtual void validateElement(FunctionNode *node);
    virtual void validateElement(IdentNode *node);
    virtual void validateElement(LiteralNode *node);
    virtual void validateElement(MsgFileNode *node);
    virtual void validateElement(NamespaceNode *node);
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
    virtual void validateElement(FieldNode *node);
    virtual void validateElement(UnknownNode *node);
    //@}
};

NAMESPACE_END

#endif

