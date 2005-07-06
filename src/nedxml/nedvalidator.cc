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

void  NEDValidatorBase::validate(NEDElement *node)
{
    validateElement(node);
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        validate(child);
}

void  NEDValidatorBase::validateElement(NEDElement *node)
{
    switch (node->getTagCode()) {
        case NED_NED_FILES: validateElement((NedFilesNode *) node); break;
        case NED_NED_FILE: validateElement((NedFileNode *) node); break;
        case NED_IMPORT: validateElement((ImportNode *) node); break;
        case NED_IMPORTED_FILE: validateElement((ImportedFileNode *) node); break;
        case NED_CHANNEL: validateElement((ChannelNode *) node); break;
        case NED_CHANNEL_ATTR: validateElement((ChannelAttrNode *) node); break;
        case NED_NETWORK: validateElement((NetworkNode *) node); break;
        case NED_SIMPLE_MODULE: validateElement((SimpleModuleNode *) node); break;
        case NED_COMPOUND_MODULE: validateElement((CompoundModuleNode *) node); break;
        case NED_PARAMS: validateElement((ParamsNode *) node); break;
        case NED_PARAM: validateElement((ParamNode *) node); break;
        case NED_GATES: validateElement((GatesNode *) node); break;
        case NED_GATE: validateElement((GateNode *) node); break;
        case NED_MACHINES: validateElement((MachinesNode *) node); break;
        case NED_MACHINE: validateElement((MachineNode *) node); break;
        case NED_SUBMODULES: validateElement((SubmodulesNode *) node); break;
        case NED_SUBMODULE: validateElement((SubmoduleNode *) node); break;
        case NED_SUBSTPARAMS: validateElement((SubstparamsNode *) node); break;
        case NED_SUBSTPARAM: validateElement((SubstparamNode *) node); break;
        case NED_GATESIZES: validateElement((GatesizesNode *) node); break;
        case NED_GATESIZE: validateElement((GatesizeNode *) node); break;
        case NED_SUBSTMACHINES: validateElement((SubstmachinesNode *) node); break;
        case NED_SUBSTMACHINE: validateElement((SubstmachineNode *) node); break;
        case NED_CONNECTIONS: validateElement((ConnectionsNode *) node); break;
        case NED_CONNECTION: validateElement((ConnectionNode *) node); break;
        case NED_CONN_ATTR: validateElement((ConnAttrNode *) node); break;
        case NED_FOR_LOOP: validateElement((ForLoopNode *) node); break;
        case NED_LOOP_VAR: validateElement((LoopVarNode *) node); break;
        case NED_DISPLAY_STRING: validateElement((DisplayStringNode *) node); break;
        case NED_EXPRESSION: validateElement((ExpressionNode *) node); break;
        case NED_OPERATOR: validateElement((OperatorNode *) node); break;
        case NED_FUNCTION: validateElement((FunctionNode *) node); break;
        case NED_PARAM_REF: validateElement((ParamRefNode *) node); break;
        case NED_IDENT: validateElement((IdentNode *) node); break;
        case NED_CONST: validateElement((ConstNode *) node); break;
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
        case NED_FIELDS: validateElement((FieldsNode *) node); break;
        case NED_FIELD: validateElement((FieldNode *) node); break;
        case NED_PROPERTIES: validateElement((PropertiesNode *) node); break;
        case NED_PROPERTY: validateElement((PropertyNode *) node); break;
        case NED_UNKNOWN: validateElement((UnknownNode *) node); break;
        default: INTERNAL_ERROR1(node,"validateElement(): unknown tag '%s'", node->getTagName());
    }
}

