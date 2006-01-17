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


#include <string.h>
#include "nederror.h"
#include "nedelements.h"

static const char *io_vals[] = {"input", "output"};
static int io_nums[] = {NED_GATEDIR_INPUT, NED_GATEDIR_OUTPUT};
static const int io_n = 2;

static const char *lr_vals[] = {"left", "right"};
static int lr_nums[] = {NED_ARROWDIR_LEFT, NED_ARROWDIR_RIGHT};
static const int lr_n = 2;

static const char *type_vals[] = {"bool", "int", "real", "string", "time"};
static int type_nums[] = {NED_CONST_BOOL, NED_CONST_INT, NED_CONST_REAL, NED_CONST_STRING, NED_CONST_TIME};
static const int type_n = 5;


int FilesNode::getNumAttributes() const
{
    return 0;
}

const char *FilesNode::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *FilesNode::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void FilesNode::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *FilesNode::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

FilesNode *FilesNode::getNextFilesNodeSibling() const
{
    return (FilesNode *)getNextSiblingWithTag(NED_FILES);
}

NedFileNode *FilesNode::getFirstNedFileChild() const
{
    return (NedFileNode *)getFirstChildWithTag(NED_NED_FILE);
}

MsgFileNode *FilesNode::getFirstMsgFileChild() const
{
    return (MsgFileNode *)getFirstChildWithTag(NED_MSG_FILE);
}

int NedFileNode::getNumAttributes() const
{
    return 2;
}

const char *NedFileNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "filename";
        case 1: return "package";
        default: return 0;
    }
}

const char *NedFileNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return filename.c_str();
        case 1: return package.c_str();
        default: return 0;
    }
}

void NedFileNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: filename = val; break;
        case 1: package = val; break;
        default: ;
    }
}

const char *NedFileNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        default: return 0;
    }
}

NedFileNode *NedFileNode::getNextNedFileNodeSibling() const
{
    return (NedFileNode *)getNextSiblingWithTag(NED_NED_FILE);
}

WhitespaceNode *NedFileNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ImportNode *NedFileNode::getFirstImportChild() const
{
    return (ImportNode *)getFirstChildWithTag(NED_IMPORT);
}

PropertydefNode *NedFileNode::getFirstPropertydefChild() const
{
    return (PropertydefNode *)getFirstChildWithTag(NED_PROPERTYDEF);
}

PropertyNode *NedFileNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

ChannelNode *NedFileNode::getFirstChannelChild() const
{
    return (ChannelNode *)getFirstChildWithTag(NED_CHANNEL);
}

ChannelInterfaceNode *NedFileNode::getFirstChannelInterfaceChild() const
{
    return (ChannelInterfaceNode *)getFirstChildWithTag(NED_CHANNEL_INTERFACE);
}

SimpleNode *NedFileNode::getFirstSimpleChild() const
{
    return (SimpleNode *)getFirstChildWithTag(NED_SIMPLE);
}

ModuleNode *NedFileNode::getFirstModuleChild() const
{
    return (ModuleNode *)getFirstChildWithTag(NED_MODULE);
}

InterfaceNode *NedFileNode::getFirstInterfaceChild() const
{
    return (InterfaceNode *)getFirstChildWithTag(NED_INTERFACE);
}

int WhitespaceNode::getNumAttributes() const
{
    return 2;
}

const char *WhitespaceNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "locid";
        case 1: return "content";
        default: return 0;
    }
}

const char *WhitespaceNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return locid.c_str();
        case 1: return content.c_str();
        default: return 0;
    }
}

void WhitespaceNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: locid = val; break;
        case 1: content = val; break;
        default: ;
    }
}

const char *WhitespaceNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        default: return 0;
    }
}

WhitespaceNode *WhitespaceNode::getNextWhitespaceNodeSibling() const
{
    return (WhitespaceNode *)getNextSiblingWithTag(NED_WHITESPACE);
}

int ImportNode::getNumAttributes() const
{
    return 1;
}

const char *ImportNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "filename";
        default: return 0;
    }
}

const char *ImportNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return filename.c_str();
        default: return 0;
    }
}

void ImportNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: filename = val; break;
        default: ;
    }
}

const char *ImportNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

ImportNode *ImportNode::getNextImportNodeSibling() const
{
    return (ImportNode *)getNextSiblingWithTag(NED_IMPORT);
}

WhitespaceNode *ImportNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int PropertydefNode::getNumAttributes() const
{
    return 1;
}

const char *PropertydefNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *PropertydefNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void PropertydefNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *PropertydefNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

PropertydefNode *PropertydefNode::getNextPropertydefNodeSibling() const
{
    return (PropertydefNode *)getNextSiblingWithTag(NED_PROPERTYDEF);
}

WhitespaceNode *PropertydefNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

KeyValueNode *PropertydefNode::getFirstKeyValueChild() const
{
    return (KeyValueNode *)getFirstChildWithTag(NED_KEY_VALUE);
}

PropertyNode *PropertydefNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

int ExtendsNode::getNumAttributes() const
{
    return 1;
}

const char *ExtendsNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *ExtendsNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void ExtendsNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *ExtendsNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

ExtendsNode *ExtendsNode::getNextExtendsNodeSibling() const
{
    return (ExtendsNode *)getNextSiblingWithTag(NED_EXTENDS);
}

WhitespaceNode *ExtendsNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int InterfaceNameNode::getNumAttributes() const
{
    return 1;
}

const char *InterfaceNameNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *InterfaceNameNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void InterfaceNameNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *InterfaceNameNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

InterfaceNameNode *InterfaceNameNode::getNextInterfaceNameNodeSibling() const
{
    return (InterfaceNameNode *)getNextSiblingWithTag(NED_INTERFACE_NAME);
}

WhitespaceNode *InterfaceNameNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int ChannelInterfaceNameNode::getNumAttributes() const
{
    return 1;
}

const char *ChannelInterfaceNameNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *ChannelInterfaceNameNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void ChannelInterfaceNameNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *ChannelInterfaceNameNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

ChannelInterfaceNameNode *ChannelInterfaceNameNode::getNextChannelInterfaceNameNodeSibling() const
{
    return (ChannelInterfaceNameNode *)getNextSiblingWithTag(NED_CHANNEL_INTERFACE_NAME);
}

WhitespaceNode *ChannelInterfaceNameNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int SimpleNode::getNumAttributes() const
{
    return 1;
}

const char *SimpleNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *SimpleNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void SimpleNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *SimpleNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

SimpleNode *SimpleNode::getNextSimpleNodeSibling() const
{
    return (SimpleNode *)getNextSiblingWithTag(NED_SIMPLE);
}

WhitespaceNode *SimpleNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExtendsNode *SimpleNode::getFirstExtendsChild() const
{
    return (ExtendsNode *)getFirstChildWithTag(NED_EXTENDS);
}

