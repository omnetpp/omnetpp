//==========================================================================
// Part of the OMNeT++/OMNEST Discrete Event Simulation System
//
// Generated from ned.dtd by dtdclassgen.pl
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

//
// THIS IS A GENERATED FILE, DO NOT EDIT!
//


#include <string.h>
#include "nederror.h"
#include "nedexception.h"
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

FilesElement::FilesElement()
{
    applyDefaults();
}

FilesElement::FilesElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int FilesElement::getNumAttributes() const
{
    return 0;
}

const char *FilesElement::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *FilesElement::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void FilesElement::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *FilesElement::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

FilesElement *FilesElement::dup() const
{
    FilesElement *element = new FilesElement();
    return element;
}

FilesElement *FilesElement::getNextFilesSibling() const
{
    return (FilesElement *)getNextSiblingWithTag(NED_FILES);
}

NedFileElement *FilesElement::getFirstNedFileChild() const
{
    return (NedFileElement *)getFirstChildWithTag(NED_NED_FILE);
}

MsgFileElement *FilesElement::getFirstMsgFileChild() const
{
    return (MsgFileElement *)getFirstChildWithTag(NED_MSG_FILE);
}

NedFileElement::NedFileElement()
{
    applyDefaults();
}

NedFileElement::NedFileElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int NedFileElement::getNumAttributes() const
{
    return 2;
}

const char *NedFileElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "filename";
        case 1: return "version";
        default: return 0;
    }
}

const char *NedFileElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return filename.c_str();
        case 1: return version.c_str();
        default: return 0;
    }
}

void NedFileElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: filename = val; break;
        case 1: version = val; break;
        default: ;
    }
}

const char *NedFileElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "2";
        default: return 0;
    }
}

NedFileElement *NedFileElement::dup() const
{
    NedFileElement *element = new NedFileElement();
    element->filename = this->filename;
    element->version = this->version;
    return element;
}

NedFileElement *NedFileElement::getNextNedFileSibling() const
{
    return (NedFileElement *)getNextSiblingWithTag(NED_NED_FILE);
}

CommentElement *NedFileElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

PackageElement *NedFileElement::getFirstPackageChild() const
{
    return (PackageElement *)getFirstChildWithTag(NED_PACKAGE);
}

ImportElement *NedFileElement::getFirstImportChild() const
{
    return (ImportElement *)getFirstChildWithTag(NED_IMPORT);
}

PropertyDeclElement *NedFileElement::getFirstPropertyDeclChild() const
{
    return (PropertyDeclElement *)getFirstChildWithTag(NED_PROPERTY_DECL);
}

PropertyElement *NedFileElement::getFirstPropertyChild() const
{
    return (PropertyElement *)getFirstChildWithTag(NED_PROPERTY);
}

ChannelElement *NedFileElement::getFirstChannelChild() const
{
    return (ChannelElement *)getFirstChildWithTag(NED_CHANNEL);
}

ChannelInterfaceElement *NedFileElement::getFirstChannelInterfaceChild() const
{
    return (ChannelInterfaceElement *)getFirstChildWithTag(NED_CHANNEL_INTERFACE);
}

SimpleModuleElement *NedFileElement::getFirstSimpleModuleChild() const
{
    return (SimpleModuleElement *)getFirstChildWithTag(NED_SIMPLE_MODULE);
}

CompoundModuleElement *NedFileElement::getFirstCompoundModuleChild() const
{
    return (CompoundModuleElement *)getFirstChildWithTag(NED_COMPOUND_MODULE);
}

ModuleInterfaceElement *NedFileElement::getFirstModuleInterfaceChild() const
{
    return (ModuleInterfaceElement *)getFirstChildWithTag(NED_MODULE_INTERFACE);
}

CommentElement::CommentElement()
{
    applyDefaults();
}

CommentElement::CommentElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int CommentElement::getNumAttributes() const
{
    return 2;
}

const char *CommentElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "locid";
        case 1: return "content";
        default: return 0;
    }
}

const char *CommentElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return locid.c_str();
        case 1: return content.c_str();
        default: return 0;
    }
}

void CommentElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: locid = val; break;
        case 1: content = val; break;
        default: ;
    }
}

const char *CommentElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        default: return 0;
    }
}

CommentElement *CommentElement::dup() const
{
    CommentElement *element = new CommentElement();
    element->locid = this->locid;
    element->content = this->content;
    return element;
}

CommentElement *CommentElement::getNextCommentSibling() const
{
    return (CommentElement *)getNextSiblingWithTag(NED_COMMENT);
}

PackageElement::PackageElement()
{
    applyDefaults();
}

PackageElement::PackageElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int PackageElement::getNumAttributes() const
{
    return 1;
}

const char *PackageElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *PackageElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void PackageElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *PackageElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

PackageElement *PackageElement::dup() const
{
    PackageElement *element = new PackageElement();
    element->name = this->name;
    return element;
}

PackageElement *PackageElement::getNextPackageSibling() const
{
    return (PackageElement *)getNextSiblingWithTag(NED_PACKAGE);
}

CommentElement *PackageElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ImportElement::ImportElement()
{
    applyDefaults();
}

ImportElement::ImportElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ImportElement::getNumAttributes() const
{
    return 1;
}

const char *ImportElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "import-spec";
        default: return 0;
    }
}

const char *ImportElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return importSpec.c_str();
        default: return 0;
    }
}

void ImportElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: importSpec = val; break;
        default: ;
    }
}

const char *ImportElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

ImportElement *ImportElement::dup() const
{
    ImportElement *element = new ImportElement();
    element->importSpec = this->importSpec;
    return element;
}

ImportElement *ImportElement::getNextImportSibling() const
{
    return (ImportElement *)getNextSiblingWithTag(NED_IMPORT);
}

CommentElement *ImportElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

PropertyDeclElement::PropertyDeclElement()
{
    isArray = false;
    applyDefaults();
}

PropertyDeclElement::PropertyDeclElement(NEDElement *parent) : NEDElement(parent)
{
    isArray = false;
    applyDefaults();
}

int PropertyDeclElement::getNumAttributes() const
{
    return 2;
}

const char *PropertyDeclElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "is-array";
        default: return 0;
    }
}

const char *PropertyDeclElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return boolToString(isArray);
        default: return 0;
    }
}

void PropertyDeclElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: isArray = stringToBool(val); break;
        default: ;
    }
}

const char *PropertyDeclElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "false";
        default: return 0;
    }
}

PropertyDeclElement *PropertyDeclElement::dup() const
{
    PropertyDeclElement *element = new PropertyDeclElement();
    element->name = this->name;
    element->isArray = this->isArray;
    return element;
}

PropertyDeclElement *PropertyDeclElement::getNextPropertyDeclSibling() const
{
    return (PropertyDeclElement *)getNextSiblingWithTag(NED_PROPERTY_DECL);
}

CommentElement *PropertyDeclElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

PropertyKeyElement *PropertyDeclElement::getFirstPropertyKeyChild() const
{
    return (PropertyKeyElement *)getFirstChildWithTag(NED_PROPERTY_KEY);
}

PropertyElement *PropertyDeclElement::getFirstPropertyChild() const
{
    return (PropertyElement *)getFirstChildWithTag(NED_PROPERTY);
}

ExtendsElement::ExtendsElement()
{
    applyDefaults();
}

ExtendsElement::ExtendsElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ExtendsElement::getNumAttributes() const
{
    return 1;
}

const char *ExtendsElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *ExtendsElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void ExtendsElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *ExtendsElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

ExtendsElement *ExtendsElement::dup() const
{
    ExtendsElement *element = new ExtendsElement();
    element->name = this->name;
    return element;
}

ExtendsElement *ExtendsElement::getNextExtendsSibling() const
{
    return (ExtendsElement *)getNextSiblingWithTag(NED_EXTENDS);
}

CommentElement *ExtendsElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

InterfaceNameElement::InterfaceNameElement()
{
    applyDefaults();
}

InterfaceNameElement::InterfaceNameElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int InterfaceNameElement::getNumAttributes() const
{
    return 1;
}

const char *InterfaceNameElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *InterfaceNameElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void InterfaceNameElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *InterfaceNameElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

InterfaceNameElement *InterfaceNameElement::dup() const
{
    InterfaceNameElement *element = new InterfaceNameElement();
    element->name = this->name;
    return element;
}

InterfaceNameElement *InterfaceNameElement::getNextInterfaceNameSibling() const
{
    return (InterfaceNameElement *)getNextSiblingWithTag(NED_INTERFACE_NAME);
}

CommentElement *InterfaceNameElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

SimpleModuleElement::SimpleModuleElement()
{
    applyDefaults();
}

