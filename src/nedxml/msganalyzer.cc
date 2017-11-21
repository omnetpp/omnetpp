//==========================================================================
//  MSGANALYZER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "common/stringtokenizer.h"
#include "common/fileutil.h"
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "msgcompiler.h"
#include "msganalyzer.h"
#include "exception.h"

//#include "omnetpp/simkerneldefs.h"

//TODO camelize names of supported properties!!! (e.g. cpptype -> cppType, fromstring -> fromString)
//TODO field's props should be produced by merging the type's props into it? (to be accessible from inspectors)

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

#define SL(x)    (x)

inline std::string str(const char *s)
{
    return s ? s : "";
}

static char charToNameFilter(char ch)
{
    return isalnum(ch) ? ch : '_';
}

inline bool isQualified(const std::string& qname)
{
    return qname.find("::") != qname.npos;
}

static std::string makeIdentifier(const std::string& qname)
{
    std::string tmp = qname;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), charToNameFilter);
    return tmp;
}

template<typename T>
std::string join(const T& v, const std::string& delim)
{
    std::ostringstream s;
    for (typename T::const_iterator i = v.begin(); i != v.end(); ++i) {
        if (i != v.begin()) {
            s << delim;
        }
        s << *i;
    }
    return s.str();
}

// Note: based on omnetpp 5.x checkandcast.h (merged check_and_cast and check_and_cast_failure)
template<class P, class T>
P check_and_cast(T *p)
{
    assert(p);
    P ret = dynamic_cast<P>(p);
    assert(ret);
    return ret;
}

MsgAnalyzer::StringSet MsgAnalyzer::RESERVED_WORDS = {
        "namespace", "cplusplus", "struct", "message", "packet", "class", "noncobject",
        "enum", "extends", "abstract", "readonly", "properties", "fields", "unsigned",
        "true", "false", "for", "while", "if", "else", "do", "enum", "class", "struct",
        "typedef", "public", "private", "protected", "auto", "register", "sizeof", "void",
        "new", "delete", "explicit", "static", "extern", "return", "try", "catch",
};

MsgAnalyzer::MsgAnalyzer(const MsgCompilerOptions& opts, MsgTypeTable *typeTable, ErrorStore *errors) : errors(errors), typeTable(typeTable), opts(opts)
{
}

MsgAnalyzer::~MsgAnalyzer()
{
}

MsgAnalyzer::Properties MsgAnalyzer::extractPropertiesOf(ASTNode *node)
{
    Properties props;

    for (PropertyElement *child = static_cast<PropertyElement *>(node->getFirstChildWithTag(NED_PROPERTY)); child; child = child->getNextPropertySibling()) {
        std::string propname = str(child->getAttribute("name"));
        std::string propval;
        for (PropertyKeyElement *key = child->getFirstPropertyKeyChild(); key; key = key->getNextPropertyKeySibling()) {
            std::string keyname = str(key->getAttribute("name"));
            if (keyname.empty()) {
                const char *sep = "";
                for (LiteralElement *lit = key->getFirstLiteralChild(); lit; lit = lit->getNextLiteralSibling()) {
                    std::string s = str(lit->getAttribute("value"));
                    propval = propval + sep + s;
                    sep = ",";
                }
            }
        }
        if (props.find(propname) == props.end()) {
            props[propname] = propval;
        }
        else {
            errors->addError(child, "the property '%s' is duplicated", propname.c_str());
        }
    }
    return props;
}

MsgAnalyzer::ClassInfo MsgAnalyzer::makeIncompleteClassInfo(ASTNode *node, const std::string& namespaceName)
{
    ClassInfo classInfo;
    classInfo.nedElement = node;
    classInfo.props = extractPropertiesOf(node);
    std::string actually = getProperty(classInfo.props, "actually", "");
    classInfo.keyword = node->getTagName();
    classInfo.msgname = actually != "" ? actually : str(node->getAttribute("name"));
    classInfo.namespacename = namespaceName;
    classInfo.msgqname = prefixWithNamespace(classInfo.msgname, namespaceName);
    return classInfo;
}

void MsgAnalyzer::ensureAnalyzed(ClassInfo& classInfo)
{
    if (!classInfo.classInfoComplete) {
        extractClassInfo(classInfo);
        analyzeClassOrStruct(classInfo, classInfo.namespacename);
        classInfo.classInfoComplete = true;
    }
}