InterfaceNameNode *SimpleNode::getFirstInterfaceNameChild() const
{
    return (InterfaceNameNode *)getFirstChildWithTag(NED_INTERFACE_NAME);
}

ParametersNode *SimpleNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

GatesNode *SimpleNode::getFirstGatesChild() const
{
    return (GatesNode *)getFirstChildWithTag(NED_GATES);
}

int InterfaceNode::getNumAttributes() const
{
    return 1;
}

const char *InterfaceNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *InterfaceNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void InterfaceNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *InterfaceNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

InterfaceNode *InterfaceNode::getNextInterfaceNodeSibling() const
{
    return (InterfaceNode *)getNextSiblingWithTag(NED_INTERFACE);
}

WhitespaceNode *InterfaceNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExtendsNode *InterfaceNode::getFirstExtendsChild() const
{
    return (ExtendsNode *)getFirstChildWithTag(NED_EXTENDS);
}

ParametersNode *InterfaceNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

GatesNode *InterfaceNode::getFirstGatesChild() const
{
    return (GatesNode *)getFirstChildWithTag(NED_GATES);
}

int ModuleNode::getNumAttributes() const
{
    return 1;
}

const char *ModuleNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *ModuleNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void ModuleNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *ModuleNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

ModuleNode *ModuleNode::getNextModuleNodeSibling() const
{
    return (ModuleNode *)getNextSiblingWithTag(NED_MODULE);
}

WhitespaceNode *ModuleNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExtendsNode *ModuleNode::getFirstExtendsChild() const
{
    return (ExtendsNode *)getFirstChildWithTag(NED_EXTENDS);
}

InterfaceNameNode *ModuleNode::getFirstInterfaceNameChild() const
{
    return (InterfaceNameNode *)getFirstChildWithTag(NED_INTERFACE_NAME);
}

ParametersNode *ModuleNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

GatesNode *ModuleNode::getFirstGatesChild() const
{
    return (GatesNode *)getFirstChildWithTag(NED_GATES);
}

SubmodulesNode *ModuleNode::getFirstSubmodulesChild() const
{
    return (SubmodulesNode *)getFirstChildWithTag(NED_SUBMODULES);
}

ConnectionsNode *ModuleNode::getFirstConnectionsChild() const
{
    return (ConnectionsNode *)getFirstChildWithTag(NED_CONNECTIONS);
}

int ParametersNode::getNumAttributes() const
{
    return 1;
}

const char *ParametersNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "is-implicit";
        default: return 0;
    }
}

const char *ParametersNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return boolToString(isImplicit);
        default: return 0;
    }
}

void ParametersNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: isImplicit = stringToBool(val); break;
        default: ;
    }
}

const char *ParametersNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "true";
        default: return 0;
    }
}

ParametersNode *ParametersNode::getNextParametersNodeSibling() const
{
    return (ParametersNode *)getNextSiblingWithTag(NED_PARAMETERS);
}

WhitespaceNode *ParametersNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

PropertyNode *ParametersNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

ParamNode *ParametersNode::getFirstParamChild() const
{
    return (ParamNode *)getFirstChildWithTag(NED_PARAM);
}

ParamGroupNode *ParametersNode::getFirstParamGroupChild() const
{
    return (ParamGroupNode *)getFirstChildWithTag(NED_PARAM_GROUP);
}

int ParamGroupNode::getNumAttributes() const
{
    return 0;
}

const char *ParamGroupNode::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *ParamGroupNode::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void ParamGroupNode::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *ParamGroupNode::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

ParamGroupNode *ParamGroupNode::getNextParamGroupNodeSibling() const
{
    return (ParamGroupNode *)getNextSiblingWithTag(NED_PARAM_GROUP);
}

WhitespaceNode *ParamGroupNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ConditionNode *ParamGroupNode::getFirstConditionChild() const
{
    return (ConditionNode *)getFirstChildWithTag(NED_CONDITION);
}

PropertyNode *ParamGroupNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

ParamNode *ParamGroupNode::getFirstParamChild() const
{
    return (ParamNode *)getFirstChildWithTag(NED_PARAM);
}

int ParamNode::getNumAttributes() const
{
    return 5;
}

const char *ParamNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "type";
        case 1: return "is-function";
        case 2: return "qualifier";
        case 3: return "name";
        case 4: return "value";
        default: return 0;
    }
}

const char *ParamNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return enumToString(type, partype_vals, partype_nums, partype_n);
        case 1: return boolToString(isFunction);
        case 2: return qualifier.c_str();
        case 3: return name.c_str();
        case 4: return value.c_str();
        default: return 0;
    }
}

void ParamNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: type = stringToEnum(val, partype_vals, partype_nums, partype_n); break;
        case 1: isFunction = stringToBool(val); break;
        case 2: qualifier = val; break;
        case 3: name = val; break;
        case 4: value = val; break;
        default: ;
    }
}

const char *ParamNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "false";
        case 2: return "";
        case 3: return "";
        case 4: return "";
        default: return 0;
    }
}

ParamNode *ParamNode::getNextParamNodeSibling() const
{
    return (ParamNode *)getNextSiblingWithTag(NED_PARAM);
}

WhitespaceNode *ParamNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExpressionNode *ParamNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

PropertyNode *ParamNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

int PropertyNode::getNumAttributes() const
{
    return 2;
}

const char *PropertyNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "qualifier";
        case 1: return "name";
        default: return 0;
    }
}

const char *PropertyNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return qualifier.c_str();
        case 1: return name.c_str();
        default: return 0;
    }
}

void PropertyNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: qualifier = val; break;
        case 1: name = val; break;
        default: ;
    }
}

const char *PropertyNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        default: return 0;
    }
}

PropertyNode *PropertyNode::getNextPropertyNodeSibling() const
{
    return (PropertyNode *)getNextSiblingWithTag(NED_PROPERTY);
}

WhitespaceNode *PropertyNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

KeyValueNode *PropertyNode::getFirstKeyValueChild() const
{
    return (KeyValueNode *)getFirstChildWithTag(NED_KEY_VALUE);
}

PropertyNode *PropertyNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

int KeyValueNode::getNumAttributes() const
{
    return 2;
}

const char *KeyValueNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "key";
        case 1: return "value";
        default: return 0;
    }
}

const char *KeyValueNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return key.c_str();
        case 1: return value.c_str();
        default: return 0;
    }
}

void KeyValueNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: key = val; break;
        case 1: value = val; break;
        default: ;
    }
}

const char *KeyValueNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        default: return 0;
    }
}

KeyValueNode *KeyValueNode::getNextKeyValueNodeSibling() const
{
    return (KeyValueNode *)getNextSiblingWithTag(NED_KEY_VALUE);
}

WhitespaceNode *KeyValueNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int GatesNode::getNumAttributes() const
{
    return 0;
}

const char *GatesNode::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *GatesNode::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void GatesNode::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *GatesNode::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