SimpleModuleElement::SimpleModuleElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int SimpleModuleElement::getNumAttributes() const
{
    return 1;
}

const char *SimpleModuleElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *SimpleModuleElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void SimpleModuleElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *SimpleModuleElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

SimpleModuleElement *SimpleModuleElement::dup() const
{
    SimpleModuleElement *element = new SimpleModuleElement();
    element->name = this->name;
    return element;
}

SimpleModuleElement *SimpleModuleElement::getNextSimpleModuleSibling() const
{
    return (SimpleModuleElement *)getNextSiblingWithTag(NED_SIMPLE_MODULE);
}

CommentElement *SimpleModuleElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExtendsElement *SimpleModuleElement::getFirstExtendsChild() const
{
    return (ExtendsElement *)getFirstChildWithTag(NED_EXTENDS);
}

InterfaceNameElement *SimpleModuleElement::getFirstInterfaceNameChild() const
{
    return (InterfaceNameElement *)getFirstChildWithTag(NED_INTERFACE_NAME);
}

ParametersElement *SimpleModuleElement::getFirstParametersChild() const
{
    return (ParametersElement *)getFirstChildWithTag(NED_PARAMETERS);
}

GatesElement *SimpleModuleElement::getFirstGatesChild() const
{
    return (GatesElement *)getFirstChildWithTag(NED_GATES);
}

ModuleInterfaceElement::ModuleInterfaceElement()
{
    applyDefaults();
}

ModuleInterfaceElement::ModuleInterfaceElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ModuleInterfaceElement::getNumAttributes() const
{
    return 1;
}

const char *ModuleInterfaceElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *ModuleInterfaceElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void ModuleInterfaceElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *ModuleInterfaceElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

ModuleInterfaceElement *ModuleInterfaceElement::dup() const
{
    ModuleInterfaceElement *element = new ModuleInterfaceElement();
    element->name = this->name;
    return element;
}

ModuleInterfaceElement *ModuleInterfaceElement::getNextModuleInterfaceSibling() const
{
    return (ModuleInterfaceElement *)getNextSiblingWithTag(NED_MODULE_INTERFACE);
}

CommentElement *ModuleInterfaceElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExtendsElement *ModuleInterfaceElement::getFirstExtendsChild() const
{
    return (ExtendsElement *)getFirstChildWithTag(NED_EXTENDS);
}

ParametersElement *ModuleInterfaceElement::getFirstParametersChild() const
{
    return (ParametersElement *)getFirstChildWithTag(NED_PARAMETERS);
}

GatesElement *ModuleInterfaceElement::getFirstGatesChild() const
{
    return (GatesElement *)getFirstChildWithTag(NED_GATES);
}

CompoundModuleElement::CompoundModuleElement()
{
    applyDefaults();
}

CompoundModuleElement::CompoundModuleElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int CompoundModuleElement::getNumAttributes() const
{
    return 1;
}

const char *CompoundModuleElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *CompoundModuleElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void CompoundModuleElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *CompoundModuleElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

CompoundModuleElement *CompoundModuleElement::dup() const
{
    CompoundModuleElement *element = new CompoundModuleElement();
    element->name = this->name;
    return element;
}

CompoundModuleElement *CompoundModuleElement::getNextCompoundModuleSibling() const
{
    return (CompoundModuleElement *)getNextSiblingWithTag(NED_COMPOUND_MODULE);
}

CommentElement *CompoundModuleElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExtendsElement *CompoundModuleElement::getFirstExtendsChild() const
{
    return (ExtendsElement *)getFirstChildWithTag(NED_EXTENDS);
}

InterfaceNameElement *CompoundModuleElement::getFirstInterfaceNameChild() const
{
    return (InterfaceNameElement *)getFirstChildWithTag(NED_INTERFACE_NAME);
}

ParametersElement *CompoundModuleElement::getFirstParametersChild() const
{
    return (ParametersElement *)getFirstChildWithTag(NED_PARAMETERS);
}

GatesElement *CompoundModuleElement::getFirstGatesChild() const
{
    return (GatesElement *)getFirstChildWithTag(NED_GATES);
}

TypesElement *CompoundModuleElement::getFirstTypesChild() const
{
    return (TypesElement *)getFirstChildWithTag(NED_TYPES);
}

SubmodulesElement *CompoundModuleElement::getFirstSubmodulesChild() const
{
    return (SubmodulesElement *)getFirstChildWithTag(NED_SUBMODULES);
}

ConnectionsElement *CompoundModuleElement::getFirstConnectionsChild() const
{
    return (ConnectionsElement *)getFirstChildWithTag(NED_CONNECTIONS);
}

ChannelInterfaceElement::ChannelInterfaceElement()
{
    applyDefaults();
}

ChannelInterfaceElement::ChannelInterfaceElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ChannelInterfaceElement::getNumAttributes() const
{
    return 1;
}

const char *ChannelInterfaceElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *ChannelInterfaceElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void ChannelInterfaceElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *ChannelInterfaceElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

ChannelInterfaceElement *ChannelInterfaceElement::dup() const
{
    ChannelInterfaceElement *element = new ChannelInterfaceElement();
    element->name = this->name;
    return element;
}

ChannelInterfaceElement *ChannelInterfaceElement::getNextChannelInterfaceSibling() const
{
    return (ChannelInterfaceElement *)getNextSiblingWithTag(NED_CHANNEL_INTERFACE);
}

CommentElement *ChannelInterfaceElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExtendsElement *ChannelInterfaceElement::getFirstExtendsChild() const
{
    return (ExtendsElement *)getFirstChildWithTag(NED_EXTENDS);
}

ParametersElement *ChannelInterfaceElement::getFirstParametersChild() const
{
    return (ParametersElement *)getFirstChildWithTag(NED_PARAMETERS);
}

ChannelElement::ChannelElement()
{
    applyDefaults();
}

ChannelElement::ChannelElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ChannelElement::getNumAttributes() const
{
    return 1;
}

const char *ChannelElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *ChannelElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void ChannelElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *ChannelElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

ChannelElement *ChannelElement::dup() const
{
    ChannelElement *element = new ChannelElement();
    element->name = this->name;
    return element;
}

ChannelElement *ChannelElement::getNextChannelSibling() const
{
    return (ChannelElement *)getNextSiblingWithTag(NED_CHANNEL);
}

CommentElement *ChannelElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExtendsElement *ChannelElement::getFirstExtendsChild() const
{
    return (ExtendsElement *)getFirstChildWithTag(NED_EXTENDS);
}

InterfaceNameElement *ChannelElement::getFirstInterfaceNameChild() const
{
    return (InterfaceNameElement *)getFirstChildWithTag(NED_INTERFACE_NAME);
}

ParametersElement *ChannelElement::getFirstParametersChild() const
{
    return (ParametersElement *)getFirstChildWithTag(NED_PARAMETERS);
}

ParametersElement::ParametersElement()
{
    isImplicit = false;
    applyDefaults();
}

ParametersElement::ParametersElement(NEDElement *parent) : NEDElement(parent)
{
    isImplicit = false;
    applyDefaults();
}

int ParametersElement::getNumAttributes() const
{
    return 1;
}

const char *ParametersElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "is-implicit";
        default: return 0;
    }
}

const char *ParametersElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return boolToString(isImplicit);
        default: return 0;
    }
}

void ParametersElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: isImplicit = stringToBool(val); break;
        default: ;
    }
}

const char *ParametersElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "false";
        default: return 0;
    }
}

ParametersElement *ParametersElement::dup() const
{
    ParametersElement *element = new ParametersElement();
    element->isImplicit = this->isImplicit;
    return element;
}

ParametersElement *ParametersElement::getNextParametersSibling() const
{
    return (ParametersElement *)getNextSiblingWithTag(NED_PARAMETERS);
}

CommentElement *ParametersElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

PropertyElement *ParametersElement::getFirstPropertyChild() const
{
    return (PropertyElement *)getFirstChildWithTag(NED_PROPERTY);
}

ParamElement *ParametersElement::getFirstParamChild() const
{
    return (ParamElement *)getFirstChildWithTag(NED_PARAM);
}

PatternElement *ParametersElement::getFirstPatternChild() const
{
    return (PatternElement *)getFirstChildWithTag(NED_PATTERN);
}

void ParamElement::setType(int val)
{
    validateEnum(val, partype_vals, partype_nums, partype_n);
    type = val;
}

ParamElement::ParamElement()
{
    type = 0;
    isVolatile = false;
    isDefault = false;
    applyDefaults();
}

ParamElement::ParamElement(NEDElement *parent) : NEDElement(parent)
{
    type = 0;
    isVolatile = false;
    isDefault = false;
    applyDefaults();
}

