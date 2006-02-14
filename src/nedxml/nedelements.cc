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

static const char *gatetype_vals[] = {"input", "output", "inout"};
static int gatetype_nums[] = {NED_GATEDIR_INPUT, NED_GATEDIR_OUTPUT, NED_GATEDIR_INOUT};
static const int gatetype_n = 3;

static const char *arrowdir_vals[] = {"l2r", "r2l", "bidir"};
static int arrowdir_nums[] = {NED_ARROWDIR_L2R, NED_ARROWDIR_R2L, NED_ARROWDIR_BIDIR};
static const int arrowdir_n = 3;

static const char *partype_vals[] = {"", "double", "int", "string", "bool", "xml"};
static int partype_nums[] = {NED_PARTYPE_NONE, NED_PARTYPE_DOUBLE, NED_PARTYPE_INT, NED_PARTYPE_STRING, NED_PARTYPE_BOOL, NED_PARTYPE_XML};
static const int partype_n = 6;

static const char *littype_vals[] = {"double", "int", "string", "bool", "unit"};
static int littype_nums[] = {NED_CONST_DOUBLE, NED_CONST_INT, NED_CONST_STRING, NED_CONST_BOOL, NED_CONST_UNIT};
static const int littype_n = 5;

static const char *subgate_vals[] = {"", "i", "o"};
static int subgate_nums[] = {NED_SUBGATE_NONE, NED_SUBGATE_I, NED_SUBGATE_O};
static const int subgate_n = 3;

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
        case 0: return NULL;
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

PropertyDeclNode *NedFileNode::getFirstPropertyDeclChild() const
{
    return (PropertyDeclNode *)getFirstChildWithTag(NED_PROPERTY_DECL);
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

SimpleModuleNode *NedFileNode::getFirstSimpleModuleChild() const
{
    return (SimpleModuleNode *)getFirstChildWithTag(NED_SIMPLE_MODULE);
}

CompoundModuleNode *NedFileNode::getFirstCompoundModuleChild() const
{
    return (CompoundModuleNode *)getFirstChildWithTag(NED_COMPOUND_MODULE);
}

ModuleInterfaceNode *NedFileNode::getFirstModuleInterfaceChild() const
{
    return (ModuleInterfaceNode *)getFirstChildWithTag(NED_MODULE_INTERFACE);
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
        case 0: return NULL;
        case 1: return NULL;
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
        case 0: return NULL;
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

int PropertyDeclNode::getNumAttributes() const
{
    return 1;
}

const char *PropertyDeclNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *PropertyDeclNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void PropertyDeclNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *PropertyDeclNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

PropertyDeclNode *PropertyDeclNode::getNextPropertyDeclNodeSibling() const
{
    return (PropertyDeclNode *)getNextSiblingWithTag(NED_PROPERTY_DECL);
}

WhitespaceNode *PropertyDeclNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

PropertyKeyNode *PropertyDeclNode::getFirstPropertyKeyChild() const
{
    return (PropertyKeyNode *)getFirstChildWithTag(NED_PROPERTY_KEY);
}

PropertyNode *PropertyDeclNode::getFirstPropertyChild() const
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
        case 0: return NULL;
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
        case 0: return NULL;
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

int SimpleModuleNode::getNumAttributes() const
{
    return 1;
}

const char *SimpleModuleNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *SimpleModuleNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void SimpleModuleNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *SimpleModuleNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

SimpleModuleNode *SimpleModuleNode::getNextSimpleModuleNodeSibling() const
{
    return (SimpleModuleNode *)getNextSiblingWithTag(NED_SIMPLE_MODULE);
}

WhitespaceNode *SimpleModuleNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExtendsNode *SimpleModuleNode::getFirstExtendsChild() const
{
    return (ExtendsNode *)getFirstChildWithTag(NED_EXTENDS);
}

InterfaceNameNode *SimpleModuleNode::getFirstInterfaceNameChild() const
{
    return (InterfaceNameNode *)getFirstChildWithTag(NED_INTERFACE_NAME);
}

ParametersNode *SimpleModuleNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

GatesNode *SimpleModuleNode::getFirstGatesChild() const
{
    return (GatesNode *)getFirstChildWithTag(NED_GATES);
}

int ModuleInterfaceNode::getNumAttributes() const
{
    return 1;
}

const char *ModuleInterfaceNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *ModuleInterfaceNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void ModuleInterfaceNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *ModuleInterfaceNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

ModuleInterfaceNode *ModuleInterfaceNode::getNextModuleInterfaceNodeSibling() const
{
    return (ModuleInterfaceNode *)getNextSiblingWithTag(NED_MODULE_INTERFACE);
}

WhitespaceNode *ModuleInterfaceNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExtendsNode *ModuleInterfaceNode::getFirstExtendsChild() const
{
    return (ExtendsNode *)getFirstChildWithTag(NED_EXTENDS);
}

ParametersNode *ModuleInterfaceNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

GatesNode *ModuleInterfaceNode::getFirstGatesChild() const
{
    return (GatesNode *)getFirstChildWithTag(NED_GATES);
}

int CompoundModuleNode::getNumAttributes() const
{
    return 2;
}

const char *CompoundModuleNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "is-network";
        default: return 0;
    }
}