GatesNode *GatesNode::getNextGatesNodeSibling() const
{
    return (GatesNode *)getNextSiblingWithTag(NED_GATES);
}

WhitespaceNode *GatesNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

GateNode *GatesNode::getFirstGateChild() const
{
    return (GateNode *)getFirstChildWithTag(NED_GATE);
}

GateGroupNode *GatesNode::getFirstGateGroupChild() const
{
    return (GateGroupNode *)getFirstChildWithTag(NED_GATE_GROUP);
}

int GateGroupNode::getNumAttributes() const
{
    return 0;
}

const char *GateGroupNode::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *GateGroupNode::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void GateGroupNode::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *GateGroupNode::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

GateGroupNode *GateGroupNode::getNextGateGroupNodeSibling() const
{
    return (GateGroupNode *)getNextSiblingWithTag(NED_GATE_GROUP);
}

WhitespaceNode *GateGroupNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ConditionNode *GateGroupNode::getFirstConditionChild() const
{
    return (ConditionNode *)getFirstChildWithTag(NED_CONDITION);
}

GateNode *GateGroupNode::getFirstGateChild() const
{
    return (GateNode *)getFirstChildWithTag(NED_GATE);
}

int GateNode::getNumAttributes() const
{
    return 3;
}

const char *GateNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "type";
        case 2: return "vector-size";
        default: return 0;
    }
}

const char *GateNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return enumToString(type, io_vals, io_nums, io_n);
        case 2: return vectorSize.c_str();
        default: return 0;
    }
}

void GateNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: type = stringToEnum(val, io_vals, io_nums, io_n); break;
        case 2: vectorSize = val; break;
        default: ;
    }
}

const char *GateNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

GateNode *GateNode::getNextGateNodeSibling() const
{
    return (GateNode *)getNextSiblingWithTag(NED_GATE);
}

WhitespaceNode *GateNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExpressionNode *GateNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

PropertyNode *GateNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

int SubmodulesNode::getNumAttributes() const
{
    return 0;
}

const char *SubmodulesNode::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *SubmodulesNode::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void SubmodulesNode::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *SubmodulesNode::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

SubmodulesNode *SubmodulesNode::getNextSubmodulesNodeSibling() const
{
    return (SubmodulesNode *)getNextSiblingWithTag(NED_SUBMODULES);
}

WhitespaceNode *SubmodulesNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

SubmoduleNode *SubmodulesNode::getFirstSubmoduleChild() const
{
    return (SubmoduleNode *)getFirstChildWithTag(NED_SUBMODULE);
}

int SubmoduleNode::getNumAttributes() const
{
    return 5;
}

const char *SubmoduleNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "type";
        case 2: return "like-type";
        case 3: return "like-param";
        case 4: return "vector-size";
        default: return 0;
    }
}

const char *SubmoduleNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return type.c_str();
        case 2: return likeType.c_str();
        case 3: return likeParam.c_str();
        case 4: return vectorSize.c_str();
        default: return 0;
    }
}

void SubmoduleNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: type = val; break;
        case 2: likeType = val; break;
        case 3: likeParam = val; break;
        case 4: vectorSize = val; break;
        default: ;
    }
}

const char *SubmoduleNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "";
        case 4: return "";
        default: return 0;
    }
}

SubmoduleNode *SubmoduleNode::getNextSubmoduleNodeSibling() const
{
    return (SubmoduleNode *)getNextSiblingWithTag(NED_SUBMODULE);
}

WhitespaceNode *SubmoduleNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExpressionNode *SubmoduleNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

ParametersNode *SubmoduleNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

GatesNode *SubmoduleNode::getFirstGatesChild() const
{
    return (GatesNode *)getFirstChildWithTag(NED_GATES);
}

int ConnectionsNode::getNumAttributes() const
{
    return 1;
}

const char *ConnectionsNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "check-unconnected";
        default: return 0;
    }
}

const char *ConnectionsNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return boolToString(checkUnconnected);
        default: return 0;
    }
}

void ConnectionsNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: checkUnconnected = stringToBool(val); break;
        default: ;
    }
}

const char *ConnectionsNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "true";
        default: return 0;
    }
}

ConnectionsNode *ConnectionsNode::getNextConnectionsNodeSibling() const
{
    return (ConnectionsNode *)getNextSiblingWithTag(NED_CONNECTIONS);
}

WhitespaceNode *ConnectionsNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ConnectionNode *ConnectionsNode::getFirstConnectionChild() const
{
    return (ConnectionNode *)getFirstChildWithTag(NED_CONNECTION);
}

ConnectionGroupNode *ConnectionsNode::getFirstConnectionGroupChild() const
{
    return (ConnectionGroupNode *)getFirstChildWithTag(NED_CONNECTION_GROUP);
}

int ConnectionNode::getNumAttributes() const
{
    return 12;
}

const char *ConnectionNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "channel-name";
        case 1: return "src-module";
        case 2: return "src-module-index";
        case 3: return "src-gate";
        case 4: return "src-gate-plusplus";
        case 5: return "src-gate-index";
        case 6: return "dest-module";
        case 7: return "dest-module-index";
        case 8: return "dest-gate";
        case 9: return "dest-gate-plusplus";
        case 10: return "dest-gate-index";
        case 11: return "is-left-to-right";
        default: return 0;
    }
}

const char *ConnectionNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return channelName.c_str();
        case 1: return srcModule.c_str();
        case 2: return srcModuleIndex.c_str();
        case 3: return srcGate.c_str();
        case 4: return boolToString(srcGatePlusplus);
        case 5: return srcGateIndex.c_str();
        case 6: return destModule.c_str();
        case 7: return destModuleIndex.c_str();
        case 8: return destGate.c_str();
        case 9: return boolToString(destGatePlusplus);
        case 10: return destGateIndex.c_str();
        case 11: return boolToString(isLeftToRight);
        default: return 0;
    }
}

void ConnectionNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: channelName = val; break;
        case 1: srcModule = val; break;
        case 2: srcModuleIndex = val; break;
        case 3: srcGate = val; break;
        case 4: srcGatePlusplus = stringToBool(val); break;
        case 5: srcGateIndex = val; break;
        case 6: destModule = val; break;
        case 7: destModuleIndex = val; break;
        case 8: destGate = val; break;
        case 9: destGatePlusplus = stringToBool(val); break;
        case 10: destGateIndex = val; break;
        case 11: isLeftToRight = stringToBool(val); break;
        default: ;
    }
}

const char *ConnectionNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "";
        case 4: return "false";
        case 5: return "";
        case 6: return "";
        case 7: return "";
        case 8: return "";
        case 9: return "false";
        case 10: return "";
        case 11: return "";
        default: return 0;
    }
}

ConnectionNode *ConnectionNode::getNextConnectionNodeSibling() const
{
    return (ConnectionNode *)getNextSiblingWithTag(NED_CONNECTION);
}

