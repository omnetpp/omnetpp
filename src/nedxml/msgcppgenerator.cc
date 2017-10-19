//==========================================================================
//  MSGCPPGENERATOR.CC - part of
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
#include "omnetpp/platdep/platmisc.h"  // unlink()
#include "msgcppgenerator.h"
#include "ned2generator.h"
#include "nedparser.h"
#include "nedexception.h"
#include "nedutil.h"

#include "omnetpp/simkerneldefs.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

using std::ostream;

#define H     (*hOutp)
#define CC    (*ccOutp)

#define PROGRAM    "nedtool"

#define SL(x)    (x)

inline std::string str(const char *s) {
    return s;
}

inline std::string TS(const std::string& s)
{
    return s.empty() ? s : s + " ";
}

static char charToNameFilter(char ch)
{
    return (isalnum(ch)) ? ch : '_';
}

inline bool isQualified(const std::string& qname)
{
    return qname.find("::") != qname.npos;
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

inline std::ostream& operator<<(std::ostream& o, const std::pair<std::string, int>& p)
{
    return o << '(' << p.first << ':' << p.second << ')';
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

const char *MsgCppGenerator::_RESERVED_WORDS[] =
{
        "namespace", "cplusplus", "struct", "message", "packet", "class", "noncobject",
        "enum", "extends", "abstract", "readonly", "properties", "fields", "unsigned",
        "true", "false", "for", "while", "if", "else", "do", "enum", "class", "struct",
        "typedef", "public", "private", "protected", "auto", "register", "sizeof", "void",
        "new", "delete", "explicit", "static", "extern", "return", "try", "catch",
        nullptr
};

void MsgCppGenerator::initDescriptors()
{
    for (int i = 0; typeTable._PRIMITIVE_TYPES[i].nedTypeName; ++i) {
        RESERVED_WORDS.insert(typeTable._PRIMITIVE_TYPES[i].nedTypeName);
    }
    for (int i = 0; _RESERVED_WORDS[i]; ++i) {
        RESERVED_WORDS.insert(_RESERVED_WORDS[i]);
    }
}

MsgCppGenerator::MsgCppGenerator(NEDErrorStore *e, const MsgCppGeneratorOptions& options)
{
    initDescriptors();

    opts = options;

    hOutp = ccOutp = nullptr;
    errors = e;
}

MsgCppGenerator::~MsgCppGenerator()
{
}

void MsgCppGenerator::generate(MsgFileElement *fileElement, const char *hFile, const char *ccFile)
{
    hFilename = hFile;
    ccFilename = ccFile;
    std::ofstream hStream(hFile);
    std::ofstream ccStream(ccFile);
    hOutp = &hStream;
    ccOutp = &ccStream;
    process(fileElement, true);
    hStream.close();
    ccStream.close();

    if (errors->containsError()) {
        // remove output files when error occurred:
        unlink(hFile);
        unlink(ccFile);
    }
}

void MsgCppGenerator::processImport(NEDElement *child, const std::string& currentDir)
{
    if (!namespaceName.empty()) {
        errors->addError(child, "imports are not allowed within a namespace\n");
        return;
    }

    std::string importName = child->getAttribute("import-spec");
    std::string fileName = resolveImport(importName, currentDir);
    if (fileName == "") {
        errors->addError(child, "cannot resolve import '%s'\n", importName.c_str());
        return;
    }

    NEDParser parser(errors);
    NEDElement *tree = parser.parseMSGFile(fileName.c_str());
    if (errors->containsError()) {
        delete tree;
        return;
    }

    typeTable.importedNedFiles.push_back(tree); // keep AST until we're done because ClassInfo/FieldInfo refer to it...

    // extract declarations
    MsgFileElement *fileElement = check_and_cast<MsgFileElement*>(tree);
    process(fileElement, false);
    namespaceName = "";
}

std::string MsgCppGenerator::resolveImport(const std::string& importName, const std::string& currentDir)
{
    std::string msgFile = opp_replacesubstring(importName, ".", "/", true) + ".msg";
    std::string candidate = concatDirAndFile(currentDir.c_str(), msgFile.c_str());
    if (fileExists(candidate.c_str()))
        return candidate;
    for (auto dir : opts.importPath) {
        std::string candidate = concatDirAndFile(dir.c_str(), msgFile.c_str());
        if (fileExists(candidate.c_str()))
            return candidate;
    }
    return "";
}

void MsgCppGenerator::extractClassDecl(NEDElement *child)
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

const char *PARSIMPACK_BOILERPLATE =
    "namespace omnetpp {\n"
    "\n"
    "// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.\n"
    "// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument\n"
    "\n"
    "// Packing/unpacking an std::vector\n"
    "template<typename T, typename A>\n"
    "void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)\n"
    "{\n"
    "    int n = v.size();\n"
    "    doParsimPacking(buffer, n);\n"
    "    for (int i = 0; i < n; i++)\n"
    "        doParsimPacking(buffer, v[i]);\n"
    "}\n"
    "\n"
    "template<typename T, typename A>\n"
    "void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)\n"
    "{\n"
    "    int n;\n"
    "    doParsimUnpacking(buffer, n);\n"
    "    v.resize(n);\n"
    "    for (int i = 0; i < n; i++)\n"
    "        doParsimUnpacking(buffer, v[i]);\n"
    "}\n"
    "\n"
    "// Packing/unpacking an std::list\n"
    "template<typename T, typename A>\n"
    "void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)\n"
    "{\n"
    "    doParsimPacking(buffer, (int)l.size());\n"
    "    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)\n"
    "        doParsimPacking(buffer, (T&)*it);\n"
    "}\n"
    "\n"
    "template<typename T, typename A>\n"
    "void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)\n"
    "{\n"
    "    int n;\n"
    "    doParsimUnpacking(buffer, n);\n"
    "    for (int i=0; i<n; i++) {\n"
    "        l.push_back(T());\n"
    "        doParsimUnpacking(buffer, l.back());\n"
    "    }\n"
    "}\n"
    "\n"
    "// Packing/unpacking an std::set\n"
    "template<typename T, typename Tr, typename A>\n"
    "void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)\n"
    "{\n"
    "    doParsimPacking(buffer, (int)s.size());\n"
    "    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)\n"
    "        doParsimPacking(buffer, *it);\n"
    "}\n"
    "\n"
    "template<typename T, typename Tr, typename A>\n"
    "void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)\n"
    "{\n"
    "    int n;\n"
    "    doParsimUnpacking(buffer, n);\n"
    "    for (int i=0; i<n; i++) {\n"
    "        T x;\n"
    "        doParsimUnpacking(buffer, x);\n"
    "        s.insert(x);\n"
    "    }\n"
    "}\n"
    "\n"
    "// Packing/unpacking an std::map\n"
    "template<typename K, typename V, typename Tr, typename A>\n"
    "void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)\n"
    "{\n"
    "    doParsimPacking(buffer, (int)m.size());\n"
    "    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {\n"
    "        doParsimPacking(buffer, it->first);\n"
    "        doParsimPacking(buffer, it->second);\n"
    "    }\n"
    "}\n"
    "\n"
    "template<typename K, typename V, typename Tr, typename A>\n"
    "void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)\n"
    "{\n"
    "    int n;\n"
    "    doParsimUnpacking(buffer, n);\n"
    "    for (int i=0; i<n; i++) {\n"
    "        K k; V v;\n"
    "        doParsimUnpacking(buffer, k);\n"
    "        doParsimUnpacking(buffer, v);\n"
    "        m[k] = v;\n"
    "    }\n"
    "}\n"
    "\n"
    "// Default pack/unpack function for arrays\n"
    "template<typename T>\n"
    "void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)\n"
    "{\n"
    "    for (int i = 0; i < n; i++)\n"
    "        doParsimPacking(b, t[i]);\n"
    "}\n"
    "\n"
    "template<typename T>\n"
    "void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)\n"
    "{\n"
    "    for (int i = 0; i < n; i++)\n"
    "        doParsimUnpacking(b, t[i]);\n"
    "}\n"
    "\n"
    "// Default rule to prevent compiler from choosing base class' doParsimPacking() function\n"
    "template<typename T>\n"
    "void doParsimPacking(omnetpp::cCommBuffer *, const T& t)\n"
    "{\n"
    "    throw omnetpp::cRuntimeError(\"Parsim error: No doParsimPacking() function for type %s\", omnetpp::opp_typename(typeid(t)));\n"
    "}\n"
    "\n"
    "template<typename T>\n"
    "void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)\n"
    "{\n"
    "    throw omnetpp::cRuntimeError(\"Parsim error: No doParsimUnpacking() function for type %s\", omnetpp::opp_typename(typeid(t)));\n"
    "}\n"
    "\n"
    "}  // namespace omnetpp\n"
    "\n";

void MsgCppGenerator::process(MsgFileElement *fileElement, bool generateCode)
{
    std::string currentDir = directoryOf(fileElement->getFilename());
    std::string headerGuard;
    if (generateCode) {
        // make header guard using the file name
        std::string hfilenamewithoutdir = hFilename;
        size_t pos = hfilenamewithoutdir.find_last_of('/');
        if (pos != hfilenamewithoutdir.npos)
            hfilenamewithoutdir = hfilenamewithoutdir.substr(pos+1);

        headerGuard = hfilenamewithoutdir;

        // add first namespace to headerguard
        NamespaceElement *firstNS = fileElement->getFirstNamespaceChild();
        if (firstNS)
            headerGuard = ptr2str(firstNS->getAttribute("name")) + "_" + headerGuard;

        // replace non-alphanumeric characters by '_'
        std::transform(headerGuard.begin(), headerGuard.end(), headerGuard.begin(), charToNameFilter);
        // capitalize
        std::transform(headerGuard.begin(), headerGuard.end(), headerGuard.begin(), ::toupper);
        headerGuard = str("__") + headerGuard;

        H << "//\n// Generated file, do not edit! Created by " PROGRAM " " << (OMNETPP_VERSION / 0x100) << "." << (OMNETPP_VERSION % 0x100)
          << " from " << fileElement->getFilename() << ".\n//\n\n";
        H << "#if defined(__clang__)\n";
        H << "#  pragma clang diagnostic ignored \"-Wreserved-id-macro\"\n";
        H << "#endif\n";
        H << "#ifndef " << headerGuard << "\n";
        H << "#define " << headerGuard << "\n\n";
        H << "#include <omnetpp.h>\n";
        H << "\n";
        H << "// " PROGRAM " version check\n";
        H << "#define MSGC_VERSION 0x" << std::hex << std::setfill('0') << std::setw(4) << OMNETPP_VERSION << "\n";
        H << "#if (MSGC_VERSION!=OMNETPP_VERSION)\n";
        H << "#    error Version mismatch! Probably this file was generated by an earlier version of " PROGRAM ": 'make clean' should help.\n";
        H << "#endif\n";
        H << "\n";

        if (opts.exportDef.length() > 4 && opts.exportDef.substr(opts.exportDef.length()-4) == "_API") {
            // # generate boilerplate code for dll export
            std::string exportbase = opts.exportDef.substr(0, opts.exportDef.length()-4);
            H << "// dll export symbol\n";
            H << "#ifndef " << opts.exportDef << "\n";
            H << "#  if defined(" << exportbase << "_EXPORT)\n";
            H << "#    define " << opts.exportDef << "  OPP_DLLEXPORT\n";
            H << "#  elif defined(" << exportbase << "_IMPORT)\n";
            H << "#    define " << opts.exportDef << "  OPP_DLLIMPORT\n";
            H << "#  else\n";
            H << "#    define " << opts.exportDef << "\n";
            H << "#  endif\n";
            H << "#endif\n";
            H << "\n";
        }

        CC << "//\n// Generated file, do not edit! Created by " PROGRAM " " << (OMNETPP_VERSION / 0x100) << "." << (OMNETPP_VERSION % 0x100) << " from " << fileElement->getFilename() << ".\n//\n\n";
        CC << "// Disable warnings about unused variables, empty switch stmts, etc:\n";
        CC << "#ifdef _MSC_VER\n";
        CC << "#  pragma warning(disable:4101)\n";
        CC << "#  pragma warning(disable:4065)\n";
        CC << "#endif\n\n";

        CC << "#if defined(__clang__)\n";
        CC << "#  pragma clang diagnostic ignored \"-Wshadow\"\n";
        CC << "#  pragma clang diagnostic ignored \"-Wconversion\"\n";
        CC << "#  pragma clang diagnostic ignored \"-Wunused-parameter\"\n";
        CC << "#  pragma clang diagnostic ignored \"-Wc++98-compat\"\n";
        CC << "#  pragma clang diagnostic ignored \"-Wunreachable-code-break\"\n";
        CC << "#  pragma clang diagnostic ignored \"-Wold-style-cast\"\n";
        CC << "#elif defined(__GNUC__)\n";
        CC << "#  pragma GCC diagnostic ignored \"-Wshadow\"\n";
        CC << "#  pragma GCC diagnostic ignored \"-Wconversion\"\n";
        CC << "#  pragma GCC diagnostic ignored \"-Wunused-parameter\"\n";
        CC << "#  pragma GCC diagnostic ignored \"-Wold-style-cast\"\n";
        CC << "#  pragma GCC diagnostic ignored \"-Wsuggest-attribute=noreturn\"\n";
        CC << "#  pragma GCC diagnostic ignored \"-Wfloat-conversion\"\n";
        CC << "#endif\n\n";

        CC << "#include <iostream>\n";
        CC << "#include <sstream>\n";
        CC << "#include <memory>\n";
        CC << "#include \"" << hfilenamewithoutdir << "\"\n\n";

        CC << PARSIMPACK_BOILERPLATE;

        if (!firstNS) {
            H << "\n\n";
            CC << "\n";
            generateTemplates();
        }
    }

    /*
       <!ELEMENT msg-file (comment*, (namespace|property-decl|property|cplusplus|
                           import|
                           struct-decl|class-decl|message-decl|packet-decl|enum-decl|
                           struct|class|message|packet|enum)*)>
     */
    for (NEDElement *child = fileElement->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case NED_NAMESPACE:
                // open namespace(s)
                if (generateCode && !namespaceName.empty())
                    generateNamespaceEnd();
                namespaceName = ptr2str(child->getAttribute("name"));
                if (generateCode)
                    generateNamespaceBegin(child);
                break;

            case NED_CPLUSPLUS: {
                // print C++ block
                if (generateCode) {
                    std::string body = ptr2str(child->getAttribute("body"));
                    std::string target = ptr2str(child->getAttribute("target"));
                    size_t pos0 = body.find_first_not_of("\r\n");
                    if (pos0 != body.npos)
                        body = body.substr(pos0);
                    size_t pos = body.find_last_not_of("\r\n");
                    if (pos != body.npos)
                        body = body.substr(0, pos+1);
                    if (target == "" || target == "h")
                        H << "// cplusplus {{\n" << body << "\n// }}\n\n";
                    else if (target == "cc")
                        CC << "// cplusplus {{\n" << body << "\n// }}\n\n";
                    else
                        errors->addError(child, "unrecognized target '%s' for cplusplus block", target.c_str());
                }
                break;
            }

            case NED_IMPORT: {
                std::string importName = child->getAttribute("import-spec");
                if (!common::contains(importsSeen, importName)) {
                    importsSeen.insert(importName);
                    processImport(child, currentDir);
                    if (generateCode) {
                        // assuming C++ include path is identical to msg import path, we can directly derive the include from importName
                        std::string header = opp_replacesubstring(importName, ".", "/", true) + "_m.h";
                        H << "#include \"" << header << "\" // import " << importName << "\n\n";
                    }
                }
                break;
            }

            case NED_STRUCT_DECL:
            case NED_CLASS_DECL:
            case NED_MESSAGE_DECL:
            case NED_PACKET_DECL:
                extractClassDecl(child);
                break;

            case NED_ENUM_DECL: {
                // forward declaration -- add to table
                std::string name = ptr2str(child->getAttribute("name"));
                if (contains(RESERVED_WORDS, name))
                    errors->addError(child, "Enum name is reserved word: '%s'", name.c_str());
                std::string qname = canonicalizeQName(namespaceName, name);
                typeTable.declaredEnums.insert(qname);
                break;
            }

            case NED_ENUM: {
                EnumInfo enumInfo = extractEnumInfo(check_and_cast<EnumElement *>(child));
                typeTable.declaredEnums.insert(enumInfo.enumQName);
                if (generateCode)
                    generateEnum(enumInfo);
                break;
            }

            case NED_STRUCT: {
                ClassInfo classInfo = extractClassInfo(child);
                analyze(classInfo);
                typeTable.addDeclaredClass(classInfo.msgname, classInfo.classtype, child);
                if (generateCode) {
                    if (classInfo.generate_class)
                        generateStruct(classInfo);
                    if (classInfo.generate_descriptor)
                        generateDescriptorClass(classInfo);
                }
                break;
            }

            case NED_CLASS:
            case NED_MESSAGE:
            case NED_PACKET: {
                ClassInfo classInfo = extractClassInfo(child);
                analyze(classInfo);
                typeTable.addDeclaredClass(classInfo.msgqname, classInfo.classtype, child);
                if (generateCode) {
                    if (classInfo.generate_class)
                        generateClass(classInfo);
                    if (classInfo.generate_descriptor)
                        generateDescriptorClass(classInfo);
                }
                break;
            }
        }
    }

    if (generateCode) {
        generateNamespaceEnd();
        H << "#endif // ifndef " << headerGuard << "\n\n";
    }
}

MsgCppGenerator::Properties MsgCppGenerator::extractPropertiesOf(NEDElement *node)
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

MsgCppGenerator::ClassInfo MsgCppGenerator::extractClassInfo(NEDElement *node)
{
    ClassInfo classInfo;
    classInfo.nedElement = node;

    classInfo.keyword = node->getTagName();
    classInfo.msgname = ptr2str(node->getAttribute("name"));
    classInfo.msgbase = ptr2str(node->getAttribute("extends-name"));
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

void MsgCppGenerator::analyzeField(ClassInfo& classInfo, FieldInfo *field)
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
            CC << "\n\n/*\n Undeclared enum: " << field->enumname << "\n";
            CC << "  Declared enums:\n";
            for (const auto & qname : typeTable.declaredEnums)
                CC << "    " << qname << "\n";
            CC << "\n*/\n\n";
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
            field->mGetter = str("get") + capfieldname;             //TODO "get" (for compatibility) or "getMutable"  or "access"
            field->getsize = str("get") + capfieldname + "ArraySize";
        }

        // allow customization of names
        if (getProperty(field->fprops, "setter") != "") {
            field->setter = getProperty(field->fprops, "setter");
        }
        if (getProperty(field->fprops, "getter") != "") {
            field->getter = getProperty(field->fprops, "getter");
        }
        field->mGetter = field->getter;
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
}