const char *CompoundModuleNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return boolToString(isNetwork);
        default: return 0;
    }
}

void CompoundModuleNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: isNetwork = stringToBool(val); break;
        default: ;
    }
}

const char *CompoundModuleNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "false";
        default: return 0;
    }
}

CompoundModuleNode *CompoundModuleNode::getNextCompoundModuleNodeSibling() const
{
    return (CompoundModuleNode *)getNextSiblingWithTag(NED_COMPOUND_MODULE);
}

WhitespaceNode *CompoundModuleNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExtendsNode *CompoundModuleNode::getFirstExtendsChild() const
{
    return (ExtendsNode *)getFirstChildWithTag(NED_EXTENDS);
}

InterfaceNameNode *CompoundModuleNode::getFirstInterfaceNameChild() const
{
    return (InterfaceNameNode *)getFirstChildWithTag(NED_INTERFACE_NAME);
}

ParametersNode *CompoundModuleNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

GatesNode *CompoundModuleNode::getFirstGatesChild() const
{
    return (GatesNode *)getFirstChildWithTag(NED_GATES);
}

TypesNode *CompoundModuleNode::getFirstTypesChild() const
{
    return (TypesNode *)getFirstChildWithTag(NED_TYPES);
}

SubmodulesNode *CompoundModuleNode::getFirstSubmodulesChild() const
{
    return (SubmodulesNode *)getFirstChildWithTag(NED_SUBMODULES);
}

ConnectionsNode *CompoundModuleNode::getFirstConnectionsChild() const
{
    return (ConnectionsNode *)getFirstChildWithTag(NED_CONNECTIONS);
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
        case 0: return NULL;
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
        case 0: return NULL;
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

InterfaceNameNode *ChannelNode::getFirstInterfaceNameChild() const
{
    return (InterfaceNameNode *)getFirstChildWithTag(NED_INTERFACE_NAME);
}

ParametersNode *ChannelNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
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
        case 0: return "false";
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

PatternNode *ParametersNode::getFirstPatternChild() const
{
    return (PatternNode *)getFirstChildWithTag(NED_PATTERN);
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

PropertyNode *ParamGroupNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

ParamNode *ParamGroupNode::getFirstParamChild() const
{
    return (ParamNode *)getFirstChildWithTag(NED_PARAM);
}

PatternNode *ParamGroupNode::getFirstPatternChild() const
{
    return (PatternNode *)getFirstChildWithTag(NED_PATTERN);
}

ConditionNode *ParamGroupNode::getFirstConditionChild() const
{
    return (ConditionNode *)getFirstChildWithTag(NED_CONDITION);
}

void ParamNode::setType(int val)
{
    validateEnum(val, partype_vals, partype_nums, partype_n);
    type = val;
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
        case 2: return "name";
        case 3: return "value";
        case 4: return "is-default";
        default: return 0;
    }
}

const char *ParamNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return enumToString(type, partype_vals, partype_nums, partype_n);
        case 1: return boolToString(isFunction);
        case 2: return name.c_str();
        case 3: return value.c_str();
        case 4: return boolToString(isDefault);
        default: return 0;
    }
}

void ParamNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: type = stringToEnum(val, partype_vals, partype_nums, partype_n); break;
        case 1: isFunction = stringToBool(val); break;
        case 2: name = val; break;
        case 3: value = val; break;
        case 4: isDefault = stringToBool(val); break;
        default: ;
    }
}

const char *ParamNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "false";
        case 2: return NULL;
        case 3: return "";
        case 4: return "false";
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

ConditionNode *ParamNode::getFirstConditionChild() const
{
    return (ConditionNode *)getFirstChildWithTag(NED_CONDITION);
}

int PatternNode::getNumAttributes() const
{
    return 3;
}

const char *PatternNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "pattern";
        case 1: return "value";
        case 2: return "is-default";
        default: return 0;
    }
}

const char *PatternNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return pattern.c_str();
        case 1: return value.c_str();
        case 2: return boolToString(isDefault);
        default: return 0;
    }
}

void PatternNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: pattern = val; break;
        case 1: value = val; break;
        case 2: isDefault = stringToBool(val); break;
        default: ;
    }
}

const char *PatternNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return NULL;
        case 2: return "false";
        default: return 0;
    }
}

PatternNode *PatternNode::getNextPatternNodeSibling() const
{
    return (PatternNode *)getNextSiblingWithTag(NED_PATTERN);
}

WhitespaceNode *PatternNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExpressionNode *PatternNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

PropertyNode *PatternNode::getFirstPropertyChild() const
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
        case 0: return "is-implicit";
        case 1: return "name";
        default: return 0;
    }
}

const char *PropertyNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return boolToString(isImplicit);
        case 1: return name.c_str();
        default: return 0;
    }
}

void PropertyNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: isImplicit = stringToBool(val); break;
        case 1: name = val; break;
        default: ;
    }
}

const char *PropertyNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "false";
        case 1: return NULL;
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

PropertyKeyNode *PropertyNode::getFirstPropertyKeyChild() const
{
    return (PropertyKeyNode *)getFirstChildWithTag(NED_PROPERTY_KEY);
}

ConditionNode *PropertyNode::getFirstConditionChild() const
{
    return (ConditionNode *)getFirstChildWithTag(NED_CONDITION);
}

int PropertyKeyNode::getNumAttributes() const
{
    return 1;
}

const char *PropertyKeyNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "key";
        default: return 0;
    }
}

const char *PropertyKeyNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return key.c_str();
        default: return 0;
    }
}

void PropertyKeyNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: key = val; break;
        default: ;
    }
}

const char *PropertyKeyNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

PropertyKeyNode *PropertyKeyNode::getNextPropertyKeyNodeSibling() const
{
    return (PropertyKeyNode *)getNextSiblingWithTag(NED_PROPERTY_KEY);
}

WhitespaceNode *PropertyKeyNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

LiteralNode *PropertyKeyNode::getFirstLiteralChild() const
{
    return (LiteralNode *)getFirstChildWithTag(NED_LITERAL);
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

GateNode *GateGroupNode::getFirstGateChild() const
{
    return (GateNode *)getFirstChildWithTag(NED_GATE);
}

ConditionNode *GateGroupNode::getFirstConditionChild() const
{
    return (ConditionNode *)getFirstChildWithTag(NED_CONDITION);
}

void GateNode::setType(int val)
{
    validateEnum(val, gatetype_vals, gatetype_nums, gatetype_n);
    type = val;
}

int GateNode::getNumAttributes() const
{
    return 4;
}

const char *GateNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "type";
        case 2: return "is-vector";
        case 3: return "vector-size";
        default: return 0;
    }
}

const char *GateNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return enumToString(type, gatetype_vals, gatetype_nums, gatetype_n);
        case 2: return boolToString(isVector);
        case 3: return vectorSize.c_str();
        default: return 0;
    }
}

void GateNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: type = stringToEnum(val, gatetype_vals, gatetype_nums, gatetype_n); break;
        case 2: isVector = stringToBool(val); break;
        case 3: vectorSize = val; break;
        default: ;
    }
}

const char *GateNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return NULL;
        case 2: return "false";
        case 3: return "";
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

ConditionNode *GateNode::getFirstConditionChild() const
{
    return (ConditionNode *)getFirstChildWithTag(NED_CONDITION);
}

int TypesNode::getNumAttributes() const
{
    return 0;
}

const char *TypesNode::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *TypesNode::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void TypesNode::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *TypesNode::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

TypesNode *TypesNode::getNextTypesNodeSibling() const
{
    return (TypesNode *)getNextSiblingWithTag(NED_TYPES);
}

WhitespaceNode *TypesNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ChannelNode *TypesNode::getFirstChannelChild() const
{
    return (ChannelNode *)getFirstChildWithTag(NED_CHANNEL);
}

ChannelInterfaceNode *TypesNode::getFirstChannelInterfaceChild() const
{
    return (ChannelInterfaceNode *)getFirstChildWithTag(NED_CHANNEL_INTERFACE);
}

SimpleModuleNode *TypesNode::getFirstSimpleModuleChild() const
{
    return (SimpleModuleNode *)getFirstChildWithTag(NED_SIMPLE_MODULE);
}

CompoundModuleNode *TypesNode::getFirstCompoundModuleChild() const
{
    return (CompoundModuleNode *)getFirstChildWithTag(NED_COMPOUND_MODULE);
}

ModuleInterfaceNode *TypesNode::getFirstModuleInterfaceChild() const
{
    return (ModuleInterfaceNode *)getFirstChildWithTag(NED_MODULE_INTERFACE);
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
        case 0: return NULL;
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
        case 0: return "allow-unconnected";
        default: return 0;
    }
}

const char *ConnectionsNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return boolToString(allowUnconnected);
        default: return 0;
    }
}

void ConnectionsNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: allowUnconnected = stringToBool(val); break;
        default: ;
    }
}

const char *ConnectionsNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "false";
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

void ConnectionNode::setSrcGateSubg(int val)
{
    validateEnum(val, subgate_vals, subgate_nums, subgate_n);
    srcGateSubg = val;
}

void ConnectionNode::setDestGateSubg(int val)
{
    validateEnum(val, subgate_vals, subgate_nums, subgate_n);
    destGateSubg = val;
}

void ConnectionNode::setArrowDirection(int val)
{
    validateEnum(val, arrowdir_vals, arrowdir_nums, arrowdir_n);
    arrowDirection = val;
}

int ConnectionNode::getNumAttributes() const
{
    return 13;
}

const char *ConnectionNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "src-module";
        case 1: return "src-module-index";
        case 2: return "src-gate";
        case 3: return "src-gate-plusplus";
        case 4: return "src-gate-index";
        case 5: return "src-gate-subg";
        case 6: return "dest-module";
        case 7: return "dest-module-index";
        case 8: return "dest-gate";
        case 9: return "dest-gate-plusplus";
        case 10: return "dest-gate-index";
        case 11: return "dest-gate-subg";
        case 12: return "arrow-direction";
        default: return 0;
    }
}

const char *ConnectionNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return srcModule.c_str();
        case 1: return srcModuleIndex.c_str();
        case 2: return srcGate.c_str();
        case 3: return boolToString(srcGatePlusplus);
        case 4: return srcGateIndex.c_str();
        case 5: return enumToString(srcGateSubg, subgate_vals, subgate_nums, subgate_n);
        case 6: return destModule.c_str();
        case 7: return destModuleIndex.c_str();
        case 8: return destGate.c_str();
        case 9: return boolToString(destGatePlusplus);
        case 10: return destGateIndex.c_str();
        case 11: return enumToString(destGateSubg, subgate_vals, subgate_nums, subgate_n);
        case 12: return enumToString(arrowDirection, arrowdir_vals, arrowdir_nums, arrowdir_n);
        default: return 0;
    }
}

void ConnectionNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: srcModule = val; break;
        case 1: srcModuleIndex = val; break;
        case 2: srcGate = val; break;
        case 3: srcGatePlusplus = stringToBool(val); break;
        case 4: srcGateIndex = val; break;
        case 5: srcGateSubg = stringToEnum(val, subgate_vals, subgate_nums, subgate_n); break;
        case 6: destModule = val; break;
        case 7: destModuleIndex = val; break;
        case 8: destGate = val; break;
        case 9: destGatePlusplus = stringToBool(val); break;
        case 10: destGateIndex = val; break;
        case 11: destGateSubg = stringToEnum(val, subgate_vals, subgate_nums, subgate_n); break;
        case 12: arrowDirection = stringToEnum(val, arrowdir_vals, arrowdir_nums, arrowdir_n); break;
        default: ;
    }
}

const char *ConnectionNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return NULL;
        case 3: return "false";
        case 4: return "";
        case 5: return "";
        case 6: return "";
        case 7: return "";
        case 8: return NULL;
        case 9: return "false";
        case 10: return "";
        case 11: return "";
        case 12: return NULL;
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

ChannelSpecNode *ConnectionNode::getFirstChannelSpecChild() const
{
    return (ChannelSpecNode *)getFirstChildWithTag(NED_CHANNEL_SPEC);
}

WhereNode *ConnectionNode::getFirstWhereChild() const
{
    return (WhereNode *)getFirstChildWithTag(NED_WHERE);
}

int ChannelSpecNode::getNumAttributes() const
{
    return 3;
}

const char *ChannelSpecNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "type";
        case 1: return "like-type";
        case 2: return "like-param";
        default: return 0;
    }
}

const char *ChannelSpecNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return type.c_str();
        case 1: return likeType.c_str();
        case 2: return likeParam.c_str();
        default: return 0;
    }
}

void ChannelSpecNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: type = val; break;
        case 1: likeType = val; break;
        case 2: likeParam = val; break;
        default: ;
    }
}

const char *ChannelSpecNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

ChannelSpecNode *ChannelSpecNode::getNextChannelSpecNodeSibling() const
{
    return (ChannelSpecNode *)getNextSiblingWithTag(NED_CHANNEL_SPEC);
}

WhitespaceNode *ChannelSpecNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExpressionNode *ChannelSpecNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

ParametersNode *ChannelSpecNode::getFirstParametersChild() const
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

ConnectionNode *ConnectionGroupNode::getFirstConnectionChild() const
{
    return (ConnectionNode *)getFirstChildWithTag(NED_CONNECTION);
}

WhereNode *ConnectionGroupNode::getFirstWhereChild() const
{
    return (WhereNode *)getFirstChildWithTag(NED_WHERE);
}

int WhereNode::getNumAttributes() const
{
    return 1;
}

const char *WhereNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "at-front";
        default: return 0;
    }
}

const char *WhereNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return boolToString(atFront);
        default: return 0;
    }
}

void WhereNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: atFront = stringToBool(val); break;
        default: ;
    }
}

const char *WhereNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "false";
        default: return 0;
    }
}

WhereNode *WhereNode::getNextWhereNodeSibling() const
{
    return (WhereNode *)getNextSiblingWithTag(NED_WHERE);
}

WhitespaceNode *WhereNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

LoopNode *WhereNode::getFirstLoopChild() const
{
    return (LoopNode *)getFirstChildWithTag(NED_LOOP);
}