WhitespaceNode *ConnectionNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExpressionNode *ConnectionNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

ParametersNode *ConnectionNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

int ChannelInterfaceNode::getNumAttributes() const
{
    return 1;
}

const char *ChannelInterfaceNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *ChannelInterfaceNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void ChannelInterfaceNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *ChannelInterfaceNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

ChannelInterfaceNode *ChannelInterfaceNode::getNextChannelInterfaceNodeSibling() const
{
    return (ChannelInterfaceNode *)getNextSiblingWithTag(NED_CHANNEL_INTERFACE);
}

WhitespaceNode *ChannelInterfaceNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExtendsNode *ChannelInterfaceNode::getFirstExtendsChild() const
{
    return (ExtendsNode *)getFirstChildWithTag(NED_EXTENDS);
}

ParametersNode *ChannelInterfaceNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

int ChannelNode::getNumAttributes() const
{
    return 1;
}

const char *ChannelNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *ChannelNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void ChannelNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *ChannelNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

ChannelNode *ChannelNode::getNextChannelNodeSibling() const
{
    return (ChannelNode *)getNextSiblingWithTag(NED_CHANNEL);
}

WhitespaceNode *ChannelNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExtendsNode *ChannelNode::getFirstExtendsChild() const
{
    return (ExtendsNode *)getFirstChildWithTag(NED_EXTENDS);
}

ChannelInterfaceNameNode *ChannelNode::getFirstChannelInterfaceNameChild() const
{
    return (ChannelInterfaceNameNode *)getFirstChildWithTag(NED_CHANNEL_INTERFACE_NAME);
}

ParametersNode *ChannelNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

int ConnectionGroupNode::getNumAttributes() const
{
    return 0;
}

const char *ConnectionGroupNode::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *ConnectionGroupNode::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void ConnectionGroupNode::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *ConnectionGroupNode::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

ConnectionGroupNode *ConnectionGroupNode::getNextConnectionGroupNodeSibling() const
{
    return (ConnectionGroupNode *)getNextSiblingWithTag(NED_CONNECTION_GROUP);
}

WhitespaceNode *ConnectionGroupNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

LoopNode *ConnectionGroupNode::getFirstLoopChild() const
{
    return (LoopNode *)getFirstChildWithTag(NED_LOOP);
}

ConditionNode *ConnectionGroupNode::getFirstConditionChild() const
{
    return (ConditionNode *)getFirstChildWithTag(NED_CONDITION);
}

ConnectionNode *ConnectionGroupNode::getFirstConnectionChild() const
{
    return (ConnectionNode *)getFirstChildWithTag(NED_CONNECTION);
}

int LoopNode::getNumAttributes() const
{
    return 3;
}

const char *LoopNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "param-name";
        case 1: return "from-value";
        case 2: return "to-value";
        default: return 0;
    }
}

const char *LoopNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return paramName.c_str();
        case 1: return fromValue.c_str();
        case 2: return toValue.c_str();
        default: return 0;
    }
}

void LoopNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: paramName = val; break;
        case 1: fromValue = val; break;
        case 2: toValue = val; break;
        default: ;
    }
}

const char *LoopNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

LoopNode *LoopNode::getNextLoopNodeSibling() const
{
    return (LoopNode *)getNextSiblingWithTag(NED_LOOP);
}

WhitespaceNode *LoopNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExpressionNode *LoopNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

int ConditionNode::getNumAttributes() const
{
    return 1;
}

const char *ConditionNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "condition";
        default: return 0;
    }
}

const char *ConditionNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return condition.c_str();
        default: return 0;
    }
}

void ConditionNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: condition = val; break;
        default: ;
    }
}

const char *ConditionNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

ConditionNode *ConditionNode::getNextConditionNodeSibling() const
{
    return (ConditionNode *)getNextSiblingWithTag(NED_CONDITION);
}

WhitespaceNode *ConditionNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExpressionNode *ConditionNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

int ExpressionNode::getNumAttributes() const
{
    return 1;
}

const char *ExpressionNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "target";
        default: return 0;
    }
}

const char *ExpressionNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return target.c_str();
        default: return 0;
    }
}

void ExpressionNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: target = val; break;
        default: ;
    }
}

const char *ExpressionNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

ExpressionNode *ExpressionNode::getNextExpressionNodeSibling() const
{
    return (ExpressionNode *)getNextSiblingWithTag(NED_EXPRESSION);
}

WhitespaceNode *ExpressionNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

OperatorNode *ExpressionNode::getFirstOperatorChild() const
{
    return (OperatorNode *)getFirstChildWithTag(NED_OPERATOR);
}

FunctionNode *ExpressionNode::getFirstFunctionChild() const
{
    return (FunctionNode *)getFirstChildWithTag(NED_FUNCTION);
}

RefNode *ExpressionNode::getFirstRefChild() const
{
    return (RefNode *)getFirstChildWithTag(NED_REF);
}

ConstNode *ExpressionNode::getFirstConstChild() const
{
    return (ConstNode *)getFirstChildWithTag(NED_CONST);
}

int OperatorNode::getNumAttributes() const
{
    return 1;
}

const char *OperatorNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *OperatorNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void OperatorNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *OperatorNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

OperatorNode *OperatorNode::getNextOperatorNodeSibling() const
{
    return (OperatorNode *)getNextSiblingWithTag(NED_OPERATOR);
}

WhitespaceNode *OperatorNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

OperatorNode *OperatorNode::getFirstOperatorChild() const
{
    return (OperatorNode *)getFirstChildWithTag(NED_OPERATOR);
}

FunctionNode *OperatorNode::getFirstFunctionChild() const
{
    return (FunctionNode *)getFirstChildWithTag(NED_FUNCTION);
}

RefNode *OperatorNode::getFirstRefChild() const
{
    return (RefNode *)getFirstChildWithTag(NED_REF);
}

ConstNode *OperatorNode::getFirstConstChild() const
{
    return (ConstNode *)getFirstChildWithTag(NED_CONST);
}

int FunctionNode::getNumAttributes() const
{
    return 1;
}

const char *FunctionNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *FunctionNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void FunctionNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *FunctionNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

FunctionNode *FunctionNode::getNextFunctionNodeSibling() const
{
    return (FunctionNode *)getNextSiblingWithTag(NED_FUNCTION);
}

WhitespaceNode *FunctionNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

OperatorNode *FunctionNode::getFirstOperatorChild() const
{
    return (OperatorNode *)getFirstChildWithTag(NED_OPERATOR);
}

FunctionNode *FunctionNode::getFirstFunctionChild() const
{
    return (FunctionNode *)getFirstChildWithTag(NED_FUNCTION);
}

RefNode *FunctionNode::getFirstRefChild() const
{
    return (RefNode *)getFirstChildWithTag(NED_REF);
}