void MsgAnalyzer::ensureFieldsAnalyzed(ClassInfo& classInfo)
{
    if (!classInfo.fieldsComplete) {
        analyzeFields(classInfo, classInfo.namespacename);
        classInfo.fieldsComplete = true;
    }
}

void MsgAnalyzer::extractClassInfo(ClassInfo& classInfo)
{
    ASTNode *node = classInfo.nedElement;

    classInfo.msgbase = str(node->getAttribute("extends-name"));

    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case NED_FIELD: {
                FieldInfo field;
                field.nedElement = child;
                field.fname = str(child->getAttribute("name"));
                field.datatype = str(child->getAttribute("data-type"));
                field.ftype = str(child->getAttribute("data-type"));
                field.fval = str(child->getAttribute("default-value"));
                field.fisabstract = str(child->getAttribute("is-abstract")) == "true";
                field.fispointer = (field.ftype[field.ftype.length()-1] == '*');
                if (field.fispointer) {
                    field.ftype = field.ftype.substr(0, field.ftype.find_last_not_of(" \t*")+1);
                }
                field.fisarray = str(child->getAttribute("is-vector")) == "true";
                field.farraysize = str(child->getAttribute("vector-size"));

                field.fprops = extractPropertiesOf(child);

                if (field.ftype.empty())
                    classInfo.baseclassFieldlist.push_back(field);
                else
                    classInfo.fieldlist.push_back(field);
                break;
            }

            case NED_PROPERTY:
                // skip properties here, properties already extracted
                break;

            case NED_PROPERTY_DECL:
                errors->addError(child, "syntax error: property '%s' declaration unaccepted here", child->getTagName()); //TODO into some validator
                break;

            case NED_PROPERTY_KEY:
                errors->addError(child, "syntax error: property key '%s' unaccepted here", child->getTagName()); //TODO into some validator
                break;

            case NED_COMMENT:
                break;

            default:
                errors->addError(child, "unaccepted element: '%s'", child->getTagName());
                break;
        }
    }
}

bool MsgAnalyzer::hasSuperclass(ClassInfo& classInfo, const std::string& superclassQName)
{
    ClassInfo *currentClass = &classInfo;
    while (currentClass->msgbaseqname != "") {
        currentClass = &typeTable->getClassInfo(currentClass->msgbaseqname);
        if (currentClass->msgqname == superclassQName)
            return true;
        ensureAnalyzed(*currentClass);
    }
    return false;
}

