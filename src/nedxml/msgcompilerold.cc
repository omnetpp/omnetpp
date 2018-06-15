//==========================================================================
//  MSGCOMPILEROLD.CC - part of
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

#include "msgcompilerold.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "common/stringtokenizer.h"
#include "common/stringutil.h"
#include "omnetpp/platdep/platmisc.h"  // unlink()
#include "msggenerator.h"
#include "exception.h"
#include "nedutil.h"

#include "omnetpp/simkerneldefs.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

using std::ostream;

#define H     (*hOutp)
#define CC    (*ccOutp)

// compatibility mode makes output more similar to opp_msgc's
// #define MSGC_COMPATIBILE

#ifdef MSGC_COMPATIBILE
#define PROGRAM    "opp_msgc"
#else
#define PROGRAM    "nedtool"
#endif

#ifdef MSGC_COMPATIBILE
// removes ":<linenumber>" from source location string
inline std::string SL(const std::string& s)
{
    return s.substr(0, s.find_last_of(':'));
}
#else
#define SL(x)    (x)
#endif

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

MsgCompilerOld::TypeDesc MsgCompilerOld::_PRIMITIVE_TYPES[] =
{ //     nedTypeName        cppTypeName        fromstring            tostring               emptyValue
        {"bool",            "bool",            "string2bool($)",     "bool2string($)",      "false"},
        {"float",           "float",           "string2double($)",   "double2string($)",    "0"},
        {"double",          "double",          "string2double($)",   "double2string($)",    "0"},
        {"simtime_t", "::omnetpp::simtime_t",  "string2simtime($)",  "simtime2string($)",   "0"},
        {"string",    "::omnetpp::opp_string", "($)",                "oppstring2string($)", ""},
        {"char",            "char",            "string2long($)",     "long2string($)",      "0"},
        {"short",           "short",           "string2long($)",     "long2string($)",      "0"},
        {"int",             "int",             "string2long($)",     "long2string($)",      "0"},
        {"long",            "long",            "string2long($)",     "long2string($)",      "0"},
        {"int8",            "int8_t",          "string2long($)",     "long2string($)",      "0"},
        {"int8_t",          "int8_t",          "string2long($)",     "long2string($)",      "0"},
        {"int16",           "int16_t",         "string2long($)",     "long2string($)",      "0"},
        {"int16_t",         "int16_t",         "string2long($)",     "long2string($)",      "0"},
        {"int32",           "int32_t",         "string2long($)",     "long2string($)",      "0"},
        {"int32_t",         "int32_t",         "string2long($)",     "long2string($)",      "0"},
        {"unsigned char",   "unsigned char",   "string2ulong($)",    "ulong2string($)",     "0"},
        {"unsigned short",  "unsigned short",  "string2ulong($)",    "ulong2string($)",     "0"},
        {"unsigned int",    "unsigned int",    "string2ulong($)",    "ulong2string($)",     "0"},
        {"unsigned long",   "unsigned long",   "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint8",           "uint8_t",         "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint8_t",         "uint8_t",         "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint16",          "uint16_t",        "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint16_t",        "uint16_t",        "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint32",          "uint32_t",        "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint32_t",        "uint32_t",        "string2ulong($)",    "ulong2string($)",     "0"},
        {"int64",           "int64_t",         "string2int64($)",    "int642string($)",     "0"},
        {"int64_t",         "int64_t",         "string2int64($)",    "int642string($)",     "0"},
        {"uint64",          "uint64_t",        "string2uint64($)",   "uint642string($)",    "0"},
        {"uint64_t",        "uint64_t",        "string2uint64($)",   "uint642string($)",    "0"},
        {nullptr,nullptr,nullptr,nullptr,nullptr}
};


const char *MsgCompilerOld::_RESERVED_WORDS[] =
{
        "namespace", "cplusplus", "struct", "message", "packet", "class", "noncobject",
        "enum", "extends", "abstract", "readonly", "properties", "fields", "unsigned",
        "true", "false", "for", "while", "if", "else", "do", "enum", "class", "struct",
        "typedef", "public", "private", "protected", "auto", "register", "sizeof", "void",
        "new", "delete", "explicit", "static", "extern", "return", "try", "catch",
        nullptr
};

void MsgCompilerOld::initDescriptors()
{
    for (int i = 0; _PRIMITIVE_TYPES[i].msgTypeName; ++i) {
        PRIMITIVE_TYPES[_PRIMITIVE_TYPES[i].msgTypeName] = _PRIMITIVE_TYPES[i];
        RESERVED_WORDS.insert(_PRIMITIVE_TYPES[i].msgTypeName);
    }
    for (int i = 0; _RESERVED_WORDS[i]; ++i) {
        RESERVED_WORDS.insert(_RESERVED_WORDS[i]);
    }
}

MsgCompilerOld::MsgCompilerOld(ErrorStore *e, const MsgCompilerOptionsOld& options)
{
    initDescriptors();

    opts = options;

    hOutp = ccOutp = nullptr;
    errors = e;

    // pre-register some OMNeT++ classes so that one doesn't need to announce them
    //
    // @classes contains fully qualified names (ie with namespace); keys to the other hashes are fully qualified as well
    //
    // note: $classtype values:
    //  'cownedobject' ==> subclasses from cOwnedObject
    //  'cnamedobject' ==> subclasses from cNamedObject but NOT from cOwnedObject
    //  'cobject'      ==> subclasses from cObject but NOT from cNamedObject
    //  'foreign'      ==> non-cObject class (classes announced as "class noncobject" or "extends void")
    //  'struct'       ==> struct (no member functions)
    //
    classType["omnetpp::cObject"] = COBJECT;
    classType["omnetpp::cNamedObject"] = CNAMEDOBJECT;
    classType["omnetpp::cOwnedObject"] = COWNEDOBJECT;
    classType["omnetpp::cMessage"] = COWNEDOBJECT;
    classType["omnetpp::cPacket"] = COWNEDOBJECT;
}

MsgCompilerOld::~MsgCompilerOld()
{
}

void MsgCompilerOld::generate(MsgFileElement *fileElement, const char *hFile, const char *ccFile)
{
    hFilename = hFile;
    ccFilename = ccFile;
    std::ofstream hStream(hFile);
    std::ofstream ccStream(ccFile);
    if (hStream.fail())
        throw opp_runtime_error("Cannot open '%s' for write", hFile);
    if (ccStream.fail())
        throw opp_runtime_error("Cannot open '%s' for write", ccFile);
    hOutp = &hStream;
    ccOutp = &ccStream;
    generate(fileElement);
    hStream.close();
    ccStream.close();

    if (errors->containsError()) {
        // remove output files when error occured:
        unlink(hFile);
        unlink(ccFile);
    }
}

void MsgCompilerOld::extractClassDecl(ASTNode *child)
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
        type = STRUCT;
    }
    else if (type0 == "message-decl" || type0 == "packet-decl") {
        type = COWNEDOBJECT;
    }
    else if (type0 == "class-decl") {
        if (!isCobject) {
            type = NONCOBJECT;
            if (!baseclass.empty()) {
                errors->addError(child, "'%s': the keywords noncobject and extends cannot be used together", name.c_str());
            }
        }
        else if (baseclass == "") {
            type = COWNEDOBJECT;
        }
        else if (baseclass == "void") {
            type = NONCOBJECT;
        }
        else {
            StringVector found = lookupExistingClassName(baseclass);
            if (found.size() == 1) {
                type = getClassType(found[0]);
            }
            else if (found.empty()) {
                errors->addError(child, "'%s': unknown ancestor class '%s'\n", myclass.c_str(), baseclass.c_str());
                type = COBJECT;
            }
            else {
                errors->addWarning(child, "'%s': ambiguous ancestor class '%s'; possibilities: '%s'\n", myclass.c_str(), baseclass.c_str(), join(found, "','").c_str());
                type = getClassType(found[0]);
            }
        }
    }
    else {
        errors->addError(child, "invalid type '%s' in class '%s'\n", type0.c_str(), myclass.c_str());
        return;
    }

    addClassType(classqname, type, child);
}

static const char *PARSIMPACK_BOILERPLATE =
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

