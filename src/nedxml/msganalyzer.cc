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
#include "common/opp_ctype.h"
#include "msgcompiler.h"
#include "msganalyzer.h"
#include "exception.h"

//#include "omnetpp/simkerneldefs.h"

//TODO camelize names of supported properties!!! (e.g. cpptype -> cppType, fromstring -> fromString)
//TODO field's props should be produced by merging the type's props into it? (to be accessible from inspectors)

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

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

template<class P, class T> //TODO move these templates into common/?
P check_and_cast(T *p)
{
    assert(p);
    P ret = dynamic_cast<P>(p);
    assert(ret);
    return ret;
}

template<class P, class T>
P check_and_cast_nullable(T *p)
{
    if (p == nullptr)
        return nullptr;
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

MsgAnalyzer::ClassInfo MsgAnalyzer::extractClassInfo(ASTNode *node, const std::string& namespaceName)
{
    ClassInfo classInfo;
    classInfo.astNode = node;
    classInfo.props = extractProperties(node);
    std::string actually = getProperty(classInfo.props, PROP_ACTUALLY, "");
    classInfo.keyword = node->getTagName();
    classInfo.msgname = actually != "" ? actually : node->getAttribute(ATT_NAME);
    classInfo.namespacename = namespaceName;
    classInfo.msgqname = prefixWithNamespace(classInfo.msgname, namespaceName);
    classInfo.msgbase = node->getAttribute(ATT_EXTENDS_NAME);
    classInfo.isClass = (classInfo.keyword != "struct");
    return classInfo;
}

MsgAnalyzer::Properties MsgAnalyzer::extractProperties(ASTNode *node)
{
    Properties props;

    for (PropertyElement *prop = check_and_cast_nullable<PropertyElement *>(node->getFirstChildWithTag(MSG_PROPERTY)); prop; prop = prop->getNextPropertySibling()) {
        std::string propElem = prop->getName();
        std::string propValue;
        for (PropertyKeyElement *pkeyElem = prop->getFirstPropertyKeyChild(); pkeyElem; pkeyElem = pkeyElem->getNextPropertyKeySibling()) {
            std::string keyName = pkeyElem->getName();
            if (keyName.empty()) {
                const char *sep = "";
                for (LiteralElement *lit = pkeyElem->getFirstLiteralChild(); lit; lit = lit->getNextLiteralSibling()) {
                    propValue += propValue + sep + lit->getValue();
                    sep = ",";
                }
            }
        }
        if (props.find(propElem) != props.end())
            errors->addError(prop, "duplicate property '%s'", propElem.c_str());
        props[propElem] = propValue;
    }
    return props;
}

void MsgAnalyzer::extractFields(ClassInfo& classInfo)
{
    ASTNode *node = classInfo.astNode;
    for (FieldElement *fieldElem = check_and_cast_nullable<FieldElement*>(node->getFirstChildWithTag(MSG_FIELD)); fieldElem; fieldElem = fieldElem->getNextFieldSibling()) {
        FieldInfo field;
        field.astNode = fieldElem;
        field.fname = fieldElem->getName();
        field.datatype = fieldElem->getDataType();
        field.ftype = fieldElem->getDataType();
        field.fval = fieldElem->getDefaultValue();
        field.fisabstract = fieldElem->getIsAbstract();
        field.fispointer = opp_stringendswith(field.ftype.c_str(), "*");
        if (field.fispointer)
            field.ftype = opp_trim(opp_substringbeforelast(field.ftype.c_str(), "*"));
        field.fisarray = fieldElem->getIsVector();
        field.farraysize = fieldElem->getVectorSize();

        field.fprops = extractProperties(fieldElem);

        if (field.ftype.empty())
            classInfo.baseclassFieldlist.push_back(field);
        else
            classInfo.fieldlist.push_back(field);
    }
}


void MsgAnalyzer::ensureAnalyzed(ClassInfo& classInfo)
{
    if (!classInfo.classInfoComplete) {
        if (classInfo.classBeingAnalyzed) {
            errors->addError(classInfo.astNode, "Recursive definition near '%s'", classInfo.msgname.c_str());
            return;
        }
        classInfo.classBeingAnalyzed = true;
        extractFields(classInfo);
        analyzeClassOrStruct(classInfo, classInfo.namespacename);
        classInfo.classBeingAnalyzed = false;
        classInfo.classInfoComplete = true;
    }
}

void MsgAnalyzer::ensureFieldsAnalyzed(ClassInfo& classInfo)
{
    if (!classInfo.fieldsComplete) {
        if (classInfo.fieldsBeingAnalyzed) {
            errors->addError(classInfo.astNode, "Recursive nesting near '%s'", classInfo.msgname.c_str());
            return;
        }
        classInfo.fieldsBeingAnalyzed = true;
        analyzeFields(classInfo, classInfo.namespacename);
        classInfo.fieldsBeingAnalyzed = false;
        classInfo.fieldsComplete = true;
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
            errors->addError(classInfo.astNode, "'%s': unknown base class '%s'", classInfo.msgname.c_str(), classInfo.msgbase.c_str());
    }

    // resolve base class/struct
    ClassInfo *baseClassInfo = nullptr;
    if (classInfo.msgbaseqname != "") {
        baseClassInfo = &typeTable->getClassInfo(classInfo.msgbaseqname);
        ensureAnalyzed(*baseClassInfo);
        if (baseClassInfo->isprimitivetype)
            errors->addError(classInfo.astNode, "'%s': primitive type '%s' cannot be used as base class", classInfo.msgname.c_str(), classInfo.msgbase.c_str());
    }

    // determine class type
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
                errors->addError(classInfo.astNode, "%s: base type is not a class", classInfo.msgname.c_str());
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
            errors->addError(classInfo.astNode, "%s: base type is not a struct", classInfo.msgname.c_str());
    }

    // message must subclass from cMessage, packet ditto
    std::string requiredSuperClass;
    if (classInfo.keyword == "message")
        requiredSuperClass = "omnetpp::cMessage";
    else if (classInfo.keyword == "packet")
        requiredSuperClass = "omnetpp::cPacket";
    if (!requiredSuperClass.empty() && !hasSuperclass(classInfo, requiredSuperClass))
        errors->addError(classInfo.astNode, "%s: base class is not a %s (must be derived from %s)", classInfo.msgname.c_str(), classInfo.keyword.c_str(), requiredSuperClass.c_str());


    // isPrimitive, isOpaque, byValue, data types, etc.
    classInfo.isprimitivetype = getPropertyAsBool(classInfo.props, PROP_PRIMITIVE, false);
    classInfo.isopaque = getPropertyAsBool(classInfo.props, PROP_OPAQUE, classInfo.isprimitivetype); // primitive types are also opaque and passed by value
    classInfo.byvalue = getPropertyAsBool(classInfo.props, PROP_BYVALUE, classInfo.isprimitivetype);
    classInfo.defaultvalue = getProperty(classInfo.props, PROP_DEFAULTVALUE, "");

    classInfo.datatype = getProperty(classInfo.props, PROP_CPPTYPE, "");
    classInfo.argtype = getProperty(classInfo.props, PROP_ARGTYPE, "");
    classInfo.rettype = getProperty(classInfo.props, PROP_RETURNTYPE, "");

    classInfo.tostring = getProperty(classInfo.props, PROP_TOSTRING, "");
    classInfo.fromstring = getProperty(classInfo.props, PROP_FROMSTRING, "");
    classInfo.maybe_c_str = getProperty(classInfo.props, PROP_MAYBECSTR, "");

    // generation gap
    bool existingClass = getPropertyAsBool(classInfo.props, PROP_EXISTINGCLASS, false);
    classInfo.generate_class = opts.generateClasses && !existingClass;
    classInfo.generate_descriptor = opts.generateDescriptors && !classInfo.isopaque && getPropertyAsBool(classInfo.props, PROP_DESCRIPTOR, true); // opaque also means no descriptor
    classInfo.generate_setters_in_descriptor = opts.generateSettersInDescriptors && (getProperty(classInfo.props, PROP_DESCRIPTOR) != "readonly");

    if (!existingClass && isQualified(classInfo.msgname))
        errors->addError(classInfo.astNode, "class name may only contain '::' when generating descriptor for an existing class");

    classInfo.gap = getPropertyAsBool(classInfo.props, PROP_CUSTOMIZE, false);

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
    classInfo.omitgetverb = getPropertyAsBool(classInfo.props, PROP_OMITGETVERB, false);
    classInfo.fieldnamesuffix = getProperty(classInfo.props, PROP_FIELDNAMESUFFIX, "");
    if (classInfo.omitgetverb && classInfo.fieldnamesuffix.empty()) {
        errors->addWarning(classInfo.astNode, "@omitGetVerb(true) and (implicit) @fieldNameSuffix(\"\") collide: "
                                                 "adding '_var' suffix to data members to prevent name conflict between them and getter methods");
        classInfo.fieldnamesuffix = "_var";
    }

    // beforeChange
    classInfo.beforeChange = getProperty(classInfo.props, PROP_BEFORECHANGE, "");
    if (classInfo.beforeChange.empty() && baseClassInfo != nullptr)
        classInfo.beforeChange = baseClassInfo->beforeChange;

    // additional base classes (interfaces)
    std::string s = getProperty(classInfo.props, PROP_IMPLEMENTS);
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

    if (!classInfo.isClass && field->fisabstract)
        errors->addError(field->astNode, "A struct cannot have abstract fields");

    if (field->fisabstract && !field->fval.empty())
        errors->addError(field->astNode, "An abstract field cannot be assigned a default value");

    if (!classInfo.isClass && field->fisarray && field->farraysize.empty())
        errors->addError(field->astNode, "A struct cannot have dynamic array fields");

    if (field->fisabstract && !classInfo.gap)
        errors->addError(field->astNode, "abstract fields need '@customize(true)' property in '%s'", classInfo.msgname.c_str());

    field->ftypeqname = lookupExistingClassName(field->ftype, namespaceName, &classInfo);
    if (field->ftypeqname.empty()) {
        errors->addError(field->astNode, "unknown type '%s' for field '%s' in '%s'", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
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
            errors->addError(field->astNode, "cannot use cOwnedObject field '%s %s' in struct or non-cObject class '%s'", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());

    if (field->fispointer && field->fisprimitivetype) {
        errors->addError(field->astNode, "pointers not supported for primitive types in '%s'", classInfo.msgname.c_str());
        return;
    }

    if (field->fispointer && field->fval.empty())
        field->fval = "nullptr";
    if (field->fval.empty() && !fieldClassInfo.defaultvalue.empty())
        field->fval = fieldClassInfo.defaultvalue;

    field->fnopack = getPropertyAsBool(field->fprops, PROP_NOPACK, false);
    field->feditable = getPropertyAsBool(field->fprops, PROP_EDITABLE, false);
    field->editNotDisabled = getPropertyAsBool(field->fprops, PROP_EDITABLE, true);
    field->fopaque = getPropertyAsBool(field->fprops, PROP_OPAQUE, fieldClassInfo.isopaque);
    field->overrideGetter = getPropertyAsBool(field->fprops, PROP_OVERRIDEGETTER, false) || getPropertyAsBool(field->fprops, "override", false);
    field->overrideSetter = getPropertyAsBool(field->fprops, PROP_OVERRIDESETTER, false) || getPropertyAsBool(field->fprops, "override", false);

    // resolve enum
    field->enumname = getProperty(field->fprops, PROP_ENUM);
    if (!field->enumname.empty()) {
        StringVector found = typeTable->lookupExistingEnumName(field->enumname, namespaceName);
        if (found.size() == 1) {
            field->enumqname = found[0];
        }
        else if (found.empty()) {
            errors->addError(field->astNode, "undeclared enum '%s' in field '%s' in '%s'", field->enumname.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
            field->enumqname = "";
        }
        else {
            errors->addWarning(field->astNode, "ambiguous enum '%s' in field '%s' in '%s';  possibilities: %s\n", field->enumname.c_str(), field->fname.c_str(), classInfo.msgname.c_str(), join(found, ", ").c_str());
            field->enumqname = found[0];
        }
        field->fprops[PROP_ENUM] = field->enumqname; // need to overwrite it in props, because Qtenv will look up the enum by qname
    }

//    if (hasProperty(field->fprops, "byvalue")) {
//        if (field->fispointer)
//            errors->addError(field->nedElement, "unaccepted @byValue property for pointer field '%s %s' in class '%s'", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
//        else if (field->fisprimitivetype)
//            errors->addError(field->nedElement, "unaccepted @byValue property for primitive type field '%s %s' in class '%s'", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
//    }

    field->byvalue = getPropertyAsBool(field->fprops, PROP_BYVALUE, fieldClassInfo.byvalue);
    field->fisownedpointer = field->fispointer && getPropertyAsBool(field->fprops, PROP_OWNED, field->iscOwnedObject);

    // fromstring/tostring
    field->fromstring = fieldClassInfo.fromstring;
    field->tostring = fieldClassInfo.tostring;
    if (!field->enumname.empty()) {
        field->tostring = str("enum2string($, \"") + field->enumqname + "\")";
        field->fromstring = str("(") + field->enumqname + ")string2enum($, \"" + field->enumqname + "\")";
    }
    field->fromstring = getProperty(field->fprops, PROP_FROMSTRING, field->fromstring);
    field->tostring = getProperty(field->fprops, PROP_TOSTRING, field->tostring);

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
            std::string fname = field->fname;
            bool is = fname.length() >= 3 && fname[0] == 'i' && fname[1] == 's' && opp_isupper(fname[2]);
            bool has = fname.length() >= 4 && fname[0] == 'h' && fname[1] == 'a' && fname[1] == 's' && opp_isupper(fname[3]);
            bool omitGet = (field->datatype == "bool") && (is || has);
            field->getter = omitGet ? fname : (str("get") + capfieldname);
            field->getsize = str("get") + capfieldname + "ArraySize";
        }
        field->mGetter = str("getMutable") + capfieldname;             //TODO "field->getter" (for compatibility) or "getMutable"  or "access"

        // allow customization of names
        if (getProperty(field->fprops, PROP_SETTER) != "")
            field->setter = getProperty(field->fprops, PROP_SETTER);
        if (getProperty(field->fprops, PROP_GETTER) != "")
            field->getter = getProperty(field->fprops, PROP_GETTER);
        if (getProperty(field->fprops, PROP_MUTABLEGETTER) != "")
            field->mGetter = getProperty(field->fprops, PROP_MUTABLEGETTER);
        if (getProperty(field->fprops, PROP_SIZESETTER) != "")
            field->alloc = getProperty(field->fprops, PROP_SIZESETTER);
        if (getProperty(field->fprops, PROP_SIZEGETTER) != "")
            field->getsize = getProperty(field->fprops, PROP_SIZEGETTER);
    }

    // variable name
    if (!classInfo.isClass) {
        field->var = field->fname;
    }
    else {
        field->var = field->fname + classInfo.fieldnamesuffix;
        if (field->var == field->getter)  // isFoo <--> isFoo() conflict
            field->var += "_";
        field->argname = field->fname;
    }

    field->varsize = field->farraysize.empty() ? (field->fname + "_arraysize") : field->farraysize;
    std::string sizetypeprop = getProperty(field->fprops, PROP_SIZETYPE);
    field->fsizetype = !sizetypeprop.empty() ? sizetypeprop : "unsigned int";  // TODO change to size_t

    // data type, argument type, conversion to/from string...
    field->datatype = getProperty(field->fprops, PROP_CPPTYPE, fieldClassInfo.datatype);
    field->argtype = getProperty(field->fprops, PROP_ARGTYPE, fieldClassInfo.argtype);
    field->rettype = getProperty(field->fprops, PROP_RETURNTYPE, fieldClassInfo.rettype);
    field->maybe_c_str = getProperty(field->fprops, PROP_MAYBECSTR, fieldClassInfo.maybe_c_str);
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
            errors->addError(field->astNode, "Field '%s' is editable, but fromstring() function is unspecified", field->fname.c_str()); //TODO only if descriptor class is also generated
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
        errors->addError(field->astNode, "An abstract field needs a type");
    if (field->fisarray)
        errors->addError(field->astNode, "Cannot set array field of super class");
    if (field->fval.empty())
        errors->addError(field->astNode, "Missing field value assignment");

    FieldInfo *fieldDefinition = findSuperclassField(classInfo, field->fname);
    if (!fieldDefinition)
        errors->addError(field->astNode, "Unknown field '%s' (not found in any super class)", field->fname.c_str());
    else {
        // copy stuff
        field->setter = fieldDefinition->setter;
        //TODO more
    }
}

MsgAnalyzer::EnumInfo MsgAnalyzer::extractEnumDecl(EnumDeclElement *enumElem, const std::string& namespaceName)
{
    EnumInfo enumInfo;
    enumInfo.astNode = enumElem;
    enumInfo.enumName = enumElem->getName();
    enumInfo.enumQName = prefixWithNamespace(enumInfo.enumName, namespaceName);
    enumInfo.isDeclaration = true;
    return enumInfo;
}

MsgAnalyzer::EnumInfo MsgAnalyzer::extractEnumInfo(EnumElement *enumElem, const std::string& namespaceName)
{
    EnumInfo enumInfo;
    enumInfo.astNode = enumElem;
    enumInfo.enumName = enumElem->getName();
    enumInfo.enumQName = prefixWithNamespace(enumInfo.enumName, namespaceName);
    enumInfo.isDeclaration = false;

    // prepare enum items
    for (EnumFieldElement *fieldElem = check_and_cast_nullable<EnumFieldElement *>(enumElem->getFirstChildWithTag(MSG_ENUM_FIELD)); fieldElem; fieldElem = fieldElem->getNextEnumFieldSibling()) {
        EnumItem item;
        item.astNode = fieldElem;
        item.name = fieldElem->getName();
        item.value = fieldElem->getValue();
        enumInfo.fieldlist.push_back(item);
    }
    return enumInfo;
}

MsgAnalyzer::ClassInfo MsgAnalyzer::extractClassInfoFromEnum(EnumElement *enumElem, const std::string& namespaceName)
{
    ClassInfo classInfo = extractClassInfo(enumElem, namespaceName);
/*
    @primitive;
    @descriptor(false);
    @fromString((namespaceName::typeName)string2enum($, "namespaceName::typeName"));
    @toString(enum2string($, "namespaceName::typeName"));
    @defaultValue(((namespaceName::typeName)-1));
 */
    classInfo.isprimitivetype = true;
    classInfo.datatype = classInfo.msgqname;
    classInfo.fromstring = str("(") + classInfo.msgqname + ")string2enum($, \"" + classInfo.msgqname + "\")";
    classInfo.tostring = str("enum2string($, \"") + classInfo.msgqname + "\")";
    classInfo.defaultvalue = str("((") + classInfo.msgqname + ")-1)";

    // determine base class
    if (classInfo.msgbase != "")
        errors->addError(classInfo.astNode, "'%s': type '%s' cannot be used as base class of enum", classInfo.msgname.c_str(), classInfo.msgbase.c_str());

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
