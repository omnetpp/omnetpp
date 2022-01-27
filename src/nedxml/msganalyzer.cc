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
        // C++ keywords
        "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel",
        "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor", "bool",
        "break", "case", "catch", "char", "char16_t", "char32_t", "class",
        "compl", "concept", "const", "constexpr", "const_cast", "continue",
        "co_await", "co_return", "co_yield", "decltype", "default", "delete",
        "do", "double", "dynamic_cast", "else", "enum", "explicit", "export",
        "extern", "false", "float", "for", "friend", "goto", "if", "import",
        "inline", "int", "long", "module", "mutable", "namespace", "new",
        "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq",
        "private", "protected", "public", "register", "reinterpret_cast",
        "requires", "return", "short", "signed", "sizeof", "static",
        "static_assert", "static_cast", "struct", "switch", "synchronized",
        "template", "this", "thread_local", "throw", "true", "try", "typedef",
        "typeid", "typename", "union", "unsigned", "using", "virtual", "void",
        "volatile", "wchar_t", "while", "xor", "xor_eq",

        // MSG keywords that are not also C++ keywords
        "cplusplus", "message", "packet", "noncobject", "extends", "abstract"
};




MsgAnalyzer::MsgAnalyzer(const MsgCompilerOptions& opts, MsgTypeTable *typeTable, ErrorStore *errors) : errors(errors), typeTable(typeTable), opts(opts)
{
}

MsgAnalyzer::ClassInfo MsgAnalyzer::extractClassInfo(ASTNode *node, const std::string& namespaceName, bool isImported)
{
    ClassInfo classInfo;
    classInfo.astNode = node;
    classInfo.props = extractProperties(node);
    std::string actually = getProperty(classInfo.props, PROP_ACTUALLY, "");
    classInfo.keyword = node->getTagName();
    classInfo.name = actually != "" ? actually : node->getAttribute(ATT_NAME);
    classInfo.namespaceName = namespaceName;
    classInfo.qname = prefixWithNamespace(classInfo.name, namespaceName);
    classInfo.extendsName = opp_nulltoempty(node->getAttribute(ATT_EXTENDS_NAME));
    classInfo.isClass = (classInfo.keyword != "struct");
    classInfo.isEnum = false;
    classInfo.isImported = isImported;
    return classInfo;
}

MsgAnalyzer::Properties MsgAnalyzer::extractProperties(ASTNode *node)
{
    const char *usage = node->getTagCode()==MSG_FIELD ? "field" : node->getTagCode()==MSG_MSG_FILE ? "file" : "class";
    Properties props;
    for (PropertyElement *propElem = check_and_cast_nullable<PropertyElement *>(node->getFirstChildWithTag(MSG_PROPERTY)); propElem; propElem = propElem->getNextPropertySibling()) {
        Property property = extractProperty(propElem);
        if (props.contains(property.getName(), property.getIndex()))
            errors->addError(node, "duplicate property '%s'", property.getIndexedName().c_str());
        validateProperty(property, usage);
        props.add(property);
    }
    return props;
}

MsgAnalyzer::Property MsgAnalyzer::extractProperty(PropertyElement *propElem)
{
    std::string propName = propElem->getName();
    std::string propIndex = propElem->getIndex();
    Property property(propName, propIndex, propElem);
    for (PropertyKeyElement *keyElem = propElem->getFirstPropertyKeyChild(); keyElem; keyElem = keyElem->getNextPropertyKeySibling()) {
        std::string keyName = keyElem->getName();
        for (LiteralElement *lit = keyElem->getFirstLiteralChild(); lit; lit = lit->getNextLiteralSibling())
            property.addValue(keyName, lit->getValue());
    }
    return property;
}