ConditionNode *WhereNode::getFirstConditionChild() const
{
    return (ConditionNode *)getFirstChildWithTag(NED_CONDITION);
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
        case 0: return NULL;
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

IdentNode *ExpressionNode::getFirstIdentChild() const
{
    return (IdentNode *)getFirstChildWithTag(NED_IDENT);
}

LiteralNode *ExpressionNode::getFirstLiteralChild() const
{
    return (LiteralNode *)getFirstChildWithTag(NED_LITERAL);
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
        case 0: return NULL;
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

IdentNode *OperatorNode::getFirstIdentChild() const
{
    return (IdentNode *)getFirstChildWithTag(NED_IDENT);
}

LiteralNode *OperatorNode::getFirstLiteralChild() const
{
    return (LiteralNode *)getFirstChildWithTag(NED_LITERAL);
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
        case 0: return NULL;
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

IdentNode *FunctionNode::getFirstIdentChild() const
{
    return (IdentNode *)getFirstChildWithTag(NED_IDENT);
}

LiteralNode *FunctionNode::getFirstLiteralChild() const
{
    return (LiteralNode *)getFirstChildWithTag(NED_LITERAL);
}

int IdentNode::getNumAttributes() const
{
    return 3;
}

const char *IdentNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "module";
        case 1: return "module-index";
        case 2: return "name";
        default: return 0;
    }
}

const char *IdentNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return module.c_str();
        case 1: return moduleIndex.c_str();
        case 2: return name.c_str();
        default: return 0;
    }
}

void IdentNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: module = val; break;
        case 1: moduleIndex = val; break;
        case 2: name = val; break;
        default: ;
    }
}

const char *IdentNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return NULL;
        default: return 0;
    }
}

IdentNode *IdentNode::getNextIdentNodeSibling() const
{
    return (IdentNode *)getNextSiblingWithTag(NED_IDENT);
}

WhitespaceNode *IdentNode::getFirstWhitespaceChild() const
{
    return (WhitespaceNode *)getFirstChildWithTag(NED_WHITESPACE);
}

ExpressionNode *IdentNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

void LiteralNode::setType(int val)
{
    validateEnum(val, littype_vals, littype_nums, littype_n);
    type = val;
}

int LiteralNode::getNumAttributes() const
{
    return 4;
}

const char *LiteralNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "type";
        case 1: return "unit-type";
        case 2: return "text";
        case 3: return "value";
        default: return 0;
    }
}

const char *LiteralNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return enumToString(type, littype_vals, littype_nums, littype_n);
        case 1: return unitType.c_str();
        case 2: return text.c_str();
        case 3: return value.c_str();
        default: return 0;
    }
}

void LiteralNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: type = stringToEnum(val, littype_vals, littype_nums, littype_n); break;
        case 1: unitType = val; break;
        case 2: text = val; break;
        case 3: value = val; break;
        default: ;
    }
}

const char *LiteralNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "";
        case 3: return "";
        default: return 0;
    }
}

LiteralNode *LiteralNode::getNextLiteralNodeSibling() const
{
    return (LiteralNode *)getNextSiblingWithTag(NED_LITERAL);
}