void MsgCppGenerator::analyze(ClassInfo& classInfo)
{
    classInfo.msgqname = prefixWithNamespace(classInfo.msgname);

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
        analyzeField(classInfo, &field);
    }
    for (auto& field :  classInfo.baseclassFieldlist) {
        analyzeField(classInfo, &field);
    }
}

std::string MsgCppGenerator::generatePreComment(NEDElement *nedElement)
{
    std::ostringstream s;
    NED2Generator(errors).generate(s, nedElement, "");
    std::string str = s.str();

    std::ostringstream o;
    o << " * <pre>\n";
    o << opp_indentlines(opp_trim(opp_replacesubstring(opp_replacesubstring(str, "*/", "  ", true), "@", "\\@", true)), " * ");
    o << "\n";
    o << " * </pre>\n";
    return o.str();
}

void MsgCppGenerator::generateClass(const ClassInfo& classInfo)
{
    if (isQualified(classInfo.msgclass)) {
        errors->addError(classInfo.nedElement, "type name may only be qualified when generating descriptor for an existing class: '%s'\n", classInfo.msgclass.c_str());
        return;
    }

    H << "/**\n";
    H << " * Class generated from <tt>" << SL(classInfo.nedElement->getSourceLocation()) << "</tt> by " PROGRAM ".\n";
    H << generatePreComment(classInfo.nedElement);

    if (classInfo.gap) {
        H << " *\n";
        H << " * " << classInfo.msgclass << " is only useful if it gets subclassed, and " << classInfo.realmsgclass << " is derived from it.\n";
        H << " * The minimum code to be written for " << classInfo.realmsgclass << " is the following:\n";
        H << " *\n";
        H << " * <pre>\n";
        H << " * class " << TS(opts.exportDef) << classInfo.realmsgclass << " : public " << classInfo.msgclass << "\n";
        H << " * {\n";
        H << " *   private:\n";
        H << " *     void copy(const " << classInfo.realmsgclass << "& other) { ... }\n\n";
        H << " *   public:\n";
        if (classInfo.classtype == ClassType::COWNEDOBJECT || classInfo.classtype == ClassType::CNAMEDOBJECT) {
            if (classInfo.keyword == "message" || classInfo.keyword == "packet") {
                H << " *     " << classInfo.realmsgclass << "(const char *name=nullptr, short kind=0) : " << classInfo.msgclass << "(name,kind) {}\n";
            }
            else {
                H << " *     " << classInfo.realmsgclass << "(const char *name=nullptr) : " << classInfo.msgclass << "(name) {}\n";
            }
        }
        else {
            H << " *     " << classInfo.realmsgclass << "() : " << classInfo.msgclass << "() {}\n";
        }
        H << " *     " << classInfo.realmsgclass << "(const " << classInfo.realmsgclass << "& other) : " << classInfo.msgclass << "(other) {copy(other);}\n";
        H << " *     " << classInfo.realmsgclass << "& operator=(const " << classInfo.realmsgclass << "& other) {if (this==&other) return *this; " << classInfo.msgclass << "::operator=(other); copy(other); return *this;}\n";
        if (classInfo.classtype == ClassType::COWNEDOBJECT || classInfo.classtype == ClassType::CNAMEDOBJECT || classInfo.classtype == ClassType::COBJECT) {
            H << " *     virtual " << classInfo.realmsgclass << " *dup() const override {return new " << classInfo.realmsgclass << "(*this);}\n";
        }
        H << " *     // ADD CODE HERE to redefine and implement pure virtual functions from " << classInfo.msgclass << "\n";
        H << " * };\n";
        H << " * </pre>\n";
        if (classInfo.classtype == ClassType::COWNEDOBJECT || classInfo.classtype == ClassType::CNAMEDOBJECT || classInfo.classtype == ClassType::COBJECT) {
            H << " *\n";
            H << " * The following should go into a .cc (.cpp) file:\n";
            H << " *\n";
            H << " * <pre>\n";
            H << " * Register_Class(" << classInfo.realmsgclass << ")\n";
            H << " * </pre>\n";
        }
    }
    H << " */\n";

    H << "class " << TS(opts.exportDef) << classInfo.msgclass;
    const char *baseclassSepar = " : ";
    if (!classInfo.msgbaseclass.empty()) {
        H << baseclassSepar << "public ::" << classInfo.msgbaseclass;  // make namespace explicit and absolute to disambiguate the way PROGRAM understood it
        baseclassSepar = ", ";
    }

    for (const auto& impl : classInfo.implements) {
        H << baseclassSepar << "public " << impl;
        baseclassSepar = ", ";
    }

    H << "\n{\n";
    H << "  protected:\n";
    for (const FieldInfo& field : classInfo.fieldlist) {
        if (field.fispointer && field.fisprimitivetype) {
            errors->addError(field.nedElement, "pointers not supported for primitive types in '%s'\n", classInfo.msgname.c_str());
            return;
        }
        if (!field.fisabstract) {
            if (field.fisarray && !field.farraysize.empty()) {
                H << "    " << field.datatype << " " << field.var << "[" << field.farraysize << "];\n";
            }
            else if (field.fisarray && field.farraysize.empty()) {
                H << "    " << field.datatype << " *" << field.var << "; // array ptr\n";
                H << "    " << field.fsizetype << " " << field.varsize << ";\n";
            }
            else {
                H << "    " << field.datatype << " " << field.var << ";\n";
            }
        }
    }
    H << "\n";
    H << "  private:\n";
    H << "    void copy(const " << classInfo.msgclass << "& other);\n\n";
    H << "  protected:\n";
    H << "    // protected and unimplemented operator==(), to prevent accidental usage\n";
    H << "    bool operator==(const " << classInfo.msgclass << "&);\n";
    if (classInfo.gap) {
        H << "    // make constructors protected to avoid instantiation\n";
    }
    else {
        H << "\n";
        H << "  public:\n";
    }
    if (classInfo.classtype == ClassType::COWNEDOBJECT || classInfo.classtype == ClassType::CNAMEDOBJECT) {
        if (classInfo.keyword == "message" || classInfo.keyword == "packet") {
            H << "    " << classInfo.msgclass << "(const char *name=nullptr, short kind=0);\n";
        }
        else {
            H << "    " << classInfo.msgclass << "(const char *name=nullptr);\n";
        }
    }
    else {
        H << "    " << classInfo.msgclass << "();\n";
    }
    H << "    " << classInfo.msgclass << "(const " << classInfo.msgclass << "& other);\n";
    if (classInfo.gap) {
        H << "    // make assignment operator protected to force the user override it\n";
        H << "    " << classInfo.msgclass << "& operator=(const " << classInfo.msgclass << "& other);\n";
        H << "\n";
        H << "  public:\n";
    }
    H << "    virtual ~" << classInfo.msgclass << "();\n";
    if (!classInfo.gap) {
        H << "    " << classInfo.msgclass << "& operator=(const " << classInfo.msgclass << "& other);\n";
    }
    if (classInfo.gap) {
        H << "    virtual " << classInfo.msgclass << " *dup() const override {throw omnetpp::cRuntimeError(\"You forgot to manually add a dup() function to class " << classInfo.realmsgclass << "\");}\n";
    }
    else {
        H << "    virtual " << classInfo.msgclass << " *dup() const override {return new " << classInfo.msgclass << "(*this);}\n";
    }
    H << "    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;\n";
    H << "    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;\n";
    H << "\n";
    H << "    // field getter/setter methods\n";
    for (const auto& field : classInfo.fieldlist) {
        std::string pure;
        if (field.fisabstract)
            pure = " = 0";
        std::string overrideGetter(field.overrideGetter ? " override" : "");
        std::string overrideSetter(field.overrideSetter ? " override" : "");
        std::string getterIndexVar("");
        std::string getterIndexArg("");
        std::string setterIndexArg("");

        if (field.fisarray) {
            getterIndexVar = "k";
            getterIndexArg = field.fsizetype + " " + getterIndexVar;
            setterIndexArg = getterIndexArg + ", ";
            if (field.farraysize.empty()) {
                H << "    virtual void " << field.alloc << "(" << field.fsizetype << " size)" << pure << ";\n";
            }
            H << "    virtual " << field.fsizetype << " " << field.getsize << "() const" << pure << ";\n";
        }
        if (field.fispointer) {
            H << "    virtual const " << field.rettype << " " << field.getter << "(" << getterIndexArg << ") const" << overrideGetter << pure << ";\n";
            H << "    virtual " << field.rettype << " " << field.mGetter << "(" << getterIndexArg << ")" << overrideGetter << " { handleChange(); return const_cast<" << field.rettype << ">(const_cast<const " << classInfo.msgclass << "*>(this)->" << field.getter << "(" << getterIndexVar << "));}\n";
            if (field.fisownedpointer)  //TODO fispointer or fisownedpointer?
                H << "    virtual " << field.rettype << " " << field.remover << "(" << getterIndexArg << ")" << overrideGetter << pure << ";\n";
        }
        else if (!field.byvalue) {
            H << "    virtual const " << field.rettype << " " << field.getter << "(" << getterIndexArg << ") const" << overrideGetter << pure << ";\n";
            H << "    virtual " << field.rettype << " " << field.mGetter << "(" << getterIndexArg << ")" << overrideGetter << " { handleChange(); return const_cast<" << field.rettype << ">(const_cast<const " << classInfo.msgclass << "*>(this)->" << field.getter << "(" << getterIndexVar << "));}\n";
        }
        else {
            H << "    virtual " << field.rettype << " " << field.getter << "(" << getterIndexArg << ") const" << overrideGetter << pure << ";\n";
        }

        H << "    virtual void " << field.setter << "(" << setterIndexArg << field.argtype << " " << field.argname << ")" << overrideSetter << pure << ";\n";
    }
    H << "};\n\n";

    if (!classInfo.gap) {
        if (classInfo.classtype == ClassType::COWNEDOBJECT || classInfo.classtype == ClassType::CNAMEDOBJECT || classInfo.classtype == ClassType::COBJECT) {
            CC << "Register_Class(" << classInfo.msgclass << ")\n\n";
        }

        H << "inline void doParsimPacking(omnetpp::cCommBuffer *b, const " << classInfo.realmsgclass << "& obj) {obj.parsimPack(b);}\n";
        H << "inline void doParsimUnpacking(omnetpp::cCommBuffer *b, " << classInfo.realmsgclass << "& obj) {obj.parsimUnpack(b);}\n\n";
    }

    // constructor:
    if (classInfo.classtype == ClassType::COWNEDOBJECT || classInfo.classtype == ClassType::CNAMEDOBJECT) {
        if (classInfo.keyword == "message" || classInfo.keyword == "packet") {
            // CAREFUL when assigning values to existing members gets implemented!
            // The msg kind passed to the ctor should take priority!!!
            CC << "" << classInfo.msgclass << "::" << classInfo.msgclass << "(const char *name, short kind) : ::" << classInfo.msgbaseclass << "(name,kind)\n";
        }
        else {
            if (classInfo.msgbaseclass == "") {
                CC << "" << classInfo.msgclass << "::" << classInfo.msgclass << "(const char *name)\n";
            }
            else {
                CC << "" << classInfo.msgclass << "::" << classInfo.msgclass << "(const char *name) : ::" << classInfo.msgbaseclass << "(name)\n";
            }
        }
    }
    else {
        if (classInfo.msgbaseclass == "") {
            CC << "" << classInfo.msgclass << "::" << classInfo.msgclass << "()\n";
        }
        else {
            CC << "" << classInfo.msgclass << "::" << classInfo.msgclass << "() : ::" << classInfo.msgbaseclass << "()\n";
        }
    }
    CC << "{\n";
    // CC << "    (void)static_cast<cObject *>(this); //sanity check\n" if (info.fieldclasstype == ClassType::COBJECT);
    // CC << "    (void)static_cast<cNamedObject *>(this); //sanity check\n" if (info.fieldclasstype == ClassType::CNAMEDOBJECT);
    // CC << "    (void)static_cast<cOwnedObject *>(this); //sanity check\n" if (info.fieldclasstype == ClassType::COWNEDOBJECT);
    for (const auto& it : classInfo.baseclassFieldlist) {
        CC << "    this->" << it.setter << "(" << it.fval << ");\n";
    }
    if (!classInfo.baseclassFieldlist.empty() && !classInfo.fieldlist.empty())
        CC << "\n";
    for (const auto& field : classInfo.fieldlist) {
        if (!field.fisabstract) {
            if (field.fisarray) {
                if (!field.farraysize.empty()) {
                    if (!field.fval.empty()) {
                        CC << "    for (" << field.fsizetype << " i=0; i<" << field.varsize << "; i++)\n";
                        CC << "        this->" << field.var << "[i] = " << field.fval << ";\n";
                    }
                    if (field.classtype == ClassType::COWNEDOBJECT || field.fisownedpointer) {
                        CC << "    for (" << field.fsizetype << " i=0; i<" << field.varsize << "; i++)\n";
                        if (field.fispointer) {
                            if (field.fisownedpointer)
                                CC << "        if (this->" << field.var << " != nullptr) { take(this->" << field.var << "[i]); } // XXX\n";
                        }
                        else
                            CC << "        take(&(this->" << field.var << "[i]));\n";
                    }
                }
                else {
                    CC << "    " << field.varsize << " = 0;\n";
                    CC << "    this->" << field.var << " = nullptr;\n";
                }
            }
            else {
                if (!field.fval.empty()) {
                    CC << "    this->" << field.var << " = " << field.fval << ";\n";
                }
                if (field.classtype == ClassType::COWNEDOBJECT) {
                    if (field.fispointer)
                        CC << "    if (this->" << field.var << " != nullptr) { take(this->" << field.var << "); }\n";
                    else
                        CC << "    take(&(this->" << field.var << "));\n";
                }
            }
        }
    }
    CC << "}\n\n";

    // copy constructor:
    CC << "" << classInfo.msgclass << "::" << classInfo.msgclass << "(const " << classInfo.msgclass << "& other)";
    if (!classInfo.msgbaseclass.empty()) {
        CC << " : ::" << classInfo.msgbaseclass << "(other)";
        //TODO @implements ???
    }
    CC << "\n{\n";
    for (const auto& field : classInfo.fieldlist) {
        if (!field.fisabstract) {
            if (field.fisarray) {
                if (!field.farraysize.empty()) {
                    if (field.fispointer) {
                        CC << "    for (" << field.fsizetype << " i=0; i<" << field.varsize << "; i++)\n";
                        CC << "        this->" << field.var << "[i] = nullptr;\n";
                    }
                    else if (field.classtype == ClassType::COWNEDOBJECT) {
                        CC << "    for (" << field.fsizetype << " i=0; i<" << field.varsize << "; i++)\n";
                        CC << "        take(&(this->" << field.var << "[i]));\n";
                    }
                }
                else
                {
                    CC << "    " << field.varsize << " = 0;\n";
                    CC << "    this->" << field.var << " = nullptr;\n";
                }
            }
            else {
                if (field.fispointer) {
                    CC << "    this->" << field.var << " = nullptr;\n";
                }
                else if (field.classtype == ClassType::COWNEDOBJECT) {
                    CC << "    take(&(this->" << field.var << "));\n";
                }
            }
        }
    }
    CC << "    copy(other);\n";
    CC << "}\n\n";

    // destructor:
    CC << "" << classInfo.msgclass << "::~" << classInfo.msgclass << "()\n";
    CC << "{\n";
    for (const auto& field : classInfo.fieldlist) {
        if (!field.fisabstract) {
            if (field.fisarray) {
                std::ostringstream s;
                if (field.fisownedpointer)
                    s << "        dropAndDelete(this->" << field.var << "[i]);\n";
                else if (!field.fispointer && field.classtype == ClassType::COWNEDOBJECT)
                    s << "        drop(&(this->" << field.var << "[i]));\n";
                if (!s.str().empty()) {
                    CC << "    for (" << field.fsizetype << " i=0; i<" << field.varsize << "; i++) {\n";
                    CC << s.str();
                    CC << "    }\n";
                }

                if (field.farraysize.empty()) {
                    CC << "    delete [] this->" << field.var << ";\n";
                }
            }
            else {
                if (field.fisownedpointer)
                    CC << "    dropAndDelete(this->" << field.var << ");\n";
                else if (!field.fispointer && field.classtype == ClassType::COWNEDOBJECT)
                    CC << "    drop(&(this->" << field.var << "));\n";
            }
        }
    }
    CC << "}\n\n";

    // operator = :
    CC << "" << classInfo.msgclass << "& " << classInfo.msgclass << "::operator=(const " << classInfo.msgclass << "& other)\n";
    CC << "{\n";
    CC << "    if (this==&other) return *this;\n";
    if (classInfo.msgbaseclass != "") {
        CC << "    ::" << classInfo.msgbaseclass << "::operator=(other);\n";
    }
    CC << "    copy(other);\n";
    CC << "    return *this;\n";
    CC << "}\n\n";

    // copy function:
    CC << "void " << classInfo.msgclass << "::copy(const " << classInfo.msgclass << "& other)\n";
    CC << "{\n";
    for (const auto& field : classInfo.fieldlist) {
        // CC << "// " << field.fname << ":\n";
        if (!field.fisabstract) {
            if (field.fisarray) {
                if (field.fispointer) {
                    if (field.fisownedpointer) {
                        CC << "    for (" << field.fsizetype << " i=0; i<" << field.varsize << "; i++) {\n";
                        CC << "        dropAndDelete(this->" << field.var << "[i]);\n";
                        CC << "    }\n";
                    }
                }
                if (field.farraysize.empty()) {
                    if (!field.fispointer && field.classtype == ClassType::COWNEDOBJECT) {
                        CC << "    for (" << field.fsizetype << " i=0; i<" << field.varsize << "; i++) {\n";
                        CC << "        drop(&(this->" << field.var << "[i]));\n";
                        CC << "    }\n";
                    }
                    CC << "    delete [] this->" << field.var << ";\n";
                    CC << "    this->" << field.var << " = (other." << field.varsize << "==0) ? nullptr : new " << field.datatype << "[other." << field.varsize << "];\n";
                    CC << "    " << field.varsize << " = other." << field.varsize << ";\n";
                    if (!field.fispointer && field.classtype == ClassType::COWNEDOBJECT) {
                        CC << "    for (" << field.fsizetype << " i=0; i<" << field.varsize << "; i++) {\n";
                        CC << "        take(&(this->" << field.var << "[i]));\n";
                        CC << "    }\n";
                    }
                }

                CC << "    for (" << field.fsizetype << " i=0; i<" << field.varsize << "; i++) {\n";
                if (field.fispointer) {
                    if (field.fisownedpointer) {
                        CC << "        this->" << field.var << "[i] = other." << field.var << "[i]->dup();\n";
                        CC << "        take(this->" << field.var << "[i]);\n";
                    }
                    else {
                        CC << "        this->" << field.var << "[i] = other." << field.var << "[i];\n";
                        if (field.classtype == ClassType::COWNEDOBJECT || field.classtype == ClassType::CNAMEDOBJECT) {
                            CC << "        this->" << field.var << "[i]->setName(other." << field.var << "[i]->getName());\n";
                        }
                    }
                }
                else {
                    CC << "        this->" << field.var << "[i] = other." << field.var << "[i];\n";
                    if (field.classtype == ClassType::COWNEDOBJECT || field.classtype == ClassType::CNAMEDOBJECT) {
                        CC << "        this->" << field.var << "[i].setName(other." << field.var << "[i].getName());\n";
                    }
                    CC << "    }\n";
                }
            }
            else {
                if (field.fispointer) {
                    if (field.fisownedpointer) {
                        CC << "    dropAndDelete(this->" << field.var << ");\n";
                        CC << "    this->" << field.var << " = other." << field.var << "->dup();\n";
                        CC << "    take(this->" << field.var << ");\n";
                    }
                    else {
                        CC << "    this->" << field.var << " = other." << field.var << ";\n";
                        if (field.classtype == ClassType::COWNEDOBJECT || field.classtype == ClassType::CNAMEDOBJECT) {
                            CC << "    this->" << field.var << "->setName(other." << field.var << "->getName());\n";
                        }
                    }
                }
                else {
                    CC << "    this->" << field.var << " = other." << field.var << ";\n";
                    if (field.classtype == ClassType::COWNEDOBJECT || field.classtype == ClassType::CNAMEDOBJECT) {
                        CC << "    this->" << field.var << ".setName(other." << field.var << ".getName());\n";
                    }
                }
            }
        }
    }
    CC << "}\n\n";

    //
    // Note: This class may not be derived from cOwnedObject, and then this parsimPack()/
    // parsimUnpack() is NOT that of cOwnedObject. However it's still needed because a
    // "friend" doParsimPacking() function could not access protected members otherwise.
    //
    CC << "void " << classInfo.msgclass << "::parsimPack(omnetpp::cCommBuffer *b) const\n";
    CC << "{\n";
    if (classInfo.msgbaseclass != "") {
        if (classInfo.classtype == ClassType::COWNEDOBJECT || classInfo.classtype == ClassType::CNAMEDOBJECT || classInfo.classtype == ClassType::COBJECT) {
            if (classInfo.msgbaseclass != "omnetpp::cObject")
                CC << "    ::" << classInfo.msgbaseclass << "::parsimPack(b);\n";
        }
        else {
            CC << "    doParsimPacking(b,(::" << classInfo.msgbaseclass << "&)*this);\n";  // this would do for cOwnedObject too, but the other is nicer
        }
    }
    for (const auto& field : classInfo.fieldlist) {
        if (field.fnopack) {
            // @nopack specified
        }
        else if (field.fisabstract) {
            CC << "    // field " << field.fname << " is abstract -- please do packing in customized class\n";
        }
        else {
            if (field.fisarray) {
                if (field.farraysize.empty())
                    CC << "    b->pack(" << field.varsize << ");\n";
                CC << "    doParsimArrayPacking(b,this->" << field.var << "," << field.varsize << ");\n";
            }
            else {
                CC << "    doParsimPacking(b,this->" << field.var << ");\n";
            }
        }
    }
    CC << "}\n\n";

    CC << "void " << classInfo.msgclass << "::parsimUnpack(omnetpp::cCommBuffer *b)\n";
    CC << "{\n";
    if (classInfo.msgbaseclass != "") {
        if (classInfo.classtype == ClassType::COWNEDOBJECT || classInfo.classtype == ClassType::CNAMEDOBJECT || classInfo.classtype == ClassType::COBJECT) {
            if (classInfo.msgbaseclass != "omnetpp::cObject")
                CC << "    ::" << classInfo.msgbaseclass << "::parsimUnpack(b);\n";
        }
        else {
            CC << "    doParsimUnpacking(b,(::" << classInfo.msgbaseclass << "&)*this);\n";  // this would do for cOwnedObject too, but the other is nicer
        }
    }
    for (const auto& field : classInfo.fieldlist) {
        if (field.fnopack) {
            // @nopack specified
        }
        else if (field.fisabstract) {
            CC << "    // field " << field.fname << " is abstract -- please do unpacking in customized class\n";
        }
        else {
            if (field.fisarray) {
                if (!field.farraysize.empty()) {
                    CC << "    doParsimArrayUnpacking(b,this->" << field.var << "," << field.farraysize << ");\n";
                }
                else {
                    CC << "    delete [] this->" << field.var << ";\n";
                    CC << "    b->unpack(" << field.varsize << ");\n";
                    CC << "    if (" << field.varsize << "==0) {\n";
                    CC << "        this->" << field.var << " = nullptr;\n";
                    CC << "    } else {\n";
                    CC << "        this->" << field.var << " = new " << field.datatype << "[" << field.varsize << "];\n";
                    CC << "        doParsimArrayUnpacking(b,this->" << field.var << "," << field.varsize << ");\n";
                    CC << "    }\n";
                }
            }
            else {
                CC << "    doParsimUnpacking(b,this->" << field.var << ");\n";
            }
        }
    }
    CC << "}\n\n";

    for (const auto& field : classInfo.fieldlist) {
        if (!field.fisabstract) {
            std::string idx = (field.fisarray) ? "[k]" : "";
            std::string ref = (field.fispointer) ? "" : "&";
            std::string idxarg = (field.fisarray) ? (field.fsizetype + " k") : std::string("");
            std::string idxarg2 = (field.fisarray) ? (idxarg + ", ") : std::string("");

            // getters:
            if (field.fisarray) {
                CC << "" << field.fsizetype << " " << classInfo.msgclass << "::" << field.getsize << "() const\n";
                CC << "{\n";
                CC << "    return " << field.varsize << ";\n";
                CC << "}\n\n";
            }

            if (field.byvalue) {
                CC << "" << field.rettype << " " << classInfo.msgclass << "::" << field.getter << "(" << idxarg << ")" << " const\n";
                CC << "{\n";
                if (field.fisarray) {
                    CC << "    if (k >= " << field.varsize << ") throw omnetpp::cRuntimeError(\"Array of size " << field.varsize << " indexed by %lu\", (unsigned long)k);\n";
                }
                CC << "    return this->" << field.var << idx << field.maybe_c_str << ";\n";
                CC << "}\n\n";
            }
            else {
                CC << "const " << field.rettype << " " << classInfo.msgclass << "::" << field.getter << "(" << idxarg << ")" << " const\n";
                CC << "{\n";
                if (field.fisarray) {
                    CC << "    if (k >= " << field.varsize << ") throw omnetpp::cRuntimeError(\"Array of size " << field.varsize << " indexed by %lu\", (unsigned long)k);\n";
                }
                CC << "    return this->" << field.var << idx << field.maybe_c_str << ";\n";
                CC << "}\n\n";

            }

            // resize:
            if (field.fisarray && field.farraysize.empty()) {
                CC << "void " << classInfo.msgclass << "::" << field.alloc << "(" << field.fsizetype << " size)\n";
                CC << "{\n";
                CC << "    handleChange();\n";
                CC << "    " << field.datatype << " *" << field.var << "2 = (size==0) ? nullptr : new " << field.datatype << "[size];\n";
                CC << "    " << field.fsizetype << " sz = " << field.varsize << " < size ? " << field.varsize << " : size;\n";
                CC << "    for (" << field.fsizetype << " i=0; i<sz; i++)\n";
                CC << "        " << field.var << "2[i] = this->" << field.var << "[i];\n";
                if (field.fisprimitivetype) {
                    CC << "    for (" << field.fsizetype << " i=sz; i<size; i++)\n";
                    CC << "        " << field.var << "2[i] = 0;\n";
                }
                if (field.classtype == ClassType::COWNEDOBJECT) {
                    CC << "    for (" << field.fsizetype << " i=sz; i<size; i++)\n";
                    CC << "        take(&(" << field.var << "2[i]));\n";
                }
                CC << "    " << field.varsize << " = size;\n";
                CC << "    delete [] this->" << field.var << ";\n";
                CC << "    this->" << field.var << " = " << field.var << "2;\n";
                CC << "}\n\n";
            }

            // setter:
            CC << "void " << classInfo.msgclass << "::" << field.setter << "(" << idxarg2 << field.argtype << " " << field.argname << ")\n";
            CC << "{\n";
            if (field.fisarray) {
                CC << "    if (k >= " << field.varsize << ") throw omnetpp::cRuntimeError(\"Array of size " << field.farraysize << " indexed by %lu\", (unsigned long)k);\n";
            }
            CC << "    handleChange();\n";
            if (field.fisownedpointer) {
                CC << "    dropAndDelete(this->" << field.var << idx << ");\n";
            }
            CC << "    this->" << field.var << idx << " = " << field.argname << ";\n";
            if (field.fisownedpointer) {
                CC << "    if (this->" << field.var << idx << " != nullptr)\n";
                CC << "        take(this->" << field.var << idx << ");\n";
            }
            CC << "}\n\n";

            // remover:
            if (field.fisownedpointer) {  //TODO fispointer or fisownedpointer?
                CC << field.rettype << " " << classInfo.msgclass << "::" << field.remover << "(" << idxarg2 << field.argtype << " " << field.argname << ")\n";
                CC << "{\n";
                if (field.fisarray) {
                    CC << "    if (k >= " << field.varsize << ") throw omnetpp::cRuntimeError(\"Array of size " << field.farraysize << " indexed by %lu\", (unsigned long)k);\n";
                }
                CC << "    handleChange();\n";
                CC << "    " << field.rettype << " retval = this->" << field.var << idx << ";\n";
                CC << "    drop(retval);\n";
                CC << "    this->" << field.var << idx << " = nullptr;\n";
                CC << "    return retval;\n";
                CC << "}\n\n";
            }
        }
    }
}

