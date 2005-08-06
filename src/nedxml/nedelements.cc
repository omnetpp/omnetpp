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


int NedFilesNode::getNumAttributes() const
{
    return 0;
}

const char *NedFilesNode::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *NedFilesNode::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void NedFilesNode::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *NedFilesNode::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

NedFilesNode *NedFilesNode::getNextNedFilesNodeSibling() const
{
    return (NedFilesNode *)getNextSiblingWithTag(NED_NED_FILES);
}

NedFilesNode *NedFilesNode::getFirstNedFilesChild() const
{
    return (NedFilesNode *)getFirstChildWithTag(NED_NED_FILES);
}

NedFileNode *NedFilesNode::getFirstNedFileChild() const
{
    return (NedFileNode *)getFirstChildWithTag(NED_NED_FILE);
}

int NedFileNode::getNumAttributes() const
{
    return 4;
}

const char *NedFileNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "filename";
        case 1: return "source-code";
        case 2: return "preferred-indent";
        case 3: return "banner-comment";
        default: return 0;
    }
}

const char *NedFileNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return filename.c_str();
        case 1: return sourceCode.c_str();
        case 2: return preferredIndent.c_str();
        case 3: return bannerComment.c_str();
        default: return 0;
    }
}

void NedFileNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: filename = val; break;
        case 1: sourceCode = val; break;
        case 2: preferredIndent = val; break;
        case 3: bannerComment = val; break;
        default: ;
    }
}

const char *NedFileNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "4";
        case 3: return "";
        default: return 0;
    }
}

NedFileNode *NedFileNode::getNextNedFileNodeSibling() const
{
    return (NedFileNode *)getNextSiblingWithTag(NED_NED_FILE);
}

ImportNode *NedFileNode::getFirstImportChild() const
{
    return (ImportNode *)getFirstChildWithTag(NED_IMPORT);
}

ChannelNode *NedFileNode::getFirstChannelChild() const
{
    return (ChannelNode *)getFirstChildWithTag(NED_CHANNEL);
}

SimpleModuleNode *NedFileNode::getFirstSimpleModuleChild() const
{
    return (SimpleModuleNode *)getFirstChildWithTag(NED_SIMPLE_MODULE);
}

CompoundModuleNode *NedFileNode::getFirstCompoundModuleChild() const
{
    return (CompoundModuleNode *)getFirstChildWithTag(NED_COMPOUND_MODULE);
}

NetworkNode *NedFileNode::getFirstNetworkChild() const
{
    return (NetworkNode *)getFirstChildWithTag(NED_NETWORK);
}

CplusplusNode *NedFileNode::getFirstCplusplusChild() const
{
    return (CplusplusNode *)getFirstChildWithTag(NED_CPLUSPLUS);
}

StructDeclNode *NedFileNode::getFirstStructDeclChild() const
{
    return (StructDeclNode *)getFirstChildWithTag(NED_STRUCT_DECL);
}

ClassDeclNode *NedFileNode::getFirstClassDeclChild() const
{
    return (ClassDeclNode *)getFirstChildWithTag(NED_CLASS_DECL);
}

MessageDeclNode *NedFileNode::getFirstMessageDeclChild() const
{
    return (MessageDeclNode *)getFirstChildWithTag(NED_MESSAGE_DECL);
}

EnumDeclNode *NedFileNode::getFirstEnumDeclChild() const
{
    return (EnumDeclNode *)getFirstChildWithTag(NED_ENUM_DECL);
}

EnumNode *NedFileNode::getFirstEnumChild() const
{
    return (EnumNode *)getFirstChildWithTag(NED_ENUM);
}

MessageNode *NedFileNode::getFirstMessageChild() const
{
    return (MessageNode *)getFirstChildWithTag(NED_MESSAGE);
}

ClassNode *NedFileNode::getFirstClassChild() const
{
    return (ClassNode *)getFirstChildWithTag(NED_CLASS);
}

StructNode *NedFileNode::getFirstStructChild() const
{
    return (StructNode *)getFirstChildWithTag(NED_STRUCT);
}

int ImportNode::getNumAttributes() const
{
    return 2;
}

const char *ImportNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "banner-comment";
        case 1: return "right-comment";
        default: return 0;
    }
}

const char *ImportNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return bannerComment.c_str();
        case 1: return rightComment.c_str();
        default: return 0;
    }
}

void ImportNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: bannerComment = val; break;
        case 1: rightComment = val; break;
        default: ;
    }
}

const char *ImportNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "\n";
        default: return 0;
    }
}

ImportNode *ImportNode::getNextImportNodeSibling() const
{
    return (ImportNode *)getNextSiblingWithTag(NED_IMPORT);
}

ImportedFileNode *ImportNode::getFirstImportedFileChild() const
{
    return (ImportedFileNode *)getFirstChildWithTag(NED_IMPORTED_FILE);
}

int ImportedFileNode::getNumAttributes() const
{
    return 3;
}

const char *ImportedFileNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "filename";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        default: return 0;
    }
}

const char *ImportedFileNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return filename.c_str();
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        default: return 0;
    }
}

void ImportedFileNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: filename = val; break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        default: ;
    }
}

const char *ImportedFileNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "\n";
        default: return 0;
    }
}

