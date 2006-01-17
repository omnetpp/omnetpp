//==========================================================================
// nedbasicvalidator.h -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NEDBasicValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __NEDBASICVALIDATOR_H
#define __NEDBASICVALIDATOR_H

#include "nedvalidator.h"


/**
 * Performs structural validation of components. Assumes the tree already
 * passed DTD validation. Every compound module and other component is
 * viewed in isolation: basic validation does NOT process imports or
 * examines validity of references to other module types, channel types, etc.
 *
 * @ingroup Validation
 */
class NEDBasicValidator : public NEDValidatorBase
{
  protected:
    bool parsedExpressions;

    // internal helper: verify that children of node with given tagcode have
    // all different values of the given attribute (e.g. "name")
    void checkUniqueness(NEDElement *node, int childtype, const char *attr);

    // internal helper: verify attributes
    void checkExpressionAttributes(NEDElement *node, const char *attrs[], bool optional[], int n);

  public:
    /**
     * Constructor. Bool argument specifies if the validator should expect
     * parsed or unparsed expressions in the tree (This affects the validation
     * process.)
     */
    NEDBasicValidator(bool parsedExpr) {parsedExpressions=parsedExpr;}

    /**
     * Destructor.
     */
    virtual ~NEDBasicValidator() {}

    /** @name Validator methods */
    //@{
    virtual void validateElement(FilesNode *node) = 0;
    virtual void validateElement(NedFileNode *node) = 0;
    virtual void validateElement(WhitespaceNode *node) = 0;
    virtual void validateElement(ImportNode *node) = 0;
    virtual void validateElement(PropertydefNode *node) = 0;
    virtual void validateElement(ExtendsNode *node) = 0;
    virtual void validateElement(InterfaceNameNode *node) = 0;
    virtual void validateElement(SimpleModuleNode *node) = 0;
    virtual void validateElement(ModuleInterfaceNode *node) = 0;
    virtual void validateElement(CompoundModuleNode *node) = 0;
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

