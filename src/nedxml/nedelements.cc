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

NAMESPACE_BEGIN

static const char *gatetype_vals[] = {"", "input", "output", "inout"};
static int gatetype_nums[] = {NED_GATETYPE_NONE, NED_GATETYPE_INPUT, NED_GATETYPE_OUTPUT, NED_GATETYPE_INOUT};
static const int gatetype_n = 4;

static const char *arrowdir_vals[] = {"l2r", "r2l", "bidir"};
static int arrowdir_nums[] = {NED_ARROWDIR_L2R, NED_ARROWDIR_R2L, NED_ARROWDIR_BIDIR};
static const int arrowdir_n = 3;

static const char *partype_vals[] = {"", "double", "int", "string", "bool", "xml"};
static int partype_nums[] = {NED_PARTYPE_NONE, NED_PARTYPE_DOUBLE, NED_PARTYPE_INT, NED_PARTYPE_STRING, NED_PARTYPE_BOOL, NED_PARTYPE_XML};
static const int partype_n = 6;

static const char *littype_vals[] = {"double", "int", "string", "bool", "spec"};
static int littype_nums[] = {NED_CONST_DOUBLE, NED_CONST_INT, NED_CONST_STRING, NED_CONST_BOOL, NED_CONST_SPEC};
static const int littype_n = 5;

static const char *subgate_vals[] = {"", "i", "o"};
static int subgate_nums[] = {NED_SUBGATE_NONE, NED_SUBGATE_I, NED_SUBGATE_O};
static const int subgate_n = 3;

FilesNode::FilesNode()
{
    applyDefaults();
}

FilesNode::FilesNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

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

FilesNode *FilesNode::dup() const
{
    FilesNode *newNode = new FilesNode();
    return newNode;
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

NedFileNode::NedFileNode()
{
    applyDefaults();
}

NedFileNode::NedFileNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int NedFileNode::getNumAttributes() const
{
    return 2;
}

const char *NedFileNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "filename";
        case 1: return "version";
        default: return 0;
    }
}

const char *NedFileNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return filename.c_str();
        case 1: return version.c_str();
        default: return 0;
    }
}

void NedFileNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: filename = val; break;
        case 1: version = val; break;
        default: ;
    }
}

const char *NedFileNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "2";
        default: return 0;
    }
}

NedFileNode *NedFileNode::dup() const
{
    NedFileNode *newNode = new NedFileNode();
    newNode->filename = this->filename;
    newNode->version = this->version;
    return newNode;
}

NedFileNode *NedFileNode::getNextNedFileNodeSibling() const
{
    return (NedFileNode *)getNextSiblingWithTag(NED_NED_FILE);
}

CommentNode *NedFileNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

PackageNode *NedFileNode::getFirstPackageChild() const
{
    return (PackageNode *)getFirstChildWithTag(NED_PACKAGE);
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

CommentNode::CommentNode()
{
    applyDefaults();
}

CommentNode::CommentNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int CommentNode::getNumAttributes() const
{
    return 2;
}

const char *CommentNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "locid";
        case 1: return "content";
        default: return 0;
    }
}

const char *CommentNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return locid.c_str();
        case 1: return content.c_str();
        default: return 0;
    }
}

void CommentNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: locid = val; break;
        case 1: content = val; break;
        default: ;
    }
}

const char *CommentNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        default: return 0;
    }
}

CommentNode *CommentNode::dup() const
{
    CommentNode *newNode = new CommentNode();
    newNode->locid = this->locid;
    newNode->content = this->content;
    return newNode;
}

CommentNode *CommentNode::getNextCommentNodeSibling() const
{
    return (CommentNode *)getNextSiblingWithTag(NED_COMMENT);
}

PackageNode::PackageNode()
{
    applyDefaults();
}

PackageNode::PackageNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int PackageNode::getNumAttributes() const
{
    return 1;
}

const char *PackageNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *PackageNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void PackageNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *PackageNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

PackageNode *PackageNode::dup() const
{
    PackageNode *newNode = new PackageNode();
    newNode->name = this->name;
    return newNode;
}

PackageNode *PackageNode::getNextPackageNodeSibling() const
{
    return (PackageNode *)getNextSiblingWithTag(NED_PACKAGE);
}

CommentNode *PackageNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

ImportNode::ImportNode()
{
    applyDefaults();
}

ImportNode::ImportNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ImportNode::getNumAttributes() const
{
    return 1;
}

const char *ImportNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "import-spec";
        default: return 0;
    }
}

const char *ImportNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return importSpec.c_str();
        default: return 0;
    }
}

void ImportNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: importSpec = val; break;
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

ImportNode *ImportNode::dup() const
{
    ImportNode *newNode = new ImportNode();
    newNode->importSpec = this->importSpec;
    return newNode;
}

ImportNode *ImportNode::getNextImportNodeSibling() const
{
    return (ImportNode *)getNextSiblingWithTag(NED_IMPORT);
}

CommentNode *ImportNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

PropertyDeclNode::PropertyDeclNode()
{
    isArray = false;
    applyDefaults();
}

PropertyDeclNode::PropertyDeclNode(NEDElement *parent) : NEDElement(parent)
{
    isArray = false;
    applyDefaults();
}

int PropertyDeclNode::getNumAttributes() const
{
    return 2;
}

const char *PropertyDeclNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "is-array";
        default: return 0;
    }
}

const char *PropertyDeclNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return boolToString(isArray);
        default: return 0;
    }
}

void PropertyDeclNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: isArray = stringToBool(val); break;
        default: ;
    }
}

const char *PropertyDeclNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "false";
        default: return 0;
    }
}

PropertyDeclNode *PropertyDeclNode::dup() const
{
    PropertyDeclNode *newNode = new PropertyDeclNode();
    newNode->name = this->name;
    newNode->isArray = this->isArray;
    return newNode;
}