int ParamElement::getNumAttributes() const
{
    return 5;
}

const char *ParamElement::getAttributeName(int k) const
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

const char *ParamElement::getAttribute(int k) const
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

void ParamElement::setAttribute(int k, const char *val)
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

const char *ParamElement::getAttributeDefault(int k) const
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

ParamElement *ParamElement::dup() const
{
    ParamElement *element = new ParamElement();
    element->type = this->type;
    element->isVolatile = this->isVolatile;
    element->name = this->name;
    element->value = this->value;
    element->isDefault = this->isDefault;
    return element;
}

ParamElement *ParamElement::getNextParamSibling() const
{
    return (ParamElement *)getNextSiblingWithTag(NED_PARAM);
}

CommentElement *ParamElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionElement *ParamElement::getFirstExpressionChild() const
{
    return (ExpressionElement *)getFirstChildWithTag(NED_EXPRESSION);
}

PropertyElement *ParamElement::getFirstPropertyChild() const
{
    return (PropertyElement *)getFirstChildWithTag(NED_PROPERTY);
}

PatternElement::PatternElement()
{
    isDefault = false;
    applyDefaults();
}

PatternElement::PatternElement(NEDElement *parent) : NEDElement(parent)
{
    isDefault = false;
    applyDefaults();
}

int PatternElement::getNumAttributes() const
{
    return 3;
}

const char *PatternElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "pattern";
        case 1: return "value";
        case 2: return "is-default";
        default: return 0;
    }
}

const char *PatternElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return pattern.c_str();
        case 1: return value.c_str();
        case 2: return boolToString(isDefault);
        default: return 0;
    }
}

void PatternElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: pattern = val; break;
        case 1: value = val; break;
        case 2: isDefault = stringToBool(val); break;
        default: ;
    }
}

const char *PatternElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "false";
        default: return 0;
    }
}

PatternElement *PatternElement::dup() const
{
    PatternElement *element = new PatternElement();
    element->pattern = this->pattern;
    element->value = this->value;
    element->isDefault = this->isDefault;
    return element;
}

PatternElement *PatternElement::getNextPatternSibling() const
{
    return (PatternElement *)getNextSiblingWithTag(NED_PATTERN);
}

CommentElement *PatternElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionElement *PatternElement::getFirstExpressionChild() const
{
    return (ExpressionElement *)getFirstChildWithTag(NED_EXPRESSION);
}

PropertyElement *PatternElement::getFirstPropertyChild() const
{
    return (PropertyElement *)getFirstChildWithTag(NED_PROPERTY);
}

PropertyElement::PropertyElement()
{
    isImplicit = false;
    applyDefaults();
}

PropertyElement::PropertyElement(NEDElement *parent) : NEDElement(parent)
{
    isImplicit = false;
    applyDefaults();
}

int PropertyElement::getNumAttributes() const
{
    return 3;
}

const char *PropertyElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "is-implicit";
        case 1: return "name";
        case 2: return "index";
        default: return 0;
    }
}

const char *PropertyElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return boolToString(isImplicit);
        case 1: return name.c_str();
        case 2: return index.c_str();
        default: return 0;
    }
}

void PropertyElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: isImplicit = stringToBool(val); break;
        case 1: name = val; break;
        case 2: index = val; break;
        default: ;
    }
}

const char *PropertyElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "false";
        case 1: return NULL;
        case 2: return "";
        default: return 0;
    }
}

PropertyElement *PropertyElement::dup() const
{
    PropertyElement *element = new PropertyElement();
    element->isImplicit = this->isImplicit;
    element->name = this->name;
    element->index = this->index;
    return element;
}

PropertyElement *PropertyElement::getNextPropertySibling() const
{
    return (PropertyElement *)getNextSiblingWithTag(NED_PROPERTY);
}

CommentElement *PropertyElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

PropertyKeyElement *PropertyElement::getFirstPropertyKeyChild() const
{
    return (PropertyKeyElement *)getFirstChildWithTag(NED_PROPERTY_KEY);
}

PropertyKeyElement::PropertyKeyElement()
{
    applyDefaults();
}

PropertyKeyElement::PropertyKeyElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int PropertyKeyElement::getNumAttributes() const
{
    return 1;
}

const char *PropertyKeyElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *PropertyKeyElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void PropertyKeyElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *PropertyKeyElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

PropertyKeyElement *PropertyKeyElement::dup() const
{
    PropertyKeyElement *element = new PropertyKeyElement();
    element->name = this->name;
    return element;
}

PropertyKeyElement *PropertyKeyElement::getNextPropertyKeySibling() const
{
    return (PropertyKeyElement *)getNextSiblingWithTag(NED_PROPERTY_KEY);
}

CommentElement *PropertyKeyElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

LiteralElement *PropertyKeyElement::getFirstLiteralChild() const
{
    return (LiteralElement *)getFirstChildWithTag(NED_LITERAL);
}

GatesElement::GatesElement()
{
    applyDefaults();
}

GatesElement::GatesElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int GatesElement::getNumAttributes() const
{
    return 0;
}

const char *GatesElement::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *GatesElement::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void GatesElement::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *GatesElement::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

GatesElement *GatesElement::dup() const
{
    GatesElement *element = new GatesElement();
    return element;
}

GatesElement *GatesElement::getNextGatesSibling() const
{
    return (GatesElement *)getNextSiblingWithTag(NED_GATES);
}

CommentElement *GatesElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

GateElement *GatesElement::getFirstGateChild() const
{
    return (GateElement *)getFirstChildWithTag(NED_GATE);
}

void GateElement::setType(int val)
{
    validateEnum(val, gatetype_vals, gatetype_nums, gatetype_n);
    type = val;
}

GateElement::GateElement()
{
    type = 0;
    isVector = false;
    applyDefaults();
}

GateElement::GateElement(NEDElement *parent) : NEDElement(parent)
{
    type = 0;
    isVector = false;
    applyDefaults();
}

int GateElement::getNumAttributes() const
{
    return 4;
}

const char *GateElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "type";
        case 2: return "is-vector";
        case 3: return "vector-size";
        default: return 0;
    }
}

const char *GateElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return enumToString(type, gatetype_vals, gatetype_nums, gatetype_n);
        case 2: return boolToString(isVector);
        case 3: return vectorSize.c_str();
        default: return 0;
    }
}

void GateElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: type = stringToEnum(val, gatetype_vals, gatetype_nums, gatetype_n); break;
        case 2: isVector = stringToBool(val); break;
        case 3: vectorSize = val; break;
        default: ;
    }
}

const char *GateElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "false";
        case 3: return "";
        default: return 0;
    }
}

GateElement *GateElement::dup() const
{
    GateElement *element = new GateElement();
    element->name = this->name;
    element->type = this->type;
    element->isVector = this->isVector;
    element->vectorSize = this->vectorSize;
    return element;
}

GateElement *GateElement::getNextGateSibling() const
{
    return (GateElement *)getNextSiblingWithTag(NED_GATE);
}

CommentElement *GateElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionElement *GateElement::getFirstExpressionChild() const
{
    return (ExpressionElement *)getFirstChildWithTag(NED_EXPRESSION);
}

PropertyElement *GateElement::getFirstPropertyChild() const
{
    return (PropertyElement *)getFirstChildWithTag(NED_PROPERTY);
}

TypesElement::TypesElement()
{
    applyDefaults();
}

TypesElement::TypesElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int TypesElement::getNumAttributes() const
{
    return 0;
}

const char *TypesElement::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *TypesElement::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void TypesElement::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *TypesElement::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

TypesElement *TypesElement::dup() const
{
    TypesElement *element = new TypesElement();
    return element;
}

TypesElement *TypesElement::getNextTypesSibling() const
{
    return (TypesElement *)getNextSiblingWithTag(NED_TYPES);
}

CommentElement *TypesElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ChannelElement *TypesElement::getFirstChannelChild() const
{
    return (ChannelElement *)getFirstChildWithTag(NED_CHANNEL);
}

ChannelInterfaceElement *TypesElement::getFirstChannelInterfaceChild() const
{
    return (ChannelInterfaceElement *)getFirstChildWithTag(NED_CHANNEL_INTERFACE);
}

SimpleModuleElement *TypesElement::getFirstSimpleModuleChild() const
{
    return (SimpleModuleElement *)getFirstChildWithTag(NED_SIMPLE_MODULE);
}

CompoundModuleElement *TypesElement::getFirstCompoundModuleChild() const
{
    return (CompoundModuleElement *)getFirstChildWithTag(NED_COMPOUND_MODULE);
}

