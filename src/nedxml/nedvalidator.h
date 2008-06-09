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


#ifndef __NEDVALIDATOR_H
#define __NEDVALIDATOR_H

#include "nederror.h"
#include "nedexception.h"
#include "nedelements.h"

NAMESPACE_BEGIN

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
    virtual void validateElement(FilesElement *node) = 0;
    virtual void validateElement(NedFileElement *node) = 0;
    virtual void validateElement(CommentElement *node) = 0;
    virtual void validateElement(PackageElement *node) = 0;
    virtual void validateElement(ImportElement *node) = 0;
    virtual void validateElement(PropertyDeclElement *node) = 0;
    virtual void validateElement(ExtendsElement *node) = 0;
    virtual void validateElement(InterfaceNameElement *node) = 0;
    virtual void validateElement(SimpleModuleElement *node) = 0;
    virtual void validateElement(ModuleInterfaceElement *node) = 0;
    virtual void validateElement(CompoundModuleElement *node) = 0;
    virtual void validateElement(ChannelInterfaceElement *node) = 0;
    virtual void validateElement(ChannelElement *node) = 0;
    virtual void validateElement(ParametersElement *node) = 0;
    virtual void validateElement(ParamElement *node) = 0;
    virtual void validateElement(PatternElement *node) = 0;
    virtual void validateElement(PropertyElement *node) = 0;
    virtual void validateElement(PropertyKeyElement *node) = 0;
    virtual void validateElement(GatesElement *node) = 0;
    virtual void validateElement(GateElement *node) = 0;
    virtual void validateElement(TypesElement *node) = 0;
    virtual void validateElement(SubmodulesElement *node) = 0;
    virtual void validateElement(SubmoduleElement *node) = 0;
    virtual void validateElement(ConnectionsElement *node) = 0;
    virtual void validateElement(ConnectionElement *node) = 0;
    virtual void validateElement(ChannelSpecElement *node) = 0;
    virtual void validateElement(ConnectionGroupElement *node) = 0;
    virtual void validateElement(LoopElement *node) = 0;
    virtual void validateElement(ConditionElement *node) = 0;
    virtual void validateElement(ExpressionElement *node) = 0;
    virtual void validateElement(OperatorElement *node) = 0;
    virtual void validateElement(FunctionElement *node) = 0;
    virtual void validateElement(IdentElement *node) = 0;
    virtual void validateElement(LiteralElement *node) = 0;
    virtual void validateElement(MsgFileElement *node) = 0;
    virtual void validateElement(NamespaceElement *node) = 0;
    virtual void validateElement(CplusplusElement *node) = 0;
    virtual void validateElement(StructDeclElement *node) = 0;
    virtual void validateElement(ClassDeclElement *node) = 0;
    virtual void validateElement(MessageDeclElement *node) = 0;
    virtual void validateElement(EnumDeclElement *node) = 0;
    virtual void validateElement(EnumElement *node) = 0;
    virtual void validateElement(EnumFieldsElement *node) = 0;
    virtual void validateElement(EnumFieldElement *node) = 0;
    virtual void validateElement(MessageElement *node) = 0;
    virtual void validateElement(ClassElement *node) = 0;
    virtual void validateElement(StructElement *node) = 0;
    virtual void validateElement(FieldElement *node) = 0;
    virtual void validateElement(UnknownElement *node) = 0;
    //@}
};

NAMESPACE_END

#endif