ImportedFileNode *ImportedFileNode::getNextImportedFileNodeSibling() const
{
    return (ImportedFileNode *)getNextSiblingWithTag(NED_IMPORTED_FILE);
}

int ChannelNode::getNumAttributes() const
{
    return 5;
}

const char *ChannelNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "source-code";
        case 2: return "banner-comment";
        case 3: return "right-comment";
        case 4: return "trailing-comment";
        default: return 0;
    }
}

const char *ChannelNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return sourceCode.c_str();
        case 2: return bannerComment.c_str();
        case 3: return rightComment.c_str();
        case 4: return trailingComment.c_str();
        default: return 0;
    }
}

void ChannelNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: sourceCode = val; break;
        case 2: bannerComment = val; break;
        case 3: rightComment = val; break;
        case 4: trailingComment = val; break;
        default: ;
    }
}

const char *ChannelNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "\n";
        case 4: return "\n";
        default: return 0;
    }
}

ChannelNode *ChannelNode::getNextChannelNodeSibling() const
{
    return (ChannelNode *)getNextSiblingWithTag(NED_CHANNEL);
}

ChannelAttrNode *ChannelNode::getFirstChannelAttrChild() const
{
    return (ChannelAttrNode *)getFirstChildWithTag(NED_CHANNEL_ATTR);
}

DisplayStringNode *ChannelNode::getFirstDisplayStringChild() const
{
    return (DisplayStringNode *)getFirstChildWithTag(NED_DISPLAY_STRING);
}

int ChannelAttrNode::getNumAttributes() const
{
    return 4;
}

const char *ChannelAttrNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "value";
        case 2: return "banner-comment";
        case 3: return "right-comment";
        default: return 0;
    }
}

const char *ChannelAttrNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return value.c_str();
        case 2: return bannerComment.c_str();
        case 3: return rightComment.c_str();
        default: return 0;
    }
}

void ChannelAttrNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: value = val; break;
        case 2: bannerComment = val; break;
        case 3: rightComment = val; break;
        default: ;
    }
}

const char *ChannelAttrNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "\n";
        default: return 0;
    }
}

ChannelAttrNode *ChannelAttrNode::getNextChannelAttrNodeSibling() const
{
    return (ChannelAttrNode *)getNextSiblingWithTag(NED_CHANNEL_ATTR);
}

ExpressionNode *ChannelAttrNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

int NetworkNode::getNumAttributes() const
{
    return 6;
}

const char *NetworkNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "type-name";
        case 2: return "source-code";
        case 3: return "banner-comment";
        case 4: return "right-comment";
        case 5: return "trailing-comment";
        default: return 0;
    }
}

const char *NetworkNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return typeName.c_str();
        case 2: return sourceCode.c_str();
        case 3: return bannerComment.c_str();
        case 4: return rightComment.c_str();
        case 5: return trailingComment.c_str();
        default: return 0;
    }
}

void NetworkNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: typeName = val; break;
        case 2: sourceCode = val; break;
        case 3: bannerComment = val; break;
        case 4: rightComment = val; break;
        case 5: trailingComment = val; break;
        default: ;
    }
}

const char *NetworkNode::getAttributeDefault(int k) const
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

NetworkNode *NetworkNode::getNextNetworkNodeSibling() const
{
    return (NetworkNode *)getNextSiblingWithTag(NED_NETWORK);
}

SubstmachinesNode *NetworkNode::getFirstSubstmachinesChild() const
{
    return (SubstmachinesNode *)getFirstChildWithTag(NED_SUBSTMACHINES);
}

SubstparamsNode *NetworkNode::getFirstSubstparamsChild() const
{
    return (SubstparamsNode *)getFirstChildWithTag(NED_SUBSTPARAMS);
}

int SimpleModuleNode::getNumAttributes() const
{
    return 5;
}

const char *SimpleModuleNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "source-code";
        case 2: return "banner-comment";
        case 3: return "right-comment";
        case 4: return "trailing-comment";
        default: return 0;
    }
}

const char *SimpleModuleNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return sourceCode.c_str();
        case 2: return bannerComment.c_str();
        case 3: return rightComment.c_str();
        case 4: return trailingComment.c_str();
        default: return 0;
    }
}

void SimpleModuleNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: sourceCode = val; break;
        case 2: bannerComment = val; break;
        case 3: rightComment = val; break;
        case 4: trailingComment = val; break;
        default: ;
    }
}

const char *SimpleModuleNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "\n";
        case 4: return "\n";
        default: return 0;
    }
}

SimpleModuleNode *SimpleModuleNode::getNextSimpleModuleNodeSibling() const
{
    return (SimpleModuleNode *)getNextSiblingWithTag(NED_SIMPLE_MODULE);
}

MachinesNode *SimpleModuleNode::getFirstMachinesChild() const
{
    return (MachinesNode *)getFirstChildWithTag(NED_MACHINES);
}

ParamsNode *SimpleModuleNode::getFirstParamsChild() const
{
    return (ParamsNode *)getFirstChildWithTag(NED_PARAMS);
}

GatesNode *SimpleModuleNode::getFirstGatesChild() const
{
    return (GatesNode *)getFirstChildWithTag(NED_GATES);
}

