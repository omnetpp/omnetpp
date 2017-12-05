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
    classInfo.name = actually != "" ? actually : node->getAttribute(ATT_NAME);
    classInfo.namespaceName = namespaceName;
    classInfo.qname = prefixWithNamespace(classInfo.name, namespaceName);
    classInfo.extendsName = node->getAttribute(ATT_EXTENDS_NAME);
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
        field.name = fieldElem->getName();
        field.dataType = fieldElem->getDataType();
        field.type = fieldElem->getDataType();
        field.value = fieldElem->getDefaultValue();
        field.isAbstract = fieldElem->getIsAbstract();
        field.isConst = fieldElem->getIsConst();
        field.isPointer = fieldElem->getIsPointer();
        field.isArray = fieldElem->getIsVector();
        field.arraySize = fieldElem->getVectorSize();

        field.props = extractProperties(fieldElem);

        if (field.type.empty())
            classInfo.baseclassFieldlist.push_back(field);
        else
            classInfo.fieldList.push_back(field);
    }
}


void MsgAnalyzer::ensureAnalyzed(ClassInfo& classInfo)
{
    if (!classInfo.classInfoComplete) {
        if (classInfo.classBeingAnalyzed) {
            errors->addError(classInfo.astNode, "Recursive definition near '%s'", classInfo.name.c_str());
            return;
        }
        classInfo.classBeingAnalyzed = true;
        extractFields(classInfo);
        analyzeClassOrStruct(classInfo, classInfo.namespaceName);
        classInfo.classBeingAnalyzed = false;
        classInfo.classInfoComplete = true;
    }
}

void MsgAnalyzer::ensureFieldsAnalyzed(ClassInfo& classInfo)
{
    if (!classInfo.fieldsComplete) {
        if (classInfo.fieldsBeingAnalyzed) {
            errors->addError(classInfo.astNode, "Recursive nesting near '%s'", classInfo.name.c_str());
            return;
        }
        classInfo.fieldsBeingAnalyzed = true;
        analyzeFields(classInfo, classInfo.namespaceName);
        classInfo.fieldsBeingAnalyzed = false;
        classInfo.fieldsComplete = true;
    }
}

bool MsgAnalyzer::hasSuperclass(ClassInfo& classInfo, const std::string& superclassQName)
{
    ClassInfo *currentClass = &classInfo;
    while (currentClass->extendsQName != "") {
        currentClass = &typeTable->getClassInfo(currentClass->extendsQName);
        if (currentClass->qname == superclassQName)
            return true;
        ensureAnalyzed(*currentClass);
    }
    return false;
}