PropertyDeclNode *PropertyDeclNode::getNextPropertyDeclNodeSibling() const
{
    return (PropertyDeclNode *)getNextSiblingWithTag(NED_PROPERTY_DECL);
}

CommentNode *PropertyDeclNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

PropertyKeyNode *PropertyDeclNode::getFirstPropertyKeyChild() const
{
    return (PropertyKeyNode *)getFirstChildWithTag(NED_PROPERTY_KEY);
}

PropertyNode *PropertyDeclNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

ExtendsNode::ExtendsNode()
{
    applyDefaults();
}

ExtendsNode::ExtendsNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

ExtendsNode *ExtendsNode::dup() const
{
    ExtendsNode *newNode = new ExtendsNode();
    newNode->name = this->name;
    return newNode;
}

ExtendsNode *ExtendsNode::getNextExtendsNodeSibling() const
{
    return (ExtendsNode *)getNextSiblingWithTag(NED_EXTENDS);
}

CommentNode *ExtendsNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

InterfaceNameNode::InterfaceNameNode()
{
    applyDefaults();
}

InterfaceNameNode::InterfaceNameNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

InterfaceNameNode *InterfaceNameNode::dup() const
{
    InterfaceNameNode *newNode = new InterfaceNameNode();
    newNode->name = this->name;
    return newNode;
}

InterfaceNameNode *InterfaceNameNode::getNextInterfaceNameNodeSibling() const
{
    return (InterfaceNameNode *)getNextSiblingWithTag(NED_INTERFACE_NAME);
}

CommentNode *InterfaceNameNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

SimpleModuleNode::SimpleModuleNode()
{
    isNetwork = false;
    applyDefaults();
}

SimpleModuleNode::SimpleModuleNode(NEDElement *parent) : NEDElement(parent)
{
    isNetwork = false;
    applyDefaults();
}

int SimpleModuleNode::getNumAttributes() const
{
    return 2;
}

const char *SimpleModuleNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "is-network";
        default: return 0;
    }
}

const char *SimpleModuleNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return boolToString(isNetwork);
        default: return 0;
    }
}

void SimpleModuleNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: isNetwork = stringToBool(val); break;
        default: ;
    }
}

const char *SimpleModuleNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "false";
        default: return 0;
    }
}

SimpleModuleNode *SimpleModuleNode::dup() const
{
    SimpleModuleNode *newNode = new SimpleModuleNode();
    newNode->name = this->name;
    newNode->isNetwork = this->isNetwork;
    return newNode;
}

SimpleModuleNode *SimpleModuleNode::getNextSimpleModuleNodeSibling() const
{
    return (SimpleModuleNode *)getNextSiblingWithTag(NED_SIMPLE_MODULE);
}

CommentNode *SimpleModuleNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

ModuleInterfaceNode::ModuleInterfaceNode()
{
    applyDefaults();
}

ModuleInterfaceNode::ModuleInterfaceNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

ModuleInterfaceNode *ModuleInterfaceNode::dup() const
{
    ModuleInterfaceNode *newNode = new ModuleInterfaceNode();
    newNode->name = this->name;
    return newNode;
}

ModuleInterfaceNode *ModuleInterfaceNode::getNextModuleInterfaceNodeSibling() const
{
    return (ModuleInterfaceNode *)getNextSiblingWithTag(NED_MODULE_INTERFACE);
}

CommentNode *ModuleInterfaceNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

CompoundModuleNode::CompoundModuleNode()
{
    isNetwork = false;
    applyDefaults();
}