#define DD(x)
void MsgCppGenerator::generateStruct(const ClassInfo& classInfo)
{
    if (isQualified(classInfo.msgclass)) {
        errors->addError(classInfo.nedElement, "type name may only be qualified when generating descriptor for an existing class: '%s'\n", classInfo.msgclass.c_str());
        return;
    }

    H << "/**\n";
    H << " * Struct generated from " << SL(classInfo.nedElement->getSourceLocation()) << " by " PROGRAM ".\n";
    H << " */\n";
    if (classInfo.msgbaseclass.empty()) {
        H << "struct " << TS(opts.exportDef) << classInfo.msgclass << "\n";
    }
    else {
        H << "struct " << TS(opts.exportDef) << classInfo.msgclass << " : public ::" << classInfo.msgbaseqname << "\n";
    }
    H << "{\n";
    H << "    " << classInfo.msgclass << "();\n";
    for (const auto& field : classInfo.fieldlist) {
        H << "    " << field.datatype << " " << field.var;
        if (field.fisarray) {
            H << "[" << field.farraysize << "]";
        }
        H << ";\n";
    }
    H << "};\n\n";

    H << "// helpers for local use\n";
    H << "void " << TS(opts.exportDef) << "__doPacking(omnetpp::cCommBuffer *b, const " << classInfo.msgclass << "& a);\n";
    H << "void " << TS(opts.exportDef) << "__doUnpacking(omnetpp::cCommBuffer *b, " << classInfo.msgclass << "& a);\n\n";

    H << "inline void doParsimPacking(omnetpp::cCommBuffer *b, const " << classInfo.realmsgclass << "& obj) { " << "__doPacking(b, obj); }\n";
    H << "inline void doParsimUnpacking(omnetpp::cCommBuffer *b, " << classInfo.realmsgclass << "& obj) { " << "__doUnpacking(b, obj); }\n\n";

    // Constructor:
    CC << "" << classInfo.msgclass << "::" << classInfo.msgclass << "()\n";
    CC << "{\n";
    // override baseclass fields initial value
    for (const auto& field : classInfo.baseclassFieldlist) {
        CC << "    this->" << field.var << " = " << field.fval << ";\n";
    }
    if (!classInfo.baseclassFieldlist.empty() && !classInfo.fieldlist.empty())
        CC << "\n";

    for (const auto& field : classInfo.fieldlist) {
        if (field.fisabstract)
            throw NEDException("Abstract fields are not supported in a struct");
        if (field.classtype == ClassType::COWNEDOBJECT)
            throw NEDException("cOwnedObject fields are not supported in a struct");
        if (field.fisarray) {
            if (field.farraysize.empty()) {
                throw NEDException("Dynamic arrays are not supported in a struct");
            }
            else {
                if (!field.fval.empty()) {
                    CC << "    for (" << field.fsizetype << " i=0; i<" << field.farraysize << "; i++)\n";
                    CC << "        this->" << field.var << "[i] = " << field.fval << ";\n";
                }
            }
        }
        else {
            if (!field.fval.empty()) {
                CC << "    this->" << field.var << " = " << field.fval << ";\n";
            }
        }
    }
    CC << "}\n\n";

    // doPacking/doUnpacking go to the global namespace
    CC << "void __doPacking(omnetpp::cCommBuffer *b, const " << classInfo.msgclass << "& a)\n";
    CC << "{\n";
    if (!classInfo.msgbaseclass.empty()) {
        CC << "    doParsimPacking(b,(::" << classInfo.msgbaseclass << "&)a);\n";
    }

    for (const auto& field : classInfo.fieldlist) {
        if (field.fisarray) {
            CC << "    doParsimArrayPacking(b,a." << field.var << "," << field.farraysize << ");\n";
        }
        else {
            CC << "    doParsimPacking(b,a." << field.var << ");\n";
        }
    }
    CC << "}\n\n";

    CC << "void __doUnpacking(omnetpp::cCommBuffer *b, " << classInfo.msgclass << "& a)\n";
    CC << "{\n";
    if (!classInfo.msgbaseclass.empty()) {
        CC << "    doParsimUnpacking(b,(::" << classInfo.msgbaseclass << "&)a);\n";
    }

    for (const auto& field : classInfo.fieldlist) {
        if (field.fisarray) {
            CC << "    doParsimArrayUnpacking(b,a." << field.var << "," << field.farraysize << ");\n";
        }
        else {
            CC << "    doParsimUnpacking(b,a." << field.var << ");\n";
        }
    }
    CC << "}\n\n";
}