DisplayStringNode *SimpleModuleNode::getFirstDisplayStringChild() const
{
    return (DisplayStringNode *)getFirstChildWithTag(NED_DISPLAY_STRING);
}

int CompoundModuleNode::getNumAttributes() const
{
    return 5;
}

const char *CompoundModuleNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "source-code";
        case 2: return "banner-comment";
        case 3: return "right-comment";
        case 4: return "trailing-comment";
        default: return 0;
    }
}

const char *CompoundModuleNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return sourceCode.c_str();
        case 2: return bannerComment.c_str();
        case 3: return rightComment.c_str();
        case 4: return trailingComment.c_str();
        default: return 0;
    }
}

void CompoundModuleNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: sourceCode = val; break;
        case 2: bannerComment = val; break;
        case 3: rightComment = val; break;
        case 4: trailingComment = val; break;
        default: ;
    }
}

const char *CompoundModuleNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "\n";
        case 4: return "\n";
        default: return 0;
    }
}

CompoundModuleNode *CompoundModuleNode::getNextCompoundModuleNodeSibling() const
{
    return (CompoundModuleNode *)getNextSiblingWithTag(NED_COMPOUND_MODULE);
}

MachinesNode *CompoundModuleNode::getFirstMachinesChild() const
{
    return (MachinesNode *)getFirstChildWithTag(NED_MACHINES);
}

ParamsNode *CompoundModuleNode::getFirstParamsChild() const
{
    return (ParamsNode *)getFirstChildWithTag(NED_PARAMS);
}

GatesNode *CompoundModuleNode::getFirstGatesChild() const
{
    return (GatesNode *)getFirstChildWithTag(NED_GATES);
}

SubmodulesNode *CompoundModuleNode::getFirstSubmodulesChild() const
{
    return (SubmodulesNode *)getFirstChildWithTag(NED_SUBMODULES);
}

ConnectionsNode *CompoundModuleNode::getFirstConnectionsChild() const
{
    return (ConnectionsNode *)getFirstChildWithTag(NED_CONNECTIONS);
}

DisplayStringNode *CompoundModuleNode::getFirstDisplayStringChild() const
{
    return (DisplayStringNode *)getFirstChildWithTag(NED_DISPLAY_STRING);
}

int ParamsNode::getNumAttributes() const
{
    return 2;
}

const char *ParamsNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "banner-comment";
        case 1: return "right-comment";
        default: return 0;
    }
}

const char *ParamsNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return bannerComment.c_str();
        case 1: return rightComment.c_str();
        default: return 0;
    }
}

void ParamsNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: bannerComment = val; break;
        case 1: rightComment = val; break;
        default: ;
    }
}

const char *ParamsNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "\n";
        default: return 0;
    }
}

ParamsNode *ParamsNode::getNextParamsNodeSibling() const
{
    return (ParamsNode *)getNextSiblingWithTag(NED_PARAMS);
}

ParamNode *ParamsNode::getFirstParamChild() const
{
    return (ParamNode *)getFirstChildWithTag(NED_PARAM);
}

int ParamNode::getNumAttributes() const
{
    return 4;
}

const char *ParamNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "data-type";
        case 2: return "banner-comment";
        case 3: return "right-comment";
        default: return 0;
    }
}

const char *ParamNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return dataType.c_str();
        case 2: return bannerComment.c_str();
        case 3: return rightComment.c_str();
        default: return 0;
    }
}

void ParamNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: dataType = val; break;
        case 2: bannerComment = val; break;
        case 3: rightComment = val; break;
        default: ;
    }
}

const char *ParamNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "numeric";
        case 2: return "";
        case 3: return "\n";
        default: return 0;
    }
}

ParamNode *ParamNode::getNextParamNodeSibling() const
{
    return (ParamNode *)getNextSiblingWithTag(NED_PARAM);
}

int GatesNode::getNumAttributes() const
{
    return 2;
}

const char *GatesNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "banner-comment";
        case 1: return "right-comment";
        default: return 0;
    }
}

const char *GatesNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return bannerComment.c_str();
        case 1: return rightComment.c_str();
        default: return 0;
    }
}

void GatesNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: bannerComment = val; break;
        case 1: rightComment = val; break;
        default: ;
    }
}

const char *GatesNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "\n";
        default: return 0;
    }
}

GatesNode *GatesNode::getNextGatesNodeSibling() const
{
    return (GatesNode *)getNextSiblingWithTag(NED_GATES);
}

GateNode *GatesNode::getFirstGateChild() const
{
    return (GateNode *)getFirstChildWithTag(NED_GATE);
}

int GateNode::getNumAttributes() const
{
    return 5;
}

const char *GateNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "direction";
        case 2: return "is-vector";
        case 3: return "banner-comment";
        case 4: return "right-comment";
        default: return 0;
    }
}

const char *GateNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return enumToString(direction, io_vals, io_nums, io_n);
        case 2: return boolToString(isVector);
        case 3: return bannerComment.c_str();
        case 4: return rightComment.c_str();
        default: return 0;
    }
}

void GateNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: direction = stringToEnum(val, io_vals, io_nums, io_n); break;
        case 2: isVector = stringToBool(val); break;
        case 3: bannerComment = val; break;
        case 4: rightComment = val; break;
        default: ;
    }
}