ModuleInterfaceElement *TypesElement::getFirstModuleInterfaceChild() const
{
    return (ModuleInterfaceElement *)getFirstChildWithTag(NED_MODULE_INTERFACE);
}

SubmodulesElement::SubmodulesElement()
{
    applyDefaults();
}

SubmodulesElement::SubmodulesElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int SubmodulesElement::getNumAttributes() const
{
    return 0;
}

const char *SubmodulesElement::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *SubmodulesElement::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void SubmodulesElement::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *SubmodulesElement::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

SubmodulesElement *SubmodulesElement::dup() const
{
    SubmodulesElement *element = new SubmodulesElement();
    return element;
}

SubmodulesElement *SubmodulesElement::getNextSubmodulesSibling() const
{
    return (SubmodulesElement *)getNextSiblingWithTag(NED_SUBMODULES);
}

CommentElement *SubmodulesElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

SubmoduleElement *SubmodulesElement::getFirstSubmoduleChild() const
{
    return (SubmoduleElement *)getFirstChildWithTag(NED_SUBMODULE);
}

SubmoduleElement::SubmoduleElement()
{
    applyDefaults();
}

SubmoduleElement::SubmoduleElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int SubmoduleElement::getNumAttributes() const
{
    return 5;
}

const char *SubmoduleElement::getAttributeName(int k) const
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

const char *SubmoduleElement::getAttribute(int k) const
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

void SubmoduleElement::setAttribute(int k, const char *val)
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

const char *SubmoduleElement::getAttributeDefault(int k) const
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

SubmoduleElement *SubmoduleElement::dup() const
{
    SubmoduleElement *element = new SubmoduleElement();
    element->name = this->name;
    element->type = this->type;
    element->likeType = this->likeType;
    element->likeParam = this->likeParam;
    element->vectorSize = this->vectorSize;
    return element;
}

SubmoduleElement *SubmoduleElement::getNextSubmoduleSibling() const
{
    return (SubmoduleElement *)getNextSiblingWithTag(NED_SUBMODULE);
}

CommentElement *SubmoduleElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionElement *SubmoduleElement::getFirstExpressionChild() const
{
    return (ExpressionElement *)getFirstChildWithTag(NED_EXPRESSION);
}

ParametersElement *SubmoduleElement::getFirstParametersChild() const
{
    return (ParametersElement *)getFirstChildWithTag(NED_PARAMETERS);
}

GatesElement *SubmoduleElement::getFirstGatesChild() const
{
    return (GatesElement *)getFirstChildWithTag(NED_GATES);
}

ConnectionsElement::ConnectionsElement()
{
    allowUnconnected = false;
    applyDefaults();
}

ConnectionsElement::ConnectionsElement(NEDElement *parent) : NEDElement(parent)
{
    allowUnconnected = false;
    applyDefaults();
}

int ConnectionsElement::getNumAttributes() const
{
    return 1;
}

const char *ConnectionsElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "allow-unconnected";
        default: return 0;
    }
}

const char *ConnectionsElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return boolToString(allowUnconnected);
        default: return 0;
    }
}

void ConnectionsElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: allowUnconnected = stringToBool(val); break;
        default: ;
    }
}

const char *ConnectionsElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "false";
        default: return 0;
    }
}

ConnectionsElement *ConnectionsElement::dup() const
{
    ConnectionsElement *element = new ConnectionsElement();
    element->allowUnconnected = this->allowUnconnected;
    return element;
}

ConnectionsElement *ConnectionsElement::getNextConnectionsSibling() const
{
    return (ConnectionsElement *)getNextSiblingWithTag(NED_CONNECTIONS);
}

CommentElement *ConnectionsElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ConnectionElement *ConnectionsElement::getFirstConnectionChild() const
{
    return (ConnectionElement *)getFirstChildWithTag(NED_CONNECTION);
}

ConnectionGroupElement *ConnectionsElement::getFirstConnectionGroupChild() const
{
    return (ConnectionGroupElement *)getFirstChildWithTag(NED_CONNECTION_GROUP);
}

void ConnectionElement::setSrcGateSubg(int val)
{
    validateEnum(val, subgate_vals, subgate_nums, subgate_n);
    srcGateSubg = val;
}

void ConnectionElement::setDestGateSubg(int val)
{
    validateEnum(val, subgate_vals, subgate_nums, subgate_n);
    destGateSubg = val;
}

void ConnectionElement::setArrowDirection(int val)
{
    validateEnum(val, arrowdir_vals, arrowdir_nums, arrowdir_n);
    arrowDirection = val;
}

ConnectionElement::ConnectionElement()
{
    srcGatePlusplus = false;
    srcGateSubg = 0;
    destGatePlusplus = false;
    destGateSubg = 0;
    arrowDirection = 0;
    applyDefaults();
}

ConnectionElement::ConnectionElement(NEDElement *parent) : NEDElement(parent)
{
    srcGatePlusplus = false;
    srcGateSubg = 0;
    destGatePlusplus = false;
    destGateSubg = 0;
    arrowDirection = 0;
    applyDefaults();
}

int ConnectionElement::getNumAttributes() const
{
    return 13;
}

const char *ConnectionElement::getAttributeName(int k) const
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

const char *ConnectionElement::getAttribute(int k) const
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

void ConnectionElement::setAttribute(int k, const char *val)
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

const char *ConnectionElement::getAttributeDefault(int k) const
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

ConnectionElement *ConnectionElement::dup() const
{
    ConnectionElement *element = new ConnectionElement();
    element->srcModule = this->srcModule;
    element->srcModuleIndex = this->srcModuleIndex;
    element->srcGate = this->srcGate;
    element->srcGatePlusplus = this->srcGatePlusplus;
    element->srcGateIndex = this->srcGateIndex;
    element->srcGateSubg = this->srcGateSubg;
    element->destModule = this->destModule;
    element->destModuleIndex = this->destModuleIndex;
    element->destGate = this->destGate;
    element->destGatePlusplus = this->destGatePlusplus;
    element->destGateIndex = this->destGateIndex;
    element->destGateSubg = this->destGateSubg;
    element->arrowDirection = this->arrowDirection;
    return element;
}

ConnectionElement *ConnectionElement::getNextConnectionSibling() const
{
    return (ConnectionElement *)getNextSiblingWithTag(NED_CONNECTION);
}

CommentElement *ConnectionElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionElement *ConnectionElement::getFirstExpressionChild() const
{
    return (ExpressionElement *)getFirstChildWithTag(NED_EXPRESSION);
}

ChannelSpecElement *ConnectionElement::getFirstChannelSpecChild() const
{
    return (ChannelSpecElement *)getFirstChildWithTag(NED_CHANNEL_SPEC);
}

LoopElement *ConnectionElement::getFirstLoopChild() const
{
    return (LoopElement *)getFirstChildWithTag(NED_LOOP);
}

ConditionElement *ConnectionElement::getFirstConditionChild() const
{
    return (ConditionElement *)getFirstChildWithTag(NED_CONDITION);
}

ChannelSpecElement::ChannelSpecElement()
{
    applyDefaults();
}

ChannelSpecElement::ChannelSpecElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ChannelSpecElement::getNumAttributes() const
{
    return 3;
}

const char *ChannelSpecElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "type";
        case 1: return "like-type";
        case 2: return "like-param";
        default: return 0;
    }
}

const char *ChannelSpecElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return type.c_str();
        case 1: return likeType.c_str();
        case 2: return likeParam.c_str();
        default: return 0;
    }
}

void ChannelSpecElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: type = val; break;
        case 1: likeType = val; break;
        case 2: likeParam = val; break;
        default: ;
    }
}

const char *ChannelSpecElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

ChannelSpecElement *ChannelSpecElement::dup() const
{
    ChannelSpecElement *element = new ChannelSpecElement();
    element->type = this->type;
    element->likeType = this->likeType;
    element->likeParam = this->likeParam;
    return element;
}

ChannelSpecElement *ChannelSpecElement::getNextChannelSpecSibling() const
{
    return (ChannelSpecElement *)getNextSiblingWithTag(NED_CHANNEL_SPEC);
}

CommentElement *ChannelSpecElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionElement *ChannelSpecElement::getFirstExpressionChild() const
{
    return (ExpressionElement *)getFirstChildWithTag(NED_EXPRESSION);
}

ParametersElement *ChannelSpecElement::getFirstParametersChild() const
{
    return (ParametersElement *)getFirstChildWithTag(NED_PARAMETERS);
}

ConnectionGroupElement::ConnectionGroupElement()
{
    applyDefaults();
}

ConnectionGroupElement::ConnectionGroupElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ConnectionGroupElement::getNumAttributes() const
{
    return 0;
}

const char *ConnectionGroupElement::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *ConnectionGroupElement::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void ConnectionGroupElement::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *ConnectionGroupElement::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