WhitespaceNode *LiteralNode::getFirstWhitespaceChild() const
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
        case 0: return NULL;
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
        case 0: return NULL;
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
        case 0: return NULL;
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
        case 0: return NULL;
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
        case 0: return NULL;
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
        case 0: return NULL;
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
        case 0: return NULL;
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
        case 0: return NULL;
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
        case 0: return NULL;
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
        case 0: return NULL;
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
        case 0: return NULL;
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
        case 0: return NULL;
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
        case 0: return NULL;
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
    if (tagname[0]=='p' && !strcmp(tagname,"property-decl"))  return new PropertyDeclNode();
    if (tagname[0]=='e' && !strcmp(tagname,"extends"))  return new ExtendsNode();
    if (tagname[0]=='i' && !strcmp(tagname,"interface-name"))  return new InterfaceNameNode();
    if (tagname[0]=='s' && !strcmp(tagname,"simple-module"))  return new SimpleModuleNode();
    if (tagname[0]=='m' && !strcmp(tagname,"module-interface"))  return new ModuleInterfaceNode();
    if (tagname[0]=='c' && !strcmp(tagname,"compound-module"))  return new CompoundModuleNode();
    if (tagname[0]=='c' && !strcmp(tagname,"channel-interface"))  return new ChannelInterfaceNode();
    if (tagname[0]=='c' && !strcmp(tagname,"channel"))  return new ChannelNode();
    if (tagname[0]=='p' && !strcmp(tagname,"parameters"))  return new ParametersNode();
    if (tagname[0]=='p' && !strcmp(tagname,"param-group"))  return new ParamGroupNode();
    if (tagname[0]=='p' && !strcmp(tagname,"param"))  return new ParamNode();
    if (tagname[0]=='p' && !strcmp(tagname,"pattern"))  return new PatternNode();
    if (tagname[0]=='p' && !strcmp(tagname,"property"))  return new PropertyNode();
    if (tagname[0]=='p' && !strcmp(tagname,"property-key"))  return new PropertyKeyNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gates"))  return new GatesNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gate-group"))  return new GateGroupNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gate"))  return new GateNode();
    if (tagname[0]=='t' && !strcmp(tagname,"types"))  return new TypesNode();
    if (tagname[0]=='s' && !strcmp(tagname,"submodules"))  return new SubmodulesNode();
    if (tagname[0]=='s' && !strcmp(tagname,"submodule"))  return new SubmoduleNode();
    if (tagname[0]=='c' && !strcmp(tagname,"connections"))  return new ConnectionsNode();
    if (tagname[0]=='c' && !strcmp(tagname,"connection"))  return new ConnectionNode();
    if (tagname[0]=='c' && !strcmp(tagname,"channel-spec"))  return new ChannelSpecNode();
    if (tagname[0]=='c' && !strcmp(tagname,"connection-group"))  return new ConnectionGroupNode();
    if (tagname[0]=='w' && !strcmp(tagname,"where"))  return new WhereNode();
    if (tagname[0]=='l' && !strcmp(tagname,"loop"))  return new LoopNode();
    if (tagname[0]=='c' && !strcmp(tagname,"condition"))  return new ConditionNode();
    if (tagname[0]=='e' && !strcmp(tagname,"expression"))  return new ExpressionNode();
    if (tagname[0]=='o' && !strcmp(tagname,"operator"))  return new OperatorNode();
    if (tagname[0]=='f' && !strcmp(tagname,"function"))  return new FunctionNode();
    if (tagname[0]=='i' && !strcmp(tagname,"ident"))  return new IdentNode();
    if (tagname[0]=='l' && !strcmp(tagname,"literal"))  return new LiteralNode();
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
        case NED_PROPERTY_DECL: return new PropertyDeclNode();
        case NED_EXTENDS: return new ExtendsNode();
        case NED_INTERFACE_NAME: return new InterfaceNameNode();
        case NED_SIMPLE_MODULE: return new SimpleModuleNode();
        case NED_MODULE_INTERFACE: return new ModuleInterfaceNode();
        case NED_COMPOUND_MODULE: return new CompoundModuleNode();
        case NED_CHANNEL_INTERFACE: return new ChannelInterfaceNode();
        case NED_CHANNEL: return new ChannelNode();
        case NED_PARAMETERS: return new ParametersNode();
        case NED_PARAM_GROUP: return new ParamGroupNode();
        case NED_PARAM: return new ParamNode();
        case NED_PATTERN: return new PatternNode();
        case NED_PROPERTY: return new PropertyNode();
        case NED_PROPERTY_KEY: return new PropertyKeyNode();
        case NED_GATES: return new GatesNode();
        case NED_GATE_GROUP: return new GateGroupNode();
        case NED_GATE: return new GateNode();
        case NED_TYPES: return new TypesNode();
        case NED_SUBMODULES: return new SubmodulesNode();
        case NED_SUBMODULE: return new SubmoduleNode();
        case NED_CONNECTIONS: return new ConnectionsNode();
        case NED_CONNECTION: return new ConnectionNode();
        case NED_CHANNEL_SPEC: return new ChannelSpecNode();
        case NED_CONNECTION_GROUP: return new ConnectionGroupNode();
        case NED_WHERE: return new WhereNode();
        case NED_LOOP: return new LoopNode();
        case NED_CONDITION: return new ConditionNode();
        case NED_EXPRESSION: return new ExpressionNode();
        case NED_OPERATOR: return new OperatorNode();
        case NED_FUNCTION: return new FunctionNode();
        case NED_IDENT: return new IdentNode();
        case NED_LITERAL: return new LiteralNode();
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