const char *GateNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "false";
        case 3: return "";
        case 4: return "\n";
        default: return 0;
    }
}

GateNode *GateNode::getNextGateNodeSibling() const
{
    return (GateNode *)getNextSiblingWithTag(NED_GATE);
}

int MachinesNode::getNumAttributes() const
{
    return 2;
}

const char *MachinesNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "banner-comment";
        case 1: return "right-comment";
        default: return 0;
    }
}

const char *MachinesNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return bannerComment.c_str();
        case 1: return rightComment.c_str();
        default: return 0;
    }
}

void MachinesNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: bannerComment = val; break;
        case 1: rightComment = val; break;
        default: ;
    }
}

const char *MachinesNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "\n";
        default: return 0;
    }
}

MachinesNode *MachinesNode::getNextMachinesNodeSibling() const
{
    return (MachinesNode *)getNextSiblingWithTag(NED_MACHINES);
}

MachineNode *MachinesNode::getFirstMachineChild() const
{
    return (MachineNode *)getFirstChildWithTag(NED_MACHINE);
}

int MachineNode::getNumAttributes() const
{
    return 3;
}

const char *MachineNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        default: return 0;
    }
}

const char *MachineNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        default: return 0;
    }
}

void MachineNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        default: ;
    }
}

const char *MachineNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "\n";
        default: return 0;
    }
}

MachineNode *MachineNode::getNextMachineNodeSibling() const
{
    return (MachineNode *)getNextSiblingWithTag(NED_MACHINE);
}

int SubmodulesNode::getNumAttributes() const
{
    return 2;
}

const char *SubmodulesNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "banner-comment";
        case 1: return "right-comment";
        default: return 0;
    }
}

const char *SubmodulesNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return bannerComment.c_str();
        case 1: return rightComment.c_str();
        default: return 0;
    }
}

void SubmodulesNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: bannerComment = val; break;
        case 1: rightComment = val; break;
        default: ;
    }
}

const char *SubmodulesNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "\n";
        default: return 0;
    }
}

SubmodulesNode *SubmodulesNode::getNextSubmodulesNodeSibling() const
{
    return (SubmodulesNode *)getNextSiblingWithTag(NED_SUBMODULES);
}

SubmoduleNode *SubmodulesNode::getFirstSubmoduleChild() const
{
    return (SubmoduleNode *)getFirstChildWithTag(NED_SUBMODULE);
}

int SubmoduleNode::getNumAttributes() const
{
    return 6;
}

const char *SubmoduleNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "type-name";
        case 2: return "like-param";
        case 3: return "vector-size";
        case 4: return "banner-comment";
        case 5: return "right-comment";
        default: return 0;
    }
}

const char *SubmoduleNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return typeName.c_str();
        case 2: return likeParam.c_str();
        case 3: return vectorSize.c_str();
        case 4: return bannerComment.c_str();
        case 5: return rightComment.c_str();
        default: return 0;
    }
}

void SubmoduleNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: typeName = val; break;
        case 2: likeParam = val; break;
        case 3: vectorSize = val; break;
        case 4: bannerComment = val; break;
        case 5: rightComment = val; break;
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
        case 5: return "\n";
        default: return 0;
    }
}

SubmoduleNode *SubmoduleNode::getNextSubmoduleNodeSibling() const
{
    return (SubmoduleNode *)getNextSiblingWithTag(NED_SUBMODULE);
}

ExpressionNode *SubmoduleNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

SubstmachinesNode *SubmoduleNode::getFirstSubstmachinesChild() const
{
    return (SubstmachinesNode *)getFirstChildWithTag(NED_SUBSTMACHINES);
}

SubstparamsNode *SubmoduleNode::getFirstSubstparamsChild() const
{
    return (SubstparamsNode *)getFirstChildWithTag(NED_SUBSTPARAMS);
}

GatesizesNode *SubmoduleNode::getFirstGatesizesChild() const
{
    return (GatesizesNode *)getFirstChildWithTag(NED_GATESIZES);
}

DisplayStringNode *SubmoduleNode::getFirstDisplayStringChild() const
{
    return (DisplayStringNode *)getFirstChildWithTag(NED_DISPLAY_STRING);
}

int SubstparamsNode::getNumAttributes() const
{
    return 3;
}

const char *SubstparamsNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "condition";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        default: return 0;
    }
}

const char *SubstparamsNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return condition.c_str();
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        default: return 0;
    }
}

void SubstparamsNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: condition = val; break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        default: ;
    }
}

const char *SubstparamsNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "\n";
        default: return 0;
    }
}

SubstparamsNode *SubstparamsNode::getNextSubstparamsNodeSibling() const
{
    return (SubstparamsNode *)getNextSiblingWithTag(NED_SUBSTPARAMS);
}

ExpressionNode *SubstparamsNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

SubstparamNode *SubstparamsNode::getFirstSubstparamChild() const
{
    return (SubstparamNode *)getFirstChildWithTag(NED_SUBSTPARAM);
}

int SubstparamNode::getNumAttributes() const
{
    return 4;
}

const char *SubstparamNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "value";
        case 2: return "banner-comment";
        case 3: return "right-comment";
        default: return 0;
    }
}

