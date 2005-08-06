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


#include <stdio.h>
#include "nederror.h"
#include "neddtdvalidator.h"

void NEDDTDValidator::validateElement(NedFilesNode *node)
{
    int tags[] = {NED_NED_FILES,NED_NED_FILE};
    checkChoice(node, tags, sizeof(tags)/sizeof(int), '*');

}

void NEDDTDValidator::validateElement(NedFileNode *node)
{
    int tags[] = {NED_IMPORT,NED_CHANNEL,NED_SIMPLE_MODULE,NED_COMPOUND_MODULE,NED_NETWORK,NED_CPLUSPLUS,NED_STRUCT_DECL,NED_CLASS_DECL,NED_MESSAGE_DECL,NED_ENUM_DECL,NED_ENUM,NED_MESSAGE,NED_CLASS,NED_STRUCT};
    checkChoice(node, tags, sizeof(tags)/sizeof(int), '*');

    checkCommentAttribute(node, "banner-comment");
}

void NEDDTDValidator::validateElement(ImportNode *node)
{
    int tags[] = {NED_IMPORTED_FILE};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(ImportedFileNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "filename");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(ChannelNode *node)
{
    int tags[] = {NED_CHANNEL_ATTR,NED_DISPLAY_STRING};
    char mult[] = {'*','?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(ChannelAttrNode *node)
{
    int tags[] = {NED_EXPRESSION};
    char mult[] = {'?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(NetworkNode *node)
{
    int tags[] = {NED_SUBSTMACHINES,NED_SUBSTPARAMS};
    char mult[] = {'?','?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkRequiredAttribute(node, "type-name");
    checkNameAttribute(node, "type-name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(SimpleModuleNode *node)
{
    int tags[] = {NED_MACHINES,NED_PARAMS,NED_GATES,NED_DISPLAY_STRING};
    char mult[] = {'?','?','?','?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(CompoundModuleNode *node)
{
    int tags[] = {NED_MACHINES,NED_PARAMS,NED_GATES,NED_SUBMODULES,NED_CONNECTIONS,NED_DISPLAY_STRING};
    char mult[] = {'?','?','?','?','?','?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(ParamsNode *node)
{
    int tags[] = {NED_PARAM};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(ParamNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(GatesNode *node)
{
    int tags[] = {NED_GATE};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(GateNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkRequiredAttribute(node, "direction");
    const char *vals1[] = {"input","output"};
    checkEnumeratedAttribute(node, "direction", vals1, sizeof(vals1)/sizeof(const char *));
    const char *vals2[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-vector", vals2, sizeof(vals2)/sizeof(const char *));
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(MachinesNode *node)
{
    int tags[] = {NED_MACHINE};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(MachineNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(SubmodulesNode *node)
{
    int tags[] = {NED_SUBMODULE};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(SubmoduleNode *node)
{
    int tags[] = {NED_EXPRESSION,NED_SUBSTMACHINES,NED_SUBSTPARAMS,NED_GATESIZES,NED_DISPLAY_STRING};
    char mult[] = {'?','*','*','*','?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkRequiredAttribute(node, "type-name");
    checkNameAttribute(node, "type-name");
    checkNMTokenAttribute(node, "like-param");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(SubstparamsNode *node)
{
    int tags[] = {NED_EXPRESSION,NED_SUBSTPARAM};
    char mult[] = {'?','*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(SubstparamNode *node)
{
    int tags[] = {NED_EXPRESSION};
    char mult[] = {'?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(GatesizesNode *node)
{
    int tags[] = {NED_EXPRESSION,NED_GATESIZE};
    char mult[] = {'?','*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(GatesizeNode *node)
{
    int tags[] = {NED_EXPRESSION};
    char mult[] = {'?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(SubstmachinesNode *node)
{
    int tags[] = {NED_EXPRESSION,NED_SUBSTMACHINE};
    char mult[] = {'?','*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(SubstmachineNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(ConnectionsNode *node)
{
    int tags[] = {NED_CONNECTION,NED_FOR_LOOP};
    checkChoice(node, tags, sizeof(tags)/sizeof(int), '*');

    const char *vals0[] = {"true","false"};
    checkEnumeratedAttribute(node, "check-unconnected", vals0, sizeof(vals0)/sizeof(const char *));
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(ConnectionNode *node)
{
    int tags[] = {NED_EXPRESSION,NED_CONN_ATTR,NED_DISPLAY_STRING};
    checkChoice(node, tags, sizeof(tags)/sizeof(int), '*');

    checkNameAttribute(node, "src-module");
    checkRequiredAttribute(node, "src-gate");
    checkNameAttribute(node, "src-gate");
    const char *vals4[] = {"true","false"};
    checkEnumeratedAttribute(node, "src-gate-plusplus", vals4, sizeof(vals4)/sizeof(const char *));
    checkNameAttribute(node, "dest-module");
    checkRequiredAttribute(node, "dest-gate");
    checkNameAttribute(node, "dest-gate");
    const char *vals9[] = {"true","false"};
    checkEnumeratedAttribute(node, "dest-gate-plusplus", vals9, sizeof(vals9)/sizeof(const char *));
    const char *vals11[] = {"left","right"};
    checkEnumeratedAttribute(node, "arrow-direction", vals11, sizeof(vals11)/sizeof(const char *));
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(ConnAttrNode *node)
{
    int tags[] = {NED_EXPRESSION};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(ForLoopNode *node)
{
    int tags[] = {NED_LOOP_VAR,NED_CONNECTION};
    char mult[] = {'+','*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(LoopVarNode *node)
{
    int tags[] = {NED_EXPRESSION};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "param-name");
    checkNameAttribute(node, "param-name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(DisplayStringNode *node)
{
    checkEmpty(node);

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(ExpressionNode *node)
{
    int tags[] = {NED_OPERATOR,NED_FUNCTION,NED_PARAM_REF,NED_IDENT,NED_CONST};
    checkChoice(node, tags, sizeof(tags)/sizeof(int), '1');

}

void NEDDTDValidator::validateElement(OperatorNode *node)
{
    int tags[] = {NED_OPERATOR,NED_FUNCTION,NED_PARAM_REF,NED_IDENT,NED_CONST};
    checkChoice(node, tags, sizeof(tags)/sizeof(int), '+');

    checkRequiredAttribute(node, "name");
}

void NEDDTDValidator::validateElement(FunctionNode *node)
{
    int tags[] = {NED_OPERATOR,NED_FUNCTION,NED_PARAM_REF,NED_IDENT,NED_CONST};
    checkChoice(node, tags, sizeof(tags)/sizeof(int), '*');

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ParamRefNode *node)
{
    int tags[] = {NED_EXPRESSION};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "param-name");
    checkNameAttribute(node, "param-name");
    const char *vals4[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-ref", vals4, sizeof(vals4)/sizeof(const char *));
    const char *vals5[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-ancestor", vals5, sizeof(vals5)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(IdentNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
}

void NEDDTDValidator::validateElement(ConstNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "type");
    const char *vals0[] = {"bool","int","real","string","time"};
    checkEnumeratedAttribute(node, "type", vals0, sizeof(vals0)/sizeof(const char *));
}

void NEDDTDValidator::validateElement(CplusplusNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "body");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(StructDeclNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(ClassDeclNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    const char *vals1[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-cobject", vals1, sizeof(vals1)/sizeof(const char *));
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(MessageDeclNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(EnumDeclNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(EnumNode *node)
{
    int tags[] = {NED_ENUM_FIELDS};
    char mult[] = {'?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(EnumFieldsNode *node)
{
    int tags[] = {NED_ENUM_FIELD};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(EnumFieldNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(MessageNode *node)
{
    int tags[] = {NED_PROPERTIES,NED_FIELDS};
    char mult[] = {'?','?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(ClassNode *node)
{
    int tags[] = {NED_PROPERTIES,NED_FIELDS};
    char mult[] = {'?','?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(StructNode *node)
{
    int tags[] = {NED_PROPERTIES,NED_FIELDS};
    char mult[] = {'?','?'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkNameAttribute(node, "extends-name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
    checkCommentAttribute(node, "trailing-comment");
}

void NEDDTDValidator::validateElement(FieldsNode *node)
{
    int tags[] = {NED_FIELD};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(FieldNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    const char *vals2[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-abstract", vals2, sizeof(vals2)/sizeof(const char *));
    const char *vals3[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-readonly", vals3, sizeof(vals3)/sizeof(const char *));
    const char *vals4[] = {"true","false"};
    checkEnumeratedAttribute(node, "is-vector", vals4, sizeof(vals4)/sizeof(const char *));
    checkNameAttribute(node, "enum-name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(PropertiesNode *node)
{
    int tags[] = {NED_PROPERTY};
    char mult[] = {'*'};
    checkSequence(node, tags, mult, sizeof(tags)/sizeof(int));

    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(PropertyNode *node)
{
    checkEmpty(node);

    checkRequiredAttribute(node, "name");
    checkNameAttribute(node, "name");
    checkCommentAttribute(node, "banner-comment");
    checkCommentAttribute(node, "right-comment");
}

void NEDDTDValidator::validateElement(UnknownNode *node)
{
    // ANY

    checkRequiredAttribute(node, "element");
    checkNMTokenAttribute(node, "element");
}

