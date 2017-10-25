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
#include "nedexception.h"
#include "nedutil.h"

#include "omnetpp/simkerneldefs.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

#define SL(x)    (x)

inline std::string str(const char *s) {
    return s;
}

static char charToNameFilter(char ch)
{
    return (isalnum(ch)) ? ch : '_';
}

static std::string canonicalizeQName(const std::string& namespac, const std::string& name)
{
    std::string qname;
    if (name.find("::") != name.npos) {
        qname = name.substr(0, 2) == "::" ? name.substr(2) : name;  // remove leading "::", because names in @classes don't have it either
    }
    else if (!namespac.empty() && !name.empty()) {
        qname = namespac + "::" + name;
    }
    else
        qname = name;
    return qname;
}

static std::string makeIdentifier(const std::string& qname)
{
    std::string tmp = qname;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), charToNameFilter);
    return tmp;
}

inline std::string ptr2str(const char *ptr)
{
    return ptr ? ptr : "";
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

static const char *_RESERVED_WORDS[] =
{
    "namespace", "cplusplus", "struct", "message", "packet", "class", "noncobject",
    "enum", "extends", "abstract", "readonly", "properties", "fields", "unsigned",
    "true", "false", "for", "while", "if", "else", "do", "enum", "class", "struct",
    "typedef", "public", "private", "protected", "auto", "register", "sizeof", "void",
    "new", "delete", "explicit", "static", "extern", "return", "try", "catch",
    nullptr
};

MsgAnalyzer::StringSet MsgAnalyzer::RESERVED_WORDS;

void MsgAnalyzer::initDescriptors()
{
    if (RESERVED_WORDS.empty()) {
        for (int i = 0; typeTable._PRIMITIVE_TYPES[i].nedTypeName; ++i)
            RESERVED_WORDS.insert(typeTable._PRIMITIVE_TYPES[i].nedTypeName);
        for (int i = 0; _RESERVED_WORDS[i]; ++i)
            RESERVED_WORDS.insert(_RESERVED_WORDS[i]);
    }
}

MsgAnalyzer::MsgAnalyzer(MsgTypeTable& typeTable, NEDErrorStore *e) : typeTable(typeTable)
{
    initDescriptors();
    errors = e;
}

MsgAnalyzer::~MsgAnalyzer()
{
}

void MsgAnalyzer::extractClassDecl(NEDElement *child, const std::string& namespaceName)
{
    std::string name = ptr2str(child->getAttribute("name"));
    if (RESERVED_WORDS.find(name) != RESERVED_WORDS.end()) {
        errors->addError(child, "type name '%s' is a reserved word\n", name.c_str());
        return;
    }

    std::string type0 = child->getTagName();
    std::string myclass = name;
    std::string baseclass = ptr2str(child->getAttribute("extends-name"));
    ClassType type;
    bool isCobject = (ptr2str(child->getAttribute("is-cobject")) == "true");

    std::string classqname = canonicalizeQName(namespaceName, myclass);

    if (type0 == "struct-decl") {
        type = ClassType::STRUCT;
    }
    else if (type0 == "message-decl" || type0 == "packet-decl") {
        type = ClassType::COWNEDOBJECT;
    }
    else if (type0 == "class-decl") {
        if (!isCobject) {
            type = ClassType::NONCOBJECT;
            if (!baseclass.empty()) {
                errors->addError(child, "'%s': the keywords noncobject and extends cannot be used together", name.c_str());
            }
        }
        else if (baseclass == "") {
            type = ClassType::COWNEDOBJECT;
        }
        else if (baseclass == "void") {
            type = ClassType::NONCOBJECT;
        }
        else {
            StringVector found = typeTable.lookupExistingClassName(baseclass, namespaceName);
            if (found.size() == 1) {
                type = typeTable.getClassType(found[0]);
            }
            else if (found.empty()) {
                errors->addError(child, "'%s': unknown ancestor class '%s'\n", myclass.c_str(), baseclass.c_str());
                type = ClassType::COBJECT;
            }
            else {
                errors->addWarning(child, "'%s': ambiguous ancestor class '%s'; possibilities: '%s'\n", myclass.c_str(), baseclass.c_str(), join(found, "','").c_str());
                type = typeTable.getClassType(found[0]);
            }
        }
    }
    else {
        errors->addError(child, "invalid type '%s' in class '%s'\n", type0.c_str(), myclass.c_str());
        return;
    }

    typeTable.addDeclaredClass(classqname, type, child);
}

MsgAnalyzer::Properties MsgAnalyzer::extractPropertiesOf(NEDElement *node)
{
    Properties props;

    for (PropertyElement *child = static_cast<PropertyElement *>(node->getFirstChildWithTag(NED_PROPERTY)); child; child = child->getNextPropertySibling()) {
        std::string propname = ptr2str(child->getAttribute("name"));
        std::string propval;
        for (PropertyKeyElement *key = child->getFirstPropertyKeyChild(); key; key = key->getNextPropertyKeySibling()) {
            std::string keyname = ptr2str(key->getAttribute("name"));
            if (keyname.empty()) {
                const char *sep = "";
                for (LiteralElement *lit = key->getFirstLiteralChild(); lit; lit = lit->getNextLiteralSibling()) {
                    std::string s = ptr2str(lit->getAttribute("value"));
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

MsgAnalyzer::ClassInfo MsgAnalyzer::extractClassInfo(NEDElement *node, const std::string& namespaceName)
{
    ClassInfo classInfo;
    classInfo.nedElement = node;

    classInfo.keyword = node->getTagName();
    classInfo.msgname = ptr2str(node->getAttribute("name"));
    classInfo.msgbase = ptr2str(node->getAttribute("extends-name"));
    classInfo.namespacename = namespaceName;
    classInfo.props = extractPropertiesOf(node);

    for (NEDElement *child = node->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case NED_FIELD: {
                FieldInfo field;
                field.nedElement = child;
                field.fname = ptr2str(child->getAttribute("name"));
                field.datatype = ptr2str(child->getAttribute("data-type"));  // ha ez nincs, eltunnek a struct mezoi....
                field.ftype = ptr2str(child->getAttribute("data-type"));
                field.fval = ptr2str(child->getAttribute("default-value"));
                field.fisabstract = ptr2str(child->getAttribute("is-abstract")) == "true";
                field.fispointer = (field.ftype[field.ftype.length()-1] == '*');
                if (field.fispointer) {
                    field.ftype = field.ftype.substr(0, field.ftype.find_last_not_of(" \t*")+1);
                }
                field.fisarray = ptr2str(child->getAttribute("is-vector")) == "true";
                field.farraysize = ptr2str(child->getAttribute("vector-size"));

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
                errors->addError(child, "syntax error: property '%s' declaration unaccepted here", child->getTagName());
                break;

            case NED_PROPERTY_KEY:
                errors->addError(child, "syntax error: property key '%s' unaccepted here", child->getTagName());
                break;

            case NED_COMMENT:
                break;

            default:
                errors->addError(child, "unaccepted element: '%s'", child->getTagName());
                break;
        }
    }
    return classInfo;
}

void MsgAnalyzer::analyzeField(ClassInfo& classInfo, FieldInfo *field, const std::string& namespaceName)
{
    if (field->fisabstract && !classInfo.gap) {
        errors->addError(field->nedElement, "abstract fields need '@customize(true)' property in '%s'\n", classInfo.msgname.c_str());
    }

    // determine field data type
    if (field->ftype.empty()) {
        // base class field assignment
        field->classtype = ClassType::UNKNOWN;
        field->fisprimitivetype = false; //FIXME we don't know
    }
    else if (containsKey(typeTable.PRIMITIVE_TYPES, field->ftype)) {
        field->fisprimitivetype = true;
        field->ftypeqname = "";  // unused
        field->classtype = ClassType::NONCOBJECT;
    }
    else {
        field->fisprimitivetype = false;

        StringVector candidateTypes = typeTable.lookupExistingClassName(field->ftype, namespaceName);
        if (candidateTypes.size() == 1) {
            field->ftypeqname = candidateTypes[0];
        }
        else if (candidateTypes.empty()) {
            errors->addError(field->nedElement, "unknown type '%s' for field '%s' in '%s'\n", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
            field->ftypeqname = "omnetpp::cObject";
        }
        else {
            errors->addError(field->nedElement, "unknown type '%s' for field '%s' in '%s'; possibilities: %s\n", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str(), join(candidateTypes, ", ").c_str());
            field->ftypeqname = candidateTypes[0];
        }

        field->classtype = typeTable.getClassType(field->ftypeqname);

        if (field->ftypeqname != "omnetpp::cObject")
            field->ftypeqname = str("::") + field->ftypeqname; //FIXME why, really?
    }

    if (classInfo.generate_class) {
        if (field->classtype == ClassType::COWNEDOBJECT && !(classInfo.classtype == ClassType::COBJECT || classInfo.classtype == ClassType::CNAMEDOBJECT || classInfo.classtype == ClassType::COWNEDOBJECT)) {
            errors->addError(field->nedElement, "cannot use cOwnedObject field '%s %s' in struct or non-cObject class '%s'\n", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
        }
    }

    if (field->fispointer && field->fisprimitivetype) {
        errors->addError(field->nedElement, "pointers not supported for primitive types in '%s'\n", classInfo.msgname.c_str());
        return;
    }

    if (field->fisarray && field->farraysize.empty() && !field->fval.empty())
        errors->addError(field->nedElement, "unaccepted default value for variable length vector field '%s %s' in class '%s'\n", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
    if (field->fispointer) {
        if (field->fval.empty())
            field->fval = "nullptr";
    }
    field->fnopack = getPropertyAsBool(field->fprops, "nopack", false);
    field->feditable = getPropertyAsBool(field->fprops, "editable", false);
    field->editNotDisabled = getPropertyAsBool(field->fprops, "editable", true);
    field->fopaque = getPropertyAsBool(field->fprops, "opaque", false);
    field->overrideGetter = getPropertyAsBool(field->fprops, "overridegetter", false) || getPropertyAsBool(field->fprops, "override", false);
    field->overrideSetter = getPropertyAsBool(field->fprops, "overridesetter", false) || getPropertyAsBool(field->fprops, "override", false);
    field->tostring = getProperty(field->fprops, "tostring", "");
    field->fromstring = getProperty(field->fprops, "fromstring", "");
    if (hasProperty(field->fprops, "byvalue")) {
        if (field->fispointer)
            errors->addError(field->nedElement, "unaccepted @byvalue property for pointer field '%s %s' in class '%s'\n", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
        else if (field->fisprimitivetype)
            errors->addError(field->nedElement, "unaccepted @byvalue property for primitive type field '%s %s' in class '%s'\n", field->ftype.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
    }
    field->byvalue = getPropertyAsBool(field->fprops, "byvalue", field->fisprimitivetype);

    // resolve enum namespace
    field->enumname = getProperty(field->fprops, "enum");
    if (!field->enumname.empty()) {
        StringVector found = typeTable.lookupExistingEnumName(field->enumname, namespaceName);
        if (found.size() == 1) {
            field->enumqname = found[0];
        }
        else if (found.empty()) {
            errors->addError(field->nedElement, "undeclared enum '%s' in field '%s' in '%s'\n", field->enumname.c_str(), field->fname.c_str(), classInfo.msgname.c_str());
            field->enumqname = "";
        }
        else {
            errors->addWarning(field->nedElement, "ambiguous enum '%s' in field '%s' in '%s';  possibilities: %s\n", field->enumname.c_str(), field->fname.c_str(), classInfo.msgname.c_str(), join(found, ", ").c_str());
            field->enumqname = found[0];
        }
        field->fprops["enum"] = field->enumqname;  // need to modify the property in place
        if (field->tostring.empty())
            field->tostring = str("enum2string($, \"") + field->enumqname + "\")";
        if (field->fromstring.empty())
            field->fromstring = str("(") + field->enumqname + ")string2enum($, \"" + field->enumqname + "\")";
    }

    // variable name
    if (classInfo.classtype == ClassType::STRUCT) {
        field->var = field->fname;
    }
    else {
        field->var = field->fname + classInfo.fieldnamesuffix;
        field->argname = field->fname;
    }

    if (field->fispointer) {
        field->fisownedpointer = getPropertyAsBool(field->fprops, "owned", field->classtype == ClassType::COWNEDOBJECT);
    }
    else
        field->fisownedpointer = false;

    field->varsize = field->farraysize.empty() ? (field->fname + "_arraysize") : field->farraysize;
    std::string sizetypeprop = getProperty(field->fprops, "sizetype");
    field->fsizetype = !sizetypeprop.empty() ? sizetypeprop : "unsigned int";  // TODO change to size_t

    // default method names
    if (classInfo.classtype != ClassType::STRUCT) {
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
        if (getProperty(field->fprops, "setter") != "") {
            field->setter = getProperty(field->fprops, "setter");
        }
        if (getProperty(field->fprops, "getter") != "") {
            field->getter = getProperty(field->fprops, "getter");
        }
        if (getProperty(field->fprops, "mgetter") != "") {
            field->mGetter = getProperty(field->fprops, "mgetter");
        }
        if (getProperty(field->fprops, "sizeSetter") != "") {
            field->alloc = getProperty(field->fprops, "sizeSetter");
        }
        if (getProperty(field->fprops, "sizeGetter") != "") {
            field->getsize = getProperty(field->fprops, "sizeGetter");
        }
    }

    // data type, argument type, conversion to/from string...
    field->maybe_c_str = "";
    if (!field->fisprimitivetype) {
        if (field->fispointer) {
            field->datatype = field->ftype + " *";
            field->argtype = field->datatype;
            field->rettype = field->datatype;
        }
        else if (field->byvalue) {
            field->datatype = field->ftype;
            field->argtype = field->datatype;
            field->rettype = field->datatype;
        }
        else {
            field->datatype = field->ftype;
            field->argtype = str("const ") + field->ftype + "&";
            field->rettype = field->ftype + "&";
        }
    }
    else {
        Assert(containsKey(typeTable.PRIMITIVE_TYPES, field->ftype));
        const TypeDesc& primitiveType = typeTable.PRIMITIVE_TYPES[field->ftype];
        field->datatype = primitiveType.cppTypeName;
        field->argtype = primitiveType.cppTypeName;
        field->rettype = primitiveType.cppTypeName;
        if (field->fval.empty())
            field->fval = primitiveType.emptyValue;
        if (field->tostring.empty())
            field->tostring = primitiveType.tostring;
        if (field->fromstring.empty())
            field->fromstring = primitiveType.fromstring;

        if (field->ftype == "string") {
            field->argtype = "const char *";
            field->rettype = "const char *";
            field->maybe_c_str = ".c_str()";
        }
    }

    if (field->feditable || (classInfo.generate_setters_in_descriptor && field->fisprimitivetype && field->editNotDisabled))
        if (field->fromstring.empty())
            errors->addError(field->nedElement, "Field '%s' is editable, but fromstring() function is unspecified", field->fname.c_str()); //TODO only if descriptor class is also generated
}

void MsgAnalyzer::analyze(ClassInfo& classInfo, const std::string& namespaceName, const MsgCompilerOptions& opts)
{
    classInfo.msgqname = prefixWithNamespace(classInfo.msgname, namespaceName);

    // determine info.msgbaseqname
    if (classInfo.msgbase != "void") {
        StringVector found = typeTable.lookupExistingClassName(classInfo.msgbase, namespaceName);
        if (found.size() == 1) {
            classInfo.msgbaseqname = found[0];
        }
        else if (found.empty()) {
            errors->addError(classInfo.nedElement, "'%s': unknown base class '%s', available classes '%s'", classInfo.msgname.c_str(), classInfo.msgbase.c_str(), join(omnetpp::common::keys(typeTable.declaredClasses), "','").c_str());
            classInfo.msgbaseqname = "omnetpp::cMessage";
        }
        else {
            errors->addError(classInfo.nedElement, "'%s': ambiguous base class '%s'; possibilities: '%s'",
                    classInfo.msgname.c_str(), classInfo.msgbase.c_str(), join(found, "','").c_str());
            classInfo.msgbaseqname = found[0];
        }
    }

    // check base class and determine type of object
    if (classInfo.msgqname == "omnetpp::cObject" || classInfo.msgqname == "omnetpp::cNamedObject" || classInfo.msgqname == "omnetpp::cOwnedObject") {
        classInfo.classtype = typeTable.getClassType(classInfo.msgqname);  // only for sim_std.msg
    }
    else if (classInfo.msgbase == "") {
        if (classInfo.keyword == "message" || classInfo.keyword == "packet") {
            classInfo.classtype = ClassType::COWNEDOBJECT;
        }
        else if (classInfo.keyword == "class") {
            classInfo.classtype = ClassType::COBJECT;  // Note: we never generate non-cObject classes
        }
        else if (classInfo.keyword == "struct") {
            classInfo.classtype = ClassType::STRUCT;
        }
        else {
            throw NEDException("Internal error: Invalid keyword:'%s' at '%s'", classInfo.keyword.c_str(), classInfo.msgclass.c_str());
        }
        // if announced earlier as noncobject, accept that.
        if (typeTable.isClassDeclared(classInfo.msgqname)) {
            if (typeTable.getClassType(classInfo.msgqname) == ClassType::NONCOBJECT && classInfo.classtype == ClassType::COBJECT) {
                classInfo.classtype = ClassType::NONCOBJECT;
            }
        }
    }
    else if (classInfo.msgbase == "void") {
        classInfo.classtype = ClassType::NONCOBJECT;
    }
    else if (classInfo.msgbaseqname != "") {
        classInfo.classtype = typeTable.getClassType(classInfo.msgbaseqname);
    }
    else {
        errors->addError(classInfo.nedElement, "unknown base class '%s' for '%s'\n", classInfo.msgbase.c_str(), classInfo.msgname.c_str());
        classInfo.classtype = ClassType::COBJECT;
    }

    // check earlier declarations and register this class
    if (typeTable.isClassDeclared(classInfo.msgqname) && false) // XXX add condition
        errors->addError(classInfo.nedElement, "attempt to redefine '%s'\n", classInfo.msgname.c_str());
    typeTable.addDeclaredClass(classInfo.msgqname, classInfo.classtype, classInfo.nedElement);

    //
    // produce all sorts of derived names
    //
    classInfo.generate_class = opts.generateClasses && !getPropertyAsBool(classInfo.props, "existingClass", false);
    classInfo.generate_descriptor = opts.generateDescriptors && getPropertyAsBool(classInfo.props, "descriptor", true);
    classInfo.generate_setters_in_descriptor = opts.generateSettersInDescriptors && (getProperty(classInfo.props, "descriptor") != "readonly");

    if (getPropertyAsBool(classInfo.props, "customize", false)) {
        classInfo.gap = 1;
        classInfo.msgclass = classInfo.msgname + "_Base";
        classInfo.realmsgclass = classInfo.msgname;
        classInfo.msgdescclass = makeIdentifier(classInfo.realmsgclass) + "Descriptor";
    }
    else {
        classInfo.gap = 0;
        classInfo.msgclass = classInfo.msgname;
        classInfo.realmsgclass = classInfo.msgname;
        classInfo.msgdescclass = makeIdentifier(classInfo.msgclass) + "Descriptor";
    }
    if (classInfo.msgbase == "") {
        if (classInfo.msgqname == "omnetpp::cObject") {
            classInfo.msgbaseclass = "";
        }
        else if (classInfo.keyword == "message") {
            classInfo.msgbaseclass = "omnetpp::cMessage";
        }
        else if (classInfo.keyword == "packet") {
            classInfo.msgbaseclass = "omnetpp::cPacket";
        }
        else if (classInfo.keyword == "class") {
            classInfo.msgbaseclass = "omnetpp::cObject";  // note: all classes we generate subclass from cObject!
        }
        else if (classInfo.keyword == "struct") {
            classInfo.msgbaseclass = "";
        }
        else {
            throw NEDException("Internal error");
        }
    }
    else if (classInfo.msgbase == "void") {
        classInfo.msgbaseclass = "";
    }
    else {
        classInfo.msgbaseclass = classInfo.msgbaseqname;
    }

    classInfo.omitgetverb = getPropertyAsBool(classInfo.props, "omitGetVerb", false);
    classInfo.fieldnamesuffix = getProperty(classInfo.props, "fieldNameSuffix", "");
    if (classInfo.omitgetverb && classInfo.fieldnamesuffix.empty()) {
        errors->addWarning(classInfo.nedElement, "@omitGetVerb(true) and (implicit) @fieldNameSuffix(\"\") collide: "
                                            "adding '_var' suffix to data members to prevent name conflict between them and getter methods\n");
        classInfo.fieldnamesuffix = "_var";
    }

    std::string s = getProperty(classInfo.props, "implements");
    if (!s.empty()) {
        classInfo.implements = StringTokenizer(s.c_str(), ",").asVector();
    }

    for (auto& field : classInfo.fieldlist) {
        analyzeField(classInfo, &field, namespaceName);
    }
    for (auto& field :  classInfo.baseclassFieldlist) {
        analyzeField(classInfo, &field, namespaceName);
    }
}

MsgAnalyzer::EnumInfo MsgAnalyzer::extractEnumInfo(EnumElement *node, const std::string& namespaceName)
{
    EnumInfo enumInfo;
    enumInfo.nedElement = node;
    enumInfo.enumName = ptr2str(node->getAttribute("name"));
    enumInfo.enumQName = canonicalizeQName(namespaceName, enumInfo.enumName);

    // prepare enum items:
    for (NEDElement *child = node->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case NED_ENUM_FIELDS:
                for (NEDElement *e = child->getFirstChild(); e; e = e->getNextSibling()) {
                    switch (e->getTagCode()) {
                        case NED_ENUM_FIELD: {
                            EnumItem item;
                            item.nedElement = e;
                            item.name = ptr2str(e->getAttribute("name"));
                            item.value = ptr2str(e->getAttribute("value"));
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

}  // namespace nedxml
}  // namespace omnetpp