CompoundModuleNode::CompoundModuleNode(NEDElement *parent) : NEDElement(parent)
{
    isNetwork = false;
    applyDefaults();
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

CompoundModuleNode *CompoundModuleNode::dup() const
{
    CompoundModuleNode *newNode = new CompoundModuleNode();
    newNode->name = this->name;
    newNode->isNetwork = this->isNetwork;
    return newNode;
}

CompoundModuleNode *CompoundModuleNode::getNextCompoundModuleNodeSibling() const
{
    return (CompoundModuleNode *)getNextSiblingWithTag(NED_COMPOUND_MODULE);
}

CommentNode *CompoundModuleNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

ChannelInterfaceNode::ChannelInterfaceNode()
{
    applyDefaults();
}

ChannelInterfaceNode::ChannelInterfaceNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

ChannelInterfaceNode *ChannelInterfaceNode::dup() const
{
    ChannelInterfaceNode *newNode = new ChannelInterfaceNode();
    newNode->name = this->name;
    return newNode;
}

ChannelInterfaceNode *ChannelInterfaceNode::getNextChannelInterfaceNodeSibling() const
{
    return (ChannelInterfaceNode *)getNextSiblingWithTag(NED_CHANNEL_INTERFACE);
}

CommentNode *ChannelInterfaceNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

ExtendsNode *ChannelInterfaceNode::getFirstExtendsChild() const
{
    return (ExtendsNode *)getFirstChildWithTag(NED_EXTENDS);
}

ParametersNode *ChannelInterfaceNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

ChannelNode::ChannelNode()
{
    applyDefaults();
}

ChannelNode::ChannelNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

ChannelNode *ChannelNode::dup() const
{
    ChannelNode *newNode = new ChannelNode();
    newNode->name = this->name;
    return newNode;
}

ChannelNode *ChannelNode::getNextChannelNodeSibling() const
{
    return (ChannelNode *)getNextSiblingWithTag(NED_CHANNEL);
}

CommentNode *ChannelNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

ParametersNode::ParametersNode()
{
    isImplicit = false;
    applyDefaults();
}

ParametersNode::ParametersNode(NEDElement *parent) : NEDElement(parent)
{
    isImplicit = false;
    applyDefaults();
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

ParametersNode *ParametersNode::dup() const
{
    ParametersNode *newNode = new ParametersNode();
    newNode->isImplicit = this->isImplicit;
    return newNode;
}

ParametersNode *ParametersNode::getNextParametersNodeSibling() const
{
    return (ParametersNode *)getNextSiblingWithTag(NED_PARAMETERS);
}

CommentNode *ParametersNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

void ParamNode::setType(int val)
{
    validateEnum(val, partype_vals, partype_nums, partype_n);
    type = val;
}

ParamNode::ParamNode()
{
    type = 0;
    isVolatile = false;
    isDefault = false;
    applyDefaults();
}

ParamNode::ParamNode(NEDElement *parent) : NEDElement(parent)
{
    type = 0;
    isVolatile = false;
    isDefault = false;
    applyDefaults();
}

int ParamNode::getNumAttributes() const
{
    return 5;
}

const char *ParamNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "type";
        case 1: return "is-volatile";
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
        case 1: return boolToString(isVolatile);
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
        case 1: isVolatile = stringToBool(val); break;
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

ParamNode *ParamNode::dup() const
{
    ParamNode *newNode = new ParamNode();
    newNode->type = this->type;
    newNode->isVolatile = this->isVolatile;
    newNode->name = this->name;
    newNode->value = this->value;
    newNode->isDefault = this->isDefault;
    return newNode;
}

ParamNode *ParamNode::getNextParamNodeSibling() const
{
    return (ParamNode *)getNextSiblingWithTag(NED_PARAM);
}

CommentNode *ParamNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionNode *ParamNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

PropertyNode *ParamNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

PatternNode::PatternNode()
{
    isDefault = false;
    applyDefaults();
}

PatternNode::PatternNode(NEDElement *parent) : NEDElement(parent)
{
    isDefault = false;
    applyDefaults();
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
        case 1: return "";
        case 2: return "false";
        default: return 0;
    }
}

PatternNode *PatternNode::dup() const
{
    PatternNode *newNode = new PatternNode();
    newNode->pattern = this->pattern;
    newNode->value = this->value;
    newNode->isDefault = this->isDefault;
    return newNode;
}

PatternNode *PatternNode::getNextPatternNodeSibling() const
{
    return (PatternNode *)getNextSiblingWithTag(NED_PATTERN);
}

CommentNode *PatternNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionNode *PatternNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

PropertyNode *PatternNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

PropertyNode::PropertyNode()
{
    isImplicit = false;
    applyDefaults();
}

PropertyNode::PropertyNode(NEDElement *parent) : NEDElement(parent)
{
    isImplicit = false;
    applyDefaults();
}

int PropertyNode::getNumAttributes() const
{
    return 3;
}

const char *PropertyNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "is-implicit";
        case 1: return "name";
        case 2: return "index";
        default: return 0;
    }
}

const char *PropertyNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return boolToString(isImplicit);
        case 1: return name.c_str();
        case 2: return index.c_str();
        default: return 0;
    }
}

void PropertyNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: isImplicit = stringToBool(val); break;
        case 1: name = val; break;
        case 2: index = val; break;
        default: ;
    }
}

const char *PropertyNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "false";
        case 1: return NULL;
        case 2: return "";
        default: return 0;
    }
}

PropertyNode *PropertyNode::dup() const
{
    PropertyNode *newNode = new PropertyNode();
    newNode->isImplicit = this->isImplicit;
    newNode->name = this->name;
    newNode->index = this->index;
    return newNode;
}

PropertyNode *PropertyNode::getNextPropertyNodeSibling() const
{
    return (PropertyNode *)getNextSiblingWithTag(NED_PROPERTY);
}

CommentNode *PropertyNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

PropertyKeyNode *PropertyNode::getFirstPropertyKeyChild() const
{
    return (PropertyKeyNode *)getFirstChildWithTag(NED_PROPERTY_KEY);
}

PropertyKeyNode::PropertyKeyNode()
{
    applyDefaults();
}

PropertyKeyNode::PropertyKeyNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int PropertyKeyNode::getNumAttributes() const
{
    return 1;
}

const char *PropertyKeyNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *PropertyKeyNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void PropertyKeyNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
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

PropertyKeyNode *PropertyKeyNode::dup() const
{
    PropertyKeyNode *newNode = new PropertyKeyNode();
    newNode->name = this->name;
    return newNode;
}

PropertyKeyNode *PropertyKeyNode::getNextPropertyKeyNodeSibling() const
{
    return (PropertyKeyNode *)getNextSiblingWithTag(NED_PROPERTY_KEY);
}

CommentNode *PropertyKeyNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

LiteralNode *PropertyKeyNode::getFirstLiteralChild() const
{
    return (LiteralNode *)getFirstChildWithTag(NED_LITERAL);
}

GatesNode::GatesNode()
{
    applyDefaults();
}

GatesNode::GatesNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

GatesNode *GatesNode::dup() const
{
    GatesNode *newNode = new GatesNode();
    return newNode;
}

GatesNode *GatesNode::getNextGatesNodeSibling() const
{
    return (GatesNode *)getNextSiblingWithTag(NED_GATES);
}

CommentNode *GatesNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

GateNode *GatesNode::getFirstGateChild() const
{
    return (GateNode *)getFirstChildWithTag(NED_GATE);
}

void GateNode::setType(int val)
{
    validateEnum(val, gatetype_vals, gatetype_nums, gatetype_n);
    type = val;
}

GateNode::GateNode()
{
    type = 0;
    isVector = false;
    applyDefaults();
}

GateNode::GateNode(NEDElement *parent) : NEDElement(parent)
{
    type = 0;
    isVector = false;
    applyDefaults();
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
        case 1: return "";
        case 2: return "false";
        case 3: return "";
        default: return 0;
    }
}

GateNode *GateNode::dup() const
{
    GateNode *newNode = new GateNode();
    newNode->name = this->name;
    newNode->type = this->type;
    newNode->isVector = this->isVector;
    newNode->vectorSize = this->vectorSize;
    return newNode;
}