void MsgAnalyzer::extractFields(ClassInfo& classInfo)
{
    ASTNode *node = classInfo.astNode;
    for (FieldElement *fieldElem = check_and_cast_nullable<FieldElement*>(node->getFirstChildWithTag(MSG_FIELD)); fieldElem; fieldElem = fieldElem->getNextFieldSibling()) {
        FieldInfo field;
        field.astNode = fieldElem;
        field.name = fieldElem->getName();
        field.type = fieldElem->getDataType();
        field.value = fieldElem->getDefaultValue();
        field.isAbstract = fieldElem->getIsAbstract();
        field.isConst = fieldElem->getIsConst();
        field.isPointer = fieldElem->getIsPointer();
        field.isArray = fieldElem->getIsVector();
        field.arraySize = fieldElem->getVectorSize();

        field.props = extractProperties(fieldElem);

        FieldInfo *duplicate = findField(classInfo, field.name);
        if (duplicate != nullptr)
            errors->addError(field.astNode, "Field '%s' already exists, see %s", field.name.c_str(), duplicate->astNode->getSourceLocation().str().c_str());
        else if (field.type.empty())
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

    // isAbstract
    bool hasAbstractBase = baseClassInfo != nullptr && baseClassInfo->isAbstract; //TODO this does not fully work: the fields of the base class are not yet analyzed, so having an abstract field is not yet reflected in its isAbstract flag. This is not a big deal, however, as it will fall out during C++ compilation
    classInfo.isAbstract = getPropertyAsBool(classInfo.props, PROP_ABSTRACT, hasAbstractBase);

    // isOpaque, byValue, data types, etc.
    bool isPrimitive = getPropertyAsBool(classInfo.props, PROP_PRIMITIVE, false); // @primitive is a shortcut for @opaque @byValue @editable @subclassable(false) @supportsPtr(false)
    classInfo.isOpaque = getPropertyAsBool(classInfo.props, PROP_OPAQUE, isPrimitive);
    classInfo.byValue = getPropertyAsBool(classInfo.props, PROP_BYVALUE, isPrimitive);
    classInfo.subclassable = getPropertyAsBool(classInfo.props, PROP_SUBCLASSABLE, !isPrimitive);
    classInfo.supportsPtr = getPropertyAsBool(classInfo.props, PROP_SUPPORTSPTR, !isPrimitive);
    classInfo.isEditable = getPropertyAsBool(classInfo.props, PROP_EDITABLE, isPrimitive);

    classInfo.defaultValue = getProperty(classInfo.props, PROP_DEFAULTVALUE, "");

    std::string cppTypeBase = getProperty(classInfo.props, PROP_CPPTYPE, classInfo.qname);
    classInfo.dataTypeBase = getProperty(classInfo.props, PROP_DATAMEMBERTYPE, cppTypeBase);
    classInfo.argTypeBase = getProperty(classInfo.props, PROP_ARGTYPE, cppTypeBase);
    classInfo.returnTypeBase = getProperty(classInfo.props, PROP_RETURNTYPE, cppTypeBase);

    classInfo.toString = getProperty(classInfo.props, PROP_TOSTRING, "");
    classInfo.fromString = getProperty(classInfo.props, PROP_FROMSTRING, "");
    classInfo.toValue = getProperty(classInfo.props, PROP_TOVALUE, "");
    classInfo.fromValue = getProperty(classInfo.props, PROP_FROMVALUE, "");

    classInfo.getterConversion = getProperty(classInfo.props, PROP_GETTERCONVERSION, "$");
    classInfo.clone = getProperty(classInfo.props, PROP_CLONE, "");
    classInfo.str = getProperty(classInfo.props, PROP_STR, "");

    // generation gap
    bool existingClass = getPropertyAsBool(classInfo.props, PROP_EXISTINGCLASS, false);
    classInfo.generateClass = opts.generateClasses && !existingClass;

    std::string descProp = getProperty(classInfo.props, PROP_DESCRIPTOR, "true");
    if (descProp != "true" && descProp != "false" && descProp != "readonly")
        errors->addError(classInfo.astNode, "invalid value '%s' for @descriptor, should be 'true', 'false' or 'readonly'", descProp.c_str());
    classInfo.generateDescriptor = opts.generateDescriptors && !classInfo.isOpaque && descProp != "false"; // opaque also means no descriptor
    classInfo.generateSettersInDescriptor = opts.generateSettersInDescriptors && descProp != "readonly";

    if (!existingClass && isQualified(classInfo.name))
        errors->addError(classInfo.astNode, "class name may only contain '::' when generating descriptor for an existing class");

    classInfo.customize = getPropertyAsBool(classInfo.props, PROP_CUSTOMIZE, false);

    classInfo.realClass = classInfo.className = classInfo.name;
    if (classInfo.customize)
        classInfo.className += "_Base";
    classInfo.descriptorClass = makeIdentifier(classInfo.realClass) + "Descriptor";
    classInfo.classQName = prefixWithNamespace(classInfo.className, namespaceName);

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
    const Property *implementsProperty = classInfo.props.get(PROP_IMPLEMENTS);
    if (implementsProperty) {
        StringVector list  = implementsProperty->getValue("");
        for (std::string& name : list) {
            std::string qname = lookupExistingClassName(name, namespaceName);
            if (qname.empty()) {
                errors->addError(classInfo.astNode, "'%s': unknown class '%s' in @implements", classInfo.name.c_str(), name.c_str());
                continue;
            }
            auto baseClassInfo = typeTable->getClassInfo(qname);
            ensureAnalyzed(baseClassInfo);
            if (!baseClassInfo.subclassable)
                errors->addError(classInfo.astNode, "'%s': type '%s' in @implements list cannot be used as base class", classInfo.name.c_str(), qname.c_str());
            classInfo.implementsQNames.push_back(qname);
        }
    }

    // isPolymorphic
    bool isPolymorphic = getPropertyAsBool(classInfo.props, PROP_POLYMORPHIC, classInfo.isClass);
    if (baseClassInfo && baseClassInfo->isPolymorphic)
        isPolymorphic = true;
    for (std::string qname : classInfo.implementsQNames) {
        auto baseClassInfo = typeTable->getClassInfo(qname);
        ensureAnalyzed(baseClassInfo);
        if (baseClassInfo.isPolymorphic)
            isPolymorphic = true;
    }
    classInfo.isPolymorphic = isPolymorphic;

    // root classes
    if (baseClassInfo == nullptr)
        classInfo.rootClasses = { classInfo.classQName };
    else
        classInfo.rootClasses = baseClassInfo->rootClasses;
    for (std::string qname : classInfo.implementsQNames) {
        auto baseClassInfo = typeTable->getClassInfo(qname);
        ensureAnalyzed(baseClassInfo);
        for (auto rootClass : baseClassInfo.rootClasses)
            if (!contains(classInfo.rootClasses, rootClass))
                classInfo.rootClasses.push_back(rootClass);
    }

    classInfo.generateCastFunction = getPropertyAsBool(classInfo.props, PROP_CASTFUNCTION, true);
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

    if (getPropertyAsBool(field->props, PROP_ABSTRACT, false)) // let @abstract take effect in addition to the "abstract" keyword
        field->isAbstract = true;

    if (!classInfo.isClass && field->isAbstract)
        errors->addError(field->astNode, "A struct cannot have abstract fields");

    if (field->isAbstract && !field->value.empty())
        errors->addError(field->astNode, "An abstract field cannot be assigned a default value");

    if (!classInfo.isClass && field->isArray && field->arraySize.empty())
        errors->addError(field->astNode, "A struct cannot have dynamic array fields");

    if (field->isAbstract && !classInfo.customize)  // assume that customization implements pure (=0) methods
        classInfo.isAbstract = true;

    if (field->isArray && field->isConst && !field->isPointer)
        errors->addError(field->astNode, "Arrays of const values/objects are not supported");

    field->typeQName = lookupExistingClassName(field->type, namespaceName, &classInfo);
    if (field->typeQName.empty()) {
        errors->addError(field->astNode, "unknown type '%s' for field '%s' in '%s'", field->type.c_str(), field->name.c_str(), classInfo.name.c_str());
        field->typeQName = "omnetpp::cObject";
    }

    field->symbolicConstant = std::string("FIELD_")+field->name;
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
    field->isOpaque = getPropertyAsBool(field->props, PROP_OPAQUE, fieldClassInfo.isOpaque);
    field->overrideGetter = getPropertyAsBool(field->props, PROP_OVERRIDEGETTER, false) || getPropertyAsBool(field->props, "override", false);
    field->overrideSetter = getPropertyAsBool(field->props, PROP_OVERRIDESETTER, false) || getPropertyAsBool(field->props, "override", false);

    bool isReadonly = getPropertyAsBool(field->props, PROP_READONLY, false);
    bool isEditableDefault = fieldClassInfo.isEditable && !field->isConst && !isReadonly && !field->isPointer && classInfo.generateSettersInDescriptor;
    field->isEditable = getPropertyAsBool(field->props, PROP_EDITABLE, isEditableDefault);
    bool isReplaceableDefault = field->isPointer && !field->isConst && !isReadonly && classInfo.generateSettersInDescriptor;
    field->isReplaceable = getPropertyAsBool(field->props, PROP_REPLACEABLE, isReplaceableDefault);
    bool isResizableDefault = classInfo.isClass && field->isArray && !field->isFixedArray && !isReadonly && classInfo.generateSettersInDescriptor;
    field->isResizable = getPropertyAsBool(field->props, PROP_RESIZABLE, isResizableDefault);

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
            errors->addWarning(field->astNode, "ambiguous enum '%s' in field '%s' in '%s';  possibilities: %s", field->enumName.c_str(), field->name.c_str(), classInfo.name.c_str(), opp_join(found, ", ").c_str());
            field->enumQName = found[0];
        }
        // need to overwrite it in props, because Qtenv will look up the enum by qname
        Property newProp(PROP_ENUM, "", field->astNode);
        newProp.addValue("", field->enumQName);
        field->props.add(newProp);
    }

    if (fieldClassInfo.isEnum) {
        Property newProp(PROP_ENUM, "", field->astNode);
        newProp.addValue("", field->typeQName);
        field->props.add(newProp);
    }

    bool supportsPtr = getPropertyAsBool(field->props, PROP_SUPPORTSPTR, fieldClassInfo.supportsPtr);
    if (field->isPointer && !supportsPtr)
        errors->addError(field->astNode, "'%s *' pointers are not allowed", field->type.c_str());

    field->byValue = getPropertyAsBool(field->props, PROP_BYVALUE, fieldClassInfo.byValue);
    field->isOwnedPointer = field->isPointer && getPropertyAsBool(field->props, PROP_OWNED, false);
    if (hasProperty(field->props, PROP_OWNED) && !field->isPointer)
        errors->addWarning(field->astNode, "ignoring @owned property for non-pointer field '%s'", field->name.c_str());

    field->hasStrMethod = fieldClassInfo.iscObject || !fieldClassInfo.str.empty();

    // fromstring/tostring
    field->fromString = fieldClassInfo.fromString;
    field->toString = fieldClassInfo.toString;
    if (!field->enumName.empty()) {
        field->toString = str("enum2string($, \"") + field->enumQName + "\")";
        field->fromString = str("(") + field->enumQName + ")string2enum($, \"" + field->enumQName + "\")";
    }
    field->fromString = getProperty(field->props, PROP_FROMSTRING, field->fromString);
    field->toString = getProperty(field->props, PROP_TOSTRING, field->toString);

    // fromvalue/tovalue
    field->fromValue = getProperty(field->props, PROP_FROMVALUE, fieldClassInfo.fromValue);
    field->toValue = getProperty(field->props, PROP_TOVALUE, fieldClassInfo.toValue);

    // default method names
    if (classInfo.isClass) {
        std::string capfieldname = field->name;
        capfieldname[0] = toupper(capfieldname[0]);
        field->setter = str("set") + capfieldname;
        field->remover = str("remove") + capfieldname;
        field->dropper = str("drop") + capfieldname;
        field->inserter = str("insert") + capfieldname;
        field->appender = str("append") + capfieldname;
        field->eraser = str("erase") + capfieldname;
        field->sizeSetter = str("set") + capfieldname + "ArraySize";
        if (classInfo.omitGetVerb) {
            field->getter = field->name;
            field->sizeGetter = field->name + "ArraySize";
        }
        else {
            std::string fname = field->name;
            bool is = fname.length() >= 3 && fname[0] == 'i' && fname[1] == 's' && opp_isupper(fname[2]);
            bool has = fname.length() >= 4 && fname[0] == 'h' && fname[1] == 'a' && fname[1] == 's' && opp_isupper(fname[3]);
            bool omitGet = (field->type == "bool") && (is || has);
            field->getter = omitGet ? fname : (str("get") + capfieldname);
            field->sizeGetter = str("get") + capfieldname + "ArraySize";
        }
        field->getterForUpdate = str("get") + capfieldname + "ForUpdate";
        field->allowReplace = getPropertyAsBool(field->props, PROP_ALLOWREPLACE, false);

        // allow customization of names
        bool existingClass = !classInfo.generateClass;
        if (getProperty(field->props, PROP_SETTER) != "")
            field->setter = getMethodProperty(field->props, PROP_SETTER, existingClass);
        if (getProperty(field->props, PROP_GETTER) != "")
            field->getter = getMethodProperty(field->props, PROP_GETTER, existingClass);
        if (getProperty(field->props, PROP_GETTERFORUPDATE) != "")
            field->getterForUpdate = getMethodProperty(field->props, PROP_GETTERFORUPDATE, existingClass);
        if (getProperty(field->props, PROP_SIZESETTER) != "")
            field->sizeSetter = getMethodProperty(field->props, PROP_SIZESETTER, existingClass);
        if (getProperty(field->props, PROP_SIZEGETTER) != "")
            field->sizeGetter = getMethodProperty(field->props, PROP_SIZEGETTER, existingClass);
        if (getProperty(field->props, PROP_INSERTER) != "")
            field->inserter = getMethodProperty(field->props, PROP_INSERTER, existingClass);
        if (getProperty(field->props, PROP_APPENDER) != "")
            field->appender = getMethodProperty(field->props, PROP_APPENDER, existingClass);
        if (getProperty(field->props, PROP_ERASER) != "")
            field->eraser = getMethodProperty(field->props, PROP_ERASER, existingClass);
        if (getProperty(field->props, PROP_REMOVER) != "")
            field->remover = getMethodProperty(field->props, PROP_REMOVER, existingClass);

        field->getterConversion = getProperty(field->props, PROP_GETTERCONVERSION, fieldClassInfo.getterConversion);
    }

    //TODO warn for non-applicable properties like @allowReplace for non-ownedpointer fields

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
    std::string cpptypeBase = getProperty(field->props, PROP_CPPTYPE, "");
    std::string datatypeBase = getProperty(field->props, PROP_DATAMEMBERTYPE, cpptypeBase);
    std::string argtypeBase = getProperty(field->props, PROP_ARGTYPE, cpptypeBase);
    std::string returntypeBase = getProperty(field->props, PROP_RETURNTYPE, cpptypeBase);

    if (datatypeBase.empty())
        datatypeBase = makeRelative(fieldClassInfo.dataTypeBase, classInfo.namespaceName);
    if (argtypeBase.empty())
        argtypeBase = makeRelative(fieldClassInfo.argTypeBase, classInfo.namespaceName);
    if (returntypeBase.empty())
        returntypeBase = makeRelative(fieldClassInfo.returnTypeBase, classInfo.namespaceName);

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

    field->baseDataType = datatypeBase;
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

    field->hasGetterForUpdate = !field->isConst && (field->isPointer ? true : !field->byValue);

    if (field->isEditable && field->fromString.empty() && classInfo.generateDescriptor && classInfo.generateSettersInDescriptor)
        errors->addError(field->astNode, "Field '%s' is editable, but @fromString is unspecified", field->name.c_str());

    field->isCustom = getPropertyAsBool(field->props, PROP_CUSTOM, false);
    field->isCustomImpl = getPropertyAsBool(field->props, PROP_CUSTOMIMPL, false);

    if (classInfo.isClass && field->isOwnedPointer) {
        field->clone = getProperty(field->props, PROP_CLONE, fieldClassInfo.clone);
        if (field->clone.empty()) {
            if (field->iscObject)
                field->clone = "->dup()";
            else
                field->clone = str("new ") + datatypeBase + "(*$)";
        }
    }
}