ConstNode *FunctionNode::getFirstConstChild() const
{
    return (ConstNode *)getFirstChildWithTag(NED_CONST);
}

int RefNode::getNumAttributes() const
{
    return 3;
}

const char *RefNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "module";
        case 1: return "module-index";
        case 2: return "name";
        default: return 0;
    }
}

const char *RefNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return module.c_str();
        case 1: return moduleIndex.c_str();
        case 2: return name.c_str();
        default: return 0;
    }
}

void RefNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: module = val; break;
        case 1: moduleIndex = val; break;
        case 2: name = val; break;
        default: ;
    }
}

const char *RefNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

RefNode *RefNode::getNextRefNodeSibling() const
{
    return (RefNode *)getNextSiblingWithTag(NED_REF);
}

WhitespaceNode *RefNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExpressionNode *RefNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

int ConstNode::getNumAttributes() const
{
    return 4;
}

const char *ConstNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "type";
        case 1: return "unit-type";
        case 2: return "text";
        case 3: return "value";
        default: return 0;
    }
}

const char *ConstNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return enumToString(type, littype_vals, littype_nums, littype_n);
        case 1: return unitType.c_str();
        case 2: return text.c_str();
        case 3: return value.c_str();
        default: return 0;
    }
}

void ConstNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: type = stringToEnum(val, littype_vals, littype_nums, littype_n); break;
        case 1: unitType = val; break;
        case 2: text = val; break;
        case 3: value = val; break;
        default: ;
    }
}

const char *ConstNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "";
        default: return 0;
    }
}

ConstNode *ConstNode::getNextConstNodeSibling() const
{
    return (ConstNode *)getNextSiblingWithTag(NED_CONST);
}

WhitespaceNode *ConstNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int MsgFileNode::getNumAttributes() const
{
    return 2;
}

const char *MsgFileNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "filename";
        case 1: return "package";
        default: return 0;
    }
}

const char *MsgFileNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return filename.c_str();
        case 1: return package.c_str();
        default: return 0;
    }
}

void MsgFileNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: filename = val; break;
        case 1: package = val; break;
        default: ;
    }
}

const char *MsgFileNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        default: return 0;
    }
}

MsgFileNode *MsgFileNode::getNextMsgFileNodeSibling() const
{
    return (MsgFileNode *)getNextSiblingWithTag(NED_MSG_FILE);
}

WhitespaceNode *MsgFileNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

CplusplusNode *MsgFileNode::getFirstCplusplusChild() const
{
    return (CplusplusNode *)getFirstChildWithTag(NED_CPLUSPLUS);
}

StructDeclNode *MsgFileNode::getFirstStructDeclChild() const
{
    return (StructDeclNode *)getFirstChildWithTag(NED_STRUCT_DECL);
}

ClassDeclNode *MsgFileNode::getFirstClassDeclChild() const
{
    return (ClassDeclNode *)getFirstChildWithTag(NED_CLASS_DECL);
}

MessageDeclNode *MsgFileNode::getFirstMessageDeclChild() const
{
    return (MessageDeclNode *)getFirstChildWithTag(NED_MESSAGE_DECL);
}

EnumDeclNode *MsgFileNode::getFirstEnumDeclChild() const
{
    return (EnumDeclNode *)getFirstChildWithTag(NED_ENUM_DECL);
}

EnumNode *MsgFileNode::getFirstEnumChild() const
{
    return (EnumNode *)getFirstChildWithTag(NED_ENUM);
}

MessageNode *MsgFileNode::getFirstMessageChild() const
{
    return (MessageNode *)getFirstChildWithTag(NED_MESSAGE);
}

ClassNode *MsgFileNode::getFirstClassChild() const
{
    return (ClassNode *)getFirstChildWithTag(NED_CLASS);
}

StructNode *MsgFileNode::getFirstStructChild() const
{
    return (StructNode *)getFirstChildWithTag(NED_STRUCT);
}

int CplusplusNode::getNumAttributes() const
{
    return 4;
}

const char *CplusplusNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "body";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        case 3: return "trailing-comment";
        default: return 0;
    }
}

const char *CplusplusNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return body.c_str();
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        case 3: return trailingComment.c_str();
        default: return 0;
    }
}

void CplusplusNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: body = val; break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        case 3: trailingComment = val; break;
        default: ;
    }
}

const char *CplusplusNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "\n";
        case 3: return "\n";
        default: return 0;
    }
}

CplusplusNode *CplusplusNode::getNextCplusplusNodeSibling() const
{
    return (CplusplusNode *)getNextSiblingWithTag(NED_CPLUSPLUS);
}

WhitespaceNode *CplusplusNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int StructDeclNode::getNumAttributes() const
{
    return 4;
}

const char *StructDeclNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        case 3: return "trailing-comment";
        default: return 0;
    }
}

const char *StructDeclNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        case 3: return trailingComment.c_str();
        default: return 0;
    }
}

void StructDeclNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        case 3: trailingComment = val; break;
        default: ;
    }
}

const char *StructDeclNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "\n";
        case 3: return "\n";
        default: return 0;
    }
}

StructDeclNode *StructDeclNode::getNextStructDeclNodeSibling() const
{
    return (StructDeclNode *)getNextSiblingWithTag(NED_STRUCT_DECL);
}

WhitespaceNode *StructDeclNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int ClassDeclNode::getNumAttributes() const
{
    return 5;
}

const char *ClassDeclNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "is-cobject";
        case 2: return "banner-comment";
        case 3: return "right-comment";
        case 4: return "trailing-comment";
        default: return 0;
    }
}

const char *ClassDeclNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return boolToString(isCobject);
        case 2: return bannerComment.c_str();
        case 3: return rightComment.c_str();
        case 4: return trailingComment.c_str();
        default: return 0;
    }
}

void ClassDeclNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: isCobject = stringToBool(val); break;
        case 2: bannerComment = val; break;
        case 3: rightComment = val; break;
        case 4: trailingComment = val; break;
        default: ;
    }
}

const char *ClassDeclNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "false";
        case 2: return "";
        case 3: return "\n";
        case 4: return "\n";
        default: return 0;
    }
}

ClassDeclNode *ClassDeclNode::getNextClassDeclNodeSibling() const
{
    return (ClassDeclNode *)getNextSiblingWithTag(NED_CLASS_DECL);
}

WhitespaceNode *ClassDeclNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int MessageDeclNode::getNumAttributes() const
{
    return 4;
}

const char *MessageDeclNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        case 3: return "trailing-comment";
        default: return 0;
    }
}

const char *MessageDeclNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        case 3: return trailingComment.c_str();
        default: return 0;
    }
}

void MessageDeclNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        case 3: trailingComment = val; break;
        default: ;
    }
}

const char *MessageDeclNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "\n";
        case 3: return "\n";
        default: return 0;
    }
}