void MsgCppGenerator::generateDescriptorClass(const ClassInfo& classInfo)
{
    CC << "class " << classInfo.msgdescclass << " : public omnetpp::cClassDescriptor\n";
    CC << "{\n";
    CC << "  private:\n";
    CC << "    mutable const char **propertynames;\n";
    CC << "  public:\n";
    CC << "    " << classInfo.msgdescclass << "();\n";
    CC << "    virtual ~" << classInfo.msgdescclass << "();\n";
    CC << "\n";
    CC << "    virtual bool doesSupport(omnetpp::cObject *obj) const override;\n";
    CC << "    virtual const char **getPropertyNames() const override;\n";
    CC << "    virtual const char *getProperty(const char *propertyname) const override;\n";
    CC << "    virtual int getFieldCount() const override;\n";
    CC << "    virtual const char *getFieldName(int field) const override;\n";
    CC << "    virtual int findField(const char *fieldName) const override;\n";
    CC << "    virtual unsigned int getFieldTypeFlags(int field) const override;\n";
    CC << "    virtual const char *getFieldTypeString(int field) const override;\n";
    CC << "    virtual const char **getFieldPropertyNames(int field) const override;\n";
    CC << "    virtual const char *getFieldProperty(int field, const char *propertyname) const override;\n";
    CC << "    virtual int getFieldArraySize(void *object, int field) const override;\n";
    CC << "\n";
    CC << "    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;\n";
    CC << "    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;\n";
    CC << "    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;\n";
    CC << "\n";
    CC << "    virtual const char *getFieldStructName(int field) const override;\n";
    CC << "    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;\n";
    CC << "};\n\n";

    // register class
    CC << "Register_ClassDescriptor(" << classInfo.msgdescclass << ")\n\n";

    // ctor, dtor
    size_t fieldcount = classInfo.fieldlist.size();
    std::string qualifiedrealmsgclass = prefixWithNamespace(classInfo.realmsgclass);
    CC << "" << classInfo.msgdescclass << "::" << classInfo.msgdescclass << "() : omnetpp::cClassDescriptor(\"" << qualifiedrealmsgclass << "\", \"" << classInfo.msgbaseclass << "\")\n";
    CC << "{\n";
    CC << "    propertynames = nullptr;\n";
    CC << "}\n";
    CC << "\n";

    CC << "" << classInfo.msgdescclass << "::~" << classInfo.msgdescclass << "()\n";
    CC << "{\n";
    CC << "    delete[] propertynames;\n";
    CC << "}\n";
    CC << "\n";

    // doesSupport()
    CC << "bool " << classInfo.msgdescclass << "::doesSupport(omnetpp::cObject *obj) const\n";
    CC << "{\n";
    CC << "    return dynamic_cast<" << classInfo.msgclass << " *>(obj)!=nullptr;\n";
    CC << "}\n";
    CC << "\n";

    // getPropertyNames()
    CC << "const char **" << classInfo.msgdescclass << "::getPropertyNames() const\n";
    CC << "{\n";
    CC << "    if (!propertynames) {\n";
    CC << "        static const char *names[] = { ";
    for (const auto& prop : classInfo.props) {
        CC << opp_quotestr(prop.first) << ", ";
    }
    CC << " nullptr };\n";
    CC << "        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;\n";
    CC << "        propertynames = mergeLists(basenames, names);\n";
    CC << "    }\n";
    CC << "    return propertynames;\n";
    CC << "}\n";
    CC << "\n";

    // getProperty()
    CC << "const char *" << classInfo.msgdescclass << "::getProperty(const char *propertyname) const\n";
    CC << "{\n";
    for (const auto& prop : classInfo.props) {
        CC << "    if (!strcmp(propertyname,"<< opp_quotestr(prop.first) << ")) return " << opp_quotestr(prop.second) << ";\n";
    }
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    return basedesc ? basedesc->getProperty(propertyname) : nullptr;\n";
    CC << "}\n";
    CC << "\n";

    // getFieldCount()
    CC << "int " << classInfo.msgdescclass << "::getFieldCount() const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    return basedesc ? " << fieldcount << "+basedesc->getFieldCount() : " << fieldcount << ";\n";
    CC << "}\n";
    CC << "\n";

    // getFieldTypeFlags()
    CC << "unsigned int " << classInfo.msgdescclass << "::getFieldTypeFlags(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldTypeFlags(field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return 0;\n";
    }
    else {
        CC << "    static unsigned int fieldTypeFlags[] = {\n";
        for (auto& field : classInfo.fieldlist) {
            StringVector flags;
            if (field.fisarray)
                flags.push_back("FD_ISARRAY");
            if (!field.fisprimitivetype)
                flags.push_back("FD_ISCOMPOUND");
            if (field.fispointer)
                flags.push_back("FD_ISPOINTER");
            if (field.classtype == ClassType::COBJECT || field.classtype == ClassType::CNAMEDOBJECT)
                flags.push_back("FD_ISCOBJECT");
            if (field.classtype == ClassType::COWNEDOBJECT)
                flags.push_back("FD_ISCOBJECT | FD_ISCOWNEDOBJECT");

            if (field.feditable || (classInfo.generate_setters_in_descriptor && field.fisprimitivetype && field.editNotDisabled))
                flags.push_back("FD_ISEDITABLE");
            std::string flagss;
            if (flags.empty())
                flagss = "0";
            else {
                flagss = flags[0];
                for (size_t i = 1; i < flags.size(); i++)
                    flagss = flagss + " | " + flags[i];
            }

            CC << "        " << flagss << ",\n";
        }
        CC << "    };\n";
        CC << "    return (field>=0 && field<" << fieldcount << ") ? fieldTypeFlags[field] : 0;\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldName()
    CC << "const char *" << classInfo.msgdescclass << "::getFieldName(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldName(field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return nullptr;\n";
    }
    else {
        CC << "    static const char *fieldNames[] = {\n";
        for (const auto& field : classInfo.fieldlist) {
            CC << "        \"" << field.fname << "\",\n";
        }
        CC << "    };\n";
        CC << "    return (field>=0 && field<" << fieldcount << ") ? fieldNames[field] : nullptr;\n";
    }
    CC << "}\n";
    CC << "\n";

    // findField()
    CC << "int " << classInfo.msgdescclass << "::findField(const char *fieldName) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    if (fieldcount > 0) {
        CC << "    int base = basedesc ? basedesc->getFieldCount() : 0;\n";
        for (size_t i = 0; i < classInfo.fieldlist.size(); ++i) {
            const FieldInfo& field = classInfo.fieldlist[i];
            char c = field.fname[0];
            CC << "    if (fieldName[0]=='" << c << "' && strcmp(fieldName, \""<<field.fname<<"\")==0) return base+" << i << ";\n";
        }
    }
    CC << "    return basedesc ? basedesc->findField(fieldName) : -1;\n";
    CC << "}\n";
    CC << "\n";

    // getFieldTypeString()
    CC << "const char *" << classInfo.msgdescclass << "::getFieldTypeString(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldTypeString(field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return nullptr;\n";
    }
    else {
        CC << "    static const char *fieldTypeStrings[] = {\n";
        for (const auto& field : classInfo.fieldlist) {
            CC << "        \"" << field.ftype << "\",\n";  // note: NOT $fieldtypeqname! that's getFieldStructName()
        }
        CC << "    };\n";
        CC << "    return (field>=0 && field<" << fieldcount << ") ? fieldTypeStrings[field] : nullptr;\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldPropertyNames()
    CC << "const char **" << classInfo.msgdescclass << "::getFieldPropertyNames(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldPropertyNames(field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; ++i) {
        const Properties& ref = classInfo.fieldlist[i].fprops;
        if (!ref.empty()) {
            CC << "        case " << i << ": {\n";
            CC << "            static const char *names[] = { ";
            for (const auto& it : ref) {
                CC << opp_quotestr(it.first) << ", ";
            }
            CC << " nullptr };\n";
            CC << "            return names;\n";
            CC << "        }\n";
        }
    }
    CC << "        default: return nullptr;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldProperty()
    CC << "const char *" << classInfo.msgdescclass << "::getFieldProperty(int field, const char *propertyname) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldProperty(field, propertyname);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    switch (field) {\n";

    for (size_t i = 0; i < fieldcount; ++i) {
        const FieldInfo& field = classInfo.fieldlist[i];
        const Properties& ref = field.fprops;
        if (!ref.empty()) {
            CC << "        case " << i << ":\n";
            for (const auto& it : ref) {
                std::string prop = opp_quotestr(it.second);
                CC << "            if (!strcmp(propertyname,\"" << it.first << "\")) return " << prop << ";\n";
            }
            CC << "            return nullptr;\n";
        }
    }

    CC << "        default: return nullptr;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldArraySize()
    CC << "int " << classInfo.msgdescclass << "::getFieldArraySize(void *object, int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldArraySize(object, field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << classInfo.msgclass << " *pp = (" << classInfo.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const FieldInfo& field = classInfo.fieldlist[i];
        if (field.fisarray) {
            CC << "        case " << i << ": ";
            if (!field.farraysize.empty()) {
                CC << "return " << field.farraysize << ";\n";
            }
            else if (classInfo.classtype == ClassType::STRUCT) {
                CC << "return pp->" << field.varsize << ";\n";
            }
            else {
                CC << "return " << makeFuncall("pp", field.getsize) << ";\n";
            }
        }
    }
    CC << "        default: return 0;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldDynamicTypeString()
    CC << "const char *" << classInfo.msgdescclass << "::getFieldDynamicTypeString(void *object, int field, int i) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldDynamicTypeString(object,field,i);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << classInfo.msgclass << " *pp = (" << classInfo.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const FieldInfo& field = classInfo.fieldlist[i];
        if (!field.fisprimitivetype && field.fispointer && field.classtype == ClassType::NONCOBJECT) {
            CC << "        case " << i << ": ";
            CC << "{ const " << field.ftype << " *value = " << makeFuncall("pp", field.getter, field.fisarray) << "; return omnetpp::opp_typename(typeid(*value)); }\n";
        }
    }
    CC << "        default: return nullptr;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldValueAsString()
    CC << "std::string " << classInfo.msgdescclass << "::getFieldValueAsString(void *object, int field, int i) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldValueAsString(object,field,i);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << classInfo.msgclass << " *pp = (" << classInfo.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const FieldInfo& field = classInfo.fieldlist[i];
        if (field.fisprimitivetype || (!field.fisprimitivetype && !field.tostring.empty())) {
            CC << "        case " << i << ": ";
            if (classInfo.classtype == ClassType::STRUCT) {
                if (field.fisarray) {
                    std::string arraySize = !field.farraysize.empty() ? field.farraysize : (str("pp->")+field.varsize);
                    CC << "if (i>=" << arraySize << ") return \"\";\n                ";
                }
                CC << "return " << makeFuncall(str("pp->") + field.var + (field.fisarray ? "[i]" : ""), field.tostring) << ";\n";
            }
            else {
                CC << "return " << makeFuncall(makeFuncall("pp", field.getter, field.fisarray), field.tostring) << ";\n";
            }
        }
        else if (!field.fisprimitivetype) {
            CC << "        case " << i << ": ";
            if (classInfo.classtype == ClassType::STRUCT) {
                CC << "{std::stringstream out; out << pp->" << field.var << (field.fisarray ? "[i]" : "") << "; return out.str();}\n";
            }
            else {
                CC << "{std::stringstream out; out << " << makeFuncall("pp", field.getter, field.fisarray) << "; return out.str();}\n";
            }
        }
        else {
            throw NEDException("Internal error");
        }
    }
    CC << "        default: return \"\";\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // setFieldValueAsString()
    CC << "bool " << classInfo.msgdescclass << "::setFieldValueAsString(void *object, int field, int i, const char *value) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->setFieldValueAsString(object,field,i,value);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << classInfo.msgclass << " *pp = (" << classInfo.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const FieldInfo& field = classInfo.fieldlist[i];
        if (field.feditable || (classInfo.generate_setters_in_descriptor && field.fisprimitivetype && field.editNotDisabled)) {
            if (field.fromstring.empty()) {
                errors->addError(field.nedElement, "Field '%s' is editable, but fromstring() function is unspecified", field.fname.c_str());
                continue;
            }
            std::string fromstringCall = makeFuncall("value", field.fromstring);
            CC << "        case " << i << ": ";
            if (classInfo.classtype == ClassType::STRUCT) {
                if (field.fisarray) {
                    std::string arraySize = !field.farraysize.empty() ? field.farraysize : (str("pp->")+field.varsize);
                    CC << "if (i>=" << arraySize << ") return false;\n                ";
                }
                CC << "pp->" << field.var << (field.fisarray ? "[i]" : "") << " = " << fromstringCall << "; return true;\n";
            }
            else {
                CC << makeFuncall("pp", field.setter, field.fisarray, fromstringCall) << "; return true;\n";
            }
        }
    }
    CC << "        default: return false;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldStructName()
    CC << "const char *" << classInfo.msgdescclass << "::getFieldStructName(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldStructName(field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    if (fieldcount == 0) {
        CC << "    return nullptr;\n";
    }
    else {
        CC << "    switch (field) {\n";
        for (size_t i = 0; i < fieldcount; i++) {
            const FieldInfo& field = classInfo.fieldlist[i];
            // TODO: @opaque and @byvalue should rather be the attribute of the field's type, not the field itself
            if (!field.fisprimitivetype && !field.fopaque && !field.byvalue) {
                CC << "        case " << i << ": return omnetpp::opp_typename(typeid(" << field.ftype << "));\n";
            }
        }
        CC << "        default: return nullptr;\n";
        CC << "    };\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldStructValuePointer()
    CC << "void *" << classInfo.msgdescclass << "::getFieldStructValuePointer(void *object, int field, int i) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldStructValuePointer(object, field, i);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << classInfo.msgclass << " *pp = (" << classInfo.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const FieldInfo& field = classInfo.fieldlist[i];
        // TODO: @opaque and @byvalue should rather be the attribute of the field's type, not the field itself
        if (!field.fisprimitivetype && !field.fopaque && !field.byvalue) {
            std::string cast;
            std::string value;
            if (classInfo.classtype == ClassType::STRUCT) {
                value = str("pp->") + field.var + (field.fisarray ? "[i]" : "");
            }
            else {
                value = makeFuncall("pp", field.getter, field.fisarray);
            }
            cast = "(void *)";
            if (field.classtype == ClassType::COBJECT || field.classtype == ClassType::CNAMEDOBJECT || field.classtype == ClassType::COWNEDOBJECT)
                cast = cast + "static_cast<const omnetpp::cObject *>";
            if (field.fispointer) {
                CC << "        case " << i << ": return " << cast << "(" << value << "); break;\n";
            }
            else {
                CC << "        case " << i << ": return " << cast << "(&" << value << "); break;\n";
            }
        }
    }
    CC << "        default: return nullptr;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";
}

