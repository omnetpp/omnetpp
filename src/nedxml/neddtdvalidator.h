//==========================================================================
// Part of the OMNeT++/OMNEST Discrete Event Simulation System
//
// Generated from ned.dtd by dtdclassgen.pl
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga

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
    virtual void validateElement(FilesElement *node);
    virtual void validateElement(NedFileElement *node);
    virtual void validateElement(CommentElement *node);
    virtual void validateElement(PackageElement *node);
    virtual void validateElement(ImportElement *node);
    virtual void validateElement(PropertyDeclElement *node);
    virtual void validateElement(ExtendsElement *node);
    virtual void validateElement(InterfaceNameElement *node);
    virtual void validateElement(SimpleModuleElement *node);
    virtual void validateElement(ModuleInterfaceElement *node);
    virtual void validateElement(CompoundModuleElement *node);
    virtual void validateElement(ChannelInterfaceElement *node);
    virtual void validateElement(ChannelElement *node);
    virtual void validateElement(ParametersElement *node);
    virtual void validateElement(ParamElement *node);
    virtual void validateElement(PropertyElement *node);
    virtual void validateElement(PropertyKeyElement *node);
    virtual void validateElement(GatesElement *node);
    virtual void validateElement(GateElement *node);
    virtual void validateElement(TypesElement *node);
    virtual void validateElement(SubmodulesElement *node);
    virtual void validateElement(SubmoduleElement *node);
    virtual void validateElement(ConnectionsElement *node);
    virtual void validateElement(ConnectionElement *node);
    virtual void validateElement(ConnectionGroupElement *node);
    virtual void validateElement(LoopElement *node);
    virtual void validateElement(ConditionElement *node);
    virtual void validateElement(ExpressionElement *node);
    virtual void validateElement(OperatorElement *node);
    virtual void validateElement(FunctionElement *node);
    virtual void validateElement(IdentElement *node);
    virtual void validateElement(LiteralElement *node);
    virtual void validateElement(MsgFileElement *node);
    virtual void validateElement(NamespaceElement *node);
    virtual void validateElement(CplusplusElement *node);
    virtual void validateElement(StructDeclElement *node);
    virtual void validateElement(ClassDeclElement *node);
    virtual void validateElement(MessageDeclElement *node);
    virtual void validateElement(PacketDeclElement *node);
    virtual void validateElement(EnumDeclElement *node);
    virtual void validateElement(EnumElement *node);
    virtual void validateElement(EnumFieldsElement *node);
    virtual void validateElement(EnumFieldElement *node);
    virtual void validateElement(MessageElement *node);
    virtual void validateElement(PacketElement *node);
    virtual void validateElement(ClassElement *node);
    virtual void validateElement(StructElement *node);
    virtual void validateElement(FieldElement *node);
    virtual void validateElement(UnknownElement *node);
    //@}
};

NAMESPACE_END

#endif

