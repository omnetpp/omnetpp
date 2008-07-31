//==========================================================================
// nedsyntaxvalidator.h -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NEDSyntaxValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __NEDSYNTAXVALIDATOR_H
#define __NEDSYNTAXVALIDATOR_H

#include "nedvalidator.h"

NAMESPACE_BEGIN


/**
 * Performs structural validation of components. Assumes the tree already
 * passed DTD validation. Every compound module and other component is
 * viewed in isolation: syntax validation does NOT process imports or
 * examines validity of references to other module types, channel types, etc.
 *
 * @ingroup Validation
 */
class NEDXML_API NEDSyntaxValidator : public NEDValidatorBase
{
  protected:
    bool parsedExpressions;

    // internal helpers: verify attributes
    void checkExpressionAttributes(NEDElement *node, const char *attrs[], bool optional[], int n);
    void checkDottedNameAttribute(NEDElement *node, const char *attr, bool wildcardsAllowed);

  public:
    /**
     * Constructor. Bool argument specifies if the validator should expect
     * parsed or unparsed expressions in the tree (This affects the validation
     * process.)
     */
    NEDSyntaxValidator(bool parsedExpr, NEDErrorStore *e)
        : NEDValidatorBase(e) {parsedExpressions=parsedExpr;}

    /**
     * Destructor.
     */
    virtual ~NEDSyntaxValidator() {}

  protected:
    /** @name Validator methods */
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
    virtual void validateElement(PatternElement *node);
    virtual void validateElement(PropertyElement *node);
    virtual void validateElement(PropertyKeyElement *node);
    virtual void validateElement(GatesElement *node);
    virtual void validateElement(GateElement *node);
    virtual void validateElement(TypesElement *node);
    virtual void validateElement(SubmodulesElement *node);
    virtual void validateElement(SubmoduleElement *node);
    virtual void validateElement(ConnectionsElement *node);
    virtual void validateElement(ConnectionElement *node);
    virtual void validateElement(ChannelSpecElement *node);
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