void MsgAnalyzer::analyzeClassOrStruct(ClassInfo& classInfo, const std::string& namespaceName)
{
    // determine base class
    if (classInfo.msgbase == "") {
        if (classInfo.keyword == "message")
            classInfo.msgbaseqname = "omnetpp::cMessage";
        else if (classInfo.keyword == "packet")
            classInfo.msgbaseqname = "omnetpp::cPacket";
        else
            classInfo.msgbaseqname = "";
    }
    else {
        classInfo.msgbaseqname = lookupExistingClassName(classInfo.msgbase, namespaceName);
        if (classInfo.msgbaseqname.empty())
            errors->addError(classInfo.nedElement, "'%s': unknown base class '%s'", classInfo.msgname.c_str(), classInfo.msgbase.c_str());
    }

    // resolve base class/struct
    ClassInfo *baseClassInfo = nullptr;
    if (classInfo.msgbaseqname != "") {
        baseClassInfo = &typeTable->getClassInfo(classInfo.msgbaseqname);
        ensureAnalyzed(*baseClassInfo);
        if (baseClassInfo->isprimitivetype)
            errors->addError(classInfo.nedElement, "'%s': primitive type '%s' cannot be used as base class", classInfo.msgname.c_str(), classInfo.msgbase.c_str());
    }

    // determine class type
    classInfo.isClass = (classInfo.keyword != "struct");
    classInfo.iscObject = classInfo.iscNamedObject = classInfo.iscOwnedObject = false;
    if (classInfo.isClass) {
        if (classInfo.msgqname == "omnetpp::cObject")
            classInfo.iscObject = true;
        else if (classInfo.msgqname == "omnetpp::cNamedObject")
            classInfo.iscObject = classInfo.iscNamedObject = true;
        else if (classInfo.msgqname == "omnetpp::cOwnedObject")
            classInfo.iscObject = classInfo.iscNamedObject = classInfo.iscOwnedObject = true;
        else if (baseClassInfo != nullptr) {
            if (!baseClassInfo->isClass)
                errors->addError(classInfo.nedElement, "%s: base type is not a class", classInfo.msgname.c_str());
            classInfo.iscObject = baseClassInfo->iscObject;
            classInfo.iscNamedObject = baseClassInfo->iscNamedObject;
            classInfo.iscOwnedObject = baseClassInfo->iscOwnedObject;
        }
        else {
            // assume non-cObject
        }
    }
    else {
        // struct
        if (baseClassInfo != nullptr && baseClassInfo->isClass)
            errors->addError(classInfo.nedElement, "%s: base type is not a struct", classInfo.msgname.c_str());
    }

    // message must subclass from cMessage, packet ditto
    std::string requiredSuperClass;
    if (classInfo.keyword == "message")
        requiredSuperClass = "omnetpp::cMessage";
    else if (classInfo.keyword == "packet")
        requiredSuperClass = "omnetpp::cPacket";
    if (!requiredSuperClass.empty() && !hasSuperclass(classInfo, requiredSuperClass))
        errors->addError(classInfo.nedElement, "%s: base class is not a %s (must be derived from %s)", classInfo.msgname.c_str(), classInfo.keyword.c_str(), requiredSuperClass.c_str());


    // isPrimitive, isOpaque, byValue, data types, etc.
    classInfo.isprimitivetype = getPropertyAsBool(classInfo.props, "primitive", false);
    classInfo.isopaque = getPropertyAsBool(classInfo.props, "opaque", classInfo.isprimitivetype); // primitive types are also opaque and passed by value
    classInfo.byvalue = getPropertyAsBool(classInfo.props, "byvalue", classInfo.isprimitivetype);
    classInfo.defaultvalue = getProperty(classInfo.props, "defaultvalue", "");

    classInfo.datatype = getProperty(classInfo.props, "cpptype", "");
    classInfo.argtype = getProperty(classInfo.props, "argtype", "");
    classInfo.rettype  = getProperty(classInfo.props, "rettype", "");

    classInfo.tostring  = getProperty(classInfo.props, "tostring", "");
    classInfo.fromstring  = getProperty(classInfo.props, "fromstring", "");
    classInfo.maybe_c_str  = getProperty(classInfo.props, "maybe_c_str", "");

    // generation gap
    bool existingClass = getPropertyAsBool(classInfo.props, "existingClass", false);
    classInfo.generate_class = opts.generateClasses && !existingClass;
    classInfo.generate_descriptor = opts.generateDescriptors && !classInfo.isopaque && getPropertyAsBool(classInfo.props, "descriptor", true);  // opaque also means no descriptor
    classInfo.generate_setters_in_descriptor = opts.generateSettersInDescriptors && (getProperty(classInfo.props, "descriptor") != "readonly");

    if (!existingClass && isQualified(classInfo.msgname))
        errors->addError(classInfo.nedElement, "class name may only contain '::' when generating descriptor for an existing class");

    classInfo.gap = getPropertyAsBool(classInfo.props, "customize", false);

    if (classInfo.gap) {
        classInfo.msgclass = classInfo.msgname + "_Base";
        classInfo.realmsgclass = classInfo.msgname;
        classInfo.msgdescclass = makeIdentifier(classInfo.realmsgclass) + "Descriptor";
    }
    else {
        classInfo.msgclass = classInfo.msgname;
        classInfo.realmsgclass = classInfo.msgname;
        classInfo.msgdescclass = makeIdentifier(classInfo.msgclass) + "Descriptor";
    }

    classInfo.msgbaseclass = classInfo.msgbaseqname;

    // omitGetVerb / fieldNameSuffix
    classInfo.omitgetverb = getPropertyAsBool(classInfo.props, "omitGetVerb", false);
    classInfo.fieldnamesuffix = getProperty(classInfo.props, "fieldNameSuffix", "");
    if (classInfo.omitgetverb && classInfo.fieldnamesuffix.empty()) {
        errors->addWarning(classInfo.nedElement, "@omitGetVerb(true) and (implicit) @fieldNameSuffix(\"\") collide: "
                                                 "adding '_var' suffix to data members to prevent name conflict between them and getter methods");
        classInfo.fieldnamesuffix = "_var";
    }

    // beforeChange
    classInfo.beforeChange = getProperty(classInfo.props, "beforeChange", "");
    if (classInfo.beforeChange.empty() && baseClassInfo != nullptr)
        classInfo.beforeChange = baseClassInfo->beforeChange;

    // additional base classes (interfaces)
    std::string s = getProperty(classInfo.props, "implements");
    if (!s.empty())
        classInfo.implements = StringTokenizer(s.c_str(), ",").asVector();
}