ConnectionGroupElement *ConnectionGroupElement::dup() const
{
    ConnectionGroupElement *element = new ConnectionGroupElement();
    return element;
}

ConnectionGroupElement *ConnectionGroupElement::getNextConnectionGroupSibling() const
{
    return (ConnectionGroupElement *)getNextSiblingWithTag(NED_CONNECTION_GROUP);
}

CommentElement *ConnectionGroupElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

LoopElement *ConnectionGroupElement::getFirstLoopChild() const
{
    return (LoopElement *)getFirstChildWithTag(NED_LOOP);
}

ConditionElement *ConnectionGroupElement::getFirstConditionChild() const
{
    return (ConditionElement *)getFirstChildWithTag(NED_CONDITION);
}

ConnectionElement *ConnectionGroupElement::getFirstConnectionChild() const
{
    return (ConnectionElement *)getFirstChildWithTag(NED_CONNECTION);
}

LoopElement::LoopElement()
{
    applyDefaults();
}

LoopElement::LoopElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int LoopElement::getNumAttributes() const
{
    return 3;
}

const char *LoopElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "param-name";
        case 1: return "from-value";
        case 2: return "to-value";
        default: return 0;
    }
}

const char *LoopElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return paramName.c_str();
        case 1: return fromValue.c_str();
        case 2: return toValue.c_str();
        default: return 0;
    }
}

void LoopElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: paramName = val; break;
        case 1: fromValue = val; break;
        case 2: toValue = val; break;
        default: ;
    }
}

const char *LoopElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

LoopElement *LoopElement::dup() const
{
    LoopElement *element = new LoopElement();
    element->paramName = this->paramName;
    element->fromValue = this->fromValue;
    element->toValue = this->toValue;
    return element;
}

LoopElement *LoopElement::getNextLoopSibling() const
{
    return (LoopElement *)getNextSiblingWithTag(NED_LOOP);
}

CommentElement *LoopElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionElement *LoopElement::getFirstExpressionChild() const
{
    return (ExpressionElement *)getFirstChildWithTag(NED_EXPRESSION);
}

ConditionElement::ConditionElement()
{
    applyDefaults();
}

ConditionElement::ConditionElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ConditionElement::getNumAttributes() const
{
    return 1;
}

const char *ConditionElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "condition";
        default: return 0;
    }
}

const char *ConditionElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return condition.c_str();
        default: return 0;
    }
}

void ConditionElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: condition = val; break;
        default: ;
    }
}

const char *ConditionElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

ConditionElement *ConditionElement::dup() const
{
    ConditionElement *element = new ConditionElement();
    element->condition = this->condition;
    return element;
}

ConditionElement *ConditionElement::getNextConditionSibling() const
{
    return (ConditionElement *)getNextSiblingWithTag(NED_CONDITION);
}

CommentElement *ConditionElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ExpressionElement *ConditionElement::getFirstExpressionChild() const
{
    return (ExpressionElement *)getFirstChildWithTag(NED_EXPRESSION);
}

ExpressionElement::ExpressionElement()
{
    applyDefaults();
}

ExpressionElement::ExpressionElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ExpressionElement::getNumAttributes() const
{
    return 1;
}

const char *ExpressionElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "target";
        default: return 0;
    }
}

const char *ExpressionElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return target.c_str();
        default: return 0;
    }
}

void ExpressionElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: target = val; break;
        default: ;
    }
}

const char *ExpressionElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        default: return 0;
    }
}

ExpressionElement *ExpressionElement::dup() const
{
    ExpressionElement *element = new ExpressionElement();
    element->target = this->target;
    return element;
}

ExpressionElement *ExpressionElement::getNextExpressionSibling() const
{
    return (ExpressionElement *)getNextSiblingWithTag(NED_EXPRESSION);
}

CommentElement *ExpressionElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

OperatorElement *ExpressionElement::getFirstOperatorChild() const
{
    return (OperatorElement *)getFirstChildWithTag(NED_OPERATOR);
}

FunctionElement *ExpressionElement::getFirstFunctionChild() const
{
    return (FunctionElement *)getFirstChildWithTag(NED_FUNCTION);
}

IdentElement *ExpressionElement::getFirstIdentChild() const
{
    return (IdentElement *)getFirstChildWithTag(NED_IDENT);
}

LiteralElement *ExpressionElement::getFirstLiteralChild() const
{
    return (LiteralElement *)getFirstChildWithTag(NED_LITERAL);
}

OperatorElement::OperatorElement()
{
    applyDefaults();
}

OperatorElement::OperatorElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int OperatorElement::getNumAttributes() const
{
    return 1;
}

const char *OperatorElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *OperatorElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void OperatorElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *OperatorElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

OperatorElement *OperatorElement::dup() const
{
    OperatorElement *element = new OperatorElement();
    element->name = this->name;
    return element;
}

OperatorElement *OperatorElement::getNextOperatorSibling() const
{
    return (OperatorElement *)getNextSiblingWithTag(NED_OPERATOR);
}

CommentElement *OperatorElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

OperatorElement *OperatorElement::getFirstOperatorChild() const
{
    return (OperatorElement *)getFirstChildWithTag(NED_OPERATOR);
}

FunctionElement *OperatorElement::getFirstFunctionChild() const
{
    return (FunctionElement *)getFirstChildWithTag(NED_FUNCTION);
}

IdentElement *OperatorElement::getFirstIdentChild() const
{
    return (IdentElement *)getFirstChildWithTag(NED_IDENT);
}

LiteralElement *OperatorElement::getFirstLiteralChild() const
{
    return (LiteralElement *)getFirstChildWithTag(NED_LITERAL);
}

FunctionElement::FunctionElement()
{
    applyDefaults();
}

FunctionElement::FunctionElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int FunctionElement::getNumAttributes() const
{
    return 1;
}

const char *FunctionElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *FunctionElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void FunctionElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *FunctionElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

FunctionElement *FunctionElement::dup() const
{
    FunctionElement *element = new FunctionElement();
    element->name = this->name;
    return element;
}

FunctionElement *FunctionElement::getNextFunctionSibling() const
{
    return (FunctionElement *)getNextSiblingWithTag(NED_FUNCTION);
}

CommentElement *FunctionElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

OperatorElement *FunctionElement::getFirstOperatorChild() const
{
    return (OperatorElement *)getFirstChildWithTag(NED_OPERATOR);
}

FunctionElement *FunctionElement::getFirstFunctionChild() const
{
    return (FunctionElement *)getFirstChildWithTag(NED_FUNCTION);
}

IdentElement *FunctionElement::getFirstIdentChild() const
{
    return (IdentElement *)getFirstChildWithTag(NED_IDENT);
}

LiteralElement *FunctionElement::getFirstLiteralChild() const
{
    return (LiteralElement *)getFirstChildWithTag(NED_LITERAL);
}

IdentElement::IdentElement()
{
    applyDefaults();
}

IdentElement::IdentElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int IdentElement::getNumAttributes() const
{
    return 2;
}

const char *IdentElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "module";
        case 1: return "name";
        default: return 0;
    }
}

const char *IdentElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return module.c_str();
        case 1: return name.c_str();
        default: return 0;
    }
}

void IdentElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: module = val; break;
        case 1: name = val; break;
        default: ;
    }
}

const char *IdentElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return NULL;
        default: return 0;
    }
}

IdentElement *IdentElement::dup() const
{
    IdentElement *element = new IdentElement();
    element->module = this->module;
    element->name = this->name;
    return element;
}

IdentElement *IdentElement::getNextIdentSibling() const
{
    return (IdentElement *)getNextSiblingWithTag(NED_IDENT);
}

CommentElement *IdentElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

OperatorElement *IdentElement::getFirstOperatorChild() const
{
    return (OperatorElement *)getFirstChildWithTag(NED_OPERATOR);
}

FunctionElement *IdentElement::getFirstFunctionChild() const
{
    return (FunctionElement *)getFirstChildWithTag(NED_FUNCTION);
}

IdentElement *IdentElement::getFirstIdentChild() const
{
    return (IdentElement *)getFirstChildWithTag(NED_IDENT);
}

LiteralElement *IdentElement::getFirstLiteralChild() const
{
    return (LiteralElement *)getFirstChildWithTag(NED_LITERAL);
}

void LiteralElement::setType(int val)
{
    validateEnum(val, littype_vals, littype_nums, littype_n);
    type = val;
}

LiteralElement::LiteralElement()
{
    type = 0;
    applyDefaults();
}

LiteralElement::LiteralElement(NEDElement *parent) : NEDElement(parent)
{
    type = 0;
    applyDefaults();
}

int LiteralElement::getNumAttributes() const
{
    return 4;
}