MsgCppGenerator::EnumInfo MsgCppGenerator::extractEnumInfo(EnumElement *node)
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

void MsgCppGenerator::generateEnum(const EnumInfo& enumInfo)
{
    if (isQualified(enumInfo.enumName)) {
        errors->addError(enumInfo.nedElement, "type name may not be qualified: '%s'\n", enumInfo.enumName.c_str());
        return;
    }

    H << "/**\n";
    H << " * Enum generated from <tt>" << SL(enumInfo.nedElement->getSourceLocation()) << "</tt> by " PROGRAM ".\n";
    H << generatePreComment(enumInfo.nedElement);
    H << " */\n";

    H << "enum " << enumInfo.enumName <<" {\n";
    for (EnumInfo::FieldList::const_iterator it = enumInfo.fieldlist.begin(); it != enumInfo.fieldlist.end(); ) {
        H << "    " << it->name << " = " << it->value;
        if (++it != enumInfo.fieldlist.end())
            H << ",";
        H << "\n";
    }
    H << "};\n\n";

    // TODO generate a Register_Enum() macro call instead
    // TODO why generating the enum and registering its descriptor (cEnum) aren't separate, independently usable operations as with classes?
    CC << "EXECUTE_ON_STARTUP(\n";
    CC << "    omnetpp::cEnum *e = omnetpp::cEnum::find(\"" << enumInfo.enumQName << "\");\n";
    CC << "    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum(\"" << enumInfo.enumQName << "\"));\n";
    // enum inheritance: we should add fields from base enum as well, but that could only be done when importing is in place
    for (const auto& enumItem : enumInfo.fieldlist) {
        CC << "    e->insert(" << enumItem.name << ", \"" << enumItem.name << "\");\n";
    }
    CC << ")\n\n";
}