void MsgAnalyzer::analyzeFields(ClassInfo& classInfo, const std::string& namespaceName)
{
    for (auto& field : classInfo.fieldlist)
        analyzeField(classInfo, &field, namespaceName);
    for (auto& field :  classInfo.baseclassFieldlist)
        analyzeInheritedField(classInfo, &field);
}

void MsgAnalyzer::analyzeField(ClassInfo& classInfo, FieldInfo *field, const std::string& namespaceName)
{
    Assert(!field->ftype.empty());

    if (field->fisabstract && !classInfo.isClass)
        errors->addError(field->nedElement, "A struct cannot have abstract fields");

    if (field->fisabstract && !field->fval.empty())
        errors->addError(field->nedElement, "An abstract field cannot be assigned a default value");

    if (field->fisarray && field->farraysize.empty() && !classInfo.isClass)
        errors->addError(field->nedElement, "A struct cannot have dynamic array fields");

    if (field->fisabstract && !classInfo.gap)
        errors->addError(field->nedElement, "abstract fields need '@customize(true)' property in '%s'", classInfo.msgname.c_str());

    field->ftypeqname = lookupExistingClassName(field->ftype, namespaceName, &classInfo);
    if (field->ftypeqname.empty()) {
        errors->addError(field->nedElement, "unknown type '%s' for field '%s' in '%s'", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
        field->ftypeqname = "omnetpp::cObject";
    }

    ClassInfo& fieldClassInfo = typeTable->getClassInfo(field->ftypeqname);
    ensureAnalyzed(fieldClassInfo);
    field->fisprimitivetype = fieldClassInfo.isprimitivetype;
    field->isClass = fieldClassInfo.isClass;
    field->iscObject = fieldClassInfo.iscObject;
    field->iscNamedObject = fieldClassInfo.iscNamedObject;
    field->iscOwnedObject = fieldClassInfo.iscOwnedObject;

    if (classInfo.generate_class)
        if (field->iscOwnedObject && !classInfo.iscObject)
            errors->addError(field->nedElement, "cannot use cOwnedObject field '%s %s' in struct or non-cObject class '%s'", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());

    if (field->fispointer && field->fisprimitivetype) {
        errors->addError(field->nedElement, "pointers not supported for primitive types in '%s'", classInfo.msgname.c_str());
        return;
    }

    if (field->fispointer && field->fval.empty())
        field->fval = "nullptr";
    if (field->fval.empty() && !fieldClassInfo.defaultvalue.empty())
        field->fval = fieldClassInfo.defaultvalue;

    field->fnopack = getPropertyAsBool(field->fprops, "nopack", false);
    field->feditable = getPropertyAsBool(field->fprops, "editable", false);
    field->editNotDisabled = getPropertyAsBool(field->fprops, "editable", true);
    field->fopaque = getPropertyAsBool(field->fprops, "opaque", fieldClassInfo.isopaque);
    field->overrideGetter = getPropertyAsBool(field->fprops, "overridegetter", false) || getPropertyAsBool(field->fprops, "override", false);
    field->overrideSetter = getPropertyAsBool(field->fprops, "overridesetter", false) || getPropertyAsBool(field->fprops, "override", false);

    // resolve enum
    field->enumname = getProperty(field->fprops, "enum");
    if (!field->enumname.empty()) {
        StringVector found = typeTable->lookupExistingEnumName(field->enumname, namespaceName);
        if (found.size() == 1) {
            field->enumqname = found[0];
        }
        else if (found.empty()) {
            errors->addError(field->nedElement, "undeclared enum '%s' in field '%s' in '%s'", field->enumname.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
            field->enumqname = "";
        }
        else {
            errors->addWarning(field->nedElement, "ambiguous enum '%s' in field '%s' in '%s';  possibilities: %s\n", field->enumname.c_str(), field->fname.c_str(), classInfo.msgname.c_str(), join(found, ", ").c_str());
            field->enumqname = found[0];
        }
        field->fprops["enum"] = field->enumqname; // need to overwrite it in props, because Qtenv will look up the enum by qname
    }

//    if (hasProperty(field->fprops, "byvalue")) {
//        if (field->fispointer)
//            errors->addError(field->nedElement, "unaccepted @byvalue property for pointer field '%s %s' in class '%s'", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
//        else if (field->fisprimitivetype)
//            errors->addError(field->nedElement, "unaccepted @byvalue property for primitive type field '%s %s' in class '%s'", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
//    }

    field->byvalue = getPropertyAsBool(field->fprops, "byvalue", fieldClassInfo.byvalue);

    // fromstring/tostring
    field->fromstring = fieldClassInfo.fromstring;
    field->tostring = fieldClassInfo.tostring;
    if (!field->enumname.empty()) {
        field->tostring = str("enum2string($, \"") + field->enumqname + "\")";
        field->fromstring = str("(") + field->enumqname + ")string2enum($, \"" + field->enumqname + "\")";
    }
    field->fromstring = getProperty(field->fprops, "fromstring", field->fromstring);
    field->tostring = getProperty(field->fprops, "tostring", field->tostring);

    // variable name
    if (!classInfo.isClass) {
        field->var = field->fname;
    }
    else {
        field->var = field->fname + classInfo.fieldnamesuffix;
        field->argname = field->fname;
    }

    field->fisownedpointer = field->fispointer && getPropertyAsBool(field->fprops, "owned", field->iscOwnedObject);

    field->varsize = field->farraysize.empty() ? (field->fname + "_arraysize") : field->farraysize;
    std::string sizetypeprop = getProperty(field->fprops, "sizetype");
    field->fsizetype = !sizetypeprop.empty() ? sizetypeprop : "unsigned int";  // TODO change to size_t

    // default method names
    if (classInfo.isClass) {
        std::string capfieldname = field->fname;
        capfieldname[0] = toupper(capfieldname[0]);
        field->setter = str("set") + capfieldname;
        field->remover = str("remove") + capfieldname;
        field->alloc = str("set") + capfieldname + "ArraySize";
        if (classInfo.omitgetverb) {
            field->getter = field->fname;
            field->getsize = field->fname + "ArraySize";
        }
        else {
            field->getter = str("get") + capfieldname;
            field->getsize = str("get") + capfieldname + "ArraySize";
        }
        field->mGetter = str("getMutable") + capfieldname;             //TODO "field->getter" (for compatibility) or "getMutable"  or "access"

        // allow customization of names
        if (getProperty(field->fprops, "setter") != "")
            field->setter = getProperty(field->fprops, "setter");
        if (getProperty(field->fprops, "getter") != "")
            field->getter = getProperty(field->fprops, "getter");
        if (getProperty(field->fprops, "mgetter") != "")
            field->mGetter = getProperty(field->fprops, "mgetter");
        if (getProperty(field->fprops, "sizeSetter") != "")
            field->alloc = getProperty(field->fprops, "sizeSetter");
        if (getProperty(field->fprops, "sizeGetter") != "")
            field->getsize = getProperty(field->fprops, "sizeGetter");
    }

    // data type, argument type, conversion to/from string...
    field->datatype = getProperty(field->fprops, "cpptype", fieldClassInfo.datatype);
    field->argtype = getProperty(field->fprops, "argtype", fieldClassInfo.argtype);
    field->rettype = getProperty(field->fprops, "rettype", fieldClassInfo.rettype);
    field->maybe_c_str = getProperty(field->fprops, "maybe_c_str", fieldClassInfo.maybe_c_str);
    if (field->datatype.empty())
        field->datatype = field->ftype;
    if (field->argtype.empty())
        field->argtype = field->datatype;
    if (field->rettype.empty())
        field->rettype = field->datatype;

    if (field->fispointer) {
        field->datatype = field->datatype + " *";
        field->argtype = field->argtype + " *";
        field->rettype = field->rettype + " *";
    }
    else if (field->byvalue) {
        // leave as is
    }
    else {
        field->argtype = str("const ") + field->argtype + "&";
        field->rettype = field->rettype + "&";
    }

    if (field->feditable || (classInfo.generate_setters_in_descriptor && field->fisprimitivetype && field->editNotDisabled))
        if (field->fromstring.empty())
            errors->addError(field->nedElement, "Field '%s' is editable, but fromstring() function is unspecified", field->fname.c_str()); //TODO only if descriptor class is also generated
}

MsgAnalyzer::FieldInfo *MsgAnalyzer::findSuperclassField(ClassInfo& classInfo, const std::string& fieldName)
{
    ClassInfo *currentClass = &classInfo;
    while (currentClass->msgbaseqname != "") {
        currentClass = &typeTable->getClassInfo(currentClass->msgbaseqname);
        ensureAnalyzed(*currentClass);
        ensureFieldsAnalyzed(*currentClass);
        for (FieldInfo& f : currentClass->fieldlist)
            if (f.fname == fieldName && !f.ftype.empty())
                return &f;
    }
    return nullptr;
}

void MsgAnalyzer::analyzeInheritedField(ClassInfo& classInfo, FieldInfo *field)
{
    Assert(field->ftype.empty()); // i.e. it is an inherited field

    if (field->fisabstract)
        errors->addError(field->nedElement, "An abstract field needs a type");
    if (field->fisarray)
        errors->addError(field->nedElement, "Cannot set array field of super class");
    if (field->fval.empty())
        errors->addError(field->nedElement, "Missing field value assigment");

    FieldInfo *fieldDefinition = findSuperclassField(classInfo, field->fname);
    if (!fieldDefinition)
        errors->addError(field->nedElement, "Unknown field '%s' (not found in any super class)", field->fname.c_str());
    else {
        // copy stuff
        field->setter = fieldDefinition->setter;
        //TODO more
    }
}

MsgAnalyzer::EnumInfo MsgAnalyzer::extractEnumDecl(EnumDeclElement *node, const std::string& namespaceName)
{
    EnumInfo enumInfo;
    enumInfo.nedElement = node;
    enumInfo.enumName = str(node->getAttribute("name"));
    enumInfo.enumQName = prefixWithNamespace(enumInfo.enumName, namespaceName);
    enumInfo.isDeclaration = true;
    return enumInfo;
}

MsgAnalyzer::EnumInfo MsgAnalyzer::extractEnumInfo(EnumElement *node, const std::string& namespaceName)
{
    EnumInfo enumInfo;
    enumInfo.nedElement = node;
    enumInfo.enumName = str(node->getAttribute("name"));
    enumInfo.enumQName = prefixWithNamespace(enumInfo.enumName, namespaceName);
    enumInfo.isDeclaration = false;

    // prepare enum items:
    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case NED_ENUM_FIELDS:
                for (ASTNode *e = child->getFirstChild(); e; e = e->getNextSibling()) {
                    switch (e->getTagCode()) {
                        case NED_ENUM_FIELD: {
                            EnumItem item;
                            item.nedElement = e;
                            item.name = str(e->getAttribute("name"));
                            item.value = str(e->getAttribute("value"));
                            enumInfo.fieldlist.push_back(item);
                            break;
                        }

                        case NED_COMMENT:
                            break;

                        default:
                            errors->addError(e, "unaccepted element '%s'", e->getTagName());
                    }
                }
                break;

            case NED_COMMENT:
                break;

            default:
                errors->addError(child, "unaccepted element '%s'", child->getTagName());
                break;
        }
    }
    return enumInfo;
}