const char *SubstparamNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return value.c_str();
        case 2: return bannerComment.c_str();
        case 3: return rightComment.c_str();
        default: return 0;
    }
}

void SubstparamNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: value = val; break;
        case 2: bannerComment = val; break;
        case 3: rightComment = val; break;
        default: ;
    }
}

const char *SubstparamNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "\n";
        default: return 0;
    }
}

SubstparamNode *SubstparamNode::getNextSubstparamNodeSibling() const
{
    return (SubstparamNode *)getNextSiblingWithTag(NED_SUBSTPARAM);
}

ExpressionNode *SubstparamNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

int GatesizesNode::getNumAttributes() const
{
    return 3;
}

const char *GatesizesNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "condition";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        default: return 0;
    }
}

const char *GatesizesNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return condition.c_str();
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        default: return 0;
    }
}

void GatesizesNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: condition = val; break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        default: ;
    }
}

const char *GatesizesNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "\n";
        default: return 0;
    }
}

GatesizesNode *GatesizesNode::getNextGatesizesNodeSibling() const
{
    return (GatesizesNode *)getNextSiblingWithTag(NED_GATESIZES);
}

ExpressionNode *GatesizesNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

GatesizeNode *GatesizesNode::getFirstGatesizeChild() const
{
    return (GatesizeNode *)getFirstChildWithTag(NED_GATESIZE);
}

int GatesizeNode::getNumAttributes() const
{
    return 4;
}

const char *GatesizeNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "vector-size";
        case 2: return "banner-comment";
        case 3: return "right-comment";
        default: return 0;
    }
}

const char *GatesizeNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return vectorSize.c_str();
        case 2: return bannerComment.c_str();
        case 3: return rightComment.c_str();
        default: return 0;
    }
}

void GatesizeNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: vectorSize = val; break;
        case 2: bannerComment = val; break;
        case 3: rightComment = val; break;
        default: ;
    }
}

const char *GatesizeNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "\n";
        default: return 0;
    }
}

GatesizeNode *GatesizeNode::getNextGatesizeNodeSibling() const
{
    return (GatesizeNode *)getNextSiblingWithTag(NED_GATESIZE);
}

ExpressionNode *GatesizeNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

int SubstmachinesNode::getNumAttributes() const
{
    return 3;
}

const char *SubstmachinesNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "condition";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        default: return 0;
    }
}

const char *SubstmachinesNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return condition.c_str();
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        default: return 0;
    }
}

void SubstmachinesNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: condition = val; break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        default: ;
    }
}

const char *SubstmachinesNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "\n";
        default: return 0;
    }
}

SubstmachinesNode *SubstmachinesNode::getNextSubstmachinesNodeSibling() const
{
    return (SubstmachinesNode *)getNextSiblingWithTag(NED_SUBSTMACHINES);
}

ExpressionNode *SubstmachinesNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

SubstmachineNode *SubstmachinesNode::getFirstSubstmachineChild() const
{
    return (SubstmachineNode *)getFirstChildWithTag(NED_SUBSTMACHINE);
}

int SubstmachineNode::getNumAttributes() const
{
    return 3;
}

const char *SubstmachineNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        default: return 0;
    }
}

const char *SubstmachineNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        default: return 0;
    }
}

void SubstmachineNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        default: ;
    }
}

const char *SubstmachineNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "\n";
        default: return 0;
    }
}

SubstmachineNode *SubstmachineNode::getNextSubstmachineNodeSibling() const
{
    return (SubstmachineNode *)getNextSiblingWithTag(NED_SUBSTMACHINE);
}

int ConnectionsNode::getNumAttributes() const
{
    return 3;
}

const char *ConnectionsNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "check-unconnected";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        default: return 0;
    }
}

const char *ConnectionsNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return boolToString(checkUnconnected);
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        default: return 0;
    }
}

void ConnectionsNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: checkUnconnected = stringToBool(val); break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        default: ;
    }
}

const char *ConnectionsNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "true";
        case 1: return "";
        case 2: return "\n";
        default: return 0;
    }
}

ConnectionsNode *ConnectionsNode::getNextConnectionsNodeSibling() const
{
    return (ConnectionsNode *)getNextSiblingWithTag(NED_CONNECTIONS);
}

ConnectionNode *ConnectionsNode::getFirstConnectionChild() const
{
    return (ConnectionNode *)getFirstChildWithTag(NED_CONNECTION);
}

ForLoopNode *ConnectionsNode::getFirstForLoopChild() const
{
    return (ForLoopNode *)getFirstChildWithTag(NED_FOR_LOOP);
}

int ConnectionNode::getNumAttributes() const
{
    return 14;
}

const char *ConnectionNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "condition";
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
        case 11: return "arrow-direction";
        case 12: return "banner-comment";
        case 13: return "right-comment";
        default: return 0;
    }
}

const char *ConnectionNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return condition.c_str();
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
        case 11: return enumToString(arrowDirection, lr_vals, lr_nums, lr_n);
        case 12: return bannerComment.c_str();
        case 13: return rightComment.c_str();
        default: return 0;
    }
}

void ConnectionNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: condition = val; break;
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
        case 11: arrowDirection = stringToEnum(val, lr_vals, lr_nums, lr_n); break;
        case 12: bannerComment = val; break;
        case 13: rightComment = val; break;
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
        case 11: return "right";
        case 12: return "";
        case 13: return "\n";
        default: return 0;
    }
}

ConnectionNode *ConnectionNode::getNextConnectionNodeSibling() const
{
    return (ConnectionNode *)getNextSiblingWithTag(NED_CONNECTION);
}

ExpressionNode *ConnectionNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

ConnAttrNode *ConnectionNode::getFirstConnAttrChild() const
{
    return (ConnAttrNode *)getFirstChildWithTag(NED_CONN_ATTR);
}

DisplayStringNode *ConnectionNode::getFirstDisplayStringChild() const
{
    return (DisplayStringNode *)getFirstChildWithTag(NED_DISPLAY_STRING);
}

int ConnAttrNode::getNumAttributes() const
{
    return 3;
}

const char *ConnAttrNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "value";
        case 2: return "right-comment";
        default: return 0;
    }
}

const char *ConnAttrNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return value.c_str();
        case 2: return rightComment.c_str();
        default: return 0;
    }
}

void ConnAttrNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: value = val; break;
        case 2: rightComment = val; break;
        default: ;
    }
}

const char *ConnAttrNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

ConnAttrNode *ConnAttrNode::getNextConnAttrNodeSibling() const
{
    return (ConnAttrNode *)getNextSiblingWithTag(NED_CONN_ATTR);
}

ExpressionNode *ConnAttrNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

int ForLoopNode::getNumAttributes() const
{
    return 3;
}

const char *ForLoopNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "banner-comment";
        case 1: return "right-comment";
        case 2: return "trailing-comment";
        default: return 0;
    }
}

const char *ForLoopNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return bannerComment.c_str();
        case 1: return rightComment.c_str();
        case 2: return trailingComment.c_str();
        default: return 0;
    }
}

void ForLoopNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: bannerComment = val; break;
        case 1: rightComment = val; break;
        case 2: trailingComment = val; break;
        default: ;
    }
}

const char *ForLoopNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "\n";
        case 2: return "\n";
        default: return 0;
    }
}

ForLoopNode *ForLoopNode::getNextForLoopNodeSibling() const
{
    return (ForLoopNode *)getNextSiblingWithTag(NED_FOR_LOOP);
}

LoopVarNode *ForLoopNode::getFirstLoopVarChild() const
{
    return (LoopVarNode *)getFirstChildWithTag(NED_LOOP_VAR);
}

ConnectionNode *ForLoopNode::getFirstConnectionChild() const
{
    return (ConnectionNode *)getFirstChildWithTag(NED_CONNECTION);
}

int LoopVarNode::getNumAttributes() const
{
    return 5;
}

const char *LoopVarNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "param-name";
        case 1: return "from-value";
        case 2: return "to-value";
        case 3: return "banner-comment";
        case 4: return "right-comment";
        default: return 0;
    }
}

const char *LoopVarNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return paramName.c_str();
        case 1: return fromValue.c_str();
        case 2: return toValue.c_str();
        case 3: return bannerComment.c_str();
        case 4: return rightComment.c_str();
        default: return 0;
    }
}

void LoopVarNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: paramName = val; break;
        case 1: fromValue = val; break;
        case 2: toValue = val; break;
        case 3: bannerComment = val; break;
        case 4: rightComment = val; break;
        default: ;
    }
}

const char *LoopVarNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "";
        case 4: return "\n";
        default: return 0;
    }
}

LoopVarNode *LoopVarNode::getNextLoopVarNodeSibling() const
{
    return (LoopVarNode *)getNextSiblingWithTag(NED_LOOP_VAR);
}

ExpressionNode *LoopVarNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

int DisplayStringNode::getNumAttributes() const
{
    return 3;
}

const char *DisplayStringNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "value";
        case 1: return "banner-comment";
        case 2: return "right-comment";
        default: return 0;
    }
}

const char *DisplayStringNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return value.c_str();
        case 1: return bannerComment.c_str();
        case 2: return rightComment.c_str();
        default: return 0;
    }
}

void DisplayStringNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: value = val; break;
        case 1: bannerComment = val; break;
        case 2: rightComment = val; break;
        default: ;
    }
}

const char *DisplayStringNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "\n";
        default: return 0;
    }
}