MessageDeclNode *MessageDeclNode::getNextMessageDeclNodeSibling() const
{
    return (MessageDeclNode *)getNextSiblingWithTag(NED_MESSAGE_DECL);
}

WhitespaceNode *MessageDeclNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int EnumDeclNode::getNumAttributes() const
{
    return 4;
}

const char *EnumDeclNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        case 3: return "trailing-comment";
        default: return 0;
    }
}

const char *EnumDeclNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        case 3: return trailingComment.c_str();
        default: return 0;
    }
}

void EnumDeclNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        case 3: trailingComment = val; break;
        default: ;
    }
}

const char *EnumDeclNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "\n";
        case 3: return "\n";
        default: return 0;
    }
}

EnumDeclNode *EnumDeclNode::getNextEnumDeclNodeSibling() const
{
    return (EnumDeclNode *)getNextSiblingWithTag(NED_ENUM_DECL);
}

WhitespaceNode *EnumDeclNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int EnumNode::getNumAttributes() const
{
    return 6;
}

const char *EnumNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        case 3: return "banner-comment";
        case 4: return "right-comment";
        case 5: return "trailing-comment";
        default: return 0;
    }
}

const char *EnumNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        case 3: return bannerComment.c_str();
        case 4: return rightComment.c_str();
        case 5: return trailingComment.c_str();
        default: return 0;
    }
}

void EnumNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        case 3: bannerComment = val; break;
        case 4: rightComment = val; break;
        case 5: trailingComment = val; break;
        default: ;
    }
}

const char *EnumNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "";
        case 4: return "\n";
        case 5: return "\n";
        default: return 0;
    }
}

EnumNode *EnumNode::getNextEnumNodeSibling() const
{
    return (EnumNode *)getNextSiblingWithTag(NED_ENUM);
}

WhitespaceNode *EnumNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

EnumFieldsNode *EnumNode::getFirstEnumFieldsChild() const
{
    return (EnumFieldsNode *)getFirstChildWithTag(NED_ENUM_FIELDS);
}

int EnumFieldsNode::getNumAttributes() const
{
    return 2;
}

const char *EnumFieldsNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "banner-comment";
        case 1: return "right-comment";
        default: return 0;
    }
}

const char *EnumFieldsNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return bannerComment.c_str();
        case 1: return rightComment.c_str();
        default: return 0;
    }
}

void EnumFieldsNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: bannerComment = val; break;
        case 1: rightComment = val; break;
        default: ;
    }
}

const char *EnumFieldsNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "\n";
        default: return 0;
    }
}

EnumFieldsNode *EnumFieldsNode::getNextEnumFieldsNodeSibling() const
{
    return (EnumFieldsNode *)getNextSiblingWithTag(NED_ENUM_FIELDS);
}

WhitespaceNode *EnumFieldsNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

EnumFieldNode *EnumFieldsNode::getFirstEnumFieldChild() const
{
    return (EnumFieldNode *)getFirstChildWithTag(NED_ENUM_FIELD);
}

int EnumFieldNode::getNumAttributes() const
{
    return 4;
}

const char *EnumFieldNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "value";
        case 2: return "banner-comment";
        case 3: return "right-comment";
        default: return 0;
    }
}

const char *EnumFieldNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return value.c_str();
        case 2: return bannerComment.c_str();
        case 3: return rightComment.c_str();
        default: return 0;
    }
}

void EnumFieldNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: value = val; break;
        case 2: bannerComment = val; break;
        case 3: rightComment = val; break;
        default: ;
    }
}

const char *EnumFieldNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "\n";
        default: return 0;
    }
}

EnumFieldNode *EnumFieldNode::getNextEnumFieldNodeSibling() const
{
    return (EnumFieldNode *)getNextSiblingWithTag(NED_ENUM_FIELD);
}

WhitespaceNode *EnumFieldNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int MessageNode::getNumAttributes() const
{
    return 6;
}

const char *MessageNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        case 3: return "banner-comment";
        case 4: return "right-comment";
        case 5: return "trailing-comment";
        default: return 0;
    }
}

const char *MessageNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        case 3: return bannerComment.c_str();
        case 4: return rightComment.c_str();
        case 5: return trailingComment.c_str();
        default: return 0;
    }
}

void MessageNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        case 3: bannerComment = val; break;
        case 4: rightComment = val; break;
        case 5: trailingComment = val; break;
        default: ;
    }
}

const char *MessageNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "";
        case 4: return "\n";
        case 5: return "\n";
        default: return 0;
    }
}

MessageNode *MessageNode::getNextMessageNodeSibling() const
{
    return (MessageNode *)getNextSiblingWithTag(NED_MESSAGE);
}

WhitespaceNode *MessageNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

PropertiesNode *MessageNode::getFirstPropertiesChild() const
{
    return (PropertiesNode *)getFirstChildWithTag(NED_PROPERTIES);
}

FieldsNode *MessageNode::getFirstFieldsChild() const
{
    return (FieldsNode *)getFirstChildWithTag(NED_FIELDS);
}

int ClassNode::getNumAttributes() const
{
    return 6;
}

const char *ClassNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        case 3: return "banner-comment";
        case 4: return "right-comment";
        case 5: return "trailing-comment";
        default: return 0;
    }
}

const char *ClassNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        case 3: return bannerComment.c_str();
        case 4: return rightComment.c_str();
        case 5: return trailingComment.c_str();
        default: return 0;
    }
}

void ClassNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        case 3: bannerComment = val; break;
        case 4: rightComment = val; break;
        case 5: trailingComment = val; break;
        default: ;
    }
}

const char *ClassNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "";
        case 4: return "\n";
        case 5: return "\n";
        default: return 0;
    }
}

ClassNode *ClassNode::getNextClassNodeSibling() const
{
    return (ClassNode *)getNextSiblingWithTag(NED_CLASS);
}

WhitespaceNode *ClassNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

PropertiesNode *ClassNode::getFirstPropertiesChild() const
{
    return (PropertiesNode *)getFirstChildWithTag(NED_PROPERTIES);
}

FieldsNode *ClassNode::getFirstFieldsChild() const
{
    return (FieldsNode *)getFirstChildWithTag(NED_FIELDS);
}

int StructNode::getNumAttributes() const
{
    return 6;
}

const char *StructNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        case 3: return "banner-comment";
        case 4: return "right-comment";
        case 5: return "trailing-comment";
        default: return 0;
    }
}

const char *StructNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        case 3: return bannerComment.c_str();
        case 4: return rightComment.c_str();
        case 5: return trailingComment.c_str();
        default: return 0;
    }
}

void StructNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        case 3: bannerComment = val; break;
        case 4: rightComment = val; break;
        case 5: trailingComment = val; break;
        default: ;
    }
}

const char *StructNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "";
        case 4: return "\n";
        case 5: return "\n";
        default: return 0;
    }
}