MsgAnalyzer::ClassInfo MsgAnalyzer::extractClassInfoFromEnum(EnumElement *node, const std::string& namespaceName)
{
    ClassInfo classInfo = makeIncompleteClassInfo(node, namespaceName);
/*
    @primitive;
    @descriptor(false);
    @fromstring((namespaceName::typeName)string2enum($, "namespaceName::typeName"));
    @tostring(enum2string($, "namespaceName::typeName"));
    @defaultvalue(((namespaceName::typeName)-1));
 */
    classInfo.isprimitivetype = true;
    classInfo.datatype = classInfo.msgqname;
    classInfo.fromstring = str("(") + classInfo.msgqname + ")string2enum($, \"" + classInfo.msgqname + "\")";
    classInfo.tostring = str("enum2string($, \"") + classInfo.msgqname + "\")";
    classInfo.defaultvalue = str("((") + classInfo.msgqname + ")-1)";

    // determine base class
    if (classInfo.msgbase != "")
        errors->addError(classInfo.nedElement, "'%s': type '%s' cannot be used as base class of enum", classInfo.msgname.c_str(), classInfo.msgbase.c_str());

    classInfo.msgbaseqname = "";

    // determine class kind
    classInfo.isClass = false;
    classInfo.iscObject = false;
    classInfo.iscOwnedObject = false;
    classInfo.iscNamedObject = false;


    // isPrimitive, isOpaque, byValue, data types, etc.
    classInfo.isprimitivetype = true;
    classInfo.isopaque = true;
    classInfo.byvalue = true;

    classInfo.datatype = classInfo.msgqname;
    classInfo.argtype = classInfo.msgqname;
    classInfo.rettype  = classInfo.msgqname;

    classInfo.maybe_c_str  = "";

    //
    // produce all sorts of derived names
    //
    classInfo.generate_class = false;
    classInfo.generate_descriptor = false;
    classInfo.generate_setters_in_descriptor = false;

    classInfo.gap = false;

    classInfo.msgclass = classInfo.msgname;
    classInfo.realmsgclass = classInfo.msgname;
    classInfo.msgdescclass = makeIdentifier(classInfo.msgclass) + "Descriptor";

    classInfo.msgbaseclass = classInfo.msgbaseqname;

    classInfo.omitgetverb = false;
    classInfo.fieldnamesuffix = "";

    classInfo.classInfoComplete = true;

    return classInfo;
}

