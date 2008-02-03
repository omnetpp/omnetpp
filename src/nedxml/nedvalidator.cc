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
#include "nedvalidator.h"

NAMESPACE_BEGIN

void  NEDValidatorBase::validate(NEDElement *node)
{
    validateElement(node);
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        validate(child);
}

void  NEDValidatorBase::validateElement(NEDElement *node)
{
    try {
        switch (node->getTagCode()) {
            case NED_FILES: validateElement((FilesNode *) node); break;
            case NED_NED_FILE: validateElement((NedFileNode *) node); break;
            case NED_COMMENT: validateElement((CommentNode *) node); break;
            case NED_PACKAGE: validateElement((PackageNode *) node); break;
            case NED_IMPORT: validateElement((ImportNode *) node); break;
            case NED_PROPERTY_DECL: validateElement((PropertyDeclNode *) node); break;
            case NED_EXTENDS: validateElement((ExtendsNode *) node); break;
            case NED_INTERFACE_NAME: validateElement((InterfaceNameNode *) node); break;
            case NED_SIMPLE_MODULE: validateElement((SimpleModuleNode *) node); break;
            case NED_MODULE_INTERFACE: validateElement((ModuleInterfaceNode *) node); break;
            case NED_COMPOUND_MODULE: validateElement((CompoundModuleNode *) node); break;
            case NED_CHANNEL_INTERFACE: validateElement((ChannelInterfaceNode *) node); break;
            case NED_CHANNEL: validateElement((ChannelNode *) node); break;
            case NED_PARAMETERS: validateElement((ParametersNode *) node); break;
            case NED_PARAM: validateElement((ParamNode *) node); break;
            case NED_PATTERN: validateElement((PatternNode *) node); break;
            case NED_PROPERTY: validateElement((PropertyNode *) node); break;
            case NED_PROPERTY_KEY: validateElement((PropertyKeyNode *) node); break;
            case NED_GATES: validateElement((GatesNode *) node); break;
            case NED_GATE: validateElement((GateNode *) node); break;
            case NED_TYPES: validateElement((TypesNode *) node); break;
            case NED_SUBMODULES: validateElement((SubmodulesNode *) node); break;
            case NED_SUBMODULE: validateElement((SubmoduleNode *) node); break;
            case NED_CONNECTIONS: validateElement((ConnectionsNode *) node); break;
            case NED_CONNECTION: validateElement((ConnectionNode *) node); break;
            case NED_CHANNEL_SPEC: validateElement((ChannelSpecNode *) node); break;
            case NED_CONNECTION_GROUP: validateElement((ConnectionGroupNode *) node); break;
            case NED_LOOP: validateElement((LoopNode *) node); break;
            case NED_CONDITION: validateElement((ConditionNode *) node); break;
            case NED_EXPRESSION: validateElement((ExpressionNode *) node); break;
            case NED_OPERATOR: validateElement((OperatorNode *) node); break;
            case NED_FUNCTION: validateElement((FunctionNode *) node); break;
            case NED_IDENT: validateElement((IdentNode *) node); break;
            case NED_LITERAL: validateElement((LiteralNode *) node); break;
            case NED_MSG_FILE: validateElement((MsgFileNode *) node); break;
            case NED_NAMESPACE: validateElement((NamespaceNode *) node); break;
            case NED_CPLUSPLUS: validateElement((CplusplusNode *) node); break;
            case NED_STRUCT_DECL: validateElement((StructDeclNode *) node); break;
            case NED_CLASS_DECL: validateElement((ClassDeclNode *) node); break;
            case NED_MESSAGE_DECL: validateElement((MessageDeclNode *) node); break;
            case NED_ENUM_DECL: validateElement((EnumDeclNode *) node); break;
            case NED_ENUM: validateElement((EnumNode *) node); break;
            case NED_ENUM_FIELDS: validateElement((EnumFieldsNode *) node); break;
            case NED_ENUM_FIELD: validateElement((EnumFieldNode *) node); break;
            case NED_MESSAGE: validateElement((MessageNode *) node); break;
            case NED_CLASS: validateElement((ClassNode *) node); break;
            case NED_STRUCT: validateElement((StructNode *) node); break;
            case NED_FIELD: validateElement((FieldNode *) node); break;
            case NED_UNKNOWN: validateElement((UnknownNode *) node); break;
            default: INTERNAL_ERROR1(node,"validateElement(): unknown tag '%s'", node->getTagName());
        }
    }
    catch (NEDException& e)
    {
        INTERNAL_ERROR1(node,"validateElement(): NEDException: %s", e.what());
    }
}

NAMESPACE_END