void MsgCompilerOld::generate(MsgFileElement *fileElement)
{
    // make header guard using the file name
    std::string hfilenamewithoutdir = hFilename;
    size_t pos = hfilenamewithoutdir.find_last_of('/');
    if (pos != hfilenamewithoutdir.npos)
        hfilenamewithoutdir = hfilenamewithoutdir.substr(pos+1);

    std::string headerGuard = hfilenamewithoutdir;

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
    CC << "#include \"" << hfilenamewithoutdir << "\"\n\n";

    CC << PARSIMPACK_BOILERPLATE;

    if (!firstNS) {
        H << "\n\n";
        CC << "\n";
        generateTemplates();
    }

    /*
       <!ELEMENT msg-file (comment*, (namespace|property-decl|property|cplusplus|
                           struct-decl|class-decl|message-decl|packet-decl|enum-decl|
                           struct|class|message|packet|enum)*)>
     */
    for (ASTNode *child = fileElement->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case MSG_NAMESPACE:
                // open namespace(s)
                if (!namespaceName.empty()) {
                    generateNamespaceEnd();
                }
                namespaceName = ptr2str(child->getAttribute("name"));
                generateNamespaceBegin(child);
                break;

            case MSG_CPLUSPLUS: {
                // print C++ block
                std::string target = ptr2str(child->getAttribute("target"));
                if (!target.empty())
                    errors->addError(child, "Targets for cplusplus blocks are not supported in legacy (4.x) mode, try invoking the message compiler with --msg6");
                std::string body = ptr2str(child->getAttribute("body"));
                body = body.substr(body.find_first_not_of("\r\n"));
                size_t pos = body.find_last_not_of("\r\n");
                if (pos != body.npos)
                    body = body.substr(0, pos+1);
                H << "// cplusplus {{\n"
                  << body
                  << "\n// }}\n\n"
                ;
                break;
            }

            case MSG_STRUCT_DECL:
            case MSG_CLASS_DECL:
            case MSG_MESSAGE_DECL:
            case MSG_PACKET_DECL:
                extractClassDecl(child);
                break;

            case MSG_ENUM_DECL: {
                // forward declaration -- add to table
                std::string name = ptr2str(child->getAttribute("name"));
                if (RESERVED_WORDS.find(name) == RESERVED_WORDS.end())
                    enumType[name] = canonicalizeQName(namespaceName, name);
                else {
                    errors->addError(child, "Namespace name is reserved word: '%s'", name.c_str());
                }
                break;
            }

            case MSG_ENUM: {
                EnumInfo info = extractEnumInfo(check_and_cast<EnumElement *>(child));
                enumType[info.enumName] = info.enumQName;
                generateEnum(info);
                break;
            }

            case MSG_STRUCT: {
                ClassInfo classInfo = extractClassInfo(child);
                prepareForCodeGeneration(classInfo);
                addClassType(classInfo.msgname, classInfo.classtype, child);
                if (classInfo.generate_class)
                    generateStruct(classInfo);
                if (classInfo.generate_descriptor)
                    generateDescriptorClass(classInfo);
                break;
            }

            case MSG_CLASS:
            case MSG_MESSAGE:
            case MSG_PACKET: {
                ClassInfo classInfo = extractClassInfo(child);
                prepareForCodeGeneration(classInfo);
                addClassType(classInfo.msgqname, classInfo.classtype, child);
                if (classInfo.generate_class)
                    generateClass(classInfo);
                if (classInfo.generate_descriptor)
                    generateDescriptorClass(classInfo);
                break;
            }
            case MSG_IMPORT:
                errors->addError(child, "Imports are not supported in legacy (4.x) mode, try invoking the message compiler with --msg6");
                break;
        }
    }

    generateNamespaceEnd();

#ifdef MSGC_COMPATIBILE
    H << "#endif // " << headerGuard << "\n";
#else
    H << "#endif // ifndef " << headerGuard << "\n\n";
#endif
}

MsgCompilerOld::Properties MsgCompilerOld::extractPropertiesOf(ASTNode *node)
{
    Properties props;

    for (PropertyElement *child = static_cast<PropertyElement *>(node->getFirstChildWithTag(MSG_PROPERTY)); child; child = child->getNextPropertySibling()) {
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

MsgCompilerOld::ClassInfo MsgCompilerOld::extractClassInfo(ASTNode *node)
{
    ClassInfo classInfo;
    classInfo.nedElement = node;

    classInfo.keyword = node->getTagName();
    classInfo.msgname = ptr2str(node->getAttribute("name"));
    classInfo.msgbase = ptr2str(node->getAttribute("extends-name"));
    classInfo.props = extractPropertiesOf(node);

    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
            case MSG_FIELD: {
                ClassInfo::FieldInfo f;
                f.astNode = child;
                f.fname = ptr2str(child->getAttribute("name"));
                f.datatype = ptr2str(child->getAttribute("data-type"));
                f.ftype = ptr2str(child->getAttribute("data-type"));
                f.fval = ptr2str(child->getAttribute("default-value"));
                f.fisabstract = ptr2str(child->getAttribute("is-abstract")) == "true";
                f.fispointer = ptr2str(child->getAttribute("is-pointer")) == "true";
                f.fisarray = ptr2str(child->getAttribute("is-vector")) == "true";
                f.farraysize = ptr2str(child->getAttribute("vector-size"));

                f.fprops = extractPropertiesOf(child);

                if (f.ftype.empty())
                    classInfo.baseclassFieldlist.push_back(f);
                else
                    classInfo.fieldlist.push_back(f);
                break;
            }

            case MSG_PROPERTY:
                // skip properties here, properties already extracted
                break;

            case MSG_PROPERTY_KEY:
                errors->addError(child, "syntax error: property key '%s' unaccepted here", child->getTagName());
                break;

            case MSG_COMMENT:
                break;

            default:
                errors->addError(child, "unaccepted element: '%s'", child->getTagName());
                break;
        }
    }
    return classInfo;
}

void MsgCompilerOld::prepareFieldForCodeGeneration(ClassInfo& info, ClassInfo::FieldInfo *it)
{
    if (it->fisabstract && !info.gap) {
        errors->addError(it->astNode, "abstract fields need '@customize(true)' property in '%s'\n", info.msgname.c_str());
    }

    // determine field data type
    TypeDescMap::const_iterator tdIt = PRIMITIVE_TYPES.find(it->ftype);
    if (tdIt != PRIMITIVE_TYPES.end()) {
        it->fisprimitivetype = true;
        it->ftypeqname = "";  // unused
        it->classtype = NONCOBJECT;
    }
    else if (it->ftype.empty()) {
        // base class field assignment
        it->classtype = UNKNOWN;
        it->fisprimitivetype = false; // we don't know
    }
    else {
        it->fisprimitivetype = false;

        // $ftypeqname
        StringVector found = lookupExistingClassName(it->ftype);
        if (found.size() == 1) {
            it->ftypeqname = found[0];
        }
        else if (found.empty()) {
            errors->addError(it->astNode, "unknown type '%s' for field '%s' in '%s'\n", it->ftype.c_str(), it->fname.c_str(), info.msgname.c_str());
            it->ftypeqname = "omnetpp::cObject";
        }
        else {
            errors->addError(it->astNode, "unknown type '%s' for field '%s' in '%s'; possibilities: %s\n", it->ftype.c_str(), it->fname.c_str(), info.msgname.c_str(), join(found, ", ").c_str());
            it->ftypeqname = found[0];
        }

        it->classtype = getClassType(it->ftypeqname);

        if (it->ftypeqname != "omnetpp::cObject")
            it->ftypeqname = str("::") + it->ftypeqname; // hmm
    }

    if (info.generate_class) {
        if (it->classtype == COWNEDOBJECT && info.classtype != COWNEDOBJECT) {
            errors->addError(it->astNode, "cannot use cOwnedObject field '%s %s' in struct or non-cOwnedObject class '%s'\n", it->ftype.c_str(), it->fname.c_str(), info.msgname.c_str());
        }
    }

    it->fnopack = getPropertyAsBool(it->fprops, "nopack", false);
    it->feditable = getPropertyAsBool(it->fprops, "editable", false);
    it->editNotDisabled = getPropertyAsBool(it->fprops, "editable", true);
    it->fopaque = getPropertyAsBool(it->fprops, "opaque", false);
    it->tostring = getProperty(it->fprops, "tostring", "");
    it->fromstring = getProperty(it->fprops, "fromstring", "");

    // resolve enum namespace
    it->enumname = getProperty(it->fprops, "enum");
    if (!it->enumname.empty()) {
        StringVector found = lookupExistingEnumName(it->enumname);
        if (found.size() == 1) {
            it->enumqname = found[0];
        }
        else if (found.empty()) {
            errors->addError(it->astNode, "undeclared enum '%s' in field '%s' in '%s'\n", it->enumname.c_str(), it->fname.c_str(), info.msgname.c_str());
            it->enumqname = "";
            CC << "\n\n/*\n Undeclared enum: " << it->enumname << "\n";
            CC << "  Declared enums:\n";
            for (auto & x : enumType)
                CC << "    " << x.first << " : " << x.second << "\n";
            CC << "\n*/\n\n";
        }
        else {
            errors->addWarning(it->astNode, "ambiguous enum '%s' in field '%s' in '%s';  possibilities: %s\n", it->enumname.c_str(), it->fname.c_str(), info.msgname.c_str(), join(found, ", ").c_str());
            it->enumqname = found[0];
        }
        it->fprops["enum"] = it->enumqname;  // need to modify the property in place
        if (it->tostring.empty())
            it->tostring = str("enum2string($, \"") + it->enumqname + "\")";
        if (it->fromstring.empty())
            it->fromstring = str("(") + it->enumqname + ")string2enum($, \"" + it->enumqname + "\")";
    }

    // variable name
    if (info.classtype == STRUCT) {
        it->var = it->fname;
    }
    else {
        it->var = it->fname + info.fieldnamesuffix;
        it->argname = it->fname;
    }

    it->varsize = it->fname + "_arraysize";
    std::string sizetypeprop = getProperty(it->fprops, "sizetype");
    it->fsizetype = !sizetypeprop.empty() ? sizetypeprop : "unsigned int";

    // default method names
    if (info.classtype != STRUCT) {
        std::string capfieldname = it->fname;
        capfieldname[0] = toupper(capfieldname[0]);
        it->setter = str("set") + capfieldname;
        it->alloc = str("set") + capfieldname + "ArraySize";
        if (info.omitgetverb) {
            it->getter = it->fname;
            it->getsize = it->fname + "ArraySize";
        }
        else {
            it->getter = str("get") + capfieldname;
            it->getsize = str("get") + capfieldname + "ArraySize";
        }

        // allow customization of names
        if (getProperty(it->fprops, "setter") != "") {
            it->setter = getProperty(it->fprops, "setter");
        }
        if (getProperty(it->fprops, "getter") != "") {
            it->getter = getProperty(it->fprops, "getter");
        }
        if (getProperty(it->fprops, "sizeSetter") != "") {
            it->alloc = getProperty(it->fprops, "sizeSetter");
        }
        if (getProperty(it->fprops, "sizeGetter") != "") {
            it->getsize = getProperty(it->fprops, "sizeGetter");
        }
    }

    // data type, argument type, conversion to/from string...
    it->maybe_c_str = "";
    if (!it->fisprimitivetype) {
        it->datatype = it->ftype;
        it->argtype = str("const ") + it->ftype + "&";
        it->rettype = it->ftype + "&";
        // it->fval = "" unless (it->fval != "");
    }
    else {
        if (tdIt == PRIMITIVE_TYPES.end())
            throw NedException("Internal error - unknown primitive data type '%s'", it->ftype.c_str());
        // defaults:
        it->datatype = tdIt->second.cppTypeName;
        it->argtype = tdIt->second.cppTypeName;
        it->rettype = tdIt->second.cppTypeName;
        if (it->fval.empty())
            it->fval = tdIt->second.emptyValue;
        if (it->tostring.empty())
            it->tostring = tdIt->second.tostring;
        if (it->fromstring.empty())
            it->fromstring = tdIt->second.fromstring;

        if (it->ftype == "string") {
            it->argtype = "const char *";
            it->rettype = "const char *";
            it->maybe_c_str = ".c_str()";
        }
    }
}