std::string MsgCppGenerator::prefixWithNamespace(const std::string& name)
{
    return !namespaceName.empty() ? namespaceName + "::" + name : name;  // prefer name from local namespace
}

std::string MsgCppGenerator::makeFuncall(const std::string& var, const std::string& funcTemplate, bool withIndex, const std::string& value)
{
    if (funcTemplate[0] == '.' || funcTemplate[0] == '-') {
        // ".foo()" becomes "var.foo()", "->foo()" becomes "var->foo()"
        return var + funcTemplate;
    }
    else if (funcTemplate.find('$') != std::string::npos) {
        // "tostring($)" becomes "tostring(var)", "getchild($,i)" becomes "getchild(var,i)"
        return opp_replacesubstring(funcTemplate, "$", var, true);
    }
    else {
        // "foo" is a shorthand for "var->foo()" or "var->foo(i)", depending on flag
        return var + "->" + funcTemplate + "(" + (withIndex ? "i" : "") + ((withIndex && value!="") ? "," : "") + value + ")";
    }
}


bool MsgCppGenerator::getPropertyAsBool(const Properties& p, const char *name, bool defval)
{
    Properties::const_iterator it = p.find(name);
    if (it == p.end())
        return defval;
    if (it->second == "false")
        return false;
    return true;
}

