//==========================================================================
// nedbasicvalidator.h - part of the OMNeT++ Discrete System Simulation System
//
// Contents:
//   class NEDBasicValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __NEDBASICVALIDATOR_H
#define __NEDBASICVALIDATOR_H

#include "nedvalidator.h"

/**
 * Performs structural validation of components. Assumes the tree already
 * passed DTD validation. Each compound module and other component is
 * viewed in isolation: basic validation does NOT process imports or
 * examines validity of references to other module types, channel types, etc.
 *
 * @ingroup Validation
 */
class NEDBasicValidator : public NEDValidatorBase
{
  protected:
    bool parsedExpressions;
    void checkUniqueness(NEDElement *node, int childtype, const char *attr);
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

    virtual void validateElement(NedFilesNode *node);
    virtual void validateElement(NedFileNode *node);
    virtual void validateElement(ImportNode *node);
    virtual void validateElement(ImportedFileNode *node);
    virtual void validateElement(ChannelNode *node);
    virtual void validateElement(ChannelAttrNode *node);
    virtual void validateElement(NetworkNode *node);
    virtual void validateElement(SimpleModuleNode *node);
    virtual void validateElement(CompoundModuleNode *node);
    virtual void validateElement(ParamsNode *node);
    virtual void validateElement(ParamNode *node);
    virtual void validateElement(GatesNode *node);
    virtual void validateElement(GateNode *node);
    virtual void validateElement(MachinesNode *node);
    virtual void validateElement(MachineNode *node);
    virtual void validateElement(SubmodulesNode *node);
    virtual void validateElement(SubmoduleNode *node);
    virtual void validateElement(SubstparamsNode *node);
    virtual void validateElement(SubstparamNode *node);
    virtual void validateElement(GatesizesNode *node);
    virtual void validateElement(GatesizeNode *node);
    virtual void validateElement(SubstmachinesNode *node);
    virtual void validateElement(SubstmachineNode *node);
    virtual void validateElement(ConnectionsNode *node);
    virtual void validateElement(ConnectionNode *node);
    virtual void validateElement(ConnAttrNode *node);
    virtual void validateElement(ForLoopNode *node);
    virtual void validateElement(LoopVarNode *node);
    virtual void validateElement(DisplayStringNode *node);
    virtual void validateElement(ExpressionNode *node);
    virtual void validateElement(OperatorNode *node);
    virtual void validateElement(FunctionNode *node);
    virtual void validateElement(ParamRefNode *node);
    virtual void validateElement(IdentNode *node);
    virtual void validateElement(ConstNode *node);
    virtual void validateElement(CppincludeNode *node);
    virtual void validateElement(CppStructNode *node);
    virtual void validateElement(CppCobjectNode *node);
    virtual void validateElement(CppNoncobjectNode *node);
    virtual void validateElement(EnumNode *node);
    virtual void validateElement(EnumFieldsNode *node);
    virtual void validateElement(EnumFieldNode *node);
    virtual void validateElement(MessageNode *node);
    virtual void validateElement(ClassNode *node);
    virtual void validateElement(StructNode *node);
    virtual void validateElement(FieldsNode *node);
    virtual void validateElement(FieldNode *node);
    virtual void validateElement(PropertiesNode *node);
    virtual void validateElement(PropertyNode *node);
    virtual void validateElement(UnknownNode *node);
};

#endif