StructNode *StructNode::getNextStructNodeSibling() const
{
    return (StructNode *)getNextSiblingWithTag(NED_STRUCT);
}

WhitespaceNode *StructNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

PropertiesNode *StructNode::getFirstPropertiesChild() const
{
    return (PropertiesNode *)getFirstChildWithTag(NED_PROPERTIES);
}

FieldsNode *StructNode::getFirstFieldsChild() const
{
    return (FieldsNode *)getFirstChildWithTag(NED_FIELDS);
}

int FieldsNode::getNumAttributes() const
{
    return 2;
}

const char *FieldsNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "banner-comment";
        case 1: return "right-comment";
        default: return 0;
    }
}

const char *FieldsNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return bannerComment.c_str();
        case 1: return rightComment.c_str();
        default: return 0;
    }
}

void FieldsNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: bannerComment = val; break;
        case 1: rightComment = val; break;
        default: ;
    }
}

const char *FieldsNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "\n";
        default: return 0;
    }
}

FieldsNode *FieldsNode::getNextFieldsNodeSibling() const
{
    return (FieldsNode *)getNextSiblingWithTag(NED_FIELDS);
}

WhitespaceNode *FieldsNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

FieldNode *FieldsNode::getFirstFieldChild() const
{
    return (FieldNode *)getFirstChildWithTag(NED_FIELD);
}

int FieldNode::getNumAttributes() const
{
    return 10;
}

const char *FieldNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "data-type";
        case 2: return "is-abstract";
        case 3: return "is-readonly";
        case 4: return "is-vector";
        case 5: return "vector-size";
        case 6: return "enum-name";
        case 7: return "default-value";
        case 8: return "banner-comment";
        case 9: return "right-comment";
        default: return 0;
    }
}

const char *FieldNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return dataType.c_str();
        case 2: return boolToString(isAbstract);
        case 3: return boolToString(isReadonly);
        case 4: return boolToString(isVector);
        case 5: return vectorSize.c_str();
        case 6: return enumName.c_str();
        case 7: return defaultValue.c_str();
        case 8: return bannerComment.c_str();
        case 9: return rightComment.c_str();
        default: return 0;
    }
}

void FieldNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: dataType = val; break;
        case 2: isAbstract = stringToBool(val); break;
        case 3: isReadonly = stringToBool(val); break;
        case 4: isVector = stringToBool(val); break;
        case 5: vectorSize = val; break;
        case 6: enumName = val; break;
        case 7: defaultValue = val; break;
        case 8: bannerComment = val; break;
        case 9: rightComment = val; break;
        default: ;
    }
}

const char *FieldNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "false";
        case 3: return "false";
        case 4: return "false";
        case 5: return "";
        case 6: return "";
        case 7: return "";
        case 8: return "";
        case 9: return "\n";
        default: return 0;
    }
}

FieldNode *FieldNode::getNextFieldNodeSibling() const
{
    return (FieldNode *)getNextSiblingWithTag(NED_FIELD);
}

WhitespaceNode *FieldNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int PropertiesNode::getNumAttributes() const
{
    return 2;
}

const char *PropertiesNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "banner-comment";
        case 1: return "right-comment";
        default: return 0;
    }
}

const char *PropertiesNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return bannerComment.c_str();
        case 1: return rightComment.c_str();
        default: return 0;
    }
}

void PropertiesNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: bannerComment = val; break;
        case 1: rightComment = val; break;
        default: ;
    }
}

const char *PropertiesNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "\n";
        default: return 0;
    }
}

PropertiesNode *PropertiesNode::getNextPropertiesNodeSibling() const
{
    return (PropertiesNode *)getNextSiblingWithTag(NED_PROPERTIES);
}

WhitespaceNode *PropertiesNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

MsgpropertyNode *PropertiesNode::getFirstMsgpropertyChild() const
{
    return (MsgpropertyNode *)getFirstChildWithTag(NED_MSGPROPERTY);
}

int MsgpropertyNode::getNumAttributes() const
{
    return 4;
}

const char *MsgpropertyNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "value";
        case 2: return "banner-comment";
        case 3: return "right-comment";
        default: return 0;
    }
}

const char *MsgpropertyNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return value.c_str();
        case 2: return bannerComment.c_str();
        case 3: return rightComment.c_str();
        default: return 0;
    }
}

void MsgpropertyNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: value = val; break;
        case 2: bannerComment = val; break;
        case 3: rightComment = val; break;
        default: ;
    }
}

const char *MsgpropertyNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "\n";
        default: return 0;
    }
}

MsgpropertyNode *MsgpropertyNode::getNextMsgpropertyNodeSibling() const
{
    return (MsgpropertyNode *)getNextSiblingWithTag(NED_MSGPROPERTY);
}

WhitespaceNode *MsgpropertyNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

int UnknownNode::getNumAttributes() const
{
    return 1;
}

const char *UnknownNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "element";
        default: return 0;
    }
}

const char *UnknownNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return element.c_str();
        default: return 0;
    }
}

void UnknownNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: element = val; break;
        default: ;
    }
}

const char *UnknownNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

UnknownNode *UnknownNode::getNextUnknownNodeSibling() const
{
    return (UnknownNode *)getNextSiblingWithTag(NED_UNKNOWN);
}

NEDElementFactory *NEDElementFactory::f;

NEDElementFactory *NEDElementFactory::getInstance()
{
    if (!f) f=new NEDElementFactory();
    return f;
}

