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
            case NED_FILES: validateElement((FilesElement *) node); break;
            case NED_NED_FILE: validateElement((NedFileElement *) node); break;
            case NED_COMMENT: validateElement((CommentElement *) node); break;
            case NED_PACKAGE: validateElement((PackageElement *) node); break;
            case NED_IMPORT: validateElement((ImportElement *) node); break;
            case NED_PROPERTY_DECL: validateElement((PropertyDeclElement *) node); break;
            case NED_EXTENDS: validateElement((ExtendsElement *) node); break;
            case NED_INTERFACE_NAME: validateElement((InterfaceNameElement *) node); break;
            case NED_SIMPLE_MODULE: validateElement((SimpleModuleElement *) node); break;
            case NED_MODULE_INTERFACE: validateElement((ModuleInterfaceElement *) node); break;
            case NED_COMPOUND_MODULE: validateElement((CompoundModuleElement *) node); break;
            case NED_CHANNEL_INTERFACE: validateElement((ChannelInterfaceElement *) node); break;
            case NED_CHANNEL: validateElement((ChannelElement *) node); break;
            case NED_PARAMETERS: validateElement((ParametersElement *) node); break;
            case NED_PARAM: validateElement((ParamElement *) node); break;
            case NED_PATTERN: validateElement((PatternElement *) node); break;
            case NED_PROPERTY: validateElement((PropertyElement *) node); break;
            case NED_PROPERTY_KEY: validateElement((PropertyKeyElement *) node); break;
            case NED_GATES: validateElement((GatesElement *) node); break;
            case NED_GATE: validateElement((GateElement *) node); break;
            case NED_TYPES: validateElement((TypesElement *) node); break;
            case NED_SUBMODULES: validateElement((SubmodulesElement *) node); break;
            case NED_SUBMODULE: validateElement((SubmoduleElement *) node); break;
            case NED_CONNECTIONS: validateElement((ConnectionsElement *) node); break;
            case NED_CONNECTION: validateElement((ConnectionElement *) node); break;
            case NED_CHANNEL_SPEC: validateElement((ChannelSpecElement *) node); break;
            case NED_CONNECTION_GROUP: validateElement((ConnectionGroupElement *) node); break;
            case NED_LOOP: validateElement((LoopElement *) node); break;
            case NED_CONDITION: validateElement((ConditionElement *) node); break;
            case NED_EXPRESSION: validateElement((ExpressionElement *) node); break;
            case NED_OPERATOR: validateElement((OperatorElement *) node); break;
            case NED_FUNCTION: validateElement((FunctionElement *) node); break;
            case NED_IDENT: validateElement((IdentElement *) node); break;
            case NED_LITERAL: validateElement((LiteralElement *) node); break;
            case NED_MSG_FILE: validateElement((MsgFileElement *) node); break;
            case NED_NAMESPACE: validateElement((NamespaceElement *) node); break;
            case NED_CPLUSPLUS: validateElement((CplusplusElement *) node); break;
            case NED_STRUCT_DECL: validateElement((StructDeclElement *) node); break;
            case NED_CLASS_DECL: validateElement((ClassDeclElement *) node); break;
            case NED_MESSAGE_DECL: validateElement((MessageDeclElement *) node); break;
            case NED_ENUM_DECL: validateElement((EnumDeclElement *) node); break;
            case NED_ENUM: validateElement((EnumElement *) node); break;
            case NED_ENUM_FIELDS: validateElement((EnumFieldsElement *) node); break;
            case NED_ENUM_FIELD: validateElement((EnumFieldElement *) node); break;
            case NED_MESSAGE: validateElement((MessageElement *) node); break;
            case NED_CLASS: validateElement((ClassElement *) node); break;
            case NED_STRUCT: validateElement((StructElement *) node); break;
            case NED_FIELD: validateElement((FieldElement *) node); break;
            case NED_UNKNOWN: validateElement((UnknownElement *) node); break;
            default: INTERNAL_ERROR1(node,"validateElement(): unknown tag '%s'", node->getTagName());
        }
    }
    catch (NEDException& e)
    {
        INTERNAL_ERROR1(node,"validateElement(): NEDException: %s", e.what());
    }
}

NAMESPACE_END