const char *LiteralElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "type";
        case 1: return "unit";
        case 2: return "text";
        case 3: return "value";
        default: return 0;
    }
}

const char *LiteralElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return enumToString(type, littype_vals, littype_nums, littype_n);
        case 1: return unit.c_str();
        case 2: return text.c_str();
        case 3: return value.c_str();
        default: return 0;
    }
}

void LiteralElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: type = stringToEnum(val, littype_vals, littype_nums, littype_n); break;
        case 1: unit = val; break;
        case 2: text = val; break;
        case 3: value = val; break;
        default: ;
    }
}

const char *LiteralElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "";
        case 3: return "";
        default: return 0;
    }
}

LiteralElement *LiteralElement::dup() const
{
    LiteralElement *element = new LiteralElement();
    element->type = this->type;
    element->unit = this->unit;
    element->text = this->text;
    element->value = this->value;
    return element;
}

LiteralElement *LiteralElement::getNextLiteralSibling() const
{
    return (LiteralElement *)getNextSiblingWithTag(NED_LITERAL);
}

CommentElement *LiteralElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

MsgFileElement::MsgFileElement()
{
    applyDefaults();
}

MsgFileElement::MsgFileElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int MsgFileElement::getNumAttributes() const
{
    return 2;
}

const char *MsgFileElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "filename";
        case 1: return "version";
        default: return 0;
    }
}

const char *MsgFileElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return filename.c_str();
        case 1: return version.c_str();
        default: return 0;
    }
}

void MsgFileElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: filename = val; break;
        case 1: version = val; break;
        default: ;
    }
}

const char *MsgFileElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return "";
        case 1: return "2";
        default: return 0;
    }
}

MsgFileElement *MsgFileElement::dup() const
{
    MsgFileElement *element = new MsgFileElement();
    element->filename = this->filename;
    element->version = this->version;
    return element;
}

MsgFileElement *MsgFileElement::getNextMsgFileSibling() const
{
    return (MsgFileElement *)getNextSiblingWithTag(NED_MSG_FILE);
}

CommentElement *MsgFileElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

NamespaceElement *MsgFileElement::getFirstNamespaceChild() const
{
    return (NamespaceElement *)getFirstChildWithTag(NED_NAMESPACE);
}

PropertyDeclElement *MsgFileElement::getFirstPropertyDeclChild() const
{
    return (PropertyDeclElement *)getFirstChildWithTag(NED_PROPERTY_DECL);
}

PropertyElement *MsgFileElement::getFirstPropertyChild() const
{
    return (PropertyElement *)getFirstChildWithTag(NED_PROPERTY);
}

CplusplusElement *MsgFileElement::getFirstCplusplusChild() const
{
    return (CplusplusElement *)getFirstChildWithTag(NED_CPLUSPLUS);
}

StructDeclElement *MsgFileElement::getFirstStructDeclChild() const
{
    return (StructDeclElement *)getFirstChildWithTag(NED_STRUCT_DECL);
}

ClassDeclElement *MsgFileElement::getFirstClassDeclChild() const
{
    return (ClassDeclElement *)getFirstChildWithTag(NED_CLASS_DECL);
}

MessageDeclElement *MsgFileElement::getFirstMessageDeclChild() const
{
    return (MessageDeclElement *)getFirstChildWithTag(NED_MESSAGE_DECL);
}

EnumDeclElement *MsgFileElement::getFirstEnumDeclChild() const
{
    return (EnumDeclElement *)getFirstChildWithTag(NED_ENUM_DECL);
}

EnumElement *MsgFileElement::getFirstEnumChild() const
{
    return (EnumElement *)getFirstChildWithTag(NED_ENUM);
}

MessageElement *MsgFileElement::getFirstMessageChild() const
{
    return (MessageElement *)getFirstChildWithTag(NED_MESSAGE);
}

ClassElement *MsgFileElement::getFirstClassChild() const
{
    return (ClassElement *)getFirstChildWithTag(NED_CLASS);
}

StructElement *MsgFileElement::getFirstStructChild() const
{
    return (StructElement *)getFirstChildWithTag(NED_STRUCT);
}

NamespaceElement::NamespaceElement()
{
    applyDefaults();
}

NamespaceElement::NamespaceElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int NamespaceElement::getNumAttributes() const
{
    return 1;
}

const char *NamespaceElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *NamespaceElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void NamespaceElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *NamespaceElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

NamespaceElement *NamespaceElement::dup() const
{
    NamespaceElement *element = new NamespaceElement();
    element->name = this->name;
    return element;
}

NamespaceElement *NamespaceElement::getNextNamespaceSibling() const
{
    return (NamespaceElement *)getNextSiblingWithTag(NED_NAMESPACE);
}

CommentElement *NamespaceElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

CplusplusElement::CplusplusElement()
{
    applyDefaults();
}

CplusplusElement::CplusplusElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int CplusplusElement::getNumAttributes() const
{
    return 1;
}

const char *CplusplusElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "body";
        default: return 0;
    }
}

const char *CplusplusElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return body.c_str();
        default: return 0;
    }
}

void CplusplusElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: body = val; break;
        default: ;
    }
}

const char *CplusplusElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

CplusplusElement *CplusplusElement::dup() const
{
    CplusplusElement *element = new CplusplusElement();
    element->body = this->body;
    return element;
}

CplusplusElement *CplusplusElement::getNextCplusplusSibling() const
{
    return (CplusplusElement *)getNextSiblingWithTag(NED_CPLUSPLUS);
}

CommentElement *CplusplusElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

StructDeclElement::StructDeclElement()
{
    applyDefaults();
}

StructDeclElement::StructDeclElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int StructDeclElement::getNumAttributes() const
{
    return 1;
}

const char *StructDeclElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *StructDeclElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void StructDeclElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *StructDeclElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

StructDeclElement *StructDeclElement::dup() const
{
    StructDeclElement *element = new StructDeclElement();
    element->name = this->name;
    return element;
}

StructDeclElement *StructDeclElement::getNextStructDeclSibling() const
{
    return (StructDeclElement *)getNextSiblingWithTag(NED_STRUCT_DECL);
}

CommentElement *StructDeclElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

ClassDeclElement::ClassDeclElement()
{
    isCobject = false;
    applyDefaults();
}

ClassDeclElement::ClassDeclElement(NEDElement *parent) : NEDElement(parent)
{
    isCobject = false;
    applyDefaults();
}

int ClassDeclElement::getNumAttributes() const
{
    return 2;
}

const char *ClassDeclElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "is-cobject";
        default: return 0;
    }
}

const char *ClassDeclElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return boolToString(isCobject);
        default: return 0;
    }
}

void ClassDeclElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: isCobject = stringToBool(val); break;
        default: ;
    }
}

const char *ClassDeclElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "false";
        default: return 0;
    }
}

ClassDeclElement *ClassDeclElement::dup() const
{
    ClassDeclElement *element = new ClassDeclElement();
    element->name = this->name;
    element->isCobject = this->isCobject;
    return element;
}

ClassDeclElement *ClassDeclElement::getNextClassDeclSibling() const
{
    return (ClassDeclElement *)getNextSiblingWithTag(NED_CLASS_DECL);
}

CommentElement *ClassDeclElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

MessageDeclElement::MessageDeclElement()
{
    applyDefaults();
}

MessageDeclElement::MessageDeclElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int MessageDeclElement::getNumAttributes() const
{
    return 1;
}

const char *MessageDeclElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *MessageDeclElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void MessageDeclElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *MessageDeclElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

MessageDeclElement *MessageDeclElement::dup() const
{
    MessageDeclElement *element = new MessageDeclElement();
    element->name = this->name;
    return element;
}

MessageDeclElement *MessageDeclElement::getNextMessageDeclSibling() const
{
    return (MessageDeclElement *)getNextSiblingWithTag(NED_MESSAGE_DECL);
}

CommentElement *MessageDeclElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

EnumDeclElement::EnumDeclElement()
{
    applyDefaults();
}

EnumDeclElement::EnumDeclElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int EnumDeclElement::getNumAttributes() const
{
    return 1;
}

const char *EnumDeclElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        default: return 0;
    }
}

const char *EnumDeclElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        default: return 0;
    }
}

void EnumDeclElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        default: ;
    }
}

const char *EnumDeclElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

EnumDeclElement *EnumDeclElement::dup() const
{
    EnumDeclElement *element = new EnumDeclElement();
    element->name = this->name;
    return element;
}

EnumDeclElement *EnumDeclElement::getNextEnumDeclSibling() const
{
    return (EnumDeclElement *)getNextSiblingWithTag(NED_ENUM_DECL);
}