DisplayStringNode *DisplayStringNode::getNextDisplayStringNodeSibling() const
{
    return (DisplayStringNode *)getNextSiblingWithTag(NED_DISPLAY_STRING);
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

OperatorNode *ExpressionNode::getFirstOperatorChild() const
{
    return (OperatorNode *)getFirstChildWithTag(NED_OPERATOR);
}

FunctionNode *ExpressionNode::getFirstFunctionChild() const
{
    return (FunctionNode *)getFirstChildWithTag(NED_FUNCTION);
}

ParamRefNode *ExpressionNode::getFirstParamRefChild() const
{
    return (ParamRefNode *)getFirstChildWithTag(NED_PARAM_REF);
}

IdentNode *ExpressionNode::getFirstIdentChild() const
{
    return (IdentNode *)getFirstChildWithTag(NED_IDENT);
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

OperatorNode *OperatorNode::getFirstOperatorChild() const
{
    return (OperatorNode *)getFirstChildWithTag(NED_OPERATOR);
}

FunctionNode *OperatorNode::getFirstFunctionChild() const
{
    return (FunctionNode *)getFirstChildWithTag(NED_FUNCTION);
}

ParamRefNode *OperatorNode::getFirstParamRefChild() const
{
    return (ParamRefNode *)getFirstChildWithTag(NED_PARAM_REF);
}

IdentNode *OperatorNode::getFirstIdentChild() const
{
    return (IdentNode *)getFirstChildWithTag(NED_IDENT);
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

OperatorNode *FunctionNode::getFirstOperatorChild() const
{
    return (OperatorNode *)getFirstChildWithTag(NED_OPERATOR);
}

FunctionNode *FunctionNode::getFirstFunctionChild() const
{
    return (FunctionNode *)getFirstChildWithTag(NED_FUNCTION);
}

ParamRefNode *FunctionNode::getFirstParamRefChild() const
{
    return (ParamRefNode *)getFirstChildWithTag(NED_PARAM_REF);
}

IdentNode *FunctionNode::getFirstIdentChild() const
{
    return (IdentNode *)getFirstChildWithTag(NED_IDENT);
}

ConstNode *FunctionNode::getFirstConstChild() const
{
    return (ConstNode *)getFirstChildWithTag(NED_CONST);
}

int ParamRefNode::getNumAttributes() const
{
    return 6;
}

const char *ParamRefNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "module";
        case 1: return "module-index";
        case 2: return "param-name";
        case 3: return "param-index";
        case 4: return "is-ref";
        case 5: return "is-ancestor";
        default: return 0;
    }
}

const char *ParamRefNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return module.c_str();
        case 1: return moduleIndex.c_str();
        case 2: return paramName.c_str();
        case 3: return paramIndex.c_str();
        case 4: return boolToString(isRef);
        case 5: return boolToString(isAncestor);
        default: return 0;
    }
}

void ParamRefNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: module = val; break;
        case 1: moduleIndex = val; break;
        case 2: paramName = val; break;
        case 3: paramIndex = val; break;
        case 4: isRef = stringToBool(val); break;
        case 5: isAncestor = stringToBool(val); break;
        default: ;
    }
}

const char *ParamRefNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "";
        case 4: return "false";
        case 5: return "false";
        default: return 0;
    }
}

ParamRefNode *ParamRefNode::getNextParamRefNodeSibling() const
{
    return (ParamRefNode *)getNextSiblingWithTag(NED_PARAM_REF);
}

ExpressionNode *ParamRefNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

int IdentNode::getNumAttributes() const
{
    return 1;
}

const char *IdentNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *IdentNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void IdentNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *IdentNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

IdentNode *IdentNode::getNextIdentNodeSibling() const
{
    return (IdentNode *)getNextSiblingWithTag(NED_IDENT);
}

int ConstNode::getNumAttributes() const
{
    return 3;
}

const char *ConstNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "type";
        case 1: return "text";
        case 2: return "value";
        default: return 0;
    }
}

const char *ConstNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return enumToString(type, type_vals, type_nums, type_n);
        case 1: return text.c_str();
        case 2: return value.c_str();
        default: return 0;
    }
}

void ConstNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: type = stringToEnum(val, type_vals, type_nums, type_n); break;
        case 1: text = val; break;
        case 2: value = val; break;
        default: ;
    }
}

const char *ConstNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

ConstNode *ConstNode::getNextConstNodeSibling() const
{
    return (ConstNode *)getNextSiblingWithTag(NED_CONST);
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

PropertyNode *PropertiesNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

int PropertyNode::getNumAttributes() const
{
    return 4;
}

const char *PropertyNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "value";
        case 2: return "banner-comment";
        case 3: return "right-comment";
        default: return 0;
    }
}

const char *PropertyNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return value.c_str();
        case 2: return bannerComment.c_str();
        case 3: return rightComment.c_str();
        default: return 0;
    }
}

void PropertyNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: value = val; break;
        case 2: bannerComment = val; break;
        case 3: rightComment = val; break;
        default: ;
    }
}

const char *PropertyNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        case 3: return "\n";
        default: return 0;
    }
}