MsgAnalyzer::FieldInfo *MsgAnalyzer::findField(ClassInfo& classInfo, const std::string& name)
{
    for (FieldInfo& field : classInfo.fieldList)
        if (field.name == name)
            return &field;
    return nullptr;
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

MsgAnalyzer::ClassInfo MsgAnalyzer::extractClassInfoFromEnum(EnumElement *enumElem, const std::string& namespaceName, bool isImported)
{
    ClassInfo classInfo = extractClassInfo(enumElem, namespaceName, isImported);
/*
    @primitive;
    @descriptor(false);
    @fromString((namespaceName::typeName)string2enum($, "namespaceName::typeName"));
    @toString(enum2string($, "namespaceName::typeName"));
    @defaultValue(static_cast<namespaceName::typeName>(-1));
 */
    classInfo.dataTypeBase = classInfo.qname;
    classInfo.fromString = str("(") + classInfo.qname + ")string2enum($, \"" + classInfo.qname + "\")";
    classInfo.toString = str("enum2string($, \"") + classInfo.qname + "\")";
    classInfo.defaultValue = str("static_cast<") + classInfo.qname + ">(-1)";
    classInfo.toValue = "static_cast<int>($)";
    classInfo.fromValue = str("static_cast<") + classInfo.qname + ">($.intValue())";

    // determine base class
    if (classInfo.extendsName != "")
        errors->addError(classInfo.astNode, "'%s': type '%s' cannot be used as base class of enum", classInfo.name.c_str(), classInfo.extendsName.c_str());

    classInfo.extendsQName = "";

    // determine class kind
    classInfo.isClass = false;
    classInfo.iscObject = false;
    classInfo.iscOwnedObject = false;
    classInfo.iscNamedObject = false;
    classInfo.isEnum = true;


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

static bool isBuiltinType(const std::string& s)
{
    static std::set<std::string> types {
        "bool", "char", "short", "int", "long", "float", "double", "const char *",
        "unsigned char", "unsigned short", "unsigned int", "unsigned long",
        "int8_t", "int16_t", "int32_t", "int64_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t",
        "int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64",
    };

    return s.find(':') == std::string::npos && types.find(s) != types.end();
}

std::string MsgAnalyzer::makeRelative(const std::string& qname, const std::string& contextNamespace)
{
    if (contextNamespace.empty())
        return qname;
    if (isBuiltinType(qname)) // doesn't allow being prefixed with "::"
        return qname;
    std::string nsPrefix = contextNamespace + "::";
    if (opp_stringbeginswith(qname.c_str(), nsPrefix.c_str()))
        return qname.substr(nsPrefix.length());  // inside the namespace: make relative
    else
        return "::" + qname; // outside the namespace: make absolute
}

bool MsgAnalyzer::getPropertyAsBool(const Properties& props, const char *name, bool defval)
{
    const Property *p = props.get(name);
    if (p == nullptr)
        return defval;
    const auto& values = p->getValue("");
    if (values.empty())
        return true;
    else  if (values.size() == 1) {
        if (values[0].empty() || values[0] == "true")
            return true;
        else if (values[0] == "false")
            return false;
        errors->addError(p->getASTNode(), "invalid value for boolean property @%s: '%s'", name, values[0].c_str());
        return defval;
    }
    errors->addError(p->getASTNode(), "property @%s does not contain a single value", name);
    return defval;
}

std::string MsgAnalyzer::getProperty(const Properties& props, const char *name, const std::string& defval)
{
    const Property *p = props.get(name);
    if (p == nullptr)
        return defval;
    const auto& values = p->getValue("");
    if (values.empty())
        return "";
    else if (values.size() == 1)
        return values[0];
    errors->addError(p->getASTNode(), "property @%s does not contain a single value", name);
    return "";
}

std::string MsgAnalyzer::getMethodProperty(const Properties& props, const char *propName, bool existingClass)
{
    std::string methodName = getProperty(props, propName);
    if (!existingClass) {
        // property value must be a valid identifier that can be used as method name
        if (!opp_isvalididentifier(methodName.c_str())) {
            const Property *p = props.get(propName);
            errors->addError(p->getASTNode(), "'%s' is an invalid name for a method; field property @%s must contain a valid identifier unless class is marked @existingClass", methodName.c_str(), propName);
        }
    }
    return methodName;
}

std::string MsgAnalyzer::lookupExistingClassName(const std::string& name, const std::string& contextNamespace,  ClassInfo *contextClass)
{
    if (name.empty())
        return "";

    if (name.find("::") == 0) {
        // if name starts with "::", take it as an explicitly qualified name
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
            auto pos = lookupNamespace.rfind("::");
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

void MsgAnalyzer::validateProperty(const Property& property, const char *usage)
{
    const Property *propertyDecl = typeTable->getGlobalProperties().get(PROP_PROPERTY, property.getName());
    if (propertyDecl == nullptr) {
        errors->addWarning(property.getASTNode(), "Unknown property @%s (missing declaration via @property?)", property.getName().c_str());
        return;
    }

    const auto& allowedUsages = propertyDecl->getValue("usage");
    if (!allowedUsages.empty() && !contains(allowedUsages, str(usage)))
        errors->addWarning(property.getASTNode(), "@%s cannot be used as %s property (see see corresponding @property declaration)", property.getName().c_str(), usage);
}

void MsgAnalyzer::validateFileProperty(const Property& property)
{
    validateProperty(property, "file");
}

void MsgAnalyzer::analyzeCplusplusBlockTarget(CplusplusElement *cppElem, const std::string& currentNamespace)
{
    std::string qname, method;
    std::string target = cppElem->getTarget();
    if (target == "" || target == "h" || target == "cc")
        return; // not per-class or per-method block

    qname = prefixWithNamespace(target, currentNamespace);
    ClassInfo *classInfo = typeTable->findClassInfo(qname);
    if (!classInfo) {
        // assume part after last "::" is a method name
        int pos = qname.rfind("::");
        if (pos > 0) {
            method = qname.substr(pos+2);
            qname = qname.substr(0, pos);
            classInfo = typeTable->findClassInfo(qname);
        }
    }

    bool ok = false;
    if (classInfo == nullptr)
        errors->addError(cppElem, "invalid target for cplusplus block: '%s' does not refer to a type in current namespace", target.c_str()); // note: qname may already be truncated here
    else if (classInfo->isEnum)
        errors->addError(cppElem, "invalid target for cplusplus block: type '%s' is an enum", qname.c_str());
    else if (classInfo->isImported)
        errors->addError(cppElem, "invalid target for cplusplus block: type '%s' is imported", qname.c_str());
    else if (!classInfo->generateClass)
        errors->addError(cppElem, "invalid target for cplusplus block: type '%s' is not generated", qname.c_str());
    else if (method == "" && classInfo->classExtraCode != "")
        errors->addError(cppElem, "invalid target for cplusplus block: duplicate block for type '%s'", qname.c_str());
    else if (method != "" && containsKey(classInfo->methodCplusplusBlocks, method))
        errors->addError(cppElem, "invalid target for cplusplus block: duplicate block for method '%s::%s", qname.c_str(), method.c_str());
    else
        ok = true;

    if (ok) {
        if (method == "")
            classInfo->classExtraCode = cppElem->getBody();
        else
            classInfo->methodCplusplusBlocks[method] = cppElem;
    }
}



}  // namespace nedxml
}  // namespace omnetpp