CommentElement *EnumDeclElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

EnumElement::EnumElement()
{
    applyDefaults();
}

EnumElement::EnumElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int EnumElement::getNumAttributes() const
{
    return 3;
}

const char *EnumElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        default: return 0;
    }
}

const char *EnumElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        default: return 0;
    }
}

void EnumElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        default: ;
    }
}

const char *EnumElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

EnumElement *EnumElement::dup() const
{
    EnumElement *element = new EnumElement();
    element->name = this->name;
    element->extendsName = this->extendsName;
    element->sourceCode = this->sourceCode;
    return element;
}

EnumElement *EnumElement::getNextEnumSibling() const
{
    return (EnumElement *)getNextSiblingWithTag(NED_ENUM);
}

CommentElement *EnumElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

EnumFieldsElement *EnumElement::getFirstEnumFieldsChild() const
{
    return (EnumFieldsElement *)getFirstChildWithTag(NED_ENUM_FIELDS);
}

EnumFieldsElement::EnumFieldsElement()
{
    applyDefaults();
}

EnumFieldsElement::EnumFieldsElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int EnumFieldsElement::getNumAttributes() const
{
    return 0;
}

const char *EnumFieldsElement::getAttributeName(int k) const
{
    switch (k) {
        default: return 0;
    }
}

const char *EnumFieldsElement::getAttribute(int k) const
{
    switch (k) {
        default: return 0;
    }
}

void EnumFieldsElement::setAttribute(int k, const char *val)
{
    switch (k) {
        default: ;
    }
}

const char *EnumFieldsElement::getAttributeDefault(int k) const
{
    switch (k) {
        default: return 0;
    }
}

EnumFieldsElement *EnumFieldsElement::dup() const
{
    EnumFieldsElement *element = new EnumFieldsElement();
    return element;
}

EnumFieldsElement *EnumFieldsElement::getNextEnumFieldsSibling() const
{
    return (EnumFieldsElement *)getNextSiblingWithTag(NED_ENUM_FIELDS);
}

CommentElement *EnumFieldsElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

EnumFieldElement *EnumFieldsElement::getFirstEnumFieldChild() const
{
    return (EnumFieldElement *)getFirstChildWithTag(NED_ENUM_FIELD);
}

EnumFieldElement::EnumFieldElement()
{
    applyDefaults();
}

EnumFieldElement::EnumFieldElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int EnumFieldElement::getNumAttributes() const
{
    return 2;
}

const char *EnumFieldElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "value";
        default: return 0;
    }
}

const char *EnumFieldElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return value.c_str();
        default: return 0;
    }
}

void EnumFieldElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: value = val; break;
        default: ;
    }
}

const char *EnumFieldElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        default: return 0;
    }
}

EnumFieldElement *EnumFieldElement::dup() const
{
    EnumFieldElement *element = new EnumFieldElement();
    element->name = this->name;
    element->value = this->value;
    return element;
}

EnumFieldElement *EnumFieldElement::getNextEnumFieldSibling() const
{
    return (EnumFieldElement *)getNextSiblingWithTag(NED_ENUM_FIELD);
}

CommentElement *EnumFieldElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

MessageElement::MessageElement()
{
    applyDefaults();
}

MessageElement::MessageElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int MessageElement::getNumAttributes() const
{
    return 3;
}

const char *MessageElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        default: return 0;
    }
}

const char *MessageElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        default: return 0;
    }
}

void MessageElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        default: ;
    }
}

const char *MessageElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

MessageElement *MessageElement::dup() const
{
    MessageElement *element = new MessageElement();
    element->name = this->name;
    element->extendsName = this->extendsName;
    element->sourceCode = this->sourceCode;
    return element;
}

MessageElement *MessageElement::getNextMessageSibling() const
{
    return (MessageElement *)getNextSiblingWithTag(NED_MESSAGE);
}

CommentElement *MessageElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

PropertyElement *MessageElement::getFirstPropertyChild() const
{
    return (PropertyElement *)getFirstChildWithTag(NED_PROPERTY);
}

FieldElement *MessageElement::getFirstFieldChild() const
{
    return (FieldElement *)getFirstChildWithTag(NED_FIELD);
}

ClassElement::ClassElement()
{
    applyDefaults();
}

ClassElement::ClassElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int ClassElement::getNumAttributes() const
{
    return 3;
}

const char *ClassElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        default: return 0;
    }
}

const char *ClassElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        default: return 0;
    }
}

void ClassElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        default: ;
    }
}

const char *ClassElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

ClassElement *ClassElement::dup() const
{
    ClassElement *element = new ClassElement();
    element->name = this->name;
    element->extendsName = this->extendsName;
    element->sourceCode = this->sourceCode;
    return element;
}

ClassElement *ClassElement::getNextClassSibling() const
{
    return (ClassElement *)getNextSiblingWithTag(NED_CLASS);
}

CommentElement *ClassElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

PropertyElement *ClassElement::getFirstPropertyChild() const
{
    return (PropertyElement *)getFirstChildWithTag(NED_PROPERTY);
}

FieldElement *ClassElement::getFirstFieldChild() const
{
    return (FieldElement *)getFirstChildWithTag(NED_FIELD);
}

StructElement::StructElement()
{
    applyDefaults();
}

StructElement::StructElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int StructElement::getNumAttributes() const
{
    return 3;
}

const char *StructElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "name";
        case 1: return "extends-name";
        case 2: return "source-code";
        default: return 0;
    }
}

const char *StructElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return name.c_str();
        case 1: return extendsName.c_str();
        case 2: return sourceCode.c_str();
        default: return 0;
    }
}

void StructElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: name = val; break;
        case 1: extendsName = val; break;
        case 2: sourceCode = val; break;
        default: ;
    }
}

const char *StructElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        case 1: return "";
        case 2: return "";
        default: return 0;
    }
}

StructElement *StructElement::dup() const
{
    StructElement *element = new StructElement();
    element->name = this->name;
    element->extendsName = this->extendsName;
    element->sourceCode = this->sourceCode;
    return element;
}

StructElement *StructElement::getNextStructSibling() const
{
    return (StructElement *)getNextSiblingWithTag(NED_STRUCT);
}

CommentElement *StructElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

PropertyElement *StructElement::getFirstPropertyChild() const
{
    return (PropertyElement *)getFirstChildWithTag(NED_PROPERTY);
}

FieldElement *StructElement::getFirstFieldChild() const
{
    return (FieldElement *)getFirstChildWithTag(NED_FIELD);
}

FieldElement::FieldElement()
{
    isAbstract = false;
    isReadonly = false;
    isVector = false;
    applyDefaults();
}

FieldElement::FieldElement(NEDElement *parent) : NEDElement(parent)
{
    isAbstract = false;
    isReadonly = false;
    isVector = false;
    applyDefaults();
}

int FieldElement::getNumAttributes() const
{
    return 8;
}

const char *FieldElement::getAttributeName(int k) const
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

const char *FieldElement::getAttribute(int k) const
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

void FieldElement::setAttribute(int k, const char *val)
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

const char *FieldElement::getAttributeDefault(int k) const
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

FieldElement *FieldElement::dup() const
{
    FieldElement *element = new FieldElement();
    element->name = this->name;
    element->dataType = this->dataType;
    element->isAbstract = this->isAbstract;
    element->isReadonly = this->isReadonly;
    element->isVector = this->isVector;
    element->vectorSize = this->vectorSize;
    element->enumName = this->enumName;
    element->defaultValue = this->defaultValue;
    return element;
}

FieldElement *FieldElement::getNextFieldSibling() const
{
    return (FieldElement *)getNextSiblingWithTag(NED_FIELD);
}

CommentElement *FieldElement::getFirstCommentChild() const
{
    return (CommentElement *)getFirstChildWithTag(NED_COMMENT);
}

UnknownElement::UnknownElement()
{
    applyDefaults();
}

UnknownElement::UnknownElement(NEDElement *parent) : NEDElement(parent)
{
    applyDefaults();
}

int UnknownElement::getNumAttributes() const
{
    return 1;
}

const char *UnknownElement::getAttributeName(int k) const
{
    switch (k) {
        case 0: return "element";
        default: return 0;
    }
}

const char *UnknownElement::getAttribute(int k) const
{
    switch (k) {
        case 0: return element.c_str();
        default: return 0;
    }
}

void UnknownElement::setAttribute(int k, const char *val)
{
    switch (k) {
        case 0: element = val; break;
        default: ;
    }
}

const char *UnknownElement::getAttributeDefault(int k) const
{
    switch (k) {
        case 0: return NULL;
        default: return 0;
    }
}