void MsgCompilerOld::prepareForCodeGeneration(ClassInfo& info)
{
    info.msgqname = prefixWithNamespace(info.msgname);

    // determine info.msgbaseqname
    if (info.msgbase != "void") {
        StringVector found = lookupExistingClassName(info.msgbase);
        if (found.size() == 1) {
            info.msgbaseqname = found[0];
        }
        else if (found.empty()) {
            errors->addError(info.nedElement, "'%s': unknown base class '%s', available classes '%s'", info.msgname.c_str(), info.msgbase.c_str(), join(classType, "','").c_str());
            info.msgbaseqname = "omnetpp::cMessage";
        }
        else {
            errors->addError(info.nedElement, "'%s': ambiguous base class '%s'; possibilities: '%s'",
                    info.msgname.c_str(), info.msgbase.c_str(), join(found, "','").c_str());
            info.msgbaseqname = found[0];
        }
    }

    // check base class and determine type of object
    if (info.msgqname == "omnetpp::cObject" || info.msgqname == "omnetpp::cNamedObject" || info.msgqname == "omnetpp::cOwnedObject") {
        info.classtype = getClassType(info.msgqname);  // only for sim_std.msg
    }
    else if (info.msgbase == "") {
        if (info.keyword == "message" || info.keyword == "packet") {
            info.classtype = COWNEDOBJECT;
        }
        else if (info.keyword == "class") {
            info.classtype = COBJECT;  // Note: we never generate non-cObject classes
        }
        else if (info.keyword == "struct") {
            info.classtype = STRUCT;
        }
        else {
            throw NedException("Internal error: Invalid keyword:'%s' at '%s'", info.keyword.c_str(), info.msgclass.c_str());
        }
        // if announced earlier as noncobject, accept that.
        if (isClassDeclared(info.msgqname)) {
            if (getClassType(info.msgqname) == NONCOBJECT && info.classtype == COBJECT) {
                info.classtype = NONCOBJECT;
            }
        }
    }
    else if (info.msgbase == "void") {
        info.classtype = NONCOBJECT;
    }
    else if (info.msgbaseqname != "") {
        info.classtype = getClassType(info.msgbaseqname);
    }
    else {
        errors->addError(info.nedElement, "unknown base class '%s' for '%s'\n", info.msgbase.c_str(), info.msgname.c_str());
        info.classtype = COBJECT;
    }

    // check earlier declarations and register this class
    if (isClassDeclared(info.msgqname) && false) // to do:  add condition
        errors->addError(info.nedElement, "attempt to redefine '%s'\n", info.msgname.c_str());
    addClassType(info.msgqname, info.classtype, info.nedElement);

    //
    // produce all sorts of derived names
    //
    info.generate_class = opts.generateClasses && !getPropertyAsBool(info.props, "existingClass", false);
    info.generate_descriptor = opts.generateDescriptors && getPropertyAsBool(info.props, "descriptor", true);
    info.generate_setters_in_descriptor = opts.generateSettersInDescriptors && (getProperty(info.props, "descriptor") != "readonly");

    if (getPropertyAsBool(info.props, "customize", false)) {
        info.gap = 1;
        info.msgclass = info.msgname + "_Base";
        info.realmsgclass = info.msgname;
        info.msgdescclass = makeIdentifier(info.realmsgclass) + "Descriptor";
    }
    else {
        info.gap = 0;
        info.msgclass = info.msgname;
        info.realmsgclass = info.msgname;
        info.msgdescclass = makeIdentifier(info.msgclass) + "Descriptor";
    }
    if (info.msgbase == "") {
        if (info.msgqname == "omnetpp::cObject") {
            info.msgbaseclass = "";
        }
        else if (info.keyword == "message") {
            info.msgbaseclass = "omnetpp::cMessage";
        }
        else if (info.keyword == "packet") {
            info.msgbaseclass = "omnetpp::cPacket";
        }
        else if (info.keyword == "class") {
            info.msgbaseclass = "omnetpp::cObject";  // note: all classes we generate subclass from cObject!
        }
        else if (info.keyword == "struct") {
            info.msgbaseclass = "";
        }
        else {
            throw NedException("Internal error");
        }
    }
    else if (info.msgbase == "void") {
        info.msgbaseclass = "";
    }
    else {
        info.msgbaseclass = info.msgbaseqname;
    }

    info.omitgetverb = getPropertyAsBool(info.props, "omitGetVerb", false);
    info.fieldnamesuffix = getProperty(info.props, "fieldNameSuffix", "");
    if (info.omitgetverb && info.fieldnamesuffix.empty()) {
        errors->addWarning(info.nedElement, "@omitGetVerb(true) and (implicit) @fieldNameSuffix(\"\") collide: "
                                            "adding '_var' suffix to data members to prevent name conflict between them and getter methods\n");
        info.fieldnamesuffix = "_var";
    }

    std::string s = getProperty(info.props, "implements");
    if (!s.empty()) {
        info.implements = StringTokenizer(s.c_str(), ",").asVector();
    }

    for (ClassInfo::Fieldlist::iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it) {
        prepareFieldForCodeGeneration(info, &(*it));
    }
    for (ClassInfo::Fieldlist::iterator it = info.baseclassFieldlist.begin(); it != info.baseclassFieldlist.end(); ++it) {
        prepareFieldForCodeGeneration(info, &(*it));
    }
}

std::string MsgCompilerOld::generatePreComment(ASTNode *nedElement)
{
    std::ostringstream s;
    MsgGenerator().generate(s, nedElement, "");
    std::string str = s.str();

#ifdef MSGC_COMPATIBILE
    // remove comments
    size_t p1;
    while ((p1 = str.find("//")) != str.npos) {
        size_t p2 = str.find("\n", p1);
        std::string s2 = str.substr(0, p1) + str.substr(p2);
        str = s2;
    }
#endif

    std::ostringstream o;
    o << " * <pre>\n";
    o << opp_indentlines(opp_trim(opp_replacesubstring(opp_replacesubstring(str, "*/", "  ", true), "@", "\\@", true)), " * ");
    o << "\n";
    o << " * </pre>\n";
    return o.str();
}