GateNode *GateNode::getNextGateNodeSibling() const
{
    return (GateNode *)getNextSiblingWithTag(NED_GATE);
}

CommentNode *GateNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionNode *GateNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

PropertyNode *GateNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

TypesNode::TypesNode()
{
    applyDefaults();
}

TypesNode::TypesNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

TypesNode *TypesNode::dup() const
{
    TypesNode *newNode = new TypesNode();
    return newNode;
}

TypesNode *TypesNode::getNextTypesNodeSibling() const
{
    return (TypesNode *)getNextSiblingWithTag(NED_TYPES);
}

CommentNode *TypesNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

SubmodulesNode::SubmodulesNode()
{
    applyDefaults();
}

SubmodulesNode::SubmodulesNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

SubmodulesNode *SubmodulesNode::dup() const
{
    SubmodulesNode *newNode = new SubmodulesNode();
    return newNode;
}

SubmodulesNode *SubmodulesNode::getNextSubmodulesNodeSibling() const
{
    return (SubmodulesNode *)getNextSiblingWithTag(NED_SUBMODULES);
}

CommentNode *SubmodulesNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

SubmoduleNode *SubmodulesNode::getFirstSubmoduleChild() const
{
    return (SubmoduleNode *)getFirstChildWithTag(NED_SUBMODULE);
}

SubmoduleNode::SubmoduleNode()
{
    applyDefaults();
}

SubmoduleNode::SubmoduleNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

SubmoduleNode *SubmoduleNode::dup() const
{
    SubmoduleNode *newNode = new SubmoduleNode();
    newNode->name = this->name;
    newNode->type = this->type;
    newNode->likeType = this->likeType;
    newNode->likeParam = this->likeParam;
    newNode->vectorSize = this->vectorSize;
    return newNode;
}

SubmoduleNode *SubmoduleNode::getNextSubmoduleNodeSibling() const
{
    return (SubmoduleNode *)getNextSiblingWithTag(NED_SUBMODULE);
}

CommentNode *SubmoduleNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

ConnectionsNode::ConnectionsNode()
{
    allowUnconnected = false;
    applyDefaults();
}

ConnectionsNode::ConnectionsNode(NEDElement *parent) : NEDElement(parent)
{
    allowUnconnected = false;
    applyDefaults();
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

ConnectionsNode *ConnectionsNode::dup() const
{
    ConnectionsNode *newNode = new ConnectionsNode();
    newNode->allowUnconnected = this->allowUnconnected;
    return newNode;
}

ConnectionsNode *ConnectionsNode::getNextConnectionsNodeSibling() const
{
    return (ConnectionsNode *)getNextSiblingWithTag(NED_CONNECTIONS);
}

CommentNode *ConnectionsNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

ConnectionNode::ConnectionNode()
{
    srcGatePlusplus = false;
    srcGateSubg = 0;
    destGatePlusplus = false;
    destGateSubg = 0;
    arrowDirection = 0;
    applyDefaults();
}

ConnectionNode::ConnectionNode(NEDElement *parent) : NEDElement(parent)
{
    srcGatePlusplus = false;
    srcGateSubg = 0;
    destGatePlusplus = false;
    destGateSubg = 0;
    arrowDirection = 0;
    applyDefaults();
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

ConnectionNode *ConnectionNode::dup() const
{
    ConnectionNode *newNode = new ConnectionNode();
    newNode->srcModule = this->srcModule;
    newNode->srcModuleIndex = this->srcModuleIndex;
    newNode->srcGate = this->srcGate;
    newNode->srcGatePlusplus = this->srcGatePlusplus;
    newNode->srcGateIndex = this->srcGateIndex;
    newNode->srcGateSubg = this->srcGateSubg;
    newNode->destModule = this->destModule;
    newNode->destModuleIndex = this->destModuleIndex;
    newNode->destGate = this->destGate;
    newNode->destGatePlusplus = this->destGatePlusplus;
    newNode->destGateIndex = this->destGateIndex;
    newNode->destGateSubg = this->destGateSubg;
    newNode->arrowDirection = this->arrowDirection;
    return newNode;
}

ConnectionNode *ConnectionNode::getNextConnectionNodeSibling() const
{
    return (ConnectionNode *)getNextSiblingWithTag(NED_CONNECTION);
}

CommentNode *ConnectionNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionNode *ConnectionNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

ChannelSpecNode *ConnectionNode::getFirstChannelSpecChild() const
{
    return (ChannelSpecNode *)getFirstChildWithTag(NED_CHANNEL_SPEC);
}

LoopNode *ConnectionNode::getFirstLoopChild() const
{
    return (LoopNode *)getFirstChildWithTag(NED_LOOP);
}

ConditionNode *ConnectionNode::getFirstConditionChild() const
{
    return (ConditionNode *)getFirstChildWithTag(NED_CONDITION);
}

ChannelSpecNode::ChannelSpecNode()
{
    applyDefaults();
}

ChannelSpecNode::ChannelSpecNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

ChannelSpecNode *ChannelSpecNode::dup() const
{
    ChannelSpecNode *newNode = new ChannelSpecNode();
    newNode->type = this->type;
    newNode->likeType = this->likeType;
    newNode->likeParam = this->likeParam;
    return newNode;
}

ChannelSpecNode *ChannelSpecNode::getNextChannelSpecNodeSibling() const
{
    return (ChannelSpecNode *)getNextSiblingWithTag(NED_CHANNEL_SPEC);
}

CommentNode *ChannelSpecNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionNode *ChannelSpecNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

ParametersNode *ChannelSpecNode::getFirstParametersChild() const
{
    return (ParametersNode *)getFirstChildWithTag(NED_PARAMETERS);
}

ConnectionGroupNode::ConnectionGroupNode()
{
    applyDefaults();
}

ConnectionGroupNode::ConnectionGroupNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

ConnectionGroupNode *ConnectionGroupNode::dup() const
{
    ConnectionGroupNode *newNode = new ConnectionGroupNode();
    return newNode;
}

ConnectionGroupNode *ConnectionGroupNode::getNextConnectionGroupNodeSibling() const
{
    return (ConnectionGroupNode *)getNextSiblingWithTag(NED_CONNECTION_GROUP);
}

CommentNode *ConnectionGroupNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

LoopNode::LoopNode()
{
    applyDefaults();
}

LoopNode::LoopNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

LoopNode *LoopNode::dup() const
{
    LoopNode *newNode = new LoopNode();
    newNode->paramName = this->paramName;
    newNode->fromValue = this->fromValue;
    newNode->toValue = this->toValue;
    return newNode;
}

LoopNode *LoopNode::getNextLoopNodeSibling() const
{
    return (LoopNode *)getNextSiblingWithTag(NED_LOOP);
}

CommentNode *LoopNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionNode *LoopNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

ConditionNode::ConditionNode()
{
    applyDefaults();
}

ConditionNode::ConditionNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

ConditionNode *ConditionNode::dup() const
{
    ConditionNode *newNode = new ConditionNode();
    newNode->condition = this->condition;
    return newNode;
}

ConditionNode *ConditionNode::getNextConditionNodeSibling() const
{
    return (ConditionNode *)getNextSiblingWithTag(NED_CONDITION);
}

CommentNode *ConditionNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionNode *ConditionNode::getFirstExpressionChild() const
{
    return (ExpressionNode *)getFirstChildWithTag(NED_EXPRESSION);
}

ExpressionNode::ExpressionNode()
{
    applyDefaults();
}

ExpressionNode::ExpressionNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

ExpressionNode *ExpressionNode::dup() const
{
    ExpressionNode *newNode = new ExpressionNode();
    newNode->target = this->target;
    return newNode;
}

ExpressionNode *ExpressionNode::getNextExpressionNodeSibling() const
{
    return (ExpressionNode *)getNextSiblingWithTag(NED_EXPRESSION);
}

CommentNode *ExpressionNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

OperatorNode::OperatorNode()
{
    applyDefaults();
}

OperatorNode::OperatorNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

OperatorNode *OperatorNode::dup() const
{
    OperatorNode *newNode = new OperatorNode();
    newNode->name = this->name;
    return newNode;
}

OperatorNode *OperatorNode::getNextOperatorNodeSibling() const
{
    return (OperatorNode *)getNextSiblingWithTag(NED_OPERATOR);
}

CommentNode *OperatorNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

FunctionNode::FunctionNode()
{
    applyDefaults();
}

FunctionNode::FunctionNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

FunctionNode *FunctionNode::dup() const
{
    FunctionNode *newNode = new FunctionNode();
    newNode->name = this->name;
    return newNode;
}

FunctionNode *FunctionNode::getNextFunctionNodeSibling() const
{
    return (FunctionNode *)getNextSiblingWithTag(NED_FUNCTION);
}

CommentNode *FunctionNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
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

IdentNode::IdentNode()
{
    applyDefaults();
}

IdentNode::IdentNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int IdentNode::getNumAttributes() const
{
    return 2;
}

const char *IdentNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "module";
        case 1: return "name";
        default: return 0;
    }
}