UnknownElement *UnknownElement::dup() const
{
    UnknownElement *element = new UnknownElement();
    element->element = this->element;
    return element;
}

UnknownElement *UnknownElement::getNextUnknownSibling() const
{
    return (UnknownElement *)getNextSiblingWithTag(NED_UNKNOWN);
}

NEDElementFactory *NEDElementFactory::f;

NEDElementFactory *NEDElementFactory::getInstance()
{
    if (!f) f=new NEDElementFactory();
    return f;
}

NEDElement *NEDElementFactory::createElementWithTag(const char *tagname)
{
    if (tagname[0]=='f' && !strcmp(tagname,"files"))  return new FilesElement();
    if (tagname[0]=='n' && !strcmp(tagname,"ned-file"))  return new NedFileElement();
    if (tagname[0]=='c' && !strcmp(tagname,"comment"))  return new CommentElement();
    if (tagname[0]=='p' && !strcmp(tagname,"package"))  return new PackageElement();
    if (tagname[0]=='i' && !strcmp(tagname,"import"))  return new ImportElement();
    if (tagname[0]=='p' && !strcmp(tagname,"property-decl"))  return new PropertyDeclElement();
    if (tagname[0]=='e' && !strcmp(tagname,"extends"))  return new ExtendsElement();
    if (tagname[0]=='i' && !strcmp(tagname,"interface-name"))  return new InterfaceNameElement();
    if (tagname[0]=='s' && !strcmp(tagname,"simple-module"))  return new SimpleModuleElement();
    if (tagname[0]=='m' && !strcmp(tagname,"module-interface"))  return new ModuleInterfaceElement();
    if (tagname[0]=='c' && !strcmp(tagname,"compound-module"))  return new CompoundModuleElement();
    if (tagname[0]=='c' && !strcmp(tagname,"channel-interface"))  return new ChannelInterfaceElement();
    if (tagname[0]=='c' && !strcmp(tagname,"channel"))  return new ChannelElement();
    if (tagname[0]=='p' && !strcmp(tagname,"parameters"))  return new ParametersElement();
    if (tagname[0]=='p' && !strcmp(tagname,"param"))  return new ParamElement();
    if (tagname[0]=='p' && !strcmp(tagname,"pattern"))  return new PatternElement();
    if (tagname[0]=='p' && !strcmp(tagname,"property"))  return new PropertyElement();
    if (tagname[0]=='p' && !strcmp(tagname,"property-key"))  return new PropertyKeyElement();
    if (tagname[0]=='g' && !strcmp(tagname,"gates"))  return new GatesElement();
    if (tagname[0]=='g' && !strcmp(tagname,"gate"))  return new GateElement();
    if (tagname[0]=='t' && !strcmp(tagname,"types"))  return new TypesElement();
    if (tagname[0]=='s' && !strcmp(tagname,"submodules"))  return new SubmodulesElement();
    if (tagname[0]=='s' && !strcmp(tagname,"submodule"))  return new SubmoduleElement();
    if (tagname[0]=='c' && !strcmp(tagname,"connections"))  return new ConnectionsElement();
    if (tagname[0]=='c' && !strcmp(tagname,"connection"))  return new ConnectionElement();
    if (tagname[0]=='c' && !strcmp(tagname,"channel-spec"))  return new ChannelSpecElement();
    if (tagname[0]=='c' && !strcmp(tagname,"connection-group"))  return new ConnectionGroupElement();
    if (tagname[0]=='l' && !strcmp(tagname,"loop"))  return new LoopElement();
    if (tagname[0]=='c' && !strcmp(tagname,"condition"))  return new ConditionElement();
    if (tagname[0]=='e' && !strcmp(tagname,"expression"))  return new ExpressionElement();
    if (tagname[0]=='o' && !strcmp(tagname,"operator"))  return new OperatorElement();
    if (tagname[0]=='f' && !strcmp(tagname,"function"))  return new FunctionElement();
    if (tagname[0]=='i' && !strcmp(tagname,"ident"))  return new IdentElement();
    if (tagname[0]=='l' && !strcmp(tagname,"literal"))  return new LiteralElement();
    if (tagname[0]=='m' && !strcmp(tagname,"msg-file"))  return new MsgFileElement();
    if (tagname[0]=='n' && !strcmp(tagname,"namespace"))  return new NamespaceElement();
    if (tagname[0]=='c' && !strcmp(tagname,"cplusplus"))  return new CplusplusElement();
    if (tagname[0]=='s' && !strcmp(tagname,"struct-decl"))  return new StructDeclElement();
    if (tagname[0]=='c' && !strcmp(tagname,"class-decl"))  return new ClassDeclElement();
    if (tagname[0]=='m' && !strcmp(tagname,"message-decl"))  return new MessageDeclElement();
    if (tagname[0]=='e' && !strcmp(tagname,"enum-decl"))  return new EnumDeclElement();
    if (tagname[0]=='e' && !strcmp(tagname,"enum"))  return new EnumElement();
    if (tagname[0]=='e' && !strcmp(tagname,"enum-fields"))  return new EnumFieldsElement();
    if (tagname[0]=='e' && !strcmp(tagname,"enum-field"))  return new EnumFieldElement();
    if (tagname[0]=='m' && !strcmp(tagname,"message"))  return new MessageElement();
    if (tagname[0]=='c' && !strcmp(tagname,"class"))  return new ClassElement();
    if (tagname[0]=='s' && !strcmp(tagname,"struct"))  return new StructElement();
    if (tagname[0]=='f' && !strcmp(tagname,"field"))  return new FieldElement();
    if (tagname[0]=='u' && !strcmp(tagname,"unknown"))  return new UnknownElement();
    throw NEDException("unknown tag '%s', cannot create object to represent it", tagname);
}

NEDElement *NEDElementFactory::createElementWithTag(int tagcode)
{
    switch (tagcode) {
        case NED_FILES: return new FilesElement();
        case NED_NED_FILE: return new NedFileElement();
        case NED_COMMENT: return new CommentElement();
        case NED_PACKAGE: return new PackageElement();
        case NED_IMPORT: return new ImportElement();
        case NED_PROPERTY_DECL: return new PropertyDeclElement();
        case NED_EXTENDS: return new ExtendsElement();
        case NED_INTERFACE_NAME: return new InterfaceNameElement();
        case NED_SIMPLE_MODULE: return new SimpleModuleElement();
        case NED_MODULE_INTERFACE: return new ModuleInterfaceElement();
        case NED_COMPOUND_MODULE: return new CompoundModuleElement();
        case NED_CHANNEL_INTERFACE: return new ChannelInterfaceElement();
        case NED_CHANNEL: return new ChannelElement();
        case NED_PARAMETERS: return new ParametersElement();
        case NED_PARAM: return new ParamElement();
        case NED_PATTERN: return new PatternElement();
        case NED_PROPERTY: return new PropertyElement();
        case NED_PROPERTY_KEY: return new PropertyKeyElement();
        case NED_GATES: return new GatesElement();
        case NED_GATE: return new GateElement();
        case NED_TYPES: return new TypesElement();
        case NED_SUBMODULES: return new SubmodulesElement();
        case NED_SUBMODULE: return new SubmoduleElement();
        case NED_CONNECTIONS: return new ConnectionsElement();
        case NED_CONNECTION: return new ConnectionElement();
        case NED_CHANNEL_SPEC: return new ChannelSpecElement();
        case NED_CONNECTION_GROUP: return new ConnectionGroupElement();
        case NED_LOOP: return new LoopElement();
        case NED_CONDITION: return new ConditionElement();
        case NED_EXPRESSION: return new ExpressionElement();
        case NED_OPERATOR: return new OperatorElement();
        case NED_FUNCTION: return new FunctionElement();
        case NED_IDENT: return new IdentElement();
        case NED_LITERAL: return new LiteralElement();
        case NED_MSG_FILE: return new MsgFileElement();
        case NED_NAMESPACE: return new NamespaceElement();
        case NED_CPLUSPLUS: return new CplusplusElement();
        case NED_STRUCT_DECL: return new StructDeclElement();
        case NED_CLASS_DECL: return new ClassDeclElement();
        case NED_MESSAGE_DECL: return new MessageDeclElement();
        case NED_ENUM_DECL: return new EnumDeclElement();
        case NED_ENUM: return new EnumElement();
        case NED_ENUM_FIELDS: return new EnumFieldsElement();
        case NED_ENUM_FIELD: return new EnumFieldElement();
        case NED_MESSAGE: return new MessageElement();
        case NED_CLASS: return new ClassElement();
        case NED_STRUCT: return new StructElement();
        case NED_FIELD: return new FieldElement();
        case NED_UNKNOWN: return new UnknownElement();
    }
    throw NEDException("unknown tag code %d, cannot create object to represent it", tagcode);
}

NAMESPACE_END