std::string MsgAnalyzer::prefixWithNamespace(const std::string& name, const std::string& namespaceName)
{
    return !namespaceName.empty() ? namespaceName + "::" + name : name;  // prefer name from local namespace
}

bool MsgAnalyzer::getPropertyAsBool(const Properties& p, const char *name, bool defval)
{
    Properties::const_iterator it = p.find(name);
    if (it == p.end())
        return defval;
    if (it->second == "false")
        return false;
    return true;
}

std::string MsgAnalyzer::getProperty(const Properties& p, const char *name, const std::string& defval)
{
    Properties::const_iterator it = p.find(name);
    if (it == p.end())
        return defval;
    return it->second;
}

std::string MsgAnalyzer::lookupExistingClassName(const std::string& name, const std::string& contextNamespace,  ClassInfo *contextClass)
{
    if (name.empty())
        return "";

    if (name.find("::") == 0) {
        // if $name start with "::", take it as an explicitly qualified name
        std::string qname = name.substr(2);
        if (typeTable->isClassDefined(qname))
            return qname;
    }
    else {
        // relative name
        // look into the enclosing type and its super types
        if (contextClass != nullptr) {
            std::string qname = prefixWithNamespace(name, contextClass->msgqname);
            if (typeTable->isClassDefined(qname))
                return qname;
            ClassInfo *currentClass = contextClass;
            while (currentClass->msgbaseqname != "") {
                currentClass = &typeTable->getClassInfo(currentClass->msgbaseqname); // go to super class
                std::string qname = prefixWithNamespace(name, currentClass->msgqname);
                if (typeTable->isClassDefined(qname))
                    return qname;
                ensureAnalyzed(*currentClass);
            }
        }

        // search from current namespace up to the top level
        std::string lookupNamespace = contextNamespace;
        while (true) {
            std::string qname = prefixWithNamespace(name, lookupNamespace);
            if (typeTable->isClassDefined(qname))
                return qname;

            if (lookupNamespace.empty())
                break;

            // drop last segment
            int pos = lookupNamespace.rfind("::");
            if (pos == std::string::npos)
                pos = 0;
            lookupNamespace = lookupNamespace.substr(0, pos);
        }

        // additionally, try in in the omnetpp namespace as well
        std::string qname = "omnetpp::" + name;
        if (typeTable->isClassDefined(qname))
            return qname;
    }
    return "";
}

}  // namespace nedxml
}  // namespace omnetpp