const char *IdentNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return module.c_str();
        case 1: return name.c_str();
        default: return 0;
    }
}

void IdentNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: module = val; break;
        case 1: name = val; break;
        default: ;
    }
}

const char *IdentNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return NULL;
        default: return 0;
    }
}

IdentNode *IdentNode::dup() const
{
    IdentNode *newNode = new IdentNode();
    newNode->module = this->module;
    newNode->name = this->name;
    return newNode;
}

IdentNode *IdentNode::getNextIdentNodeSibling() const
{
    return (IdentNode *)getNextSiblingWithTag(NED_IDENT);
}

CommentNode *IdentNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

OperatorNode *IdentNode::getFirstOperatorChild() const
{
    return (OperatorNode *)getFirstChildWithTag(NED_OPERATOR);
}

FunctionNode *IdentNode::getFirstFunctionChild() const
{
    return (FunctionNode *)getFirstChildWithTag(NED_FUNCTION);
}

IdentNode *IdentNode::getFirstIdentChild() const
{
    return (IdentNode *)getFirstChildWithTag(NED_IDENT);
}

LiteralNode *IdentNode::getFirstLiteralChild() const
{
    return (LiteralNode *)getFirstChildWithTag(NED_LITERAL);
}

void LiteralNode::setType(int val)
{
    validateEnum(val, littype_vals, littype_nums, littype_n);
    type = val;
}

LiteralNode::LiteralNode()
{
    type = 0;
    applyDefaults();
}

LiteralNode::LiteralNode(NEDElement *parent) : NEDElement(parent)
{
    type = 0;
    applyDefaults();
}

int LiteralNode::getNumAttributes() const
{
    return 4;
}

const char *LiteralNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "type";
        case 1: return "unit";
        case 2: return "text";
        case 3: return "value";
        default: return 0;
    }
}

const char *LiteralNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return enumToString(type, littype_vals, littype_nums, littype_n);
        case 1: return unit.c_str();
        case 2: return text.c_str();
        case 3: return value.c_str();
        default: return 0;
    }
}

void LiteralNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: type = stringToEnum(val, littype_vals, littype_nums, littype_n); break;
        case 1: unit = val; break;
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

LiteralNode *LiteralNode::dup() const
{
    LiteralNode *newNode = new LiteralNode();
    newNode->type = this->type;
    newNode->unit = this->unit;
    newNode->text = this->text;
    newNode->value = this->value;
    return newNode;
}

LiteralNode *LiteralNode::getNextLiteralNodeSibling() const
{
    return (LiteralNode *)getNextSiblingWithTag(NED_LITERAL);
}

CommentNode *LiteralNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

MsgFileNode::MsgFileNode()
{
    applyDefaults();
}