NEDElement *NEDElementFactory::createNodeWithTag(const char *tagname)
{
    if (tagname[0]=='f' && !strcmp(tagname,"files"))  return new FilesNode();
    if (tagname[0]=='n' && !strcmp(tagname,"ned-file"))  return new NedFileNode();
    if (tagname[0]=='w' && !strcmp(tagname,"whitespace"))  return new WhitespaceNode();
    if (tagname[0]=='i' && !strcmp(tagname,"import"))  return new ImportNode();
    if (tagname[0]=='p' && !strcmp(tagname,"propertydef"))  return new PropertydefNode();
    if (tagname[0]=='e' && !strcmp(tagname,"extends"))  return new ExtendsNode();
    if (tagname[0]=='i' && !strcmp(tagname,"interface-name"))  return new InterfaceNameNode();
    if (tagname[0]=='c' && !strcmp(tagname,"channel-interface-name"))  return new ChannelInterfaceNameNode();
    if (tagname[0]=='s' && !strcmp(tagname,"simple"))  return new SimpleNode();
    if (tagname[0]=='i' && !strcmp(tagname,"interface"))  return new InterfaceNode();
    if (tagname[0]=='m' && !strcmp(tagname,"module"))  return new ModuleNode();
    if (tagname[0]=='p' && !strcmp(tagname,"parameters"))  return new ParametersNode();
    if (tagname[0]=='p' && !strcmp(tagname,"param-group"))  return new ParamGroupNode();
    if (tagname[0]=='p' && !strcmp(tagname,"param"))  return new ParamNode();
    if (tagname[0]=='p' && !strcmp(tagname,"property"))  return new PropertyNode();
    if (tagname[0]=='k' && !strcmp(tagname,"key-value"))  return new KeyValueNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gates"))  return new GatesNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gate-group"))  return new GateGroupNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gate"))  return new GateNode();
    if (tagname[0]=='s' && !strcmp(tagname,"submodules"))  return new SubmodulesNode();
    if (tagname[0]=='s' && !strcmp(tagname,"submodule"))  return new SubmoduleNode();
    if (tagname[0]=='c' && !strcmp(tagname,"connections"))  return new ConnectionsNode();
    if (tagname[0]=='c' && !strcmp(tagname,"connection"))  return new ConnectionNode();
    if (tagname[0]=='c' && !strcmp(tagname,"channel-interface"))  return new ChannelInterfaceNode();
    if (tagname[0]=='c' && !strcmp(tagname,"channel"))  return new ChannelNode();
    if (tagname[0]=='c' && !strcmp(tagname,"connection-group"))  return new ConnectionGroupNode();
    if (tagname[0]=='l' && !strcmp(tagname,"loop"))  return new LoopNode();
    if (tagname[0]=='c' && !strcmp(tagname,"condition"))  return new ConditionNode();
    if (tagname[0]=='e' && !strcmp(tagname,"expression"))  return new ExpressionNode();
    if (tagname[0]=='o' && !strcmp(tagname,"operator"))  return new OperatorNode();
    if (tagname[0]=='f' && !strcmp(tagname,"function"))  return new FunctionNode();
    if (tagname[0]=='r' && !strcmp(tagname,"ref"))  return new RefNode();
    if (tagname[0]=='c' && !strcmp(tagname,"const"))  return new ConstNode();
    if (tagname[0]=='m' && !strcmp(tagname,"msg-file"))  return new MsgFileNode();
    if (tagname[0]=='c' && !strcmp(tagname,"cplusplus"))  return new CplusplusNode();
    if (tagname[0]=='s' && !strcmp(tagname,"struct-decl"))  return new StructDeclNode();
    if (tagname[0]=='c' && !strcmp(tagname,"class-decl"))  return new ClassDeclNode();
    if (tagname[0]=='m' && !strcmp(tagname,"message-decl"))  return new MessageDeclNode();
    if (tagname[0]=='e' && !strcmp(tagname,"enum-decl"))  return new EnumDeclNode();
    if (tagname[0]=='e' && !strcmp(tagname,"enum"))  return new EnumNode();
    if (tagname[0]=='e' && !strcmp(tagname,"enum-fields"))  return new EnumFieldsNode();
    if (tagname[0]=='e' && !strcmp(tagname,"enum-field"))  return new EnumFieldNode();
    if (tagname[0]=='m' && !strcmp(tagname,"message"))  return new MessageNode();
    if (tagname[0]=='c' && !strcmp(tagname,"class"))  return new ClassNode();
    if (tagname[0]=='s' && !strcmp(tagname,"struct"))  return new StructNode();
    if (tagname[0]=='f' && !strcmp(tagname,"fields"))  return new FieldsNode();
    if (tagname[0]=='f' && !strcmp(tagname,"field"))  return new FieldNode();
    if (tagname[0]=='p' && !strcmp(tagname,"properties"))  return new PropertiesNode();
    if (tagname[0]=='m' && !strcmp(tagname,"msgproperty"))  return new MsgpropertyNode();
    if (tagname[0]=='u' && !strcmp(tagname,"unknown"))  return new UnknownNode();
    throw new NEDException("unknown tag '%s', cannot create object to represent it", tagname);
}

NEDElement *NEDElementFactory::createNodeWithTag(int tagcode)
{
    switch (tagcode) {
        case NED_FILES: return new FilesNode();
        case NED_NED_FILE: return new NedFileNode();
        case NED_WHITESPACE: return new WhitespaceNode();
        case NED_IMPORT: return new ImportNode();
        case NED_PROPERTYDEF: return new PropertydefNode();
        case NED_EXTENDS: return new ExtendsNode();
        case NED_INTERFACE_NAME: return new InterfaceNameNode();
        case NED_CHANNEL_INTERFACE_NAME: return new ChannelInterfaceNameNode();
        case NED_SIMPLE: return new SimpleNode();
        case NED_INTERFACE: return new InterfaceNode();
        case NED_MODULE: return new ModuleNode();
        case NED_PARAMETERS: return new ParametersNode();
        case NED_PARAM_GROUP: return new ParamGroupNode();
        case NED_PARAM: return new ParamNode();
        case NED_PROPERTY: return new PropertyNode();
        case NED_KEY_VALUE: return new KeyValueNode();
        case NED_GATES: return new GatesNode();
        case NED_GATE_GROUP: return new GateGroupNode();
        case NED_GATE: return new GateNode();
        case NED_SUBMODULES: return new SubmodulesNode();
        case NED_SUBMODULE: return new SubmoduleNode();
        case NED_CONNECTIONS: return new ConnectionsNode();
        case NED_CONNECTION: return new ConnectionNode();
        case NED_CHANNEL_INTERFACE: return new ChannelInterfaceNode();
        case NED_CHANNEL: return new ChannelNode();
        case NED_CONNECTION_GROUP: return new ConnectionGroupNode();
        case NED_LOOP: return new LoopNode();
        case NED_CONDITION: return new ConditionNode();
        case NED_EXPRESSION: return new ExpressionNode();
        case NED_OPERATOR: return new OperatorNode();
        case NED_FUNCTION: return new FunctionNode();
        case NED_REF: return new RefNode();
        case NED_CONST: return new ConstNode();
        case NED_MSG_FILE: return new MsgFileNode();
        case NED_CPLUSPLUS: return new CplusplusNode();
        case NED_STRUCT_DECL: return new StructDeclNode();
        case NED_CLASS_DECL: return new ClassDeclNode();
        case NED_MESSAGE_DECL: return new MessageDeclNode();
        case NED_ENUM_DECL: return new EnumDeclNode();
        case NED_ENUM: return new EnumNode();
        case NED_ENUM_FIELDS: return new EnumFieldsNode();
        case NED_ENUM_FIELD: return new EnumFieldNode();
        case NED_MESSAGE: return new MessageNode();
        case NED_CLASS: return new ClassNode();
        case NED_STRUCT: return new StructNode();
        case NED_FIELDS: return new FieldsNode();
        case NED_FIELD: return new FieldNode();
        case NED_PROPERTIES: return new PropertiesNode();
        case NED_MSGPROPERTY: return new MsgpropertyNode();
        case NED_UNKNOWN: return new UnknownNode();
    }
    throw new NEDException("unknown tag code %d, cannot create object to represent it", tagcode);
}

