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


#ifndef __NEDVALIDATOR_H
#define __NEDVALIDATOR_H

#include "nedelements.h"

/**
 * GENERATED CLASS. Abtract base class for NED validators.
 * 
 * @ingroup Validation
 */
class NEDXML_API NEDValidatorBase
{
  protected:
    NEDErrorStore *errors;
  public:
    /** @name Constructor, destructor */
    //@{
    NEDValidatorBase(NEDErrorStore *e) {errors = e;}
    virtual ~NEDValidatorBase() {}
    //@}

    /** Validates the node recursively */
    virtual void validate(NEDElement *node);
    /** Dispatches to the corresponding overloaded validateElement() function */
    virtual void validateElement(NEDElement *node);

  protected:
    /** @name Validation functions, to be implemented in subclasses */
    //@{
    virtual void validateElement(FilesNode *node) = 0;
    virtual void validateElement(NedFileNode *node) = 0;
    virtual void validateElement(CommentNode *node) = 0;
    virtual void validateElement(PackageNode *node) = 0;
    virtual void validateElement(ImportNode *node) = 0;
    virtual void validateElement(PropertyDeclNode *node) = 0;
    virtual void validateElement(ExtendsNode *node) = 0;
    virtual void validateElement(InterfaceNameNode *node) = 0;
    virtual void validateElement(SimpleModuleNode *node) = 0;
    virtual void validateElement(ModuleInterfaceNode *node) = 0;
    virtual void validateElement(CompoundModuleNode *node) = 0;
    virtual void validateElement(ChannelInterfaceNode *node) = 0;
    virtual void validateElement(ChannelNode *node) = 0;
    virtual void validateElement(ParametersNode *node) = 0;
    virtual void validateElement(ParamNode *node) = 0;
    virtual void validateElement(PatternNode *node) = 0;
    virtual void validateElement(PropertyNode *node) = 0;
    virtual void validateElement(PropertyKeyNode *node) = 0;
    virtual void validateElement(GatesNode *node) = 0;
    virtual void validateElement(GateNode *node) = 0;
    virtual void validateElement(TypesNode *node) = 0;
    virtual void validateElement(SubmodulesNode *node) = 0;
    virtual void validateElement(SubmoduleNode *node) = 0;
    virtual void validateElement(ConnectionsNode *node) = 0;
    virtual void validateElement(ConnectionNode *node) = 0;
    virtual void validateElement(ChannelSpecNode *node) = 0;
    virtual void validateElement(ConnectionGroupNode *node) = 0;
    virtual void validateElement(LoopNode *node) = 0;
    virtual void validateElement(ConditionNode *node) = 0;
    virtual void validateElement(ExpressionNode *node) = 0;
    virtual void validateElement(OperatorNode *node) = 0;
    virtual void validateElement(FunctionNode *node) = 0;
    virtual void validateElement(IdentNode *node) = 0;
    virtual void validateElement(LiteralNode *node) = 0;
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
    virtual void validateElement(FieldNode *node) = 0;
    virtual void validateElement(UnknownNode *node) = 0;
    //@}
};

#endif