MsgFileNode::MsgFileNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int MsgFileNode::getNumAttributes() const
{
    return 2;
}

const char *MsgFileNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "filename";
        case 1: return "version";
        default: return 0;
    }
}

const char *MsgFileNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return filename.c_str();
        case 1: return version.c_str();
        default: return 0;
    }
}

void MsgFileNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: filename = val; break;
        case 1: version = val; break;
        default: ;
    }
}

const char *MsgFileNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "2";
        default: return 0;
    }
}

MsgFileNode *MsgFileNode::dup() const
{
    MsgFileNode *newNode = new MsgFileNode();
    newNode->filename = this->filename;
    newNode->version = this->version;
    return newNode;
}

MsgFileNode *MsgFileNode::getNextMsgFileNodeSibling() const
{
    return (MsgFileNode *)getNextSiblingWithTag(NED_MSG_FILE);
}

CommentNode *MsgFileNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

NamespaceNode *MsgFileNode::getFirstNamespaceChild() const
{
    return (NamespaceNode *)getFirstChildWithTag(NED_NAMESPACE);
}

PropertyDeclNode *MsgFileNode::getFirstPropertyDeclChild() const
{
    return (PropertyDeclNode *)getFirstChildWithTag(NED_PROPERTY_DECL);
}

PropertyNode *MsgFileNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
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

NamespaceNode::NamespaceNode()
{
    applyDefaults();
}

NamespaceNode::NamespaceNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int NamespaceNode::getNumAttributes() const
{
    return 1;
}

const char *NamespaceNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *NamespaceNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void NamespaceNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *NamespaceNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

NamespaceNode *NamespaceNode::dup() const
{
    NamespaceNode *newNode = new NamespaceNode();
    newNode->name = this->name;
    return newNode;
}

NamespaceNode *NamespaceNode::getNextNamespaceNodeSibling() const
{
    return (NamespaceNode *)getNextSiblingWithTag(NED_NAMESPACE);
}

CommentNode *NamespaceNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

CplusplusNode::CplusplusNode()
{
    applyDefaults();
}

CplusplusNode::CplusplusNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int CplusplusNode::getNumAttributes() const
{
    return 1;
}

const char *CplusplusNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "body";
        default: return 0;
    }
}

const char *CplusplusNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return body.c_str();
        default: return 0;
    }
}

void CplusplusNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: body = val; break;
        default: ;
    }
}

const char *CplusplusNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

CplusplusNode *CplusplusNode::dup() const
{
    CplusplusNode *newNode = new CplusplusNode();
    newNode->body = this->body;
    return newNode;
}

CplusplusNode *CplusplusNode::getNextCplusplusNodeSibling() const
{
    return (CplusplusNode *)getNextSiblingWithTag(NED_CPLUSPLUS);
}

CommentNode *CplusplusNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

StructDeclNode::StructDeclNode()
{
    applyDefaults();
}

StructDeclNode::StructDeclNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int StructDeclNode::getNumAttributes() const
{
    return 1;
}

const char *StructDeclNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *StructDeclNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void StructDeclNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *StructDeclNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

StructDeclNode *StructDeclNode::dup() const
{
    StructDeclNode *newNode = new StructDeclNode();
    newNode->name = this->name;
    return newNode;
}

StructDeclNode *StructDeclNode::getNextStructDeclNodeSibling() const
{
    return (StructDeclNode *)getNextSiblingWithTag(NED_STRUCT_DECL);
}

CommentNode *StructDeclNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

ClassDeclNode::ClassDeclNode()
{
    isCobject = false;
    applyDefaults();
}

ClassDeclNode::ClassDeclNode(NEDElement *parent) : NEDElement(parent)
{
    isCobject = false;
    applyDefaults();
}

int ClassDeclNode::getNumAttributes() const
{
    return 2;
}

const char *ClassDeclNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "is-cobject";
        default: return 0;
    }
}

const char *ClassDeclNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return boolToString(isCobject);
        default: return 0;
    }
}

void ClassDeclNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: isCobject = stringToBool(val); break;
        default: ;
    }
}

const char *ClassDeclNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "false";
        default: return 0;
    }
}

ClassDeclNode *ClassDeclNode::dup() const
{
    ClassDeclNode *newNode = new ClassDeclNode();
    newNode->name = this->name;
    newNode->isCobject = this->isCobject;
    return newNode;
}

ClassDeclNode *ClassDeclNode::getNextClassDeclNodeSibling() const
{
    return (ClassDeclNode *)getNextSiblingWithTag(NED_CLASS_DECL);
}

CommentNode *ClassDeclNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

MessageDeclNode::MessageDeclNode()
{
    applyDefaults();
}

MessageDeclNode::MessageDeclNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int MessageDeclNode::getNumAttributes() const
{
    return 1;
}

const char *MessageDeclNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *MessageDeclNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void MessageDeclNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *MessageDeclNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

MessageDeclNode *MessageDeclNode::dup() const
{
    MessageDeclNode *newNode = new MessageDeclNode();
    newNode->name = this->name;
    return newNode;
}

MessageDeclNode *MessageDeclNode::getNextMessageDeclNodeSibling() const
{
    return (MessageDeclNode *)getNextSiblingWithTag(NED_MESSAGE_DECL);
}

CommentNode *MessageDeclNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

EnumDeclNode::EnumDeclNode()
{
    applyDefaults();
}

EnumDeclNode::EnumDeclNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int EnumDeclNode::getNumAttributes() const
{
    return 1;
}

const char *EnumDeclNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *EnumDeclNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void EnumDeclNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *EnumDeclNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

EnumDeclNode *EnumDeclNode::dup() const
{
    EnumDeclNode *newNode = new EnumDeclNode();
    newNode->name = this->name;
    return newNode;
}

EnumDeclNode *EnumDeclNode::getNextEnumDeclNodeSibling() const
{
    return (EnumDeclNode *)getNextSiblingWithTag(NED_ENUM_DECL);
}