std::string MsgCppGenerator::getProperty(const Properties& p, const char *name, const std::string& defval)
{
    Properties::const_iterator it = p.find(name);
    if (it == p.end())
        return defval;
    return it->second;
}

void MsgCppGenerator::generateTemplates()
{
    CC << "// forward\n";
    CC << "template<typename T, typename A>\n";
    CC << "std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);\n\n";

    CC << "// Template rule to generate operator<< for shared_ptr<T>\n";
    CC << "template<typename T>\n";
    CC << "inline std::ostream& operator<<(std::ostream& out,const std::shared_ptr<T>& t) { return out << t.get(); }\n\n";

    CC << "// Template rule which fires if a struct or class doesn't have operator<<\n";
    CC << "template<typename T>\n";
    CC << "inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}\n\n";

    CC << "// operator<< for std::vector<T>\n";
    CC << "template<typename T, typename A>\n";
    CC << "inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)\n";
    CC << "{\n";
    CC << "    out.put('{');\n";
    CC << "    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)\n";
    CC << "    {\n";
    CC << "        if (it != vec.begin()) {\n";
    CC << "            out.put(','); out.put(' ');\n";
    CC << "        }\n";
    CC << "        out << *it;\n";
    CC << "    }\n";
    CC << "    out.put('}');\n";
    CC << "\n";
    CC << "    char buf[32];\n";
    CC << "    sprintf(buf, \" (size=%u)\", (unsigned int)vec.size());\n";
    CC << "    out.write(buf, strlen(buf));\n";
    CC << "    return out;\n";
    CC << "}\n";
    CC << "\n";
}