PropertyNode *PropertyNode::getNextPropertyNodeSibling() const
{
    return (PropertyNode *)getNextSiblingWithTag(NED_PROPERTY);
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
    if (tagname[0]=='n' && !strcmp(tagname,"ned-files"))  return new NedFilesNode();
    if (tagname[0]=='n' && !strcmp(tagname,"ned-file"))  return new NedFileNode();
    if (tagname[0]=='i' && !strcmp(tagname,"import"))  return new ImportNode();
    if (tagname[0]=='i' && !strcmp(tagname,"imported-file"))  return new ImportedFileNode();
    if (tagname[0]=='c' && !strcmp(tagname,"channel"))  return new ChannelNode();
    if (tagname[0]=='c' && !strcmp(tagname,"channel-attr"))  return new ChannelAttrNode();
    if (tagname[0]=='n' && !strcmp(tagname,"network"))  return new NetworkNode();
    if (tagname[0]=='s' && !strcmp(tagname,"simple-module"))  return new SimpleModuleNode();
    if (tagname[0]=='c' && !strcmp(tagname,"compound-module"))  return new CompoundModuleNode();
    if (tagname[0]=='p' && !strcmp(tagname,"params"))  return new ParamsNode();
    if (tagname[0]=='p' && !strcmp(tagname,"param"))  return new ParamNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gates"))  return new GatesNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gate"))  return new GateNode();
    if (tagname[0]=='m' && !strcmp(tagname,"machines"))  return new MachinesNode();
    if (tagname[0]=='m' && !strcmp(tagname,"machine"))  return new MachineNode();
    if (tagname[0]=='s' && !strcmp(tagname,"submodules"))  return new SubmodulesNode();
    if (tagname[0]=='s' && !strcmp(tagname,"submodule"))  return new SubmoduleNode();
    if (tagname[0]=='s' && !strcmp(tagname,"substparams"))  return new SubstparamsNode();
    if (tagname[0]=='s' && !strcmp(tagname,"substparam"))  return new SubstparamNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gatesizes"))  return new GatesizesNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gatesize"))  return new GatesizeNode();
    if (tagname[0]=='s' && !strcmp(tagname,"substmachines"))  return new SubstmachinesNode();
    if (tagname[0]=='s' && !strcmp(tagname,"substmachine"))  return new SubstmachineNode();
    if (tagname[0]=='c' && !strcmp(tagname,"connections"))  return new ConnectionsNode();
    if (tagname[0]=='c' && !strcmp(tagname,"connection"))  return new ConnectionNode();
    if (tagname[0]=='c' && !strcmp(tagname,"conn-attr"))  return new ConnAttrNode();
    if (tagname[0]=='f' && !strcmp(tagname,"for-loop"))  return new ForLoopNode();
    if (tagname[0]=='l' && !strcmp(tagname,"loop-var"))  return new LoopVarNode();
    if (tagname[0]=='d' && !strcmp(tagname,"display-string"))  return new DisplayStringNode();
    if (tagname[0]=='e' && !strcmp(tagname,"expression"))  return new ExpressionNode();
    if (tagname[0]=='o' && !strcmp(tagname,"operator"))  return new OperatorNode();
    if (tagname[0]=='f' && !strcmp(tagname,"function"))  return new FunctionNode();
    if (tagname[0]=='p' && !strcmp(tagname,"param-ref"))  return new ParamRefNode();
    if (tagname[0]=='i' && !strcmp(tagname,"ident"))  return new IdentNode();
    if (tagname[0]=='c' && !strcmp(tagname,"const"))  return new ConstNode();
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
    if (tagname[0]=='p' && !strcmp(tagname,"property"))  return new PropertyNode();
    if (tagname[0]=='u' && !strcmp(tagname,"unknown"))  return new UnknownNode();
    throw new NEDException("unknown tag '%s', cannot create object to represent it", tagname);
}

NEDElement *NEDElementFactory::createNodeWithTag(int tagcode)
{
    switch (tagcode) {
        case NED_NED_FILES: return new NedFilesNode();
        case NED_NED_FILE: return new NedFileNode();
        case NED_IMPORT: return new ImportNode();
        case NED_IMPORTED_FILE: return new ImportedFileNode();
        case NED_CHANNEL: return new ChannelNode();
        case NED_CHANNEL_ATTR: return new ChannelAttrNode();
        case NED_NETWORK: return new NetworkNode();
        case NED_SIMPLE_MODULE: return new SimpleModuleNode();
        case NED_COMPOUND_MODULE: return new CompoundModuleNode();
        case NED_PARAMS: return new ParamsNode();
        case NED_PARAM: return new ParamNode();
        case NED_GATES: return new GatesNode();
        case NED_GATE: return new GateNode();
        case NED_MACHINES: return new MachinesNode();
        case NED_MACHINE: return new MachineNode();
        case NED_SUBMODULES: return new SubmodulesNode();
        case NED_SUBMODULE: return new SubmoduleNode();
        case NED_SUBSTPARAMS: return new SubstparamsNode();
        case NED_SUBSTPARAM: return new SubstparamNode();
        case NED_GATESIZES: return new GatesizesNode();
        case NED_GATESIZE: return new GatesizeNode();
        case NED_SUBSTMACHINES: return new SubstmachinesNode();
        case NED_SUBSTMACHINE: return new SubstmachineNode();
        case NED_CONNECTIONS: return new ConnectionsNode();
        case NED_CONNECTION: return new ConnectionNode();
        case NED_CONN_ATTR: return new ConnAttrNode();
        case NED_FOR_LOOP: return new ForLoopNode();
        case NED_LOOP_VAR: return new LoopVarNode();
        case NED_DISPLAY_STRING: return new DisplayStringNode();
        case NED_EXPRESSION: return new ExpressionNode();
        case NED_OPERATOR: return new OperatorNode();
        case NED_FUNCTION: return new FunctionNode();
        case NED_PARAM_REF: return new ParamRefNode();
        case NED_IDENT: return new IdentNode();
        case NED_CONST: return new ConstNode();
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
        case NED_PROPERTY: return new PropertyNode();
        case NED_UNKNOWN: return new UnknownNode();
    }
    throw new NEDException("unknown tag code %d, cannot create object to represent it", tagcode);
}