CommentNode *EnumDeclNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

EnumNode::EnumNode()
{
    applyDefaults();
}

EnumNode::EnumNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int EnumNode::getNumAttributes() const
{
    return 3;
}

const char *EnumNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        default: return 0;
    }
}

const char *EnumNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        default: return 0;
    }
}

void EnumNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        default: ;
    }
}

const char *EnumNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

EnumNode *EnumNode::dup() const
{
    EnumNode *newNode = new EnumNode();
    newNode->name = this->name;
    newNode->extendsName = this->extendsName;
    newNode->sourceCode = this->sourceCode;
    return newNode;
}

EnumNode *EnumNode::getNextEnumNodeSibling() const
{
    return (EnumNode *)getNextSiblingWithTag(NED_ENUM);
}

CommentNode *EnumNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

EnumFieldsNode *EnumNode::getFirstEnumFieldsChild() const
{
    return (EnumFieldsNode *)getFirstChildWithTag(NED_ENUM_FIELDS);
}

EnumFieldsNode::EnumFieldsNode()
{
    applyDefaults();
}

EnumFieldsNode::EnumFieldsNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int EnumFieldsNode::getNumAttributes() const
{
    return 0;
}

const char *EnumFieldsNode::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *EnumFieldsNode::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void EnumFieldsNode::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *EnumFieldsNode::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

EnumFieldsNode *EnumFieldsNode::dup() const
{
    EnumFieldsNode *newNode = new EnumFieldsNode();
    return newNode;
}

EnumFieldsNode *EnumFieldsNode::getNextEnumFieldsNodeSibling() const
{
    return (EnumFieldsNode *)getNextSiblingWithTag(NED_ENUM_FIELDS);
}

CommentNode *EnumFieldsNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

EnumFieldNode *EnumFieldsNode::getFirstEnumFieldChild() const
{
    return (EnumFieldNode *)getFirstChildWithTag(NED_ENUM_FIELD);
}

EnumFieldNode::EnumFieldNode()
{
    applyDefaults();
}

EnumFieldNode::EnumFieldNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int EnumFieldNode::getNumAttributes() const
{
    return 2;
}

const char *EnumFieldNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "value";
        default: return 0;
    }
}

const char *EnumFieldNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return value.c_str();
        default: return 0;
    }
}

void EnumFieldNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: value = val; break;
        default: ;
    }
}

const char *EnumFieldNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        default: return 0;
    }
}

EnumFieldNode *EnumFieldNode::dup() const
{
    EnumFieldNode *newNode = new EnumFieldNode();
    newNode->name = this->name;
    newNode->value = this->value;
    return newNode;
}

EnumFieldNode *EnumFieldNode::getNextEnumFieldNodeSibling() const
{
    return (EnumFieldNode *)getNextSiblingWithTag(NED_ENUM_FIELD);
}

CommentNode *EnumFieldNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

MessageNode::MessageNode()
{
    applyDefaults();
}

MessageNode::MessageNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int MessageNode::getNumAttributes() const
{
    return 3;
}

const char *MessageNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        default: return 0;
    }
}

const char *MessageNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        default: return 0;
    }
}

void MessageNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        default: ;
    }
}

const char *MessageNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

MessageNode *MessageNode::dup() const
{
    MessageNode *newNode = new MessageNode();
    newNode->name = this->name;
    newNode->extendsName = this->extendsName;
    newNode->sourceCode = this->sourceCode;
    return newNode;
}

MessageNode *MessageNode::getNextMessageNodeSibling() const
{
    return (MessageNode *)getNextSiblingWithTag(NED_MESSAGE);
}

CommentNode *MessageNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

PropertyNode *MessageNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

FieldNode *MessageNode::getFirstFieldChild() const
{
    return (FieldNode *)getFirstChildWithTag(NED_FIELD);
}

ClassNode::ClassNode()
{
    applyDefaults();
}

ClassNode::ClassNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ClassNode::getNumAttributes() const
{
    return 3;
}

const char *ClassNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        default: return 0;
    }
}

const char *ClassNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        default: return 0;
    }
}

void ClassNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        default: ;
    }
}

const char *ClassNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

ClassNode *ClassNode::dup() const
{
    ClassNode *newNode = new ClassNode();
    newNode->name = this->name;
    newNode->extendsName = this->extendsName;
    newNode->sourceCode = this->sourceCode;
    return newNode;
}

ClassNode *ClassNode::getNextClassNodeSibling() const
{
    return (ClassNode *)getNextSiblingWithTag(NED_CLASS);
}

CommentNode *ClassNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

PropertyNode *ClassNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

FieldNode *ClassNode::getFirstFieldChild() const
{
    return (FieldNode *)getFirstChildWithTag(NED_FIELD);
}

StructNode::StructNode()
{
    applyDefaults();
}

StructNode::StructNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int StructNode::getNumAttributes() const
{
    return 3;
}

const char *StructNode::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        default: return 0;
    }
}

const char *StructNode::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        default: return 0;
    }
}

void StructNode::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        default: ;
    }
}

const char *StructNode::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

StructNode *StructNode::dup() const
{
    StructNode *newNode = new StructNode();
    newNode->name = this->name;
    newNode->extendsName = this->extendsName;
    newNode->sourceCode = this->sourceCode;
    return newNode;
}

StructNode *StructNode::getNextStructNodeSibling() const
{
    return (StructNode *)getNextSiblingWithTag(NED_STRUCT);
}

CommentNode *StructNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

PropertyNode *StructNode::getFirstPropertyChild() const
{
    return (PropertyNode *)getFirstChildWithTag(NED_PROPERTY);
}

FieldNode *StructNode::getFirstFieldChild() const
{
    return (FieldNode *)getFirstChildWithTag(NED_FIELD);
}