void MsgAnalyzer::analyzeClassOrStruct(ClassInfo& classInfo, const std::string& namespaceName)
{
    // determine base class
    if (classInfo.extendsName == "") {
        if (classInfo.keyword == "message")
            classInfo.extendsQName = "omnetpp::cMessage";
        else if (classInfo.keyword == "packet")
            classInfo.extendsQName = "omnetpp::cPacket";
        else
            classInfo.extendsQName = "";
    }
    else {
        classInfo.extendsQName = lookupExistingClassName(classInfo.extendsName, namespaceName);
        if (classInfo.extendsQName.empty())
            errors->addError(classInfo.astNode, "'%s': unknown base class '%s'", classInfo.name.c_str(), classInfo.extendsName.c_str());
    }

    // resolve base class/struct
    ClassInfo *baseClassInfo = nullptr;
    if (classInfo.extendsQName != "") {
        baseClassInfo = &typeTable->getClassInfo(classInfo.extendsQName);
        ensureAnalyzed(*baseClassInfo);
        if (!baseClassInfo->subclassable)
            errors->addError(classInfo.astNode, "'%s': type '%s' cannot be used as base class", classInfo.name.c_str(), classInfo.extendsName.c_str());
    }

    // determine class type
    classInfo.iscObject = classInfo.iscNamedObject = classInfo.iscOwnedObject = false;
    if (classInfo.isClass) {
        if (classInfo.qname == "omnetpp::cObject")
            classInfo.iscObject = true;
        else if (classInfo.qname == "omnetpp::cNamedObject")
            classInfo.iscObject = classInfo.iscNamedObject = true;
        else if (classInfo.qname == "omnetpp::cOwnedObject")
            classInfo.iscObject = classInfo.iscNamedObject = classInfo.iscOwnedObject = true;
        else if (baseClassInfo != nullptr) {
            if (!baseClassInfo->isClass)
                errors->addError(classInfo.astNode, "%s: base type is not a class", classInfo.name.c_str());
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
            errors->addError(classInfo.astNode, "%s: base type is not a struct", classInfo.name.c_str());
    }

    // message must subclass from cMessage, packet ditto
    std::string requiredSuperClass;
    if (classInfo.keyword == "message")
        requiredSuperClass = "omnetpp::cMessage";
    else if (classInfo.keyword == "packet")
        requiredSuperClass = "omnetpp::cPacket";
    if (!requiredSuperClass.empty() && !hasSuperclass(classInfo, requiredSuperClass))
        errors->addError(classInfo.astNode, "%s: base class is not a %s (must be derived from %s)", classInfo.name.c_str(), classInfo.keyword.c_str(), requiredSuperClass.c_str());


    // isOpaque, byValue, data types, etc.
    bool isPrimitive = getPropertyAsBool(classInfo.props, PROP_PRIMITIVE, false); // @primitive is a shortcut for @opaque @byValue @subclassable(false) @supportsPtr(false)
    classInfo.isOpaque = getPropertyAsBool(classInfo.props, PROP_OPAQUE, isPrimitive);
    classInfo.byValue = getPropertyAsBool(classInfo.props, PROP_BYVALUE, isPrimitive);
    classInfo.subclassable = getPropertyAsBool(classInfo.props, PROP_SUBCLASSABLE, !isPrimitive);
    classInfo.supportsPtr = getPropertyAsBool(classInfo.props, PROP_SUPPORTSPTR, !isPrimitive);


    classInfo.defaultValue = getProperty(classInfo.props, PROP_DEFAULTVALUE, "");

    classInfo.dataTypeBase = getProperty(classInfo.props, PROP_CPPTYPE, "");
    classInfo.argTypeBase = getProperty(classInfo.props, PROP_ARGTYPE, "");
    classInfo.returnTypeBase = getProperty(classInfo.props, PROP_RETURNTYPE, "");

    classInfo.toString = getProperty(classInfo.props, PROP_TOSTRING, "");
    classInfo.fromString = getProperty(classInfo.props, PROP_FROMSTRING, "");
    classInfo.getterConversion = getProperty(classInfo.props, PROP_GETTERCONVERSION, "$");
    classInfo.dupper = getProperty(classInfo.props, PROP_DUPPER, "");

    // generation gap
    bool existingClass = getPropertyAsBool(classInfo.props, PROP_EXISTINGCLASS, false);
    classInfo.generateClass = opts.generateClasses && !existingClass;
    classInfo.generateDescriptor = opts.generateDescriptors && !classInfo.isOpaque && getPropertyAsBool(classInfo.props, PROP_DESCRIPTOR, true); // opaque also means no descriptor
    classInfo.generateSettersInDescriptor = opts.generateSettersInDescriptors && (getProperty(classInfo.props, PROP_DESCRIPTOR) != "readonly");

    if (!existingClass && isQualified(classInfo.name))
        errors->addError(classInfo.astNode, "class name may only contain '::' when generating descriptor for an existing class");

    classInfo.customize = getPropertyAsBool(classInfo.props, PROP_CUSTOMIZE, false);

    if (classInfo.customize) {
        classInfo.className = classInfo.name + "_Base";
        classInfo.realClass = classInfo.name;
        classInfo.descriptorClass = makeIdentifier(classInfo.realClass) + "Descriptor";
    }
    else {
        classInfo.className = classInfo.name;
        classInfo.realClass = classInfo.name;
        classInfo.descriptorClass = makeIdentifier(classInfo.className) + "Descriptor";
    }

    classInfo.baseClass = classInfo.extendsQName;

    // omitGetVerb / fieldNameSuffix
    classInfo.omitGetVerb = getPropertyAsBool(classInfo.props, PROP_OMITGETVERB, false);
    classInfo.fieldNameSuffix = getProperty(classInfo.props, PROP_FIELDNAMESUFFIX, "");
    if (classInfo.omitGetVerb && classInfo.fieldNameSuffix.empty()) {
        errors->addWarning(classInfo.astNode, "@omitGetVerb(true) and (implicit) @fieldNameSuffix(\"\") collide: "
                                                 "adding '_var' suffix to data members to prevent name conflict between them and getter methods");
        classInfo.fieldNameSuffix = "_var";
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
    for (auto& field : classInfo.fieldList)
        analyzeField(classInfo, &field, namespaceName);
    for (auto& field :  classInfo.baseclassFieldlist)
        analyzeInheritedField(classInfo, &field);
}

void MsgAnalyzer::analyzeField(ClassInfo& classInfo, FieldInfo *field, const std::string& namespaceName)
{
    Assert(!field->type.empty());

    if (!classInfo.isClass && field->isAbstract)
        errors->addError(field->astNode, "A struct cannot have abstract fields");

    if (field->isAbstract && !field->value.empty())
        errors->addError(field->astNode, "An abstract field cannot be assigned a default value");

    if (!classInfo.isClass && field->isArray && field->arraySize.empty())
        errors->addError(field->astNode, "A struct cannot have dynamic array fields");

    if (field->isAbstract && !classInfo.customize)
        errors->addError(field->astNode, "abstract fields need '@customize(true)' property in '%s'", classInfo.name.c_str());

    if (field->isArray && field->isConst && !field->isPointer)
        errors->addError(field->astNode, "Arrays of const values/objects are not supported");

    field->typeQName = lookupExistingClassName(field->type, namespaceName, &classInfo);
    if (field->typeQName.empty()) {
        errors->addError(field->astNode, "unknown type '%s' for field '%s' in '%s'", field->type.c_str(), field->name.c_str(), classInfo.name.c_str());
        field->typeQName = "omnetpp::cObject";
    }

    ClassInfo& fieldClassInfo = typeTable->getClassInfo(field->typeQName);
    ensureAnalyzed(fieldClassInfo);
    field->isClass = fieldClassInfo.isClass;
    field->iscObject = fieldClassInfo.iscObject;
    field->iscNamedObject = fieldClassInfo.iscNamedObject;
    field->iscOwnedObject = fieldClassInfo.iscOwnedObject;

    if (classInfo.generateClass)
        if (field->iscOwnedObject && !classInfo.iscObject)
            errors->addError(field->astNode, "cannot use cOwnedObject field '%s %s' in struct or non-cObject class '%s'", field->type.c_str(), field->name.c_str(), classInfo.name.c_str());

    if (field->isPointer && field->value.empty())
        field->value = "nullptr";
    if (field->value.empty() && !fieldClassInfo.defaultValue.empty())
        field->value = fieldClassInfo.defaultValue;

    field->isDynamicArray = field->isArray && field->arraySize.empty();
    field->isFixedArray = field->isArray && !field->arraySize.empty();

    field->nopack = getPropertyAsBool(field->props, PROP_NOPACK, false);
    field->isEditable = getPropertyAsBool(field->props, PROP_EDITABLE, false);
    field->editNotDisabled = getPropertyAsBool(field->props, PROP_EDITABLE, true);
    field->isOpaque = getPropertyAsBool(field->props, PROP_OPAQUE, fieldClassInfo.isOpaque);
    field->overrideGetter = getPropertyAsBool(field->props, PROP_OVERRIDEGETTER, false) || getPropertyAsBool(field->props, "override", false);
    field->overrideSetter = getPropertyAsBool(field->props, PROP_OVERRIDESETTER, false) || getPropertyAsBool(field->props, "override", false);

    // resolve enum
    field->enumName = getProperty(field->props, PROP_ENUM);
    if (!field->enumName.empty()) {
        StringVector found = typeTable->lookupExistingEnumName(field->enumName, namespaceName);
        if (found.size() == 1) {
            field->enumQName = found[0];
        }
        else if (found.empty()) {
            errors->addError(field->astNode, "undeclared enum '%s' in field '%s' in '%s'", field->enumName.c_str(), field->name.c_str(), classInfo.name.c_str());
            field->enumQName = "";
        }
        else {
            errors->addWarning(field->astNode, "ambiguous enum '%s' in field '%s' in '%s';  possibilities: %s", field->enumName.c_str(), field->name.c_str(), classInfo.name.c_str(), join(found, ", ").c_str());
            field->enumQName = found[0];
        }
        field->props[PROP_ENUM] = field->enumQName; // need to overwrite it in props, because Qtenv will look up the enum by qname
    }

    bool supportsPtr = getPropertyAsBool(field->props, PROP_SUPPORTSPTR, fieldClassInfo.supportsPtr);
    if (field->isPointer && !supportsPtr)
        errors->addError(field->astNode, "'%s *' pointers are not allowed", field->type.c_str());

    field->byValue = getPropertyAsBool(field->props, PROP_BYVALUE, fieldClassInfo.byValue);
    field->isOwnedPointer = field->isPointer && getPropertyAsBool(field->props, PROP_OWNED, false);
    if (hasProperty(field->props, PROP_OWNED) && !field->isPointer)
        errors->addWarning(field->astNode, "ignoring @owned property for non-pointer field '%s'", field->name.c_str());

    // fromstring/tostring
    field->fromString = fieldClassInfo.fromString;
    field->toString = fieldClassInfo.toString;
    if (!field->enumName.empty()) {
        field->toString = str("enum2string($, \"") + field->enumQName + "\")";
        field->fromString = str("(") + field->enumQName + ")string2enum($, \"" + field->enumQName + "\")";
    }
    field->fromString = getProperty(field->props, PROP_FROMSTRING, field->fromString);
    field->toString = getProperty(field->props, PROP_TOSTRING, field->toString);

    // default method names
    if (classInfo.isClass) {
        std::string capfieldname = field->name;
        capfieldname[0] = toupper(capfieldname[0]);
        field->setter = str("set") + capfieldname;
        field->remover = str("remove") + capfieldname;
        field->sizeSetter = str("set") + capfieldname + "ArraySize";
        if (classInfo.omitGetVerb) {
            field->getter = field->name;
            field->sizeGetter = field->name + "ArraySize";
        }
        else {
            std::string fname = field->name;
            bool is = fname.length() >= 3 && fname[0] == 'i' && fname[1] == 's' && opp_isupper(fname[2]);
            bool has = fname.length() >= 4 && fname[0] == 'h' && fname[1] == 'a' && fname[1] == 's' && opp_isupper(fname[3]);
            bool omitGet = (field->dataType == "bool") && (is || has);
            field->getter = omitGet ? fname : (str("get") + capfieldname);
            field->sizeGetter = str("get") + capfieldname + "ArraySize";
        }
        field->mutableGetter = str("getMutable") + capfieldname;             //TODO "field->getter" (for compatibility) or "getMutable"  or "access"

        // allow customization of names
        if (getProperty(field->props, PROP_SETTER) != "")
            field->setter = getProperty(field->props, PROP_SETTER);
        if (getProperty(field->props, PROP_GETTER) != "")
            field->getter = getProperty(field->props, PROP_GETTER);
        if (getProperty(field->props, PROP_MUTABLEGETTER) != "")
            field->mutableGetter = getProperty(field->props, PROP_MUTABLEGETTER);
        if (getProperty(field->props, PROP_SIZESETTER) != "")
            field->sizeSetter = getProperty(field->props, PROP_SIZESETTER);
        if (getProperty(field->props, PROP_SIZEGETTER) != "")
            field->sizeGetter = getProperty(field->props, PROP_SIZEGETTER);

        field->getterConversion = getProperty(field->props, PROP_GETTERCONVERSION, fieldClassInfo.getterConversion);
        field->dupper = getProperty(field->props, PROP_DUPPER, fieldClassInfo.dupper);
        if (field->dupper.empty())
            field->dupper = str("new ") + field->dataType + "(*$)";
    }

    // variable name
    if (!classInfo.isClass) {
        field->var = field->name;
    }
    else {
        field->var = field->name + classInfo.fieldNameSuffix;
        if (field->var == field->getter)  // isFoo <--> isFoo() conflict
            field->var += "_";
        field->argName = field->name;
    }

    field->sizeVar = field->arraySize.empty() ? (field->name + "_arraysize") : field->arraySize;
    std::string sizetypeprop = getProperty(field->props, PROP_SIZETYPE);
    field->sizeType = !sizetypeprop.empty() ? sizetypeprop : "size_t";

    // data type, argument type, conversion to/from string...
    std::string datatypeBase = getProperty(field->props, PROP_CPPTYPE, fieldClassInfo.dataTypeBase);
    std::string argtypeBase = getProperty(field->props, PROP_ARGTYPE, fieldClassInfo.argTypeBase);
    std::string returntypeBase = getProperty(field->props, PROP_RETURNTYPE, fieldClassInfo.returnTypeBase);

    if (datatypeBase.empty()) datatypeBase = field->type;
    if (argtypeBase.empty()) argtypeBase = datatypeBase;
    if (returntypeBase.empty()) returntypeBase = datatypeBase;

    //
    // Intended const usage for various isPointer/isConst/byValue combinations:
    //   Foo*:          setter: Foo*        getter: const Foo*    mgetter: Foo*
    //   const Foo*:    setter: const Foo*  getter: const Foo*    mgetter: -
    //   Foo:           setter: Foo&        getter: const Foo&    mgetter: Foo&
    //   const Foo:     setter: -           getter: const Foo&    mgetter: -
    //   int (byvalue): setter: int         getter: int           mgetter: -
    //   const int:     setter: -           getter: int           mgetter: -
    //   int*:          same as Foo*
    //   const int*:    const Foo*
    //

    bool byRef = !field->isPointer && !field->byValue;

    field->dataType = decorateType(datatypeBase, field->isConst, field->isPointer, false);
    field->argType = decorateType(argtypeBase, field->isConst || byRef, field->isPointer, byRef);  //TODO check  "|| byRef" part -- removing it makes tests fail
    if (!field->isPointer && field->byValue) {
        field->returnType = decorateType(returntypeBase, false, false, false);
        field->argType = decorateType(argtypeBase, false, false, false);
    }
    else {
        field->returnType = decorateType(returntypeBase, true, field->isPointer, byRef);
        field->mutableReturnType = decorateType(returntypeBase, false, field->isPointer, byRef);
    }

    field->hasMutableGetter = !field->isConst && (field->isPointer ? true : !field->byValue);

    if (field->isEditable && field->fromString.empty() && classInfo.generateDescriptor && classInfo.generateSettersInDescriptor)
        errors->addError(field->astNode, "Field '%s' is editable, but fromstring() function is unspecified", field->name.c_str());
}

MsgAnalyzer::FieldInfo *MsgAnalyzer::findSuperclassField(ClassInfo& classInfo, const std::string& fieldName)
{
    ClassInfo *currentClass = &classInfo;
    while (currentClass->extendsQName != "") {
        currentClass = &typeTable->getClassInfo(currentClass->extendsQName);
        ensureAnalyzed(*currentClass);
        ensureFieldsAnalyzed(*currentClass);
        for (FieldInfo& f : currentClass->fieldList)
            if (f.name == fieldName && !f.type.empty())
                return &f;
    }
    return nullptr;
}

void MsgAnalyzer::analyzeInheritedField(ClassInfo& classInfo, FieldInfo *field)
{
    Assert(field->type.empty()); // i.e. it is an inherited field

    if (field->isAbstract)
        errors->addError(field->astNode, "An abstract field needs a type");
    if (field->isArray)
        errors->addError(field->astNode, "Cannot set array field of super class");
    if (field->value.empty())
        errors->addError(field->astNode, "Missing field value assignment");

    FieldInfo *fieldDefinition = findSuperclassField(classInfo, field->name);
    if (!fieldDefinition)
        errors->addError(field->astNode, "Unknown field '%s' (not found in any super class)", field->name.c_str());
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
        enumInfo.fieldList.push_back(item);
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
    classInfo.dataTypeBase = classInfo.qname;
    classInfo.fromString = str("(") + classInfo.qname + ")string2enum($, \"" + classInfo.qname + "\")";
    classInfo.toString = str("enum2string($, \"") + classInfo.qname + "\")";
    classInfo.defaultValue = str("((") + classInfo.qname + ")-1)";

    // determine base class
    if (classInfo.extendsName != "")
        errors->addError(classInfo.astNode, "'%s': type '%s' cannot be used as base class of enum", classInfo.name.c_str(), classInfo.extendsName.c_str());

    classInfo.extendsQName = "";

    // determine class kind
    classInfo.isClass = false;
    classInfo.iscObject = false;
    classInfo.iscOwnedObject = false;
    classInfo.iscNamedObject = false;


    // isOpaque, byValue, data types, etc.
    classInfo.isOpaque = true;
    classInfo.byValue = true;
    classInfo.subclassable = false;
    classInfo.supportsPtr = false;

    classInfo.dataTypeBase = classInfo.qname;
    classInfo.argTypeBase = classInfo.qname;
    classInfo.returnTypeBase  = classInfo.qname;

    classInfo.getterConversion  = "$";

    //
    // produce all sorts of derived names
    //
    classInfo.generateClass = false;
    classInfo.generateDescriptor = false;
    classInfo.generateSettersInDescriptor = false;

    classInfo.customize = false;

    classInfo.className = classInfo.name;
    classInfo.realClass = classInfo.name;
    classInfo.descriptorClass = makeIdentifier(classInfo.className) + "Descriptor";

    classInfo.baseClass = classInfo.extendsQName;

    classInfo.omitGetVerb = false;
    classInfo.fieldNameSuffix = "";

    classInfo.classInfoComplete = true;

    return classInfo;
}

std::string MsgAnalyzer::prefixWithNamespace(const std::string& name, const std::string& namespaceName)
{
    return !namespaceName.empty() ? namespaceName + "::" + name : name;  // prefer name from local namespace
}

std::string MsgAnalyzer::decorateType(const std::string& typeName, bool isConst, bool isPointer, bool isRef)
{
    bool alreadyConst = opp_stringbeginswith(typeName.c_str(), "const ");  // use with "const char *"
    return ((isConst && !alreadyConst) ? "const " : "") + typeName + (isPointer ? " *" : "") + (isRef ? "&" : "");
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
            std::string qname = prefixWithNamespace(name, contextClass->qname);
            if (typeTable->isClassDefined(qname))
                return qname;
            ClassInfo *currentClass = contextClass;
            while (currentClass->extendsQName != "") {
                currentClass = &typeTable->getClassInfo(currentClass->extendsQName); // go to super class
                std::string qname = prefixWithNamespace(name, currentClass->qname);
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