void MsgCppGenerator::generateNamespaceBegin(NEDElement *element)
{
    if (namespaceName.empty()) {
        errors->addError(element, "namespace name is empty\n");
    }

    // split namespacename into namespacenamevector
    for (size_t pos = 0; ; ) {
        size_t colonPos = namespaceName.find("::", pos);
        if (colonPos != namespaceName.npos) {
            namespaceNameVector.push_back(namespaceName.substr(pos, colonPos-pos));
            pos = colonPos + 2;
        }
        else {
            namespaceNameVector.push_back(namespaceName.substr(pos));
            break;
        }
    }

    // output namespace-begin lines; also check for reserved words
    H << std::endl;
    for (StringVector::const_iterator it = namespaceNameVector.begin(); it != namespaceNameVector.end(); ++it) {
        if (RESERVED_WORDS.find(*it) != RESERVED_WORDS.end())
            errors->addError(element, "namespace name '%s' is a reserved word\n", (*it).c_str());
        H << "namespace " << *it << " {\n";
        CC << "namespace " << *it << " {\n";
    }
    H << std::endl;
    CC << std::endl;

    generateTemplates();
}

void MsgCppGenerator::generateNamespaceEnd()
{
    for (StringVector::const_reverse_iterator it = namespaceNameVector.rbegin(); it != namespaceNameVector.rend(); ++it) {
        H << "} // namespace " << *it << std::endl;
        CC << "} // namespace " << *it << std::endl;
    }
    H << std::endl;
    CC << std::endl;
    namespaceNameVector.clear();
}

}  // namespace nedxml
}  // namespace omnetpp