FieldNode::FieldNode()
{
    isAbstract = false;
    isReadonly = false;
    isVector = false;
    applyDefaults();
}

FieldNode::FieldNode(NEDElement *parent) : NEDElement(parent)
{
    isAbstract = false;
    isReadonly = false;
    isVector = false;
    applyDefaults();
}

int FieldNode::getNumAttributes() const
{
    return 8;
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
        default: return 0;
    }
}

FieldNode *FieldNode::dup() const
{
    FieldNode *newNode = new FieldNode();
    newNode->name = this->name;
    newNode->dataType = this->dataType;
    newNode->isAbstract = this->isAbstract;
    newNode->isReadonly = this->isReadonly;
    newNode->isVector = this->isVector;
    newNode->vectorSize = this->vectorSize;
    newNode->enumName = this->enumName;
    newNode->defaultValue = this->defaultValue;
    return newNode;
}

FieldNode *FieldNode::getNextFieldNodeSibling() const
{
    return (FieldNode *)getNextSiblingWithTag(NED_FIELD);
}

CommentNode *FieldNode::getFirstCommentChild() const
{
    return (CommentNode *)getFirstChildWithTag(NED_COMMENT);
}

UnknownNode::UnknownNode()
{
    applyDefaults();
}

UnknownNode::UnknownNode(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
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

UnknownNode *UnknownNode::dup() const
{
    UnknownNode *newNode = new UnknownNode();
    newNode->element = this->element;
    return newNode;
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
    if (tagname[0]=='c' && !strcmp(tagname,"comment"))  return new CommentNode();
    if (tagname[0]=='p' && !strcmp(tagname,"package"))  return new PackageNode();
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
    if (tagname[0]=='p' && !strcmp(tagname,"param"))  return new ParamNode();
    if (tagname[0]=='p' && !strcmp(tagname,"pattern"))  return new PatternNode();
    if (tagname[0]=='p' && !strcmp(tagname,"property"))  return new PropertyNode();
    if (tagname[0]=='p' && !strcmp(tagname,"property-key"))  return new PropertyKeyNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gates"))  return new GatesNode();
    if (tagname[0]=='g' && !strcmp(tagname,"gate"))  return new GateNode();
    if (tagname[0]=='t' && !strcmp(tagname,"types"))  return new TypesNode();
    if (tagname[0]=='s' && !strcmp(tagname,"submodules"))  return new SubmodulesNode();
    if (tagname[0]=='s' && !strcmp(tagname,"submodule"))  return new SubmoduleNode();
    if (tagname[0]=='c' && !strcmp(tagname,"connections"))  return new ConnectionsNode();
    if (tagname[0]=='c' && !strcmp(tagname,"connection"))  return new ConnectionNode();
    if (tagname[0]=='c' && !strcmp(tagname,"channel-spec"))  return new ChannelSpecNode();
    if (tagname[0]=='c' && !strcmp(tagname,"connection-group"))  return new ConnectionGroupNode();
    if (tagname[0]=='l' && !strcmp(tagname,"loop"))  return new LoopNode();
    if (tagname[0]=='c' && !strcmp(tagname,"condition"))  return new ConditionNode();
    if (tagname[0]=='e' && !strcmp(tagname,"expression"))  return new ExpressionNode();
    if (tagname[0]=='o' && !strcmp(tagname,"operator"))  return new OperatorNode();
    if (tagname[0]=='f' && !strcmp(tagname,"function"))  return new FunctionNode();
    if (tagname[0]=='i' && !strcmp(tagname,"ident"))  return new IdentNode();
    if (tagname[0]=='l' && !strcmp(tagname,"literal"))  return new LiteralNode();
    if (tagname[0]=='m' && !strcmp(tagname,"msg-file"))  return new MsgFileNode();
    if (tagname[0]=='n' && !strcmp(tagname,"namespace"))  return new NamespaceNode();
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
    if (tagname[0]=='f' && !strcmp(tagname,"field"))  return new FieldNode();
    if (tagname[0]=='u' && !strcmp(tagname,"unknown"))  return new UnknownNode();
    throw NEDException("unknown tag '%s', cannot create object to represent it", tagname);
}

NEDElement *NEDElementFactory::createNodeWithTag(int tagcode)
{
    switch (tagcode) {
        case NED_FILES: return new FilesNode();
        case NED_NED_FILE: return new NedFileNode();
        case NED_COMMENT: return new CommentNode();
        case NED_PACKAGE: return new PackageNode();
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
        case NED_PARAM: return new ParamNode();
        case NED_PATTERN: return new PatternNode();
        case NED_PROPERTY: return new PropertyNode();
        case NED_PROPERTY_KEY: return new PropertyKeyNode();
        case NED_GATES: return new GatesNode();
        case NED_GATE: return new GateNode();
        case NED_TYPES: return new TypesNode();
        case NED_SUBMODULES: return new SubmodulesNode();
        case NED_SUBMODULE: return new SubmoduleNode();
        case NED_CONNECTIONS: return new ConnectionsNode();
        case NED_CONNECTION: return new ConnectionNode();
        case NED_CHANNEL_SPEC: return new ChannelSpecNode();
        case NED_CONNECTION_GROUP: return new ConnectionGroupNode();
        case NED_LOOP: return new LoopNode();
        case NED_CONDITION: return new ConditionNode();
        case NED_EXPRESSION: return new ExpressionNode();
        case NED_OPERATOR: return new OperatorNode();
        case NED_FUNCTION: return new FunctionNode();
        case NED_IDENT: return new IdentNode();
        case NED_LITERAL: return new LiteralNode();
        case NED_MSG_FILE: return new MsgFileNode();
        case NED_NAMESPACE: return new NamespaceNode();
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
        case NED_FIELD: return new FieldNode();
        case NED_UNKNOWN: return new UnknownNode();
    }
    throw NEDException("unknown tag code %d, cannot create object to represent it", tagcode);
}

NAMESPACE_END