void MsgCompilerOld::generateClass(const ClassInfo& info)
{
    if (isQualified(info.msgclass)) {
        errors->addError(info.nedElement, "type name may only be qualified when generating descriptor for an existing class: '%s'\n", info.msgclass.c_str());
        return;
    }

    H << "/**\n";
    H << " * Class generated from <tt>" << SL(info.nedElement->getSourceLocation()) << "</tt> by " PROGRAM ".\n";
    H << generatePreComment(info.nedElement);

    if (info.gap) {
        H << " *\n";
        H << " * " << info.msgclass << " is only useful if it gets subclassed, and " << info.realmsgclass << " is derived from it.\n";
        H << " * The minimum code to be written for " << info.realmsgclass << " is the following:\n";
        H << " *\n";
        H << " * <pre>\n";
        H << " * class " << TS(opts.exportDef) << info.realmsgclass << " : public " << info.msgclass << "\n";
        H << " * {\n";
        H << " *   private:\n";
        H << " *     void copy(const " << info.realmsgclass << "& other) { ... }\n\n";
        H << " *   public:\n";
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT) {
            if (info.keyword == "message" || info.keyword == "packet") {
                H << " *     " << info.realmsgclass << "(const char *name=nullptr, short kind=0) : " << info.msgclass << "(name,kind) {}\n";
            }
            else {
                H << " *     " << info.realmsgclass << "(const char *name=nullptr) : " << info.msgclass << "(name) {}\n";
            }
        }
        else {
            H << " *     " << info.realmsgclass << "() : " << info.msgclass << "() {}\n";
        }
        H << " *     " << info.realmsgclass << "(const " << info.realmsgclass << "& other) : " << info.msgclass << "(other) {copy(other);}\n";
        H << " *     " << info.realmsgclass << "& operator=(const " << info.realmsgclass << "& other) {if (this==&other) return *this; " << info.msgclass << "::operator=(other); copy(other); return *this;}\n";
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT || info.classtype == COBJECT) {
            H << " *     virtual " << info.realmsgclass << " *dup() const override {return new " << info.realmsgclass << "(*this);}\n";
        }
        H << " *     // ADD CODE HERE to redefine and implement pure virtual functions from " << info.msgclass << "\n";
        H << " * };\n";
        H << " * </pre>\n";
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT || info.classtype == COBJECT) {
            H << " *\n";
            H << " * The following should go into a .cc (.cpp) file:\n";
            H << " *\n";
            H << " * <pre>\n";
            H << " * Register_Class(" << info.realmsgclass << ")\n";
            H << " * </pre>\n";
        }
    }
    H << " */\n";

    H << "class " << TS(opts.exportDef) << info.msgclass;
    const char *baseclassSepar = " : ";
    if (!info.msgbaseclass.empty()) {
        H << baseclassSepar << "public ::" << info.msgbaseclass;  // make namespace explicit and absolute to disambiguate the way PROGRAM understood it
        baseclassSepar = ", ";
    }

    for (const auto & impl : info.implements) {
        H << baseclassSepar << "public " << impl;
        baseclassSepar = ", ";
    }

    H << "\n{\n";
    H << "  protected:\n";
    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it) {
        if (it->fispointer) {
            errors->addError(it->astNode, "pointers not supported yet in '%s'\n", info.msgname.c_str());
            return;
        }
        if (!it->fisabstract) {
            if (it->fisarray && !it->farraysize.empty()) {
                H << "    " << it->datatype << " " << it->var << "[" << it->farraysize << "];\n";
            }
            else if (it->fisarray && it->farraysize.empty()) {
                H << "    " << it->datatype << " *" << it->var << "; // array ptr\n";
                H << "    " << it->fsizetype << " " << it->varsize << ";\n";
            }
            else {
                H << "    " << it->datatype << " " << it->var << ";\n";
            }
        }
    }
    H << "\n";
    H << "  private:\n";
    H << "    void copy(const " << info.msgclass << "& other);\n\n";
    H << "  protected:\n";
    H << "    // protected and unimplemented operator==(), to prevent accidental usage\n";
    H << "    bool operator==(const " << info.msgclass << "&);\n";
    if (info.gap) {
        H << "    // make constructors protected to avoid instantiation\n";
    }
    else {
        H << "\n";
        H << "  public:\n";
    }
    if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT) {
        if (info.keyword == "message" || info.keyword == "packet") {
            H << "    " << info.msgclass << "(const char *name=nullptr, short kind=0);\n";
        }
        else {
            H << "    " << info.msgclass << "(const char *name=nullptr);\n";
        }
    }
    else {
        H << "    " << info.msgclass << "();\n";
    }
    H << "    " << info.msgclass << "(const " << info.msgclass << "& other);\n";
    if (info.gap) {
        H << "    // make assignment operator protected to force the user override it\n";
        H << "    " << info.msgclass << "& operator=(const " << info.msgclass << "& other);\n";
        H << "\n";
        H << "  public:\n";
    }
    H << "    virtual ~" << info.msgclass << "();\n";
    if (!info.gap) {
        H << "    " << info.msgclass << "& operator=(const " << info.msgclass << "& other);\n";
    }
    if (info.gap) {
        H << "    virtual " << info.msgclass << " *dup() const override {throw omnetpp::cRuntimeError(\"You forgot to manually add a dup() function to class " << info.realmsgclass << "\");}\n";
    }
    else {
        H << "    virtual " << info.msgclass << " *dup() const override {return new " << info.msgclass << "(*this);}\n";
    }
    H << "    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;\n";
    H << "    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;\n";
    H << "\n";
    H << "    // field getter/setter methods\n";
    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it) {
        std::string pure;
        if (it->fisabstract)
            pure = " = 0";

        bool isstruct = !it->fisprimitivetype;
        std::string constifprimitivetype = (!isstruct ? " const" : "");
        if (it->fisarray && !it->farraysize.empty()) {
            H << "    virtual " << it->fsizetype << " " << it->getsize << "() const" << pure << ";\n";
            H << "    virtual " << it->rettype << " " << it->getter << "(" << it->fsizetype << " k)" << constifprimitivetype << "" << pure << ";\n";
            if (isstruct)
                H << "    virtual const " << it->rettype << " " << it->getter << "(" << it->fsizetype << " k) const {return const_cast<" << info.msgclass << "*>(this)->" << it->getter << "(k);}\n";
            H << "    virtual void " << it->setter << "(" << it->fsizetype << " k, " << it->argtype << " " << it->argname << ")" << pure << ";\n";
        }
        else if (it->fisarray && it->farraysize.empty()) {
            H << "    virtual void " << it->alloc << "(" << it->fsizetype << " size)" << pure << ";\n";
            H << "    virtual " << it->fsizetype << " " << it->getsize << "() const" << pure << ";\n";
            H << "    virtual " << it->rettype << " " << it->getter << "(" << it->fsizetype << " k)" << constifprimitivetype << "" << pure << ";\n";
            if (isstruct)
                H << "    virtual const " << it->rettype << " " << it->getter << "(" << it->fsizetype << " k) const {return const_cast<" << info.msgclass << "*>(this)->" << it->getter << "(k);}\n";
            H << "    virtual void " << it->setter << "(" << it->fsizetype << " k, " << it->argtype << " " << it->argname << ")" << pure << ";\n";
        }
        else {
            H << "    virtual " << it->rettype << " " << it->getter << "()" << constifprimitivetype << "" << pure << ";\n";
            if (isstruct)
                H << "    virtual const " << it->rettype << " " << it->getter << "() const {return const_cast<" << info.msgclass << "*>(this)->" << it->getter << "();}\n";
            H << "    virtual void " << it->setter << "(" << it->argtype << " " << it->argname << ")" << pure << ";\n";
        }
    }
    H << "};\n\n";

    if (!info.gap) {
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT || info.classtype == COBJECT) {
            CC << "Register_Class(" << info.msgclass << ")\n\n";
        }

        H << "inline void doParsimPacking(omnetpp::cCommBuffer *b, const " << info.realmsgclass << "& obj) {obj.parsimPack(b);}\n";
        H << "inline void doParsimUnpacking(omnetpp::cCommBuffer *b, " << info.realmsgclass << "& obj) {obj.parsimUnpack(b);}\n\n";
    }

    if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT) {
        if (info.keyword == "message" || info.keyword == "packet") {
            // CAREFUL when assigning values to existing members gets implemented!
            // The msg kind passed to the ctor should take priority!!!
            CC << "" << info.msgclass << "::" << info.msgclass << "(const char *name, short kind) : ::" << info.msgbaseclass << "(name,kind)\n";
        }
        else {
            if (info.msgbaseclass == "") {
                CC << "" << info.msgclass << "::" << info.msgclass << "(const char *name)\n";
            }
            else {
                CC << "" << info.msgclass << "::" << info.msgclass << "(const char *name) : ::" << info.msgbaseclass << "(name)\n";
            }
        }
    }
    else {
        if (info.msgbaseclass == "") {
            CC << "" << info.msgclass << "::" << info.msgclass << "()\n";
        }
        else {
            CC << "" << info.msgclass << "::" << info.msgclass << "() : ::" << info.msgbaseclass << "()\n";
        }
    }
    CC << "{\n";
    // CC << "    (void)static_cast<cObject *>(this); //sanity check\n" if (info.fieldclasstype == COBJECT);
    // CC << "    (void)static_cast<cNamedObject *>(this); //sanity check\n" if (info.fieldclasstype == CNAMEDOBJECT);
    // CC << "    (void)static_cast<cOwnedObject *>(this); //sanity check\n" if (info.fieldclasstype == COWNEDOBJECT);
    for (const auto & it : info.baseclassFieldlist) {
        std::string capfieldname = it.fname;
        capfieldname[0] = toupper(capfieldname[0]);
        std::string setter = "set" + capfieldname;
        CC << "    this->" << setter << "(" << it.fval << ");\n";  // note: we should take setter name from base class desc
    }
    if (!info.baseclassFieldlist.empty() && !info.fieldlist.empty())
        CC << "\n";
    for (const auto & it : info.fieldlist) {
        if (!it.fisabstract) {
            if (it.fisarray && !it.farraysize.empty()) {
                if (it.fisprimitivetype && !it.fval.empty()) {
                    CC << "    for (" << it.fsizetype << " i=0; i<" << it.farraysize << "; i++)\n";
                    CC << "        this->" << it.var << "[i] = " << it.fval << ";\n";
                }
                if (it.classtype == COWNEDOBJECT) {
                    CC << "    for (" << it.fsizetype << " i=0; i<" << it.farraysize << "; i++)\n";
                    CC << "        take(&(this->" << it.var << "[i]));\n";
                }
            }
            else if (it.fisarray && it.farraysize.empty()) {
                CC << "    " << it.varsize << " = 0;\n";
                CC << "    this->" << it.var << " = 0;\n";
            }
            else {
                if (!it.fval.empty()) {
                    CC << "    this->" << it.var << " = " << it.fval << ";\n";
                }
                if (it.classtype == COWNEDOBJECT) {
                    CC << "    take(&(this->" << it.var << "));\n";
                }
            }
        }
    }
    CC << "}\n\n";

    CC << "" << info.msgclass << "::" << info.msgclass << "(const " << info.msgclass << "& other)";
    if (!info.msgbaseclass.empty()) {
        CC << " : ::" << info.msgbaseclass << "(other)";
    }
    CC << "\n{\n";
    for (const auto & it : info.fieldlist) {
        if (!it.fisabstract) {
            if (it.fisarray && !it.farraysize.empty()) {
                if (it.classtype == COWNEDOBJECT) {
                    CC << "    for (" << it.fsizetype << " i=0; i<" << it.farraysize << "; i++)\n";
                    CC << "        take(&(this->" << it.var << "[i]));\n";
                }
            }
            else if (it.fisarray && it.farraysize.empty()) {
                CC << "    " << it.varsize << " = 0;\n";
                CC << "    this->" << it.var << " = 0;\n";
            }
            else if (!it.fisarray && it.classtype == COWNEDOBJECT) {
                CC << "    take(&(this->" << it.var << "));\n";
            }
        }
    }
    CC << "    copy(other);\n";
    CC << "}\n\n";

    CC << "" << info.msgclass << "::~" << info.msgclass << "()\n";
    CC << "{\n";
    for (const auto & it : info.fieldlist) {
        if (!it.fisabstract) {
            if (it.classtype == COWNEDOBJECT) {
                if (!it.fisarray) {
                    CC << "    drop(&(this->" << it.var << "));\n";
                }
                else if (!it.farraysize.empty()) {
                    CC << "    for (" << it.fsizetype << " i=0; i<" << it.farraysize << "; i++)\n";
                    CC << "        drop(&(this->" << it.var << "[i]));\n";
                }
                else {
                    CC << "    for (" << it.fsizetype << " i=0; i<" << it.varsize << "; i++)\n";
                    CC << "        drop(&(this->" << it.var << "[i]));\n";
                }
            }
            if (it.fisarray && it.farraysize.empty()) {
                CC << "    delete [] this->" << it.var << ";\n";
            }
        }
    }
    CC << "}\n\n";

    CC << "" << info.msgclass << "& " << info.msgclass << "::operator=(const " << info.msgclass << "& other)\n";
    CC << "{\n";
    CC << "    if (this==&other) return *this;\n";
    if (info.msgbaseclass != "") {
        CC << "    ::" << info.msgbaseclass << "::operator=(other);\n";
    }
    CC << "    copy(other);\n";
    CC << "    return *this;\n";
    CC << "}\n\n";

    CC << "void " << info.msgclass << "::copy(const " << info.msgclass << "& other)\n";
    CC << "{\n";
    for (const auto & it : info.fieldlist) {
        if (!it.fisabstract) {
            if (it.fisarray && !it.farraysize.empty()) {
                CC << "    for (" << it.fsizetype << " i=0; i<" << it.farraysize << "; i++)\n";
                CC << "        this->" << it.var << "[i] = other." << it.var << "[i];\n";
                if (it.classtype == COWNEDOBJECT) {
                    CC << "    for (" << it.fsizetype << " i=0; i<" << it.farraysize << "; i++)\n";
                    CC << "        this->" << it.var << "[i].setName(other." << it.var << "[i].getName());\n";
                }
            }
            else if (it.fisarray && it.farraysize.empty()) {
                CC << "    delete [] this->" << it.var << ";\n";
                CC << "    this->" << it.var << " = (other." << it.varsize << "==0) ? nullptr : new " << it.datatype << "[other." << it.varsize << "];\n";
                CC << "    " << it.varsize << " = other." << it.varsize << ";\n";
                CC << "    for (" << it.fsizetype << " i=0; i<" << it.varsize << "; i++)\n";
                if (it.classtype == COWNEDOBJECT) {
                    CC << "    {\n";
                    CC << "        take(&(this->" << it.var << "[i]));\n";
                    CC << "        this->" << it.var << "[i] = other." << it.var << "[i];\n";
                    CC << "        this->" << it.var << "[i].setName(other." << it.var << "[i].getName());\n";
                    CC << "    }\n";
                }
                else {
                    CC << "        this->" << it.var << "[i] = other." << it.var << "[i];\n";
                }
            }
            else {
                CC << "    this->" << it.var << " = other." << it.var << ";\n";
                if (!it.fisarray && (it.classtype == COWNEDOBJECT || it.classtype == CNAMEDOBJECT)) {
                    CC << "    this->" << it.var << ".setName(other." << it.var << ".getName());\n";
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
    CC << "void " << info.msgclass << "::parsimPack(omnetpp::cCommBuffer *b) const\n";
    CC << "{\n";
    if (info.msgbaseclass != "") {
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT || info.classtype == COBJECT) {
            if (info.msgbaseclass != "omnetpp::cObject")
                CC << "    ::" << info.msgbaseclass << "::parsimPack(b);\n";
        }
        else {
            CC << "    doParsimPacking(b,(::" << info.msgbaseclass << "&)*this);\n";  // this would do for cOwnedObject too, but the other is nicer
        }
    }
    for (const auto & it : info.fieldlist) {
        if (it.fnopack) {
            // @nopack specified
        }
        else if (it.fisabstract) {
            CC << "    // field " << it.fname << " is abstract -- please do packing in customized class\n";
        }
        else {
            if (it.fisarray && !it.farraysize.empty()) {
                CC << "    doParsimArrayPacking(b,this->" << it.var << "," << it.farraysize << ");\n";
            }
            else if (it.fisarray && it.farraysize.empty()) {
                CC << "    b->pack(" << it.varsize << ");\n";
                CC << "    doParsimArrayPacking(b,this->" << it.var << "," << it.varsize << ");\n";
            }
            else {
                CC << "    doParsimPacking(b,this->" << it.var << ");\n";
            }
        }
    }
    CC << "}\n\n";

    CC << "void " << info.msgclass << "::parsimUnpack(omnetpp::cCommBuffer *b)\n";
    CC << "{\n";
    if (info.msgbaseclass != "") {
        if (info.classtype == COWNEDOBJECT || info.classtype == CNAMEDOBJECT || info.classtype == COBJECT) {
            if (info.msgbaseclass != "omnetpp::cObject")
                CC << "    ::" << info.msgbaseclass << "::parsimUnpack(b);\n";
        }
        else {
            CC << "    doParsimUnpacking(b,(::" << info.msgbaseclass << "&)*this);\n";  // this would do for cOwnedObject too, but the other is nicer
        }
    }
    for (const auto & it : info.fieldlist) {
        if (it.fnopack) {
            // @nopack specified
        }
        else if (it.fisabstract) {
            CC << "    // field " << it.fname << " is abstract -- please do unpacking in customized class\n";
        }
        else {
            if (it.fisarray && !it.farraysize.empty()) {
                CC << "    doParsimArrayUnpacking(b,this->" << it.var << "," << it.farraysize << ");\n";
            }
            else if (it.fisarray && it.farraysize.empty()) {
                CC << "    delete [] this->" << it.var << ";\n";
                CC << "    b->unpack(" << it.varsize << ");\n";
                CC << "    if (" << it.varsize << "==0) {\n";
                CC << "        this->" << it.var << " = 0;\n";
                CC << "    } else {\n";
                CC << "        this->" << it.var << " = new " << it.datatype << "[" << it.varsize << "];\n";
                CC << "        doParsimArrayUnpacking(b,this->" << it.var << "," << it.varsize << ");\n";
                CC << "    }\n";
            }
            else {
                CC << "    doParsimUnpacking(b,this->" << it.var << ");\n";
            }
        }
    }
    CC << "}\n\n";

    for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it) {
        if (!it->fisabstract) {
            bool isstruct = !it->fisprimitivetype;
            const char *constifprimitivetype = (!isstruct ? " const" : "");
            if (it->fisarray && !it->farraysize.empty()) {
                CC << "" << it->fsizetype << " " << info.msgclass << "::" << it->getsize << "() const\n";
                CC << "{\n";
                CC << "    return " << it->farraysize << ";\n";
                CC << "}\n\n";
                CC << "" << it->rettype << " " << info.msgclass << "::" << it->getter << "(" << it->fsizetype << " k)" << constifprimitivetype << "\n";
                CC << "{\n";
                CC << "    if (k>=" << it->farraysize << ") throw omnetpp::cRuntimeError(\"Array of size " << it->farraysize << " indexed by %lu\", (unsigned long)k);\n";
                CC << "    return this->" << it->var << "[k]" << it->maybe_c_str << ";\n";
                CC << "}\n\n";
                CC << "void " << info.msgclass << "::" << it->setter << "(" << it->fsizetype << " k, " << it->argtype << " " << it->argname << ")\n";
                CC << "{\n";
                CC << "    if (k>=" << it->farraysize << ") throw omnetpp::cRuntimeError(\"Array of size " << it->farraysize << " indexed by %lu\", (unsigned long)k);\n";
                CC << "    this->" << it->var << "[k] = " << it->argname << ";\n";
                CC << "}\n\n";
            }
            else if (it->fisarray && it->farraysize.empty()) {
                CC << "void " << info.msgclass << "::" << it->alloc << "(" << it->fsizetype << " size)\n";
                CC << "{\n";
                CC << "    " << it->datatype << " *" << it->var << "2 = (size==0) ? nullptr : new " << it->datatype << "[size];\n";
                CC << "    " << it->fsizetype << " sz = " << it->varsize << " < size ? " << it->varsize << " : size;\n";
                CC << "    for (" << it->fsizetype << " i=0; i<sz; i++)\n";
                CC << "        " << it->var << "2[i] = this->" << it->var << "[i];\n";
                if (it->fisprimitivetype) {
                    CC << "    for (" << it->fsizetype << " i=sz; i<size; i++)\n";
                    CC << "        " << it->var << "2[i] = 0;\n";
                }
                if (it->classtype == COWNEDOBJECT) {
                    CC << "    for (" << it->fsizetype << " i=sz; i<size; i++)\n";
                    CC << "        take(&(" << it->var << "2[i]));\n";
                }
                CC << "    " << it->varsize << " = size;\n";
                CC << "    delete [] this->" << it->var << ";\n";
                CC << "    this->" << it->var << " = " << it->var << "2;\n";
                CC << "}\n\n";
                CC << "" << it->fsizetype << " " << info.msgclass << "::" << it->getsize << "() const\n";
                CC << "{\n";
                CC << "    return " << it->varsize << ";\n";
                CC << "}\n\n";
                CC << "" << it->rettype << " " << info.msgclass << "::" << it->getter << "(" << it->fsizetype << " k)" << constifprimitivetype << "\n";
                CC << "{\n";
                CC << "    if (k>=" << it->varsize << ") throw omnetpp::cRuntimeError(\"Array of size %d indexed by %d\", " << it->varsize << ", k);\n";
                CC << "    return this->" << it->var << "[k]" << it->maybe_c_str << ";\n";
                CC << "}\n\n";
                CC << "void " << info.msgclass << "::" << it->setter << "(" << it->fsizetype << " k, " << it->argtype << " " << it->argname << ")\n";
                CC << "{\n";
                CC << "    if (k>=" << it->varsize << ") throw omnetpp::cRuntimeError(\"Array of size %d indexed by %d\", " << it->varsize << ", k);\n";
                CC << "    this->" << it->var << "[k] = " << it->argname << ";\n";
                CC << "}\n\n";
            }
            else {
                CC << "" << it->rettype << " " << info.msgclass << "::" << it->getter << "()" << constifprimitivetype << "\n";
                CC << "{\n";
                CC << "    return this->" << it->var << "" << it->maybe_c_str << ";\n";
                CC << "}\n\n";
                CC << "void " << info.msgclass << "::" << it->setter << "(" << it->argtype << " " << it->argname << ")\n";
                CC << "{\n";
                CC << "    this->" << it->var << " = " << it->argname << ";\n";
                CC << "}\n\n";
            }
        }
    }
}

#define DD(x)
void MsgCompilerOld::generateStruct(const ClassInfo& info)
{
    if (isQualified(info.msgclass)) {
        errors->addError(info.nedElement, "type name may only be qualified when generating descriptor for an existing class: '%s'\n", info.msgclass.c_str());
        return;
    }

    H << "/**\n";
    H << " * Struct generated from " << SL(info.nedElement->getSourceLocation()) << " by " PROGRAM ".\n";
    H << " */\n";
    if (info.msgbaseclass.empty()) {
        H << "struct " << TS(opts.exportDef) << info.msgclass << "\n";
    }
    else {
        H << "struct " << TS(opts.exportDef) << info.msgclass << " : public ::" << info.msgbaseqname << "\n";
    }
    H << "{\n";
    H << "    " << info.msgclass << "();\n";
    for (const auto & it : info.fieldlist) {
        H << "    " << it.datatype << " " << it.var;
        if (it.fisarray) {
            H << "[" << it.farraysize << "]";
        }
        H << ";\n";
    }
    H << "};\n\n";

    H << "// helpers for local use\n";
    H << "void " << TS(opts.exportDef) << "__doPacking(omnetpp::cCommBuffer *b, const " << info.msgclass << "& a);\n";
    H << "void " << TS(opts.exportDef) << "__doUnpacking(omnetpp::cCommBuffer *b, " << info.msgclass << "& a);\n\n";

    H << "inline void doParsimPacking(omnetpp::cCommBuffer *b, const " << info.realmsgclass << "& obj) { " << "__doPacking(b, obj); }\n";
    H << "inline void doParsimUnpacking(omnetpp::cCommBuffer *b, " << info.realmsgclass << "& obj) { " << "__doUnpacking(b, obj); }\n\n";

    // Constructor:
    CC << "" << info.msgclass << "::" << info.msgclass << "()\n";
    CC << "{\n";
    // override baseclass fields initial value
    for (const auto & it : info.baseclassFieldlist) {
        CC << "    this->" << it.var << " = " << it.fval << ";\n";
    }
    if (!info.baseclassFieldlist.empty() && !info.fieldlist.empty())
        CC << "\n";

    for (const auto & it : info.fieldlist) {
        if (it.fisabstract)
            throw NedException("Abstract fields are not supported in a struct");
        if (it.classtype == COWNEDOBJECT)
            throw NedException("cOwnedObject fields are not supported in a struct");
        if (it.fisarray && it.farraysize.empty())
            throw NedException("Dynamic arrays are not supported in a struct");
        if (it.fisarray && !it.farraysize.empty()) {
            if (it.fisprimitivetype && !it.fval.empty()) {
                CC << "    for (" << it.fsizetype << " i=0; i<" << it.farraysize << "; i++)\n";
                CC << "        this->" << it.var << "[i] = " << it.fval << ";\n";
            }
        }
        else {
            if (!it.fval.empty()) {
                CC << "    this->" << it.var << " = " << it.fval << ";\n";
            }
        }
    }
    CC << "}\n\n";

    // doPacking/doUnpacking go to the global namespace
    CC << "void __doPacking(omnetpp::cCommBuffer *b, const " << info.msgclass << "& a)\n";
    CC << "{\n";
    if (!info.msgbaseclass.empty()) {
        CC << "    doParsimPacking(b,(::" << info.msgbaseclass << "&)a);\n";
    }

    for (const auto & it : info.fieldlist) {
        if (it.fisarray) {
            CC << "    doParsimArrayPacking(b,a." << it.var << "," << it.farraysize << ");\n";
        }
        else {
            CC << "    doParsimPacking(b,a." << it.var << ");\n";
        }
    }
    CC << "}\n\n";

    CC << "void __doUnpacking(omnetpp::cCommBuffer *b, " << info.msgclass << "& a)\n";
    CC << "{\n";
    if (!info.msgbaseclass.empty()) {
        CC << "    doParsimUnpacking(b,(::" << info.msgbaseclass << "&)a);\n";
    }

    for (const auto & it : info.fieldlist) {
        if (it.fisarray) {
            CC << "    doParsimArrayUnpacking(b,a." << it.var << "," << it.farraysize << ");\n";
        }
        else {
            CC << "    doParsimUnpacking(b,a." << it.var << ");\n";
        }
    }
    CC << "}\n\n";
}

void MsgCompilerOld::generateDescriptorClass(const ClassInfo& info)
{
    CC << "class " << info.msgdescclass << " : public omnetpp::cClassDescriptor\n";
    CC << "{\n";
    CC << "  private:\n";
    CC << "    mutable const char **propertynames;\n";
    CC << "  public:\n";
    CC << "    " << info.msgdescclass << "();\n";
    CC << "    virtual ~" << info.msgdescclass << "();\n";
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
    CC << "Register_ClassDescriptor(" << info.msgdescclass << ")\n\n";

    // ctor, dtor
    size_t fieldcount = info.fieldlist.size();
    std::string qualifiedrealmsgclass = prefixWithNamespace(info.realmsgclass);
    CC << "" << info.msgdescclass << "::" << info.msgdescclass << "() : omnetpp::cClassDescriptor(\"" << qualifiedrealmsgclass << "\", \"" << info.msgbaseclass << "\")\n";
    CC << "{\n";
    CC << "    propertynames = nullptr;\n";
    CC << "}\n";
    CC << "\n";

    CC << "" << info.msgdescclass << "::~" << info.msgdescclass << "()\n";
    CC << "{\n";
    CC << "    delete[] propertynames;\n";
    CC << "}\n";
    CC << "\n";

    // doesSupport()
    CC << "bool " << info.msgdescclass << "::doesSupport(omnetpp::cObject *obj) const\n";
    CC << "{\n";
    CC << "    return dynamic_cast<" << info.msgclass << " *>(obj)!=nullptr;\n";
    CC << "}\n";
    CC << "\n";

    // getPropertyNames()
    CC << "const char **" << info.msgdescclass << "::getPropertyNames() const\n";
    CC << "{\n";
    CC << "    if (!propertynames) {\n";
    CC << "        static const char *names[] = { ";
    for (const auto & prop : info.props) {
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
    CC << "const char *" << info.msgdescclass << "::getProperty(const char *propertyname) const\n";
    CC << "{\n";
    for (const auto & prop : info.props) {
        CC << "    if (!strcmp(propertyname,"<< opp_quotestr(prop.first) << ")) return " << opp_quotestr(prop.second) << ";\n";
    }
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    return basedesc ? basedesc->getProperty(propertyname) : nullptr;\n";
    CC << "}\n";
    CC << "\n";

    // getFieldCount()
    CC << "int " << info.msgdescclass << "::getFieldCount() const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    return basedesc ? " << fieldcount << "+basedesc->getFieldCount() : " << fieldcount << ";\n";
    CC << "}\n";
    CC << "\n";

    // getFieldTypeFlags()
    CC << "unsigned int " << info.msgdescclass << "::getFieldTypeFlags(int field) const\n";
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
        for (ClassInfo::Fieldlist::const_iterator it = info.fieldlist.begin(); it != info.fieldlist.end(); ++it) {
            StringVector flags;
            if (it->fisarray)
                flags.push_back("FD_ISARRAY");
            if (!it->fisprimitivetype)
                flags.push_back("FD_ISCOMPOUND");
            if (it->fispointer)
                flags.push_back("FD_ISPOINTER");
            if (it->classtype == COBJECT || it->classtype == CNAMEDOBJECT)
                flags.push_back("FD_ISCOBJECT");
            if (it->classtype == COWNEDOBJECT)
                flags.push_back("FD_ISCOBJECT | FD_ISCOWNEDOBJECT");

            if (it->feditable || (info.generate_setters_in_descriptor && it->fisprimitivetype && it->editNotDisabled))
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
    CC << "const char *" << info.msgdescclass << "::getFieldName(int field) const\n";
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
        for (const auto & it : info.fieldlist) {
            CC << "        \"" << it.fname << "\",\n";
        }
        CC << "    };\n";
        CC << "    return (field>=0 && field<" << fieldcount << ") ? fieldNames[field] : nullptr;\n";
    }
    CC << "}\n";
    CC << "\n";

    // findField()
    CC << "int " << info.msgdescclass << "::findField(const char *fieldName) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    if (fieldcount > 0) {
        CC << "    int base = basedesc ? basedesc->getFieldCount() : 0;\n";
        for (size_t i = 0; i < info.fieldlist.size(); ++i) {
            const ClassInfo::FieldInfo& field = info.fieldlist[i];
            char c = field.fname[0];
            CC << "    if (fieldName[0]=='" << c << "' && strcmp(fieldName, \""<<field.fname<<"\")==0) return base+" << i << ";\n";
        }
    }
    CC << "    return basedesc ? basedesc->findField(fieldName) : -1;\n";
    CC << "}\n";
    CC << "\n";

    // getFieldTypeString()
    CC << "const char *" << info.msgdescclass << "::getFieldTypeString(int field) const\n";
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
        for (const auto & it : info.fieldlist) {
            CC << "        \"" << it.ftype << "\",\n";  // note: NOT $fieldtypeqname! that's getFieldStructName()
        }
        CC << "    };\n";
        CC << "    return (field>=0 && field<" << fieldcount << ") ? fieldTypeStrings[field] : nullptr;\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldPropertyNames()
    CC << "const char **" << info.msgdescclass << "::getFieldPropertyNames(int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldPropertyNames(field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; ++i) {
        const Properties& ref = info.fieldlist[i].fprops;
        if (!ref.empty()) {
            CC << "        case " << i << ": {\n";
            CC << "            static const char *names[] = { ";
            for (const auto & it : ref) {
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
    CC << "const char *" << info.msgdescclass << "::getFieldProperty(int field, const char *propertyname) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldProperty(field, propertyname);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    switch (field) {\n";

    for (size_t i = 0; i < fieldcount; ++i) {
        const ClassInfo::FieldInfo& field = info.fieldlist[i];
        const Properties& ref = field.fprops;
        if (!ref.empty()) {
            CC << "        case " << i << ":\n";
            for (const auto & it : ref) {
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
    CC << "int " << info.msgdescclass << "::getFieldArraySize(void *object, int field) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldArraySize(object, field);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << info.msgclass << " *pp = (" << info.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const ClassInfo::FieldInfo& field = info.fieldlist[i];
        if (field.fisarray) {
            CC << "        case " << i << ": ";
            if (!field.farraysize.empty()) {
                CC << "return " << field.farraysize << ";\n";
            }
            else if (info.classtype == STRUCT) {
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
    CC << "const char *" << info.msgdescclass << "::getFieldDynamicTypeString(void *object, int field, int i) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldDynamicTypeString(object,field,i);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << info.msgclass << " *pp = (" << info.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const ClassInfo::FieldInfo& field = info.fieldlist[i];
        if (!field.fisprimitivetype && field.fispointer && field.classtype == NONCOBJECT) {
            CC << "        case " << i << ": ";
            CC << "{" << field.datatype << " *value = " << makeFuncall("pp", field.getter, field.fisarray) << "; return omnetpp::opp_typename(typeid(*value));}\n";
        }
    }
    CC << "        default: return nullptr;\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // getFieldValueAsString()
    CC << "std::string " << info.msgdescclass << "::getFieldValueAsString(void *object, int field, int i) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldValueAsString(object,field,i);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << info.msgclass << " *pp = (" << info.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const ClassInfo::FieldInfo& field = info.fieldlist[i];
        if (field.fisprimitivetype || (!field.fisprimitivetype && !field.tostring.empty())) {
            CC << "        case " << i << ": ";
            if (info.classtype == STRUCT) {
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
            if (info.classtype == STRUCT) {
                CC << "{std::stringstream out; out << pp->" << field.var << (field.fisarray ? "[i]" : "") << "; return out.str();}\n";
            }
            else {
                CC << "{std::stringstream out; out << " << makeFuncall("pp", field.getter, field.fisarray) << "; return out.str();}\n";
            }
        }
        else {
            throw NedException("Internal error");
        }
    }
    CC << "        default: return \"\";\n";
    CC << "    }\n";
    CC << "}\n";
    CC << "\n";

    // setFieldValueAsString()
    CC << "bool " << info.msgdescclass << "::setFieldValueAsString(void *object, int field, int i, const char *value) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->setFieldValueAsString(object,field,i,value);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << info.msgclass << " *pp = (" << info.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const ClassInfo::FieldInfo& field = info.fieldlist[i];
        if (field.feditable || (info.generate_setters_in_descriptor && field.fisprimitivetype && field.editNotDisabled)) {
            if (field.fromstring.empty()) {
                errors->addError(field.astNode, "Field '%s' is editable, but fromstring() function is unspecified", field.fname.c_str());
                continue;
            }
            std::string fromstringCall = makeFuncall("value", field.fromstring);
            CC << "        case " << i << ": ";
            if (info.classtype == STRUCT) {
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
    CC << "const char *" << info.msgdescclass << "::getFieldStructName(int field) const\n";
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
            const ClassInfo::FieldInfo& field = info.fieldlist[i];
            bool opaque = field.fopaque;
            if (!field.fisprimitivetype && !opaque) {
                CC << "        case " << i << ": return omnetpp::opp_typename(typeid(" << field.ftype << "));\n";
            }
        }
        CC << "        default: return nullptr;\n";
        CC << "    };\n";
    }
    CC << "}\n";
    CC << "\n";

    // getFieldStructValuePointer()
    CC << "void *" << info.msgdescclass << "::getFieldStructValuePointer(void *object, int field, int i) const\n";
    CC << "{\n";
    CC << "    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();\n";
    CC << "    if (basedesc) {\n";
    CC << "        if (field < basedesc->getFieldCount())\n";
    CC << "            return basedesc->getFieldStructValuePointer(object, field, i);\n";
    CC << "        field -= basedesc->getFieldCount();\n";
    CC << "    }\n";
    CC << "    " << info.msgclass << " *pp = (" << info.msgclass << " *)object; (void)pp;\n";
    CC << "    switch (field) {\n";
    for (size_t i = 0; i < fieldcount; i++) {
        const ClassInfo::FieldInfo& field = info.fieldlist[i];
        bool opaque = field.fopaque;
        if (!field.fisprimitivetype && !opaque) {
            std::string cast;
            std::string value;
            if (info.classtype == STRUCT) {
                value = str("pp->") + field.var + (field.fisarray ? "[i]" : "");
            }
            else {
                value = makeFuncall("pp", field.getter, field.fisarray);
            }
            cast = "(void *)";
            if (field.classtype == COBJECT || field.classtype == CNAMEDOBJECT || field.classtype == COWNEDOBJECT)
                cast = cast + "static_cast<omnetpp::cObject *>";
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

MsgCompilerOld::EnumInfo MsgCompilerOld::extractEnumInfo(EnumElement *node)
{
    EnumInfo info;
    info.nedElement = node;
    info.enumName = ptr2str(node->getAttribute("name"));
    info.enumQName = canonicalizeQName(namespaceName, info.enumName);

    // prepare enum items:
    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling()) {
        switch (child->getTagCode()) {
        case MSG_ENUM_FIELD: {
            EnumInfo::EnumItem f;
            f.nedElement = child;
            f.name = ptr2str(child->getAttribute("name"));
            f.value = ptr2str(child->getAttribute("value"));
            info.fieldlist.push_back(f);
            break;
        }

        case MSG_COMMENT:
            break;

        default:
            errors->addError(child, "unaccepted element '%s'", child->getTagName());
        }
    }
    return info;
}

void MsgCompilerOld::generateEnum(const EnumInfo& enumInfo)
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

    CC << "EXECUTE_ON_STARTUP(\n";
    CC << "    omnetpp::cEnum *e = omnetpp::cEnum::find(\"" << enumInfo.enumQName << "\");\n";
    CC << "    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum(\"" << enumInfo.enumQName << "\"));\n";
    // enum inheritance: we should add fields from base enum as well, but that could only be done when importing is in place
    for (const auto & it : enumInfo.fieldlist) {
        CC << "    e->insert(" << it.name << ", \"" << it.name << "\");\n";
    }
    CC << ")\n\n";
}

std::string MsgCompilerOld::prefixWithNamespace(const std::string& name)
{
    return !namespaceName.empty() ? namespaceName + "::" + name : name;  // prefer name from local namespace
}

std::string MsgCompilerOld::makeFuncall(const std::string& var, const std::string& funcTemplate, bool withIndex, const std::string& value)
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


bool MsgCompilerOld::getPropertyAsBool(const Properties& p, const char *name, bool defval)
{
    Properties::const_iterator it = p.find(name);
    if (it == p.end())
        return defval;
    if (it->second == "false")
        return false;
    return true;
}

std::string MsgCompilerOld::getProperty(const Properties& p, const char *name, const std::string& defval)
{
    Properties::const_iterator it = p.find(name);
    if (it == p.end())
        return defval;
    return it->second;
}

MsgCompilerOld::StringVector MsgCompilerOld::lookupExistingClassName(const std::string& name)
{
    StringVector ret;
    std::map<std::string, ClassType>::iterator it;

    if (name.empty()) {
        ret.push_back(name);
        return ret;
    }

    // if $name contains "::" then user means explicitly qualified name; otherwise he means 'in whichever namespace it is'
    if (name.find("::") != name.npos) {
        std::string qname = name.substr(0, 2) == "::" ? name.substr(2) : name;  // remove leading "::", because names in @classes don't have it either
        it = classType.find(qname);
        if (it != classType.end()) {
            ret.push_back(it->first);
            return ret;
        }
    }
    else {
        std::string qname = prefixWithNamespace(name);
        it = classType.find(qname);
        if (it != classType.end()) {
            ret.push_back(it->first);
            return ret;
        }
    }
    size_t namelength = name.length();
    for (it = classType.begin(); it != classType.end(); ++it) {
        size_t l = it->first.length();
        if (l >= namelength) {
            size_t pos = l - namelength;
            if ((pos == 0 || it->first[pos-1] == ':') && (it->first.substr(pos) == name)) {
                ret.push_back(it->first);
            }
        }
    }
    return ret;
}

MsgCompilerOld::StringVector MsgCompilerOld::lookupExistingEnumName(const std::string& name)
{
    StringVector ret;
    std::map<std::string, std::string>::iterator it;

    if (name.empty()) {
        // ret.push_back(name);
        return ret;
    }

    // if $name contains "::" then user means explicitly qualified name; otherwise he means 'in whichever namespace it is'
    if (name.find("::") != name.npos) {
        std::string qname = name.substr(0, 2) == "::" ? name.substr(2) : name;  // remove leading "::", because names in @classes don't have it either
        it = enumType.find(qname);
        if (it != enumType.end()) {
            ret.push_back(it->second);
            return ret;
        }
    }
    else {
        std::string qname = prefixWithNamespace(name);  // prefer name from local namespace
        it = enumType.find(qname);
        if (it != enumType.end()) {
            ret.push_back(it->second);
            return ret;
        }
    }

    size_t namelength = name.length();
    for (it = enumType.begin(); it != enumType.end(); ++it) {
        size_t l = it->second.length();
        if (l >= namelength) {
            size_t pos = l - namelength;
            if ((pos == 0 || it->second[pos-1] == ':') && (it->second.substr(pos) == name)) {
                ret.push_back(it->second);
            }
        }
    }
    return ret;
}

void MsgCompilerOld::generateTemplates()
{
    CC << "// forward\n";
    CC << "template<typename T, typename A>\n";
    CC << "std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);\n\n";

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
    CC << "    \n";
    CC << "    char buf[32];\n";
    CC << "    sprintf(buf, \" (size=%u)\", (unsigned int)vec.size());\n";
    CC << "    out.write(buf, strlen(buf));\n";
    CC << "    return out;\n";
    CC << "}\n";
    CC << "\n";
}

void MsgCompilerOld::generateNamespaceBegin(ASTNode *element)
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

void MsgCompilerOld::generateNamespaceEnd()
{
#ifdef MSGC_COMPATIBILE
    for (StringVector::const_iterator it = namespaceNameVector.begin(); it != namespaceNameVector.end(); ++it) {
        H << "}; // end namespace " << *it << std::endl;
        CC << "}; // end namespace " << *it << std::endl;
    }
#else
    for (StringVector::const_reverse_iterator it = namespaceNameVector.rbegin(); it != namespaceNameVector.rend(); ++it) {
        H << "} // namespace " << *it << std::endl;
        CC << "} // namespace " << *it << std::endl;
    }
#endif
    H << std::endl;
    CC << std::endl;
    namespaceNameVector.clear();
}

void MsgCompilerOld::addClassType(const std::string& classqname, ClassType type, ASTNode *context)
{
    if (classType.find(classqname) != classType.end()) {
        if (classType[classqname] != type)
            errors->addError(context, "different declarations for '%s' are inconsistent\n", classqname.c_str());
    }
    else {
        classType[classqname] = type;
    }
}

MsgCompilerOld::ClassType MsgCompilerOld::getClassType(const std::string& classqname)
{
    Assert(!classqname.empty() && classqname[0] != ':');  // must not start with "::"
    std::map<std::string, ClassType>::iterator it = classType.find(classqname);
    ClassType type = it != classType.end() ? it->second : UNKNOWN;
    return type;
}

}  // namespace nedxml
}  // namespace omnetpp
